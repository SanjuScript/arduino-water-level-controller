# 🚰 Arduino Water Tank Controller

An advanced, reliable water tank motor controller built using Arduino. It features dual-mode operation (Automatic control / Manual override), software-debounced inputs to prevent EMI issues, an OLED status display with fluid filling animations, and I2C timeout protection.

---

## 📌 Key Features

*   🔄 **Dual-Mode Operation**:
    *   **AUTO Mode**: Fully automatic motor control based on the tank's water level.
    *   **MANUAL Mode**: Direct override control using dedicated push buttons.
*   🛡️ **Noise & EMI Rejection**:
    *   **Software Button Debouncing**: High-frequency noise spikes from relay switching are filtered out using a `50ms` stability check.
    *   **Float Switch Debouncing**: A `1000ms` (1 second) buffer prevents motor chattering caused by water ripples/waves.
    *   **I2C Timeout Protection**: Protects the Arduino from freezing if electrical noise disrupts OLED communication.
*   📺 **Dynamic OLED Dashboard**:
    *   **Layout Divider**: Clear separation of status information and graphics.
    *   **Real-time Metrics**: Displays Current Mode, Motor Status, Tank Level, and the raw Float Pin value.
    *   **Fluid Filling Animation**: Displays dynamic flowing droplets and undulating waves on the water surface *only* when the motor is running.

---

## 🔌 System Wiring Diagram

Below is the verified pin mapping used in the sketch:

| Arduino Pin | Component | Description |
| :---: | :--- | :--- |
| **D2** | Float Sensor Switch | Closed = GND (Tank FULL), Open = HIGH (Tank LOW) |
| **D3** | Mode Push Button | Toggles between **AUTO** and **MANUAL** modes |
| **D4** | Motor Push Button | Toggles Motor **ON/OFF** in **MANUAL** mode |
| **D6** | Blue LED | ON when Motor is running in **MANUAL** mode |
| **D7** | Green LED | ON when system is in **MANUAL** mode |
| **D8** | Relay Control Pin | Outputs `HIGH` (`RELAY_ON`) to run, `LOW` (`RELAY_OFF`) to stop |
| **A4** | OLED SDA | I2C Data line for the SH1106 OLED |
| **A5** | OLED SCL | I2C Clock line for the SH1106 OLED |
| **5V / GND** | Power Rails | Commons for sensors, OLED, and relay coils |

---

## ⚙️ How It Works

### 1. Control Logic
*   **AUTO Mode (Default)**:
    *   When the float switch drops to **LOW** level (reading `HIGH` / `1` on Pin 2), the controller turns the motor **ON**.
    *   When the water level rises to **FULL** (reading `LOW` / `0` on Pin 2), the controller turns the motor **OFF**.
*   **MANUAL Mode**:
    *   Toggled using the Mode button. The Green LED lights up to indicate manual control.
    *   The Motor button toggles the motor state. The Blue LED lights up when the motor is manually running.

### 2. Software Debouncing
Inductive loads (like AC water pumps) produce high-voltage arcs across the relay contacts when switching off. This emits Electromagnetic Interference (EMI) which can cause weak pull-up inputs on the Arduino to briefly register false state changes. 
By using non-blocking, millis-based software debouncing:
- False button triggers on the Mode and Motor buttons are completely blocked.
- The system remains securely in its selected mode during motor switching.

---

## 🚀 Setup & Installation

1.  **Install Required Arduino Libraries**:
    *   [U8g2](https://github.com/olikraus/u8g2) (For OLED SH1106 display)
    *   `Wire` (Standard Arduino I2C library)
2.  **Upload the Sketch**:
    *   Open `src/WaterTankController/WaterTankController.ino` in the Arduino IDE.
    *   Select your board (e.g., Arduino Uno) and the correct COM port.
    *   Compile and upload the code.

---

## 🧪 Troubleshooting

| Issue | Potential Cause | Solution |
| :--- | :--- | :--- |
| **Motor doesn't start automatically** | The system switched to MANUAL mode due to EMI noise | Upload the updated code containing **software debouncing** and keep signal wires separate from power wires. |
| **OLED screen is frozen** | I2C bus locked up due to electrical noise | The updated code implements `Wire.setWireTimeout(...)` to auto-recover from I2C noise hangs. Check wiring shielding. |
| **OLED screen doesn't turn on** | Incorrect I2C wiring or address | Verify SCL is on A5 and SDA is on A4. Check that the display is using the SH1106 driver. |
| **Motor clicks rapidly (chattering)** | Water ripples triggering the float switch | The sketch includes a `1-second` float switch debounce buffer. For heavy splashing, increase `FLOAT_DEBOUNCE_DELAY` to `3000ms`. |

---

## 📄 License

This project is licensed under the MIT License. See the LICENSE file for details.
