# 🚰 Arduino Water Tank Controller

An automated water tank motor controller using Arduino, with features for auto control, manual override, OLED status display, and safe relay-based switching.

---

## 📌 Features

- ✅ **Automatic Motor ON/OFF** based on tank water level.
- 🔘 **Manual Override** using a push button.
- 📺 **OLED Display** for real-time water level and motor status.
- ⚡ **Relay Isolation** for safe control of high-voltage motor circuits.

---

## 🧰 Components Required

| Component              | Quantity |
|------------------------|----------|
| Arduino Uno/Esp32      | 1        |
| Ultrasonic Sensor\*    | 1        |
| OLED Display (SSD1306) | 1        |
| Push Button Switch     | 1        |
| 1-Channel Relay Module | 1        |
| Motor Pump             | 1        |
| Power Supply           | 1        |
| Resistors, Wires, Breadboard | As needed |

> \* You can use float sensors, ultrasonic sensors, or conductive probes based on availability.

---

## 🔌 Wiring Diagram (Overview)

> **Note:** Adjust pins as per your actual circuit. Always ensure safe isolation for AC-powered motors.

| Arduino Pin | Connected To       |
|-------------|--------------------|
| D7          | Sensor TRIG        |
| D8          | Sensor ECHO        |
| D6          | Push Button        |
| D7          | Relay IN           |
| A4, A5      | OLED (SDA, SCL)    |
| GND, 5V     | Common Ground/Power |

---

## ⚙️ How It Works

- The system reads the **water level sensor** to determine the tank status.
- If water level drops **below a threshold**, the **motor turns ON**.
- Once the tank is **full**, the motor turns OFF automatically.
- A **manual override** button lets the user manually toggle the motor regardless of the water level.
- The **OLED** shows:
  - Current mode: `AUTO` or `MANUAL`
  - Motor status: `ON` or `OFF`
  - Water level percentage or status 

---

## 🚀 Getting Started

1. **Clone this repository:**

    ```bash
    git clone https://github.com/SanjuScript/arduino-water-level-controller.git
    cd water-tank-controller
    ```

2. **Install Arduino Libraries:**
    - [Wire.h](https://github.com/czukowski/Wire.h.git)
    - [Ug82](https://docs.arduino.cc/libraries/u8g2/)
    - Any required library for your water level sensor

3. **Upload the Code:**
    - Open `water_tank_controller.ino` in Arduino IDE
    - Select your board and COM port
    - Upload the sketch

4. **Power Up and Test:**
    - Connect relay to motor (through proper insulation)
    - Power the Arduino and test both AUTO and MANUAL modes

---

## 🧪 Troubleshooting

| Issue                            | Solution |
|----------------------------------|----------|
| OLED not displaying              | Check I2C address and wiring |
| Manual override not working      | Check button debounce or pin connection |
| Relay not switching              | Ensure 5V power, verify IN pin logic |
| Incorrect water level reading    | Recalibrate sensor or check placement |

---

## 📄 License

This project is licensed under the MIT License.  
See the [LICENSE](LICENSE) file for details.

---

## 🙏 Acknowledgments

- Ug82 for the OLED libraries
- Arduino Community for continuous support and inspiration

---

## 👤 Author

**[Sanjay N P]**  
💼 Hobbyist & Embedded Systems Enthusiast

---

