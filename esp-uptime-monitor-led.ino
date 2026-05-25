#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>

/*

====================================================
ESP32 HTTPS UPTIME MONITOR
====================================================

✔ LED task synchronization
✔ No overlapping LED blinking
✔ Next task waits for LED completion
✔ Accurate LED status
✔ Faster HTTPS bulk checking
✔ Lower CPU usage
✔ Lower ESP32 heat
✔ Better HTTPS stability
✔ Better WiFi reconnect
✔ Better memory cleanup
✔ Reduced false failures
✔ Stable multi URL handling
✔ Sequential task execution

BUTTON:
--------------------------------
Single Press  -> Check Main URL
Double Press  -> Check All URLs
Long Press    -> Deep Retry Check

LED:
--------------------------------
Boot          -> 3 blinks
WiFi          -> Slow blink
Checking      -> 2 quick blinks
Success       -> ON 1.5 sec
Failure       -> 5 fast blinks
NTFY          -> 2 tiny blinks

*/

// =====================================================
// WIFI
// =====================================================

const char* WIFI_SSID     = "";
const char* WIFI_PASSWORD = "";

// =====================================================
// NTFY
// =====================================================

const char* NTFY_URL =
"https://ntfy.sh/status";

// =====================================================
// TARGETS
// =====================================================

struct Target {

  const char* name;
  const char* url;
};

Target targets[] = {

  { "Main Site",      "https://example.com/" },
  { "Site 1",   "https://example.com/" },
  { "Site 2",     "https://example.com/" },
  { "Site 3",   "https://example.com/" },
  { "Site 4",     "https://example.com/" },
  { "Site 5", "https://example.com/" }
};

const uint8_t TARGET_COUNT =
sizeof(targets) / sizeof(targets[0]);

// =====================================================
// GPIO
// =====================================================

#define LED_PIN    5
#define BUTTON_PIN 4

// =====================================================
// BUTTON
// =====================================================

const unsigned long DEBOUNCE_MS      = 45;
const unsigned long DOUBLE_CLICK_MS  = 350;
const unsigned long LONG_PRESS_MS    = 1400;

// =====================================================
// WIFI
// =====================================================

const unsigned long WIFI_TIMEOUT_MS  = 15000;
const unsigned long WIFI_RETRY_MS    = 30000;

// =====================================================
// HTTP
// =====================================================

const uint16_t HTTP_TIMEOUT_MS       = 4500;

// =====================================================
// STATES
// =====================================================

bool stableButtonState = HIGH;
bool lastReading       = HIGH;

unsigned long lastDebounceTime = 0;

unsigned long pressStartTime   = 0;
unsigned long lastReleaseTime  = 0;

unsigned long lastWiFiRetry    = 0;

uint8_t clickCount = 0;

bool failureState = false;

// =====================================================
// SAFE DELAY
// =====================================================

void safeDelay(unsigned long ms) {

  unsigned long start = millis();

  while (millis() - start < ms) {

    delay(1);

    yield();
  }
}

// =====================================================
// LED HELPERS
// =====================================================

inline void ledOn() {

  digitalWrite(LED_PIN, HIGH);
}

inline void ledOff() {

  digitalWrite(LED_PIN, LOW);
}

// =====================================================
// LED WAIT STABILIZER
// =====================================================

void ledStabilize() {

  ledOff();

  safeDelay(180);
}

// =====================================================
// LED BLINK
// =====================================================

void blinkLED(
  uint8_t times,
  uint16_t onMs,
  uint16_t offMs
) {

  for (uint8_t i = 0; i < times; i++) {

    ledOn();

    safeDelay(onMs);

    ledOff();

    safeDelay(offMs);
  }

  // Important:
  // wait after animation fully completed
  ledStabilize();
}

// =====================================================
// LED PATTERNS
// =====================================================

void bootLED() {

  blinkLED(3, 90, 90);
}

void wifiLED() {

  blinkLED(1, 250, 150);
}

void checkingLED() {

  blinkLED(2, 60, 60);
}

void ntfyLED() {

  blinkLED(2, 35, 35);
}

void successLED() {

  ledOn();

  safeDelay(1500);

  ledOff();

  ledStabilize();
}

void failureLED() {

  blinkLED(5, 70, 70);
}

// =====================================================
// WIFI
// =====================================================

bool connectWiFi() {

  if (
    WiFi.status() ==
    WL_CONNECTED
  ) {

    return true;
  }

  Serial.println(
    "\n[WiFi] Connecting"
  );

  WiFi.disconnect(true);

  safeDelay(250);

  WiFi.mode(WIFI_STA);

  // Lower heat
  WiFi.setSleep(true);

  WiFi.begin(
    WIFI_SSID,
    WIFI_PASSWORD
  );

  unsigned long startTime =
    millis();

  while (
    WiFi.status() !=
    WL_CONNECTED &&
    millis() -
    startTime <
    WIFI_TIMEOUT_MS
  ) {

    wifiLED();

    delay(10);
  }

  if (
    WiFi.status() ==
    WL_CONNECTED
  ) {

    Serial.println(
      "[WiFi] Connected"
    );

    Serial.println(
      WiFi.localIP()
    );

    successLED();

    return true;
  }

  Serial.println(
    "[WiFi] Failed"
  );

  failureLED();

  return false;
}

// =====================================================
// NTFY
// =====================================================

void sendNtfy(
  const String& title,
  const String& body,
  const String& priority =
  "default"
) {

  // LED fully completes first
  ntfyLED();

  if (
    WiFi.status() !=
    WL_CONNECTED
  ) {

    return;
  }

  WiFiClientSecure client;

  client.setInsecure();

  client.setTimeout(
    HTTP_TIMEOUT_MS
  );

  HTTPClient http;

  http.setReuse(false);

  http.setConnectTimeout(
    HTTP_TIMEOUT_MS
  );

  http.setTimeout(
    HTTP_TIMEOUT_MS
  );

  if (
    !http.begin(
      client,
      NTFY_URL
    )
  ) {

    return;
  }

  http.addHeader(
    "Title",
    title
  );

  http.addHeader(
    "Priority",
    priority
  );

  http.addHeader(
    "Tags",
    "satellite"
  );

  int code =
    http.POST(body);

  Serial.print(
    "[NTFY] "
  );

  Serial.println(code);

  http.end();

  safeDelay(100);
}

// =====================================================
// URL CHECK
// =====================================================

bool checkURL(
  const char* url,
  bool retry = true
) {

  if (
    WiFi.status() !=
    WL_CONNECTED
  ) {

    return false;
  }

  bool success = false;

  uint8_t attempts =
    retry ? 2 : 1;

  for (
    uint8_t i = 0;
    i < attempts;
    i++
  ) {

    WiFiClientSecure client;

    client.setInsecure();

    client.setTimeout(
      HTTP_TIMEOUT_MS
    );

    HTTPClient http;

    http.setReuse(false);

    http.setConnectTimeout(
      HTTP_TIMEOUT_MS
    );

    http.setTimeout(
      HTTP_TIMEOUT_MS
    );

    if (
      !http.begin(
        client,
        url
      )
    ) {

      http.end();

      safeDelay(80);

      continue;
    }

    http.addHeader(
      "User-Agent",
      "ESP32-Uptime-Monitor"
    );

    int code =
      http.GET();

    Serial.print(
      "[CHECK] "
    );

    Serial.print(url);

    Serial.print(
      " -> "
    );

    Serial.println(code);

    if (
      code >= 200 &&
      code < 400
    ) {

      success = true;
    }

    http.end();

    if (success) {
      break;
    }

    // HTTPS stabilization
    safeDelay(100);
  }

  return success;
}

// =====================================================
// MAIN CHECK
// =====================================================

void checkMainWebsite() {

  Serial.println(
    "\n[CHECK] MAIN"
  );

  // LED fully completes first
  checkingLED();

  bool ok =
    checkURL(
      targets[0].url
    );

  if (ok) {

    failureState = false;

    successLED();

    sendNtfy(
      "✅ Website UP",
      "Main website healthy"
    );

  } else {

    failureState = true;

    failureLED();

    sendNtfy(
      "🚨 Website DOWN",
      "Main website failed",
      "high"
    );
  }
}

// =====================================================
// BULK CHECK
// =====================================================

void checkAllTargets(
  bool deepRetry = false
) {

  Serial.println(
    "\n[CHECK] ALL"
  );

  checkingLED();

  uint8_t failed = 0;

  String report;

  report.reserve(512);

  for (
    uint8_t i = 0;
    i < TARGET_COUNT;
    i++
  ) {

    bool ok =
      checkURL(
        targets[i].url,
        deepRetry
      );

    if (ok) {

      report += "✅ ";

    } else {

      report += "❌ ";

      failed++;
    }

    report +=
      targets[i].name;

    report += "\n";

    // tiny stabilization
    safeDelay(40);
  }

  if (failed == 0) {

    failureState = false;

    successLED();

    sendNtfy(
      "✅ All Healthy",
      report
    );

  } else {

    failureState = true;

    failureLED();

    sendNtfy(
      "🚨 Failure Detected",
      report,
      "high"
    );
  }
}

// =====================================================
// BUTTON
// =====================================================

void handleButton() {

  bool reading =
    digitalRead(
      BUTTON_PIN
    );

  // Debounce
  if (
    reading !=
    lastReading
  ) {

    lastDebounceTime =
      millis();
  }

  // Stable
  if (
    millis() -
    lastDebounceTime >
    DEBOUNCE_MS
  ) {

    if (
      reading !=
      stableButtonState
    ) {

      stableButtonState =
        reading;

      // PRESSED
      if (
        stableButtonState ==
        LOW
      ) {

        pressStartTime =
          millis();
      }

      // RELEASED
      else {

        unsigned long
        pressDuration =
        millis() -
        pressStartTime;

        // LONG
        if (
          pressDuration >=
          LONG_PRESS_MS
        ) {

          Serial.println(
            "[BUTTON] LONG"
          );

          checkAllTargets(true);

          clickCount = 0;
        }

        // SHORT
        else {

          clickCount++;

          lastReleaseTime =
            millis();
        }
      }
    }
  }

  // CLICK ACTION
  if (
    clickCount > 0 &&
    millis() -
    lastReleaseTime >
    DOUBLE_CLICK_MS
  ) {

    // SINGLE
    if (clickCount == 1) {

      Serial.println(
        "[BUTTON] SINGLE"
      );

      checkMainWebsite();
    }

    // DOUBLE
    else {

      Serial.println(
        "[BUTTON] DOUBLE"
      );

      checkAllTargets();
    }

    clickCount = 0;
  }

  lastReading = reading;
}

// =====================================================
// SETUP
// =====================================================

void setup() {

  Serial.begin(115200);

  pinMode(
    LED_PIN,
    OUTPUT
  );

  pinMode(
    BUTTON_PIN,
    INPUT_PULLUP
  );

  ledOff();

  bootLED();

  connectWiFi();

  sendNtfy(
    "🚀 ESP32 Online",
    "HTTPS uptime monitor started"
  );
}

// =====================================================
// LOOP
// =====================================================

void loop() {

  // WiFi recovery
  if (
    WiFi.status() !=
    WL_CONNECTED &&
    millis() -
    lastWiFiRetry >
    WIFI_RETRY_MS
  ) {

    lastWiFiRetry =
      millis();

    connectWiFi();
  }

  handleButton();

  delay(2);
}
