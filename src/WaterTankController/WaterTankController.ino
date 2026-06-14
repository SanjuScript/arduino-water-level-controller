#include <Wire.h>
#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C display(
  U8G2_R0,
  U8X8_PIN_NONE);

// Pins
#define FLOAT_PIN 2
#define MODE_BTN 3
#define MOTOR_BTN 4

#define BLUE_LED 6
#define GREEN_LED 7

#define RELAY_PIN 8

// Relay active LOW
#define RELAY_ON HIGH
#define RELAY_OFF LOW

bool autoMode = true;
bool motorState = false;

// Debounce state variables
unsigned long lastModeBtnTime = 0;
bool modeBtnStableState = HIGH;
bool lastModeBtnReading = HIGH;

unsigned long lastMotorBtnTime = 0;
bool motorBtnStableState = HIGH;
bool lastMotorBtnReading = HIGH;

unsigned long lastFloatBtnTime = 0;
bool floatStableState = HIGH;
bool lastFloatReading = HIGH;

const unsigned long DEBOUNCE_DELAY = 50;          // 50ms for manual buttons
const unsigned long FLOAT_DEBOUNCE_DELAY = 1000;  // 1s for float switch to filter water ripples

// Animation state variables
int animFrame = 0;
unsigned long lastAnimTime = 0;
const unsigned long ANIM_INTERVAL = 150;  // 150ms per frame

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
#if defined(ARDUINO_ARCH_AVR)
  Wire.setWireTimeout(25000, true);  // 25ms timeout, reset bus on timeout to prevent hangs from EMI noise
#endif

  display.clearBuffer();
  display.setFont(u8g2_font_ncenB08_tr);
  display.drawStr(15, 25, "Water Tank");
  display.drawStr(15, 45, "Controller");
  display.sendBuffer();

  delay(1500);
}

void loop() {

  unsigned long currentMillis = millis();

  // Read raw inputs
  bool rawFloat = digitalRead(FLOAT_PIN);
  bool rawModeBtn = digitalRead(MODE_BTN);
  bool rawMotorBtn = digitalRead(MOTOR_BTN);

  // ===== DEBOUNCE MODE BUTTON =====
  if (rawModeBtn != lastModeBtnReading) {
    lastModeBtnTime = currentMillis;
  }
  if ((currentMillis - lastModeBtnTime) > DEBOUNCE_DELAY) {
    if (rawModeBtn != modeBtnStableState) {
      modeBtnStableState = rawModeBtn;
      if (modeBtnStableState == LOW) {
        autoMode = !autoMode;
        Serial.print("Mode: ");
        Serial.println(autoMode ? "AUTO" : "MANUAL");
      }
    }
  }
  lastModeBtnReading = rawModeBtn;

  // ===== DEBOUNCE MOTOR BUTTON =====
  if (rawMotorBtn != lastMotorBtnReading) {
    lastMotorBtnTime = currentMillis;
  }
  if ((currentMillis - lastMotorBtnTime) > DEBOUNCE_DELAY) {
    if (rawMotorBtn != motorBtnStableState) {
      motorBtnStableState = rawMotorBtn;
      if (motorBtnStableState == LOW) {
        if (!autoMode) {
          motorState = !motorState;
          Serial.print("Motor: ");
          Serial.println(motorState ? "ON" : "OFF");
        }
      }
    }
  }
  lastMotorBtnReading = rawMotorBtn;

  // ===== DEBOUNCE FLOAT SWITCH =====
  if (rawFloat != lastFloatReading) {
    lastFloatBtnTime = currentMillis;
  }
  if ((currentMillis - lastFloatBtnTime) > FLOAT_DEBOUNCE_DELAY) {
    if (rawFloat != floatStableState) {
      floatStableState = rawFloat;
      Serial.print("Float Level: ");
      Serial.println(floatStableState == LOW ? "FULL" : "LOW");
    }
  }
  lastFloatReading = rawFloat;

  // ===== AUTO MODE =====
  if (autoMode) {
    // UP = 0 = FULL (LOW)
    // DOWN = 1 = LOW Level (HIGH)
    if (floatStableState == HIGH) {
      motorState = true;
    } else {
      motorState = false;
    }
  }

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

  // ===== ANIMATION TIMER =====
  if (currentMillis - lastAnimTime >= ANIM_INTERVAL) {
    lastAnimTime = currentMillis;
    animFrame = (animFrame + 1) % 12;
  }

  // ===== OLED =====
  display.clearBuffer();

  display.setFont(u8g2_font_6x12_tr);

  // Status text on the left (x=0 to 89)
  display.drawStr(0, 10, "TANK CONTROL");

  display.drawStr(0, 22,
                  autoMode ? "Mode: AUTO"
                           : "Mode: MANUAL");

  display.drawStr(0, 34,
                  motorState ? "Motor: ON"
                             : "Motor: OFF");

  display.drawStr(0, 46,
                  floatStableState == LOW
                    ? "Tank: FULL"
                    : "Tank: LOW");

  display.drawStr(0, 58,
                  rawFloat == LOW
                    ? "Float Pin: 0"
                    : "Float Pin: 1");

  // Layout Divider
  display.drawVLine(90, 0, 64);

  // Water Tank Outline (x=95 to 123, y=20 to 60)
  display.drawVLine(95, 20, 41);
  display.drawVLine(123, 20, 41);
  display.drawHLine(95, 60, 29);

  // Inlet pipe crossing the divider
  display.drawHLine(85, 14, 18);  // x=85 to x=102, y=14
  display.drawVLine(102, 14, 4);  // nozzle down to y=18

  // Water level Y coordinate
  int y_surf = (floatStableState == LOW) ? 30 : 52;

  // Draw water body & waves
  if (motorState) {
    // Pouring water stream (flowing dashes)
    for (int y = 18; y < y_surf; y += 4) {
      int dotY = y + (animFrame % 4);
      if (dotY < y_surf) {
        display.drawVLine(102, dotY, 2);
      }
    }

    // Moving waves on the surface
    for (int x = 96; x < 123; x++) {
      int waveOffset = ((x + animFrame * 2) % 6 < 3) ? 1 : 0;
      display.drawVLine(x, y_surf + waveOffset, 60 - (y_surf + waveOffset));
    }
  } else {
    // Static flat water body
    display.drawBox(96, y_surf, 27, 60 - y_surf);
  }

  display.sendBuffer();

  // Short delay for loop stability
  delay(10);
}