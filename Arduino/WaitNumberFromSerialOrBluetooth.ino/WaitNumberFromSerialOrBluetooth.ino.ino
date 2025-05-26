#include <SoftwareSerial.h>

// Definisci i pin RX e TX per SoftwareSerial (ad esempio, per il modulo Bluetooth)
const int bluetoothRxPin = 12;
const int bluetoothTxPin = 11;

SoftwareSerial bluetoothSerial(bluetoothRxPin, bluetoothTxPin);

String serialReadString = "";       // Stringa per i dati in ingresso dal Serial Monitor
String bluetoothReadString = "";    // Stringa per i dati in ingresso dal Bluetooth

void setup() {
  Serial.begin(115200); // Inizializza la comunicazione seriale standard con il computer
  Serial.println("Avvio del programma.");
  Serial.println("In attesa di un numero da Serial Monitor o Bluetooth (0-999)...");

  bluetoothSerial.begin(9600); // Inizializza la comunicazione SoftwareSerial con il modulo Bluetooth
}

void loop() {
  int receivedNumber = WaitSerialAndBluetooth();
  Serial.print("Numero ricevuto (valido 0-999): ");
  Serial.println(receivedNumber);

  // Qui puoi usare il numero ricevuto per la logica del tuo programma
  // ... ad esempio:
  // if (receivedNumber == 100) {
  //   Serial.println("Comando 100 eseguito!");
  // }
  // else if (receivedNumber == 200) {
  //   bluetoothSerial.println("Comando 200 ricevuto via Bluetooth!");
  // }

  // Potresti voler aggiungere un piccolo delay o altre operazioni qui
  // prima di rientrare in attesa di un nuovo numero.
  delay(1000); // Esempio: aspetta 1 secondo prima di chiedere un nuovo input
  Serial.println("\nIn attesa di un nuovo numero...");
}


/*
 * int WaitSerialAndBluetooth():
 * Questa funzione resta in attesa bloccante finché non riceve una stringa numerica
 * valida (0-999) da Serial Monitor o da Bluetooth SoftwareSerial.
 *
 * Ritorna:
 * Il numero intero valido ricevuto.
 */
int WaitSerialAndBluetooth() {
  serialReadString = "";    // Resetta la stringa per il Serial Monitor
  bluetoothReadString = ""; // Resetta la stringa per il Bluetooth

  Serial.println("\nInserisci un numero (0-999) e premi Invio:");
  bluetoothSerial.println("Inserisci un numero (0-999) e premi Invio:");

  while (true) { // Loop infinito finché non si riceve un numero valido

    // --- Gestione input da Serial Monitor (USB) ---
    while (Serial.available()) {
      char inChar = (char)Serial.read();
      serialReadString += inChar;

      // Se il carattere è un newline (Invio), processa la stringa
      if (inChar == '\n') {
        int num = validateAndConvert(serialReadString);
        if (num != -1) { // -1 indica un numero non valido
          Serial.println("Numero ricevuto da Serial Monitor.");
          return num; // Numero valido, esci dalla funzione
        } else {
          Serial.print("Input non valido da Serial Monitor: '");
          serialReadString.trim();
          Serial.print(serialReadString); // trim() rimuove spazi, newline, ecc.
          Serial.println("'. Riprova.");
          serialReadString = ""; // Resetta per il prossimo input
          // Potresti voler ri-promuovere l'input qui
          Serial.println("Inserisci un numero (0-999) e premi Invio:");
        }
      }
    }

    // --- Gestione input da Bluetooth SoftwareSerial ---
    while (bluetoothSerial.available()) {
      char inChar = (char)bluetoothSerial.read();
      bluetoothReadString += inChar;

      // Se il carattere è un newline (Invio), processa la stringa
      if (inChar == '\n') {
        int num = validateAndConvert(bluetoothReadString);
        if (num != -1) { // -1 indica un numero non valido
          Serial.println("Numero ricevuto da Bluetooth.");
          bluetoothSerial.print("Ricevuto: ");
          bluetoothSerial.println(num);
          return num; // Numero valido, esci dalla funzione
        } else {
          Serial.print("Input non valido da Bluetooth: '");
          bluetoothReadString.trim();
          Serial.print(bluetoothReadString);
          Serial.println("'. Riprova.");
          bluetoothSerial.print("Input non valido. Riprova (0-999): ");
          bluetoothReadString = ""; // Resetta per il prossimo input
        }
      }
    }
  }
}

/*
 * Funzione helper per validare e convertire una stringa in un numero intero.
 * Controlla se la stringa contiene solo cifre e se il numero rientra nel range 0-999.
 *
 * Parametri:
 * inputString: La stringa da validare e convertire.
 *
 * Ritorna:
 * Il numero intero se valido, altrimenti -1.
 */
int validateAndConvert(String inputString) {
  // Rimuovi spazi bianchi, newline, carriage return all'inizio e alla fine
  inputString.trim();

  // Se la stringa è vuota dopo il trim, non è un numero valido
  if (inputString.length() == 0) {
    return -1;
  }

  // Controlla che tutti i caratteri siano cifre
  for (unsigned int i = 0; i < inputString.length(); i++) {
    if (!isDigit(inputString.charAt(i))) {
      return -1; // Contiene caratteri non numerici
    }
  }

  // Converti la stringa in un intero
  int num = inputString.toInt();

  // Controlla se il numero rientra nel range desiderato
  if (num >= 0 && num <= 999) {
    return num; // Numero valido
  } else {
    return -1; // Numero fuori range
  }
}