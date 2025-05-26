#include <SoftwareSerial.h>

// Definisci i pin RX e TX per SoftwareSerial (ad esempio, per il modulo Bluetooth)
// Collegamenti comuni:
// Bluetooth TX -> Arduino RX pin (es. 10)
// Bluetooth RX -> Arduino TX pin (es. 11) -> Ricorda di usare un partitore di tensione per i 3.3V
// (es. 1k Ohm e 2k Ohm per dividere 5V a 3.3V, se il modulo Bluetooth non è tollerante ai 5V)
const int bluetoothRxPin = 12; // Pin a cui è collegato il TX del modulo Bluetooth
const int bluetoothTxPin = 11; // Pin a cui è collegato il RX del modulo Bluetooth (tramite partitore di tensione se necessario)

// Inizializza un nuovo oggetto SoftwareSerial
SoftwareSerial bluetoothSerial(bluetoothRxPin, bluetoothTxPin);

String readString; // Una Stringa per memorizzare i dati in ingresso

void setup() {
  Serial.begin(115200); // Inizializza la comunicazione seriale standard con il computer
  Serial.println("SerialEvent test");

  bluetoothSerial.begin(9600); // Inizializza la comunicazione SoftwareSerial con il modulo Bluetooth
  Serial.println("Bluetooth Serial Ready");
}

void loop() {
  // Qui puoi aggiungere la logica principale del tuo programma,
  // senza bloccare il SerialEvent o la lettura dal Bluetooth
  // ad esempio, controllando sensori o attuando qualcosa.
  BluetoothSerialEvent();
  // Non è necessario chiamare bluetoothSerialEvent() esplicitamente,
  // ma è buona pratica leggere il buffer Bluetooth nel loop
  // in modo simile a come SerialEvent gestisce Serial.

}


void BluetoothSerialEvent(){
  while (bluetoothSerial.available()) {
    char c = bluetoothSerial.read();
    // Esempio: Invia al Serial Monitor ciò che ricevi via Bluetooth
    //Serial.print("Dato ricevuto da bluetooth: ");
    Serial.write(c);
    readString += c;

    if (c == '\n') {
      Serial.print("Dato ricevuto da BLUTUT: ");
      Serial.println(readString);
      // Invia lo stesso dato anche al modulo Bluetooth
      bluetoothSerial.print("Hai inviato: ");
      bluetoothSerial.println(readString);
      readString = ""; // Resetta la stringa
    }
  }
}

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay() in loop()
  will delay this routine. As of Arduino 1.0, this is a callout function and
  is not a part of the Interrupt Service Routine.

  Una SerialEvent si verifica ogni volta che un nuovo dato arriva sulla seriale hardware RX.
  Questa routine viene eseguita tra ogni esecuzione di loop(), quindi usare delay() in loop()
  ritarderà questa routine. A partire da Arduino 1.0, questa è una funzione di callback e
  non fa parte della Interrupt Service Routine.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    readString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      Serial.print("Dato ricevuto da Serial (computer): ");
      Serial.println(readString);
      // Invia lo stesso dato anche al modulo Bluetooth
      bluetoothSerial.print("Hai inviato: ");
      bluetoothSerial.println(readString);
      readString = ""; // Resetta la stringa
    }
  }
}