#include <EEPROM.h>

// Pin definizioni
const int VBAT_SERVIZI_PIN = A3;
const int VBAT_MOTORE_PIN = A2;
const int TRIMMER_SERVIZI_PIN = A0;
const int TRIMMER_MOTORE_PIN = A1;

// Partitore di tensione
const float R1 = 10000.0;
const float R2 = 3300.0;
const float VREF = 5.0;

// Coefficienti di calibrazione (caricati da EEPROM)
float Calibrazione_servizi;
float Calibrazione_motore;

// Indirizzi EEPROM
const int EEPROM_ADDR_CAL_SRV = 0;
const int EEPROM_ADDR_CAL_MOT = EEPROM_ADDR_CAL_SRV + sizeof(float);

// Funzione per leggere un float da EEPROM
float EEPROM_readFloat(int address) {
  float value;
  EEPROM.get(address, value);
  return value;
}

// Funzione per scrivere un float in EEPROM
void EEPROM_writeFloat(int address, float value) {
  EEPROM.put(address, value);
}

// Funzione di lettura tensioni calibrate
float leggiTensione(int pin, float calibrazione) {
  int adc = analogRead(pin);
  float tensione = adc * VREF / 1023.0 * (R1 + R2) / R2;
  return tensione * calibrazione;
}

// Procedura di calibrazione da trimmer
void Calibrazione() {
  Serial.println("== Modalità Calibrazione ==\nRegola i trimmer. Premi 'Q' per salvare e uscire.");

  float volt_trimmer_srv = 0.0;
  float volt_trimmer_mot = 0.0;

  while (true) {
    if (Serial.available()) {
      char c = Serial.read();
      if (c == 'Q') break;
    }

    // Lettura trimmer
    int adc_trimmer_srv = analogRead(TRIMMER_SERVIZI_PIN);
    int adc_trimmer_mot = analogRead(TRIMMER_MOTORE_PIN);

    volt_trimmer_srv = adc_trimmer_srv * VREF / 1023.0;
    volt_trimmer_mot = adc_trimmer_mot * VREF / 1023.0;

    // Lettura batterie
    int adc_servizi = analogRead(VBAT_SERVIZI_PIN);
    int adc_motore  = analogRead(VBAT_MOTORE_PIN);

    float volt_misurata_srv = adc_servizi * VREF / 1023.0 * (R1 + R2) / R2;
    float volt_misurata_mot = adc_motore  * VREF / 1023.0 * (R1 + R2) / R2;

    float cal_srv = volt_trimmer_srv / volt_misurata_srv;
    float cal_mot = volt_trimmer_mot / volt_misurata_mot;

    Serial.print("Trimmer SRV: ");
    Serial.print(volt_trimmer_srv, 3);
    Serial.print(" V | Batt SRV: ");
    Serial.print(volt_misurata_srv, 3);
    Serial.print(" V | Calib SRV: ");
    Serial.print(cal_srv, 4);

    Serial.print(" || Trimmer MOT: ");
    Serial.print(volt_trimmer_mot, 3);
    Serial.print(" V | Batt MOT: ");
    Serial.print(volt_misurata_mot, 3);
    Serial.print(" V | Calib MOT: ");
    Serial.println(cal_mot, 4);

    delay(1000);
  }

  // Ricalcolo finale prima del salvataggio
  float volt_misurata_srv = analogRead(VBAT_SERVIZI_PIN) * VREF / 1023.0 * (R1 + R2) / R2;
  float volt_misurata_mot = analogRead(VBAT_MOTORE_PIN)  * VREF / 1023.0 * (R1 + R2) / R2;

  Calibrazione_servizi = volt_trimmer_srv / volt_misurata_srv;
  Calibrazione_motore  = volt_trimmer_mot / volt_misurata_mot;

  EEPROM_writeFloat(EEPROM_ADDR_CAL_SRV, Calibrazione_servizi);
  EEPROM_writeFloat(EEPROM_ADDR_CAL_MOT, Calibrazione_motore);

  Serial.println("Calibrazione completata e salvata in EEPROM.");
}


void setup() {
  Serial.begin(9600);

  // Lettura valori di calibrazione da EEPROM
  Calibrazione_servizi = EEPROM_readFloat(EEPROM_ADDR_CAL_SRV);
  Calibrazione_motore  = EEPROM_readFloat(EEPROM_ADDR_CAL_MOT);

  // Se EEPROM è "vuota", valori default
  if (isnan(Calibrazione_servizi)) Calibrazione_servizi = 1.0;
  if (isnan(Calibrazione_motore))  Calibrazione_motore = 1.0;
}

void loop() {
  float vbat_servizi_volt = leggiTensione(VBAT_SERVIZI_PIN, Calibrazione_servizi);
  float vbat_motore_volt  = leggiTensione(VBAT_MOTORE_PIN, Calibrazione_motore);

  Serial.print("Batteria Servizi: ");
  Serial.print(vbat_servizi_volt, 2);
  Serial.print(" V  |  Motore: ");
  Serial.print(vbat_motore_volt, 2);
  Serial.println(" V");

  delay(1000);

  // Esegui Calibrazione solo in base a un comando
  // Per esempio: se ricevi un carattere via seriale
  if (Serial.available() && Serial.read() == 'c') {
    Calibrazione();
    Serial.println("Calibrazione completata e salvata in EEPROM.");
  }
}
