# ESP32 Uptime Monitor

A lightweight ESP32 HTTPS uptime monitoring project with LED status indicators, push button controls, WiFi auto reconnect, bulk website checking, and ntfy push notifications. Built using Arduino IDE with optimized CPU usage and stable HTTPS handling.  

---

<img width="500" height="580" alt="ESP32 Uptime Monitor with led and push button" src="https://github.com/user-attachments/assets/55870756-26bf-4cd1-9c58-a1af57b415d2" /> <img width="580" height="560" alt="ESP32 Uptime Monitor" src="https://github.com/user-attachments/assets/20c442e0-61b9-4982-96dd-2304a8328f54" />

---

Fast and lightweight HTTPS uptime monitoring system using ESP32, LED indicators, push button controls, and ntfy notifications.  

Monitor multiple HTTPS websites directly from your ESP32 with accurate LED status feedback, WiFi auto reconnect, retry handling, and low CPU usage.  

## 📷 Project Preview

ESP32 + LED + Push Button based website monitoring system.  

- Single press → Check main website
- Double press → Check all websites
- Long press → Deep retry check
- LED shows live status
- ntfy.sh notifications support
- HTTPS supported
- Low CPU usage
- Stable WiFi reconnect

## ✨ Features

- ✅ HTTPS website monitoring
- ✅ Multiple URL support
- ✅ Fast bulk checking
- ✅ LED status indicators
- ✅ Push button controls
- ✅ ntfy.sh notifications
- ✅ WiFi auto reconnect
- ✅ Lower CPU usage
- ✅ Better HTTPS stability
- ✅ Retry handling
- ✅ Sequential task execution
- ✅ Stable LED synchronization
- ✅ ESP32 optimized

## 🎥 Demo Video

Watch the live working demo of the ESP32 HTTPS Uptime Monitor project: <https://www.youtube.com/shorts/VhSiiZyeA0Q>  

## 🧰 Hardware Used

| Component                  | Quantity |
| -------------------------- | -------- |
| ESP32 Dev Board            | 1        |
| 5mm LED                    | 1        |
| 220Ω Resistor              | 1        |
| Push Button / Micro Switch | 1        |
| Breadboard                 | 1        |
| Male to Male Jumper Wires  | Few      |
| USB Cable                  | 1        |

## 🔌 GPIO Connections

| ESP32 Pin        | Component             |
| ---------------- | --------------------- |
| GPIO 5           | LED Positive          |
| GPIO 4           | Push Button           |
| GND              | LED Negative + Button |
| 3.3V / USB Power | ESP32 Power           |

## 🌳 Wiring Diagram

```sh
                 ESP32 HTTPS Uptime Monitor

                      ┌───────────────┐
                      │    ESP32      │
                      │               │
                      │ GPIO 5 ───────┼─────[220Ω]─────► LED (+)
                      │               │
                      │ GND ──────────┼────────────────► LED (-)
                      │               │
                      │ GPIO 4 ───────┼───────────────► Push Button
                      │               │
                      │ GND ──────────┼───────────────► Push Button
                      └───────────────┘

```

## 🔧 Breadboard Connection Layout

```sh
ESP32 GPIO 5
      │
      └────► 220Ω Resistor ───► LED Positive Leg

LED Negative Leg
      │
      └────► GND

ESP32 GPIO 4
      │
      └────► Push Button

Other Push Button Pin
      │
      └────► GND
```

## 🎮 Button Controls

| Action       | Function           |
| ------------ | ------------------ |
| Single Press | Check Main Website |
| Double Press | Check All Websites |
| Long Press   | Deep Retry Check   |

## 💡 LED Status Indicators

| LED Pattern    | Meaning           |
| -------------- | ----------------- |
| 3 Blinks       | ESP32 Boot        |
| Slow Blink     | WiFi Connecting   |
| 2 Quick Blinks | Checking Website  |
| ON 1.5 Seconds | Success           |
| 5 Fast Blinks  | Failure           |
| 2 Tiny Blinks  | ntfy Notification |

## 4️⃣ Configure WiFi

```sh
const char* WIFI_SSID     = "YOUR_WIFI";
const char* WIFI_PASSWORD = "YOUR_PASSWORD";
```

## 5️⃣ Configure ntfy

```sh
const char* NTFY_URL =
"https://ntfy.sh/your-topic";
```
## 6️⃣ Add Your Websites

```sh
Target targets[] = {

  { "Main Site", "https://example.com/" },
  { "API",       "https://api.example.com/" },
  { "Blog",      "https://blog.example.com/" }
};
```
## 📷 Hardware Setup

- ESP32 Dev Board
- 5mm LED
- 220Ω resistor
- Micro switch button
- Breadboard wiring  

## 🛠️ Future Improvements

- OLED display support
- Buzzer alerts
- Web dashboard  

## 📜 License

MIT License

