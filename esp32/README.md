# ESP32 · ESP‑IDF

| Project | What |
|---|---|
| [`esp-idf_blink`](esp-idf_blink) | ESP‑IDF blink: plain GPIO or addressable LED via the `led_strip` component, Kconfig menu, pytest |

```bash
cd esp-idf_blink
idf.py set-target esp32
idf.py build flash monitor
```
A full ESP32 project (HC‑SR04 + Wi‑Fi + web page) lives in its own repo: [esp32-ultrasonic-wifi-monitor](https://github.com/Bugee03/esp32-ultrasonic-wifi-monitor).
Next: Wi‑Fi, MQTT, FreeRTOS tasks.
