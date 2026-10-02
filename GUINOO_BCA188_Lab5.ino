#include <Arduino.h>

const uint8_t BUTTON_PIN = 4;
const uint8_t POT_PIN = 5;
const uint8_t STATUS_LED_PIN = 18;
const uint8_t PWM_LED_PIN = 17;

bool buttonPressed = false;
bool pwmReady = false;

int rawInput = 0;
int requestedDuty = 0;
int appliedDuty = 0;

void readInputs();
void processInputs();
void updateOutputs();

void setup() {

  Serial.begin(115200);

  // Button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // Status LED
  pinMode(STATUS_LED_PIN, OUTPUT);
  digitalWrite(STATUS_LED_PIN, LOW);

  // PWM LED
  pinMode(PWM_LED_PIN, OUTPUT);
  digitalWrite(PWM_LED_PIN, LOW);

  // Potentiometer / ADC
  analogReadResolution(12);
  analogSetPinAttenuation(POT_PIN, ADC_11db);

  // PWM
  pwmReady = ledcAttach(PWM_LED_PIN, 5000, 8);

  if (pwmReady) {
    ledcWrite(PWM_LED_PIN, 0);
  } else {
    Serial.println("PWM setup failed.");
  }

  Serial.println("System started.");
  Serial.println("Reading potentiometer...");
}

void loop() {

  readInputs();
  processInputs();
  updateOutputs();

  delay(20);
}

void readInputs() {

  buttonPressed = (digitalRead(BUTTON_PIN) == LOW);

  rawInput = analogRead(POT_PIN);

  // Print potentiometer ADC reading
  Serial.print("Potentiometer ADC: ");
  Serial.println(rawInput);
}

void processInputs() {

  requestedDuty = constrain(
    map(rawInput, 0, 4095, 0, 255),
    0L,
    255L
  );

  if (buttonPressed && pwmReady) {
    appliedDuty = requestedDuty;
  } else {
    appliedDuty = 0;
  }
}

void updateOutputs() {

  digitalWrite(
    STATUS_LED_PIN,
    (buttonPressed && pwmReady) ? HIGH : LOW
  );

  if (pwmReady) {
    ledcWrite(PWM_LED_PIN, appliedDuty);
  }
}
