#include <Wire.h>
#include <U8g2lib.h>

// DISPLAY
U8G2_SH1106_128X64_NONAME_F_HW_I2C display(U8G2_R0, /* reset=*/U8X8_PIN_NONE);

// SENSORS
#define TRIG_PIN 9
#define ECHO_PIN 8

// RELAY
#define RELAY_PIN 7

// BUTTON (we're not using manual override now)
#define BUTTON_PIN 6
#define BUTTON_LED_PIN 5
// bool lastButtonState = HIGH;
// bool manualOverride = false;

// Tank height thresholds
const int motorOnThreshold = 75;
const int motorOffThreshold = 7;

bool pumpState = false;

void setup() {
  Serial.begin(9600);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH); // Motor OFF

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(BUTTON_LED_PIN, OUTPUT);
  digitalWrite(BUTTON_LED_PIN, LOW); // LED OFF

  display.begin();
  display.clearBuffer();
  display.setFont(u8g2_font_6x12_tr);
  display.drawStr(10, 30, "Made by sanju");
  display.sendBuffer();
  delay(1500);
}

int getFilteredDistance() {
  long sum = 0;
  int readings = 5;
  for (int i = 0; i < readings; i++) {
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG_PIN, LOW);
    long duration = pulseIn(ECHO_PIN, HIGH, 30000);
    int dist = duration * 0.034 / 2;
    if (dist > 0 && dist < 400) {
      sum += dist;
    } else {
      sum += 999;
    }
    delay(50);
  }
  return sum / readings;
}

void loop() {
  int distance = getFilteredDistance();

  // --- Automatic Control Only ---
  if (!pumpState && distance >= motorOnThreshold) {
    pumpState = true;
  } else if (pumpState && distance <= motorOffThreshold) {
    pumpState = false;
  }

  digitalWrite(RELAY_PIN, pumpState ? LOW : HIGH);  // Motor ON = LOW
  digitalWrite(BUTTON_LED_PIN, pumpState ? HIGH : LOW);  // LED ON = Pump ON

  // Debug print
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Display UI
  display.clearBuffer();

  // Tank Display
  const int tankX = 100;
  const int tankY = 10;
  const int tankWidth = 25;
  const int tankHeight = 50;

  display.drawFrame(tankX, tankY, tankWidth, tankHeight);

  int waterHeight = map(constrain(distance, motorOffThreshold, motorOnThreshold),
                        motorOnThreshold, motorOffThreshold,
                        0, tankHeight);

  display.drawBox(tankX + 1, tankY + tankHeight - waterHeight, tankWidth - 2, waterHeight);

  int waterPercentage = map(constrain(distance, motorOffThreshold, motorOnThreshold),
                            motorOnThreshold, motorOffThreshold,
                            0, 100);

  display.setFont(u8g2_font_ncenB08_tr);
  char buf[20];
  sprintf(buf, "Level: %d cm", distance);
  display.drawStr(0, 10, buf);

  if (pumpState) {
    display.drawStr(0, 35, "Motor: ON");
  } else {
    display.drawStr(0, 35, "Motor: OFF");
  }

  sprintf(buf, "Water: %d%%", waterPercentage);
  display.drawStr(0, 60, buf);

  // display.drawCircle(120, 10, 5);  // Manual override indicator - removed

  display.sendBuffer();
  delay(1000);
}
