#include <avr/wdt.h>
#include <avr/io.h>
#include <EEPROM.h>

// Pin definitions
const int RELAY_PIN = 6;
const int LED_PIN = 3;
const int BUZZER_PIN = 2;
const int THRESHOLD_HIGH_PIN = A0;
const int THRESHOLD_LOW_PIN = A1;
const int VBAT_PIN = A3;

// Timer variables
unsigned long previousMillis = 0;
unsigned long previous60sMillis = 0;
const unsigned long interval1s = 1000;
const unsigned long interval60s = 60000;

// Partitore valori
const float R1 = 2200.0;
const float R2 = 1000.0;
const float VREF = 5.0;

// EEPROM addresses for 2-byte counter
const int EEPROM_ADDR_LOW = 0;
const int EEPROM_ADDR_HIGH = 1;

bool relayState = false;

void setup() {
  // Verifica causa del reset
  byte resetCause = MCUSR;
  MCUSR = 0;
  wdt_disable(); // Disattiva temporaneamente il watchdog

  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  if (resetCause & _BV(WDRF)) {
    Serial.println("⚠️ Reset da Watchdog!");

    // Leggi contatore 2 byte da EEPROM
    uint16_t wdResetCount = EEPROM.read(EEPROM_ADDR_LOW);
    wdResetCount |= (uint16_t)EEPROM.read(EEPROM_ADDR_HIGH) << 8;

    wdResetCount++; // Incrementa
    EEPROM.write(EEPROM_ADDR_LOW, wdResetCount & 0xFF);
    EEPROM.write(EEPROM_ADDR_HIGH, (wdResetCount >> 8) & 0xFF);

    Serial.print("Contatore reset watchdog: ");
    Serial.println(wdResetCount);

    beep(5); // Beep di avviso
  } else {
    Serial.println("Sistema avviato normalmente.");
  }

  wdt_enable(WDTO_2S); // Riattiva watchdog
}

void loop() {
  wdt_reset(); // Resetta watchdog

  unsigned long currentMillis = millis();

  // Blink LED ogni secondo
  if (currentMillis - previousMillis >= interval1s) {
    previousMillis = currentMillis;
    digitalWrite(LED_PIN, !digitalRead(LED_PIN));
  }

  // Ogni 60 secondi: controllo tensione batteria
  if (currentMillis - previous60sMillis >= interval60s) {
    previous60sMillis = currentMillis;

    int adc_vbat = analogRead(VBAT_PIN);
    int thresholdLow = analogRead(THRESHOLD_LOW_PIN);
    int thresholdHigh = analogRead(THRESHOLD_HIGH_PIN);

    float vbatVolt = adc_vbat * VREF / 1023.0 * (R1 + R2) / R2;

    Serial.println("----------");
    Serial.print("VBAT (ADC): ");
    Serial.print(adc_vbat);
    Serial.print(" → ");
    Serial.print(vbatVolt, 2);
    Serial.println(" V");
    Serial.print("Soglia bassa (A1): ");
    Serial.println(thresholdLow);
    Serial.print("Soglia alta (A0): ");
    Serial.println(thresholdHigh);

    if (adc_vbat < thresholdLow) {
      relayState = false;
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("VBAT sotto soglia bassa → relè disattivato");
      beep(2);
    } else if (adc_vbat > thresholdHigh) {
      relayState = true;
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("VBAT sopra soglia alta → relè attivato");
      beep(3);
    } else {
      Serial.println("VBAT nella zona neutra → nessuna azione");
    }

    // Stampa contatore watchdog ogni 60 secondi
    uint16_t wdResetCount = EEPROM.read(EEPROM_ADDR_LOW);
    wdResetCount |= (uint16_t)EEPROM.read(EEPROM_ADDR_HIGH) << 8;
    Serial.print("Contatore reset watchdog attuale: ");
    Serial.println(wdResetCount);
  }

  // Comandi seriali
  if (Serial.available()) {
    char cmd = Serial.read();
    if (cmd == 'z') {
      EEPROM.write(EEPROM_ADDR_LOW, 0);
      EEPROM.write(EEPROM_ADDR_HIGH, 0);
      Serial.println("Contatore reset watchdog azzerato.");
    } else if (cmd == 'r') {
      uint16_t wdResetCount = EEPROM.read(EEPROM_ADDR_LOW);
      wdResetCount |= (uint16_t)EEPROM.read(EEPROM_ADDR_HIGH) << 8;
      Serial.print("Contatore reset watchdog attuale: ");
      Serial.println(wdResetCount);
    }
  }
}

void beep(int count) {
  for (int i = 0; i < count; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(100);
    digitalWrite(BUZZER_PIN, LOW);
    delay(100);
  }
}
