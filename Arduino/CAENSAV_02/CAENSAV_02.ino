String Versione = "CAENSAV_02";
/*
Al reset si accende semaforo rosso
Quando lampeggia velocemente il semaforo giallo è nel Menù
Quando lampeggia una volta al secondo è in RUN normale


// 📝 LEGENDA EMOJI PER COMMENTI ARDUINO

// 📦 Librerie e inclusioni
// 🧰 Setup iniziale
// ⚙️  Configurazione pin o periferiche
// 🔁 Loop principale
// 🧪 Test e diagnostica
// 🐛 Sezione debug
// 📡 Comunicazione seriale
// 🔋 Gestione batteria o alimentazione
// 🔌 Connessioni o relè
// 💡 LED o segnalazioni visive
// 🎛️ Controlli analogici (potenziometri, trimmer)
// ⏱️  Timer o millis()
// 🛑 Condizione di arresto o errore grave
// ⚠️  Avviso o soglia superata
// ✅ Azione riuscita / stato attivo
// ❌ Azione fallita / stato disattivo
// 🚨 Allarme o intervento urgente
// 🔐 Sicurezza o autorizzazione
// 📊 Lettura sensori
// 📈 Logica condizionale
// 🧮 Calcoli o conversioni
// 📲 Output su display o seriale
// 🗃️  EEPROM o memorizzazione



/*
📁 Sezioni o titoli

    📝 Note / Appunti

    📦 Pacchetti / Librerie

    📁 Cartelle / File

    📋 Lista

    📌 Punto importante

    🔖 Etichetta

    🧭 Navigazione

⚙️ Stato ed esiti

    ✅ Successo / OK

    ❌ Errore / Fallimento

    ⚠️ Attenzione

    🛑 Fermare qualcosa

    🔁 Ripetizione / Loop

    🔄 Aggiornamento

    ⏳ In attesa

    ⏹️ Fermato

    🔔 Notifica

🧪 Debug e testing

    🧪 Test

    🐛 Bug

    🔍 Analisi / Verifica

    🐢 Lento

    🚀 Veloce / Ottimizzato

🛠 Strumenti e sviluppo

    🛠️ Attrezzi

    🔧 Configurazione

    🧰 Kit strumenti

    🧵 Thread

    🧮 Calcoli

🔌 Hardware e connessioni

    🔋 Batteria

    🔌 Connessione

    💡 LED / Idea

    📟 Display

    📡 Wireless

    🛰️ Comunicazione

    🎛️ Controllo

📡 Serial Monitor e comunicazione

    📡 Trasmissione

    📲 Seriale

    🖧 Rete

    🔐 Protezione


Watchdog hardware

Rilevamento reset da watchdog

Contatore dei reset salvato in EEPROM (2 byte)

Beep di notifica al reset da watchdog

Controllo tensione batteria ogni 60s

Blink LED ogni 1s

Comandi seriali:

    z → azzera contatore

    r → mostra contatore

Stampa automatica del contatore ogni 60 secondi

*/


#include <avr/wdt.h>
#include <avr/io.h>
#include <EEPROM.h>
bool Silenzioso = true;
// Pin definitions
const int RELAY_PIN = 6;
const int LED_PIN = 3;
const int BUZZER_PIN = 2;

const int LED_SEM_VERDE_PIN = 7;
const int LED_SEM_GIALLO_PIN = 13;
const int LED_SEM_ROSSO_PIN = 4;



const int TRIMMER_SOGLIA_ALTA_PIN = A0;
const int TRIMMER_SOGLIA_BASSA_PIN = A1;

int VBAT_SERVIZI_PIN = A3;
int VBAT_MOTORE_PIN = A2;




int ContaSecondiVerificaBatterie=0;
bool Test_a_banco = true; //inverte vbat servizi e motore
bool debug= false;

// Timer variables
unsigned long previousMillis = 0;
//unsigned long previous60sMillis = 0;
const unsigned long interval1s = 1000;
int SecondiVerificaBatterie = 60;

bool VerificaBatterieEseguita = false;

//OROLOGIO
unsigned int Secondi = 0;
unsigned int Minuti = 0;
unsigned int Ore = 0;
unsigned int Giorni = 0;


// Partitore valori
const float R1 = 10000.0;
const float R2 = 3300.0;
const float VREF = 5.0;

int adc_vbat_servizi_bit; // = analogRead(VBAT_SERVIZI_PIN);
int adc_vbat_motore_bit; // = analogRead(VBAT_MOTORE_PIN);
int trimmer_soglia_bassa_bit; // = analogRead(TRIMMER_SOGLIA_BASSA_PIN);
int trimmer_soglia_alta_bit; // = analogRead(TRIMMER_SOGLIA_ALTA_PIN);
float trimmer_soglia_bassa_volt; // = trimmer_soglia_bassa_bit * VREF / 1023.0 ;
float trimmer_soglia_alta_volt; // = trimmer_soglia_alta_bit * VREF / 1023.0 ;
float vbat_servizi_volt; // = adc_vbat_servizi_bit * VREF / 1023.0 * (R1 + R2) / R2;
float vbat_motore_volt; // = adc_vbat_servizi_bit * VREF / 1023.0 * (R1 + R2) / R2;




// EEPROM addresses for 2-byte counter
const int EEPROM_ADDR_LOW = 0;
const int EEPROM_ADDR_HIGH = 1;

bool Relay_Status = false;
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
bool ExitFromMenu = false;
char Cestino;
int ledState = LOW;             // ledState used to set the LED
const int ledPin =  LED_BUILTIN;// the number of the LED pin
const long interval = 250;           // interval at which to blink (milliseconds)
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
/*
verde led d1 port D7
giallo led d3 port D13
verde led d5 port D4
*/

void SemaforoVerdeOn(){
  digitalWrite(LED_SEM_VERDE_PIN, HIGH);
}

void SemaforoVerdeOff(){
  digitalWrite(LED_SEM_VERDE_PIN, LOW);
}

  
void SemaforoGialloOn(){
  digitalWrite(LED_SEM_GIALLO_PIN, HIGH);
}

void  SemaforoGialloOff(){
  digitalWrite(LED_SEM_GIALLO_PIN, LOW);
}

void SemaforoRossoOn(){
  digitalWrite(LED_SEM_ROSSO_PIN,HIGH);
}

void SemaforoRossoOff(){
  digitalWrite(LED_SEM_ROSSO_PIN, LOW);
}





//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void Action1(){
  SemaforoGialloOff();
  SemaforoRossoOff();
  SemaforoVerdeOff();
  delay(1000);

  SemaforoVerdeOn();
  delay(1000);
  SemaforoVerdeOff();
  delay(1000);
  
  SemaforoGialloOn();
  delay(1000);
  SemaforoGialloOff();
  delay(1000);
  
  SemaforoRossoOn();
  delay(1000);
  SemaforoRossoOff();
  delay(1000);
  
  Serial.println(F("Test semaforo eseguito"));
  /*
  ex test azzera wdog
  EEPROM.write(EEPROM_ADDR_LOW, 0);
  EEPROM.write(EEPROM_ADDR_HIGH, 0);
  Serial.println("Contatore reset watchdog azzerato.");
  */
}
//---------------------
void Action2(){
  digitalWrite(RELAY_PIN, LOW);
  delay(1000);
  digitalWrite(RELAY_PIN, HIGH);
  delay(1000);
  digitalWrite(RELAY_PIN, LOW);
  Serial.println(F("Test Relay eseguito"));

  /*
  ex era leggi wdog
  uint16_t wdResetCount = EEPROM.read(EEPROM_ADDR_LOW);
  wdResetCount |= (uint16_t)EEPROM.read(EEPROM_ADDR_HIGH) << 8;
  Serial.print("Contatore reset watchdog attuale: ");
  Serial.println(wdResetCount);
  */
}
//---------------------

void Action3(){

  wdt_reset(); // Resetta watchdog
  wdt_enable(WDTO_2S); // Riattiva watchdog

}
//---------------------

void Action4(){

  

  while (true) {


    Acquisisce_e_Stampa_Analogiche();
    Serial.println(F("🔷 Premere 'Q' per uscire"));
    delay(250);
    
    while (Serial.available()) {
      
      char c = Serial.read();

      // Ignora fine riga CR o LF
      if (c == '\r' || c == '\n') continue;

      // Accetta q o Q per uscire
      if (c == 'q' || c == 'Q') {
        Serial.println("⏹️ Ricevuto 'q' o 'Q', esco dal ciclo");
        return;
      }

      // Opzionale: notifica caratteri non validi
      Serial.print("❌ Ignorato: ");
      Serial.println(c);
    }
  }
}    
    
//---------------------

void Action5(){

}
//---------------------

void Action6(){

}
//---------------------

void Action7(){

}
//---------------------

void Action8(){

}
//---------------------

void Action9(){

}
//---------------------

void Action10(){

}
//---------------------
//action n. 99
void Action99(){
  Serial.println("Action no. 99 selected");
  delay(2000);
  Serial.println("Returning to loop'");
  delay(2000);
  ExitFromMenu = true;  
}
//---------------------


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void(* resetFunc) (void) = 0; //declare reset function @ address 0
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

void Menu() {
  wdt_disable(); // Disattiva temporaneamente il watchdog
  //stay in the menu until you press 0
  while (!ExitFromMenu) {

    TypeMenuList();

    //clear the buffer
    while (Serial.available()) {
      Cestino = Serial.read();
    }

    Serial.println(" ");

    // here the waiting and LED blinking loop until there is a character on the serial
    while (!Serial.available()) {
      
      //--------------------------------------
      // FAST BLINK MANAGEMENT TO SHOW THAT THE MENU IS WAITING FOR COMMANDS
      unsigned long currentMillis = millis();
      if (currentMillis - previousMillis >= interval/4) {
        // save the last time you blinked the LED
        previousMillis = currentMillis;

        // if the LED is off turn it on and vice-versa:
        if (ledState == LOW) {
          ledState = HIGH;
        } else {
          ledState = LOW;
        }

        // set the LED with the ledState of the variable:
        digitalWrite(ledPin, ledState);
      }     
    }

    // a character has arrived
    int CmdMenu = Serial.parseInt();
    Serial.print("CmdMenu received ");Serial.println(CmdMenu);

  

    //process the received command 
	  switch (CmdMenu){

      //-------------------------------------------------
      case 0:
        Serial.println("resetting in 1 sec");
        delay(1000);               // wait for a second
        resetFunc();  //call reset

        delay(100);
        Serial.println("Reset did not work");
      break;      //-------------------------------------------------
      case 1:
        Action1();
      
      break;
      //-------------------------------------------------
      case 2:
        Action2();

      break;
      //-------------------------------------------------
      case 3:
        Action3();

      break;
      //-------------------------------------------------
      case 4:
        Action4();

      break;
      //-------------------------------------------------
      case 5:
        Action5();
      break;
      //-------------------------------------------------
      case 6:
        Action6();
      break;
      //-------------------------------------------------
      case 7:
        Action7();
      break;
      //-------------------------------------------------
      case 8:
        Action8();
      break;
      //-------------------------------------------------
      case 9:
        Action9();
      break;
      //-------------------------------------------------
      case 10:
        Action10();
      break;
      //-------------------------------------------------
      case 99:
        Action99();
      break;
      //-------------------------------------------------
      default:
        Serial.println("Invalid command. Try again!");
    } //Serial.available
  } // ExitFromMenu
} // Menu


   //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//*******************************************************

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop() runs, so using delay inside loop can
  delay response. Multiple bytes of data may be available.
*/
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
//print the list of available commands
void TypeMenuList(void){
    debug=false;
    Serial.println();
    Serial.println(F("|************************|"));
    Serial.println(F("|  ʘ‿ʘ   Menu   (◡_◡)   |"));
      Serial.print(F("|  Ver. "));Serial.println(Versione);
    Serial.println(F("|************************|"));
    Serial.println(F("  0 Reset"));
    Serial.println(F(" 1 Test Semaforo"));
    Serial.println(F(" 2 Test Relè"));
    Serial.println(F(" 3 Attiva Wdog senza resettarlo.. Farà reset"));
    Serial.println(F(" 4 Lettura continua Trimmer"));
    Serial.println(F(" 5 Action 5"));
    Serial.println(F(" 6 Action 6"));
    Serial.println(F(" 7 Action 7"));
    Serial.println(F(" 8 Action 8"));
    Serial.println(F(" 9 Action 9"));
    Serial.println(F(" 10 Action10 "));
    Serial.println(F(" 11 Action 11"));
    Serial.println(F(" 99 Return to loop without reset"));
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


void AggiornaOrologio(){
  Secondi = Secondi+1;

  if (Secondi==60){
    Secondi = 0;
    Minuti = Minuti+1;
  }

  if (Minuti==60){
    Minuti=0;
    Ore = Ore+1;
  }

  if (Ore==24){
    Ore = 0;
    Giorni=Giorni+1;
  }
}

//--------------orologio
void StampaOrologio(){
  
  char buffer[24];  
  // %u = unsigned int; %02u = due cifre, zero-pad
  sprintf(buffer, "⏳ Timer GG:HH:MM:SS %u:%02u:%02u:%02u", 
          Giorni, 
          Ore, 
          Minuti, 
          Secondi);

  Serial.println(buffer);
}



//*******************************************************************

void StampaStato(){
  Serial.print(F(" Le dua batterie sono "));
  if (Relay_Status){
    Serial.println(F(" CONNESSE ⚠️"));
  } else {
    Serial.println(F(" NON CONNESSE ❌"));
  }
}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void EseguiVerificaBatterie(){
  VerificaBatterieEseguita = true;

  if (adc_vbat_servizi_bit < trimmer_soglia_bassa_bit) {
      Relay_Status = false;
      digitalWrite(RELAY_PIN, LOW);
      Serial.println("VBAT sotto soglia bassa → relè disattivato");
      beep(2);
  } else if (adc_vbat_servizi_bit > trimmer_soglia_alta_bit) {
      Relay_Status = true;
      digitalWrite(RELAY_PIN, HIGH);
      Serial.println("VBAT sopra soglia alta → relè attivato");
      beep(3);
  } else {
      Serial.println("VBAT nella zona neutra → nessuna azione");
  }
}
   

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx





void setup() {
if (Test_a_banco){
  int VBAT_SERVIZI_PIN = A3;
  int VBAT_MOTORE_PIN = A2;
} else {
 int VBAT_SERVIZI_PIN = A2;
 int VBAT_MOTORE_PIN = A3;
}  


if (Test_a_banco){
  SecondiVerificaBatterie = 2; // ogni 2 secondi
}
  
  // Verifica causa del reset
  byte resetCause = MCUSR;
  MCUSR = 0;
  wdt_disable(); // Disattiva temporaneamente il watchdog

  Serial.begin(115200);
  Serial.println(F("RESET"));
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);


  pinMode(LED_SEM_VERDE_PIN, OUTPUT);
  pinMode(LED_SEM_GIALLO_PIN, OUTPUT);
  pinMode(LED_SEM_ROSSO_PIN, OUTPUT);  

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(LED_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  digitalWrite(LED_SEM_ROSSO_PIN, HIGH);
  digitalWrite(LED_SEM_GIALLO_PIN, LOW);
  digitalWrite(LED_SEM_VERDE_PIN, LOW);



  beep(1);

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

  //batterie da testare?
  ContaSecondiVerificaBatterie = ContaSecondiVerificaBatterie +1;
  if (ContaSecondiVerificaBatterie > SecondiVerificaBatterie){
    ContaSecondiVerificaBatterie = 0;

    EseguiVerificaBatterie();
    VerificaBatterieEseguita = true;
  }


  // Blink LED ogni secondo
  if (currentMillis - previousMillis >= interval1s) {
    previousMillis = currentMillis;
    digitalWrite(LED_SEM_GIALLO_PIN, !digitalRead(LED_SEM_GIALLO_PIN));

    Acquisisce_e_Stampa_Analogiche();


    StampaStato();
    AggiornaOrologio();
    StampaOrologio();

  }

  // print the string when a newline arrives:
  if (stringComplete) {
    inputString.trim();
    Serial.println(inputString);
    //parse commands
    if ( (inputString.substring(0, 4) ) == "Menu" ) {
      Menu();
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }


}

//*--------------------------------------
void Acquisisce_e_Stampa_Analogiche(){
    adc_vbat_servizi_bit = analogRead(VBAT_SERVIZI_PIN);
    adc_vbat_motore_bit = analogRead(VBAT_MOTORE_PIN);
    trimmer_soglia_bassa_bit = analogRead(TRIMMER_SOGLIA_BASSA_PIN);
    trimmer_soglia_alta_bit = analogRead(TRIMMER_SOGLIA_ALTA_PIN);

  
    trimmer_soglia_bassa_volt = trimmer_soglia_bassa_bit * VREF / 1023.0 * (R1 + R2) / R2;
    trimmer_soglia_alta_volt = trimmer_soglia_alta_bit * VREF / 1023.0 * (R1 + R2) / R2;
    
    vbat_servizi_volt = adc_vbat_servizi_bit * VREF / 1023.0 * (R1 + R2) / R2;
    vbat_motore_volt = adc_vbat_motore_bit * VREF / 1023.0 * (R1 + R2) / R2;
    
    Serial.println(F("--------------------------"));
    Serial.print(F("⚙️ Trimmer soglia alta "));Serial.print(trimmer_soglia_alta_bit);Serial.print(F(" BIT; "));;Serial.print(trimmer_soglia_alta_volt);Serial.println(F(" VOLT; "));
    Serial.print(F("⚙️ Trimmer soglia bassa "));Serial.print(trimmer_soglia_bassa_bit);Serial.print(F(" BIT; "));;Serial.print(trimmer_soglia_bassa_volt);Serial.println(F(" VOLT; "));
    Serial.print(F("🔋 Batteria servizi "));Serial.print(adc_vbat_servizi_bit);Serial.print(F(" BIT; "));;Serial.print(vbat_servizi_volt);Serial.println(F(" VOLT; "));
    Serial.print(F("🚀 Batteria motore "));Serial.print(adc_vbat_motore_bit);Serial.print(F(" BIT; "));;Serial.print(vbat_motore_volt);Serial.println(F(" VOLT; "));
    
}     


//*******************************************************************


void beep(int count) {
  for (int i = 0; i < count; i++) {
    if (Silenzioso){
      Serial.print(". ");
    } else {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);
      delay(100);
    }
  }
}
