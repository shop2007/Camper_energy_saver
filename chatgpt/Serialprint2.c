#include <SoftwareSerial.h>
SoftwareSerial bluetooth(10, 11); // RX, TX

// BUFFER per sprintf
char serialBuffer[128];

void setup() {
  Serial.begin(115200);
  bluetooth.begin(9600);

  // Esempio di uso
  int Giorni = 1, Ore = 2, Minuti = 3, Secondi = 4;
  Serialprintf2("; ⏳ %u:%02u:%02u:%02u", Giorni, Ore, Minuti, Secondi);
  Serialprint2ln("");
}

void loop() {
  // esempio ciclico
  float tensione = analogRead(A0) * (5.0 / 1023);
  Serialprintf2("Tensione: %.2f V\n", tensione);
  delay(1000);
}

// ---------- FUNZIONI DI STAMPA ----------

// Template generico per qualsiasi tipo (Serial.print e bluetooth.print)
template <typename T>
void Serialprint2(T dato) {
  Serial.print(dato);
  bluetooth.print(dato);
}

template <typename T>
void Serialprint2ln(T dato) {
  Serial.println(dato);
  bluetooth.println(dato);
}

// printf-style, con formattazione
void Serialprintf2(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  vsnprintf(serialBuffer, sizeof(serialBuffer), fmt, args);
  va_end(args);
  Serial.print(serialBuffer);
  bluetooth.print(serialBuffer);
}
