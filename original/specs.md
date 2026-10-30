# System Specification: Wearable AI Voice Input and CO2 Pendant (`specs.md`)

## 1. System Overview
The device is a wearable pendant and desk companion. It performs two primary functions:
1. Captures near-field voice audio and transmits it to external AI agents via Bluetooth Low Energy (BLE).
2. Measures ambient carbon dioxide ($\text{CO}_2$), temperature, and humidity levels in real time.

The device has no onboard speaker or display. It sends audio streams and environmental telemetry to the user's smartphone. A linear resonant actuator (LRA) and a single diffused status LED provide local tactile and visual alerts.

---

## 2. Hardware Architecture (Core BOM)

| Subsystem | Component | Interface | Function |
|---|---|---|---|
| **MCU / Radio** | ESP32-S3-MINI-1 (or nRF52840) | I2C / I2S | System control, BLE 5.0 audio streaming, and sensor polling. |
| **Voice Input** | Knowles SPH0645LM4H (or ICS-43434) | I2S (Digital) | Captures near-field voice audio (16 kHz, 16-bit mono). |
| **$\text{CO}_2$ Sensor** | Sensirion STCC4 + SHT40 (or SCD41) | I2C | Measures real-time ambient $\text{CO}_2$ (400 ppm to 5,000 ppm). |
| **Tactile Input** | Low-profile tactile switch (or capacitive pad) | GPIO | Push-to-talk (PTT) or tap-to-toggle voice recording. |
| **Privacy Control** | SPDT Sub-Miniature Slide Switch | Hardware Cut | Physically disconnects the VDD power rail to the microphone. |
| **Haptic Output** | TI DRV2605L + 8 mm LRA | I2C | Generates tactile alerts for high $\text{CO}_2$ and recording status. |
| **Power / Charging** | BQ25180 + 250 mAh LiPo | I2C / Analog | Manages battery charging via 2-pin magnetic pogo connector. |

---

## 3. Functional Requirements

### 3.1 AI Voice Input
* **Activation:** The user presses the tactile button to start or stop voice recording. The LRA provides a single crisp haptic click to confirm recording has started.
* **Transmission:** The MCU compresses the I2S audio stream (Opus or ADPCM codec) and transmits it over BLE to the host smartphone application for AI agent processing.
* **Hardware Privacy:** When the user slides the privacy switch to the OFF position, the circuit physically breaks the microphone power line. $\text{CO}_2$ monitoring continues to operate.

### 3.2 Environmental Monitoring ($\text{CO}_2$)
* **Sampling Rate:** The sensor measures $\text{CO}_2$ every 60 seconds during normal operation to optimize battery life.
* **Exhalation Filtering:** When the microphone is active (user is speaking into the pendant), the firmware pauses $\text{CO}_2$ logging for 30 seconds after speech ends. This prevents false high readings caused by the user's exhaled breath (~40,000 ppm).
* **Alert Thresholds:**
  * **Less than 1,000 ppm:** Normal operation.
  * **1,000 to 1,499 ppm:** Log warning in the phone app; attach environmental context to AI voice notes.
  * **1,500 ppm or greater:** Trigger a distinct double-pulse haptic alert to tell the user to open a window or change rooms.

---

## 4. Mechanical and Enclosure Constraints
* **Target Dimensions:** Maximum 38 mm diameter × 9.5 mm thickness.
* **Target Weight:** Less than 22 grams.
* **Wearability:** Works equally well worn on a neck cord, clipped magnetically to a shirt, or placed flat on a desk during meetings.
* **Thermal & Airflow Isolation:** An internal baffle wall isolates the $\text{CO}_2$ sensor chamber from MCU and battery heat. $\text{CO}_2$ vents sit on the bottom/side edges; the microphone port sits on the top front face.

---

## 5. Power Budget
* **Battery Capacity:** 250 mAh LiPo cell.
* **Target Runtime:** Minimum 24 hours per charge (based on 60-second $\text{CO}_2$ polling and 45 minutes of daily BLE voice streaming).
