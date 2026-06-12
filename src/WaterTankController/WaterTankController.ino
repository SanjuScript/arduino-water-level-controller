
#include <Wire.h>
#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(
  U8G2_R0,
  U8X8_PIN_NONE
);

// Pins
#define FLOAT_PIN   2
#define MODE_BTN    3
#define MOTOR_BTN   4

#define BLUE_LED    6
#define GREEN_LED   7

#define RELAY_PIN   8

// Relay active LOW
#define RELAY_ON  HIGH
#define RELAY_OFF LOW

bool autoMode = true;
bool motorState = false;

bool lastModeBtn = HIGH;
bool lastMotorBtn = HIGH;

void setup() {

  Serial.begin(115200);

  pinMode(FLOAT_PIN, INPUT_PULLUP);

  pinMode(MODE_BTN, INPUT_PULLUP);
  pinMode(MOTOR_BTN, INPUT_PULLUP);

  pinMode(BLUE_LED, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);

  pinMode(RELAY_PIN, OUTPUT);

  digitalWrite(RELAY_PIN, RELAY_OFF);

  display.begin();

  display.clearBuffer();
  display.setFont(u8g2_font_ncenB08_tr);
  display.drawStr(15, 25, "Water Tank");
  display.drawStr(15, 45, "Controller");
  display.sendBuffer();

  delay(1500);
}

void loop() {

  bool floatState = digitalRead(FLOAT_PIN);
  bool modeBtn = digitalRead(MODE_BTN);
  bool motorBtn = digitalRead(MOTOR_BTN);

  // ===== MODE BUTTON =====
  if (lastModeBtn == HIGH && modeBtn == LOW) {

    autoMode = !autoMode;

    Serial.print("Mode: ");
    Serial.println(autoMode ? "AUTO" : "MANUAL");

    delay(250);
  }

  lastModeBtn = modeBtn;

  // ===== AUTO MODE =====
  if (autoMode) {

    // UP = 0 = FULL
    // DOWN = 1 = LOW

    if (floatState == HIGH) {
      motorState = true;
    }

    if (floatState == LOW) {
      motorState = false;
    }
  }

  // ===== MANUAL MODE =====
 else {

  if (lastMotorBtn == HIGH && motorBtn == LOW) {

    motorState = !motorState;

    Serial.print("Motor: ");
    Serial.println(motorState ? "ON" : "OFF");

    delay(250);
  }
}

  lastMotorBtn = motorBtn;

  // ===== RELAY =====
  digitalWrite(RELAY_PIN,
               motorState ? RELAY_ON : RELAY_OFF);

  // ===== LEDS =====

  // Green LED ON only in MANUAL mode
  digitalWrite(GREEN_LED,
               autoMode ? LOW : HIGH);

  // Blue LED ON only when motor ON in MANUAL mode
  digitalWrite(BLUE_LED,
               (!autoMode && motorState) ? HIGH : LOW);

  // ===== OLED =====
  display.clearBuffer();

  display.setFont(u8g2_font_6x12_tr);

  display.drawStr(0, 12, "Water Controller");

  display.drawStr(0, 28,
                  autoMode ? "Mode : AUTO"
                           : "Mode : MANUAL");

  display.drawStr(0, 44,
                  motorState ? "Motor: ON"
                             : "Motor: OFF");

  display.drawStr(0, 60,
                  floatState == LOW
                    ? "Tank : FULL"
                    : "Tank : LOW");

  display.sendBuffer();

  delay(50);
}
