String Versione = "Camp_Energ_Sav_11";
/*
Al reset si accende semaforo rosso
Quando lampeggia velocemente il semaforo giallo è nel Menù
Quando lampeggia una volta al secondo è in RUN normale

due beep tornato ai servizi
tre bip passato al morore

bluetoot lampeggia: attende connessione
bluetooth fisso connesso

jumper D4 sinistro = rele ogni secondo, se inseriro, oppure ogni 10 secondi
jumper d2 se inserito stampa normale se inserito, oppure stampa anche adc per debug

******************************************
PROCEDURA DI CALIBRAZIONE
nota: i trimmer sono usati per calibrare e poi torneranno a fare la funzione di soglia alta e soglia bassa
- Entra nel Menu
- lancia 6 Calibrazione
- Leggere con un teste preciso la tensione Servizi
- Regolare il trimmer di destra RV1 in modo che La tensione letta su Calib SRV sia uguale alla tensione reale
- Ripetere per tensione motore
- Salvare uscendo con Q (maiuscolo)
- Resettare con 0
- A questo punto occorre tarare i due trimmer per la soglia desiderata 
  Trimmer RV2 sinistra = soglia bassa
  Trimmer RV1 destra = soglia alta


HARDDWARE MAK_NANO2RL-107

arduino
D0  RISERVATO USB
D1  RISERVATO USB
D2  BUZZER
D3  LED D4 JUMPER AZIONA OGNI SECONDO (SE INSERITO)
D4  LED D5 SEMAFORO ROSSO
D5  
D6  RELAY
D7  LED D1 SEMAFORO VERDE
D8  
D9  
D10 LED D2 JUMPER STAMPA DETTAGLIATA (SE TOLTO)
D11 BLUETOOTH
D12 BLUETOOTH
D13 LED D3 SEMAFORO GIALLO
A0  
A1  
A2  
A3  
A4  
A5  
A6  
A7  





*/







#include <avr/wdt.h>
#include <avr/io.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
SoftwareSerial bluetooth(12, 11); // RX, TX
bool Beep_Status = false; //beep al cmbio status
bool Silenzioso = false;
// Pin definitions
const int RELAY_PIN = 6;

const int BUZZER_PIN = 2;

const int JUMPER_LEDD4 = 3; //relè ogni secondo se inserito
const int JUMPER_LEDD2 = 10; //stampa estesa se inserito


const int LED_SEM_VERDE_PIN = 7;
const int LED_SEM_GIALLO_PIN = 13;
const int LED_SEM_ROSSO_PIN = 4;



const int TRIMMER_SOGLIA_ALTA_PIN = A0;
const int TRIMMER_SOGLIA_BASSA_PIN = A1;

int ADC_BAT_SERVIZI_PIN = A2;
int ADC_BAT_MOTORE_PIN = A3;






bool Test_a_banco = false; //inverte vbat servizi e motore
bool debug= false; // 


// Timer variables
unsigned long previousMillis = 0;
//unsigned long previous60sMillis = 0;
const unsigned long interval1s = 1000;



//OROLOGIO
unsigned int Secondi = 0;
unsigned int Minuti = 0;
unsigned int Ore = 0;
unsigned int Giorni = 0;

unsigned int ContaSecondi = 0; //mai azzerato
unsigned int SecondiVerificaBatterie = 10;       // Intervallo modificabile
unsigned int lastEsecuzione = 0;           // Ultimo valore di Secondi usato per chiamare la funzione


// Coefficienti di calibrazione (caricati da EEPROM)
float Calibrazione_servizi;
float Calibrazione_motore;

// Indirizzi EEPROM
const int EEPROM_ADDR_volt_calibrata_srv = 0;
const int EEPROM_ADDR_volt_calibrata_mot = EEPROM_ADDR_volt_calibrata_srv + sizeof(float);


// Partitore valori
const float R1 = 10000.0;
const float R2 = 3300.0;
const float VREF = 5.0;

int adc_servizi_bit; // = analogRead(ADC_BAT_SERVIZI_PIN);
int adc_motore_bit; // = analogRead(ADC_BAT_MOTORE_PIN);
int trimmer_soglia_bassa_bit; // = analogRead(TRIMMER_SOGLIA_BASSA_PIN);
int trimmer_soglia_alta_bit; // = analogRead(TRIMMER_SOGLIA_ALTA_PIN);
float trimmer_soglia_bassa_volt; // = trimmer_soglia_bassa_bit * VREF / 1023.0 ;
float trimmer_soglia_alta_volt; // = trimmer_soglia_alta_bit * VREF / 1023.0 ;
float batt_servizi_volt; // = adc_servizi_bit * VREF / 1023.0 * (R1 + R2) / R2;
float batt_motore_volt; // = adc_servizi_bit * VREF / 1023.0 * (R1 + R2) / R2;



/*
contatore wdog in eeprom eliminato, non funziona, viene azzerato dal boot di arduino
// EEPROM addresses for 2-byte counter
const int EEPROM_ADDR_LOW = 0;
const int EEPROM_ADDR_HIGH = 1;
*/

bool Relay_Status = false;
String inputString = "";         // a String to hold incoming data
bool stringComplete = false;  // whether the string is complete
bool ExitFromMenu = false;
int Cestino;
int ledState = LOW;             // ledState used to set the LED
const int ledPin =  LED_BUILTIN;// the number of the LED pin
const long interval = 250;           // interval at which to blink (milliseconds)



//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx

// Funzione per mappare da intero a float
float mapFloat(long x, long in_min, long in_max, float out_min, float out_max) {
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
// Procedura di calibrazione da trimmer
void Calibrazione() {
  Serialprint2ln(F("== Modalità Calibrazione ==\nRegola i trimmer. Premi 'Q' per salvare e uscire."));

    // uso temporaneamente i trimmer soglie per calibrare
    int adc_trimmer_srv;// = analogRead(TRIMMER_SOGLIA_ALTA_PIN);
    int adc_trimmer_mot;// = analogRead(TRIMMER_SOGLIA_BASSA_PIN);

    //float volt_trimmer_srv;// = adc_trimmer_srv * VREF / 1023.0;
    //float volt_trimmer_mot;// = adc_trimmer_mot * VREF / 1023.0;

    // Lettura batterie
    int adc_servizi;// = analogRead(ADC_BAT_SERVIZI_PIN);
    int adc_motore;//  = analogRead(ADC_BAT_MOTORE_PIN);

    float volt_misurata_srv;// = adc_servizi * VREF / 1023.0 * (R1 + R2) / R2;
    float volt_misurata_mot;// = adc_motore  * VREF / 1023.0 * (R1 + R2) / R2;

    // Calcolo nuovi coefficienti (in tempo reale)
    float volt_calibrata_srv;// = volt_trimmer_srv / volt_misurata_srv;
    float volt_calibrata_mot;// = volt_trimmer_mot / volt_misurata_mot;



  while (true) {
    // Controllo uscita
    if (Serial.available()) {
      int c = SerialRead2();
      if (c == 'Q') break;
    }



    adc_trimmer_srv = analogRead(TRIMMER_SOGLIA_ALTA_PIN);
    adc_trimmer_mot = analogRead(TRIMMER_SOGLIA_BASSA_PIN);

    //volt_trimmer_srv = adc_trimmer_srv * VREF / 1023.0;
    //volt_trimmer_mot = adc_trimmer_mot * VREF / 1023.0;

    // Lettura batterie
    adc_servizi = analogRead(ADC_BAT_SERVIZI_PIN);
    adc_motore  = analogRead(ADC_BAT_MOTORE_PIN);

    volt_misurata_srv = adc_servizi * VREF / 1023.0 * (R1 + R2) / R2;
    volt_misurata_mot = adc_motore  * VREF / 1023.0 * (R1 + R2) / R2;


    // Mappa il valore da 0-1023 a 0.8-1.2
    Calibrazione_servizi = mapFloat(adc_trimmer_srv, 0, 1023, 0.8, 1.2);
    Calibrazione_motore = mapFloat(adc_trimmer_mot, 0, 1023, 0.8, 1.2);

    //vbat calibrate
    volt_calibrata_srv = volt_misurata_srv * Calibrazione_servizi;
    volt_calibrata_mot = volt_misurata_mot * Calibrazione_motore;

    // Stampa valori
    Serialprint2(F("Calib_serv: "));
    Serialprint2(Calibrazione_servizi);
    //Serialprint2(F(" | Trimmer SRV: "));
    //Serialprint2(volt_trimmer_srv, 3);
    Serialprint2(F(" | Batt SRV: "));
    Serialprint2(volt_misurata_srv, 3);
    Serialprint2(F(" V | Calib SRV: "));
    Serialprint2(volt_calibrata_srv, 4);

    Serialprint2(F(" || Calib_moto: "));
    Serialprint2(Calibrazione_motore);
    //Serialprint2(F(" | Trimmer MOT: "));
    //Serialprint2(volt_trimmer_mot, 3);
    Serialprint2(F(" | Batt MOT: "));
    Serialprint2(volt_misurata_mot, 3);
    Serialprint2(F(" V | Calib MOT: "));
    Serialprint2(volt_calibrata_mot, 4);
    Serialprint2ln("");
    

    delay(1000);
  }

  // Salvataggio in EEPROM

  EEPROM_writeFloat(EEPROM_ADDR_volt_calibrata_srv, Calibrazione_servizi);
  EEPROM_writeFloat(EEPROM_ADDR_volt_calibrata_mot, Calibrazione_motore);

  Serialprint2ln(F("Calibrazione completata e salvata in EEPROM."));
  beep(2);
}








//-------------------------------------------------------------





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
  
  Serialprint2ln(F("Test semaforo eseguito"));
  /*
  ex test azzera wdog
  EEPROM.write(EEPROM_ADDR_LOW, 0);
  EEPROM.write(EEPROM_ADDR_HIGH, 0);
  Serialprint2ln("Contatore reset watchdog azzerato.");
  */
}
//---------------------
void Action2(){
  digitalWrite(RELAY_PIN, LOW);
  delay(1000);
  digitalWrite(RELAY_PIN, HIGH);
  delay(1000);
  digitalWrite(RELAY_PIN, LOW);
  Serialprint2ln(F("Test Relay eseguito"));

  /*
  ex era leggi wdog
  uint16_t wdResetCount = EEPROM.read(EEPROM_ADDR_LOW);
  wdResetCount |= (uint16_t)EEPROM.read(EEPROM_ADDR_HIGH) << 8;
  Serialprint2("Contatore reset watchdog attuale: ");
  Serialprint2ln(wdResetCount);
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
    Serialprint2ln(F("🔷 Premere 'Q' per uscire"));
    delay(250);
    
    while (Serial.available()) {
      
      int c = SerialRead2();

      // Ignora fine riga CR o LF
      if (c == '\r' || c == '\n') continue;

      // Accetta q o Q per uscire
      if (c == 'q' || c == 'Q') {
        Serialprint2ln("⏹️ Ricevuto 'q' o 'Q', esco dal ciclo");
        return;
      }

      // Opzionale: notifica caratteri non validi
      Serialprint2("❌ Ignorato: ");
      Serialprint2ln(c);
    }
  }
}    
    
//---------------------

void Action5(){
  int stato ;

  stato = digitalRead(JUMPER_LEDD4);
  // Se il pulsante è premuto (pin collegato a GND), stato sarà LOW
  Serialprint2(F("Jumper D4 - "));
  if (stato == LOW) {
    Serialprint2ln(F("INSERITO azione relay ogni secondo"));
  } else {
    Serialprint2ln(F("MANCANTE azione relay ogni 0 secondi"));
  }

  stato = digitalRead(JUMPER_LEDD2);
  // Se il pulsante è premuto (pin collegato a GND), stato sarà LOW
  Serialprint2(F("Jumper D2 - "));
  if (stato == LOW) {
    Serialprint2ln(F("INSERITO stampa breve"));
  } else {
    Serialprint2ln(F("MANCANTE stampa completa"));
  }



}
//---------------------

void Action6(){
  Calibrazione();
  Serialprint2ln(F("Calibrazione completata e salvata in EEPROM."));
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
  Serialprint2ln("Action no. 99 selected");
  delay(2000);
  Serialprint2ln("Returning to loop'");
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
      Cestino = SerialRead2();
    }

    Serialprint2ln(" ");

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
    Serialprint2("CmdMenu received ");Serialprint2ln(CmdMenu);

  

    //process the received command 
	  switch (CmdMenu){

      //-------------------------------------------------
      case 0:
        Serialprint2ln("resetting in 1 sec");
        delay(1000);               // wait for a second
        resetFunc();  //call reset

        delay(100);
        Serialprint2ln("Reset did not work");
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
        Serialprint2ln("Invalid command. Try again!");
    } //SerialRead2Available
  } // ExitFromMenu
} // Menu


   //XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX

//*******************************************************

/*
  SerialEvent occurs whenever a new data comes in the hardware serial RX. This
  routine is run between each time loop( ) runs, so using delay inside loop can
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
    //debug=false;
    Serialprint2ln("");
    Serialprint2ln(F("|************************|"));
    Serialprint2ln(F("|  ʘ‿ʘ   Menu   (◡_◡)   |"));
      Serialprint2(F("|  Ver. "));Serialprint2ln(Versione);
    Serialprint2ln(F("|************************|"));
    Serialprint2ln(F("  0 Reset"));
    Serialprint2ln(F(" 1 Test Semaforo"));
    Serialprint2ln(F(" 2 Test Relè"));
    Serialprint2ln(F(" 3 Attiva Wdog senza resettarlo.. Farà reset"));
    Serialprint2ln(F(" 4 Lettura continua Trimmer"));
    Serialprint2ln(F(" 5 Stato JUMPERS"));
    Serialprint2ln(F(" 6 Calibrazione"));
    /*
    Serialprint2ln(F(" 7 Action 7"));
    Serialprint2ln(F(" 8 Action 8"));
    Serialprint2ln(F(" 9 Action 9"));
    Serialprint2ln(F(" 10 Action10 "));
    Serialprint2ln(F(" 11 Action 11"));
    */
    Serialprint2ln(F(" 99 Return to loop without reset"));
}
//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


void AggiornaOrologio(){
  Secondi = Secondi+1;
  ContaSecondi = ContaSecondi+1;
  

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
  sprintf(buffer, "; ⏳ %u:%02u:%02u:%02u", 
          Giorni, 
          Ore, 
          Minuti, 
          Secondi);

  Serialprint2ln(buffer);
  bluetooth.println(buffer);
  
}



//*******************************************************************

void StampaStato(){

  Serialprint2(F(" Batterie "));
  bluetooth.print(F(" Batt. "));
  
  if (Relay_Status){
    Serialprint2(F(" CONNESSE 🛑🛑🛑"));

  } else {
    Serialprint2(F(" NON CONNESSE ✅✅✅"));
  }


}


//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx
void EseguiVerificaBatterie(){
  
  if (adc_servizi_bit < trimmer_soglia_bassa_bit) {
      //gestisce il beep una sola volta
      if (Beep_Status==true){
        Beep_Status = false;
        beep(2); //lo fa solo una volta ogni cambio status
      }
      //
      Relay_Status = false;      
      digitalWrite(RELAY_PIN, LOW);
      Serialprint2ln(F("VBAT serv. inf. soglia bassa → relè disattivato ✅"));

      //beep(2);
      SemaforoVerdeOn();
      SemaforoRossoOff();
      //

      


  } else if (adc_servizi_bit > trimmer_soglia_alta_bit) {
      //gestisce il beep una sola volta
      if (Beep_Status==false){
        Beep_Status = true;
        beep(3); //lo fa solo una volta ogni cambio status
      }
      //
      Relay_Status = true;
      Beep_Status=true;
      digitalWrite(RELAY_PIN, HIGH);
      Serialprint2ln(F("VBAT serv. super. soglia alta → relè attivato 🛑"));

      //beep(3);
      SemaforoVerdeOff();
      SemaforoRossoOn();      
  } else {
      Serialprint2ln("VBAT nella zona neutra → nessuna azione");
  }
}
   



//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx


 //Versione compatta e potente usando i template C++: 
 // ti permette di usare una sola funzione per stampare qualsiasi tipo di dato, sia su Serial 
 // che su bluetooth.

// Funzione template per print
template <typename T>
void Serialprint2(T dato) {
  Serial.print(dato);
  bluetooth.print(dato);
}

// Funzione template per println
template <typename T>
void Serialprint2ln(T dato) {
  Serial.println(dato);
  bluetooth.println(dato);
}


//per i print con due parametri ggiungi un overload specifico per float con precisione:

void Serialprint2(float val, int decimali) {
  Serial.print(val, decimali);
  bluetooth.print(val, decimali);
}


//-----------------
//funzione SerialRead2 che:
//    Controlla se c'è un carattere disponibile su Serial o bluetooth.
//   Lo legge e lo restituisce.
//    Oppure restituisce -1 se nessun carattere è presente.
int SerialRead2(){
  if (Serial.available()) {
    return Serial.read();
  }
  if (bluetooth.available()) {
    return bluetooth.read();
  }
  return -1; // Nessun dato disponibile
}

/*
bool SerialRead2Available() {
  return Serial.available();
}
*/

//xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx





void setup() {
if (Test_a_banco){
  ADC_BAT_SERVIZI_PIN = A3;
  ADC_BAT_MOTORE_PIN = A2;
} else {
 ADC_BAT_SERVIZI_PIN = A2;
 ADC_BAT_MOTORE_PIN = A3;
}  



  
  // Verifica causa del reset
  byte resetCause = MCUSR;
  MCUSR = 0;
  wdt_disable(); // Disattiva temporaneamente il watchdog

  Serial.begin(115200);
  bluetooth.begin(9600);
  Serialprint2ln(F("RESET"));
  Serialprint2(F("Nome file: "));
  Serialprint2ln(__FILE__);

  // Lettura valori di calibrazione da EEPROM
  Calibrazione_servizi = EEPROM_readFloat(EEPROM_ADDR_volt_calibrata_srv);
  Calibrazione_motore  = EEPROM_readFloat(EEPROM_ADDR_volt_calibrata_mot);

  // Se EEPROM è "vuota", valori default
  if (isnan(Calibrazione_servizi)) Calibrazione_servizi = 1.0;
  if (isnan(Calibrazione_motore))  Calibrazione_motore = 1.0;

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  pinMode(JUMPER_LEDD2, INPUT_PULLUP);
  pinMode(JUMPER_LEDD4, INPUT_PULLUP);


  pinMode(LED_SEM_VERDE_PIN, OUTPUT);
  pinMode(LED_SEM_GIALLO_PIN, OUTPUT);
  pinMode(LED_SEM_ROSSO_PIN, OUTPUT);  

  digitalWrite(RELAY_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);

  digitalWrite(LED_SEM_ROSSO_PIN, HIGH);
  delay(200);
  digitalWrite(LED_SEM_GIALLO_PIN, HIGH);
  delay(200);
  digitalWrite(LED_SEM_VERDE_PIN, HIGH);
  delay(200);
  beep(1);
  digitalWrite(LED_SEM_ROSSO_PIN, LOW);
  delay(200);
  digitalWrite(LED_SEM_GIALLO_PIN, LOW);
  delay(200);
  digitalWrite(LED_SEM_VERDE_PIN, LOW);
  delay(200);
  beep(1);

  /*
contatore wdog in eeprom eliminato, non funziona, viene azzerato dal boot di arduino

  if (resetCause & _BV(WDRF)) {
    Serialprint2ln("⚠️ Reset da Watchdog!");

    // Leggi contatore 2 byte da EEPROM
    uint16_t wdResetCount = EEPROM.read(EEPROM_ADDR_LOW);
    wdResetCount |= (uint16_t)EEPROM.read(EEPROM_ADDR_HIGH) << 8;

    wdResetCount++; // Incrementa
    EEPROM.write(EEPROM_ADDR_LOW, wdResetCount & 0xFF);
    EEPROM.write(EEPROM_ADDR_HIGH, (wdResetCount >> 8) & 0xFF);

    Serialprint2("Contatore reset watchdog: ");
    Serialprint2ln(wdResetCount);

    beep(5); // Beep di avviso
  } else {
    Serialprint2ln("Sistema avviato normalmente.");
  }
  */

  wdt_enable(WDTO_2S); // Riattiva watchdog
  Acquisisce_e_Stampa_Analogiche();
  EseguiVerificaBatterie();

}

//*****************************end setup

void loop() {
  int statojp4 ;

  
  wdt_reset(); // Resetta watchdog

  unsigned long currentMillis = millis();



  //batterie da testare?
  statojp4 = digitalRead(JUMPER_LEDD4);
  // Se il pulsante è premuto (pin collegato a GND), stato sarà LOW
  //Serialprint2(F("Jumper D4 - "));
  if (statojp4 == LOW) {
    //Serialprint2ln(F("INSERITO azione relay ogni secondo"));
    if ((SecondiVerificaBatterie > 0) && (ContaSecondi % 1 == 0) && (ContaSecondi != 0)) {
  
    if (ContaSecondi != lastEsecuzione) {
      
      EseguiVerificaBatterie();
      lastEsecuzione = ContaSecondi;
    }
  }
  } else {
    //Serialprint2ln(F("MANCANTE azione relay ogni 10 secondi"));
    if ((SecondiVerificaBatterie > 0) && (ContaSecondi % SecondiVerificaBatterie == 0) && (ContaSecondi != 0)) {
  
    if (ContaSecondi != lastEsecuzione) {
      
      EseguiVerificaBatterie();
      lastEsecuzione = ContaSecondi;
    }
  }
  }






  // Blink LED ogni secondo
  if (currentMillis - previousMillis >= interval1s) {
    previousMillis = currentMillis;
    digitalWrite(LED_SEM_GIALLO_PIN, !digitalRead(LED_SEM_GIALLO_PIN));

    Acquisisce_e_Stampa_Analogiche();


    StampaStato();
    AggiornaOrologio();
    StampaOrologio();

    if (debug){
      Serialprint2(F("Debug attivo"));
      Serialprint2(F("; ContaSecondi=")); Serialprint2(ContaSecondi);
      Serialprint2(F("; SecondiVerificaBatterie=")); Serialprint2(SecondiVerificaBatterie);
      Serialprint2(F("; lastEsecuzione=")); Serialprint2(lastEsecuzione); 
      Serialprint2ln("");
    }

  }

  // print the string when a newline arrives:
  if (stringComplete) {
    inputString.trim();
    Serialprint2ln(inputString);
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
    adc_servizi_bit = analogRead(ADC_BAT_SERVIZI_PIN);
    adc_motore_bit = analogRead(ADC_BAT_MOTORE_PIN);
    trimmer_soglia_bassa_bit = analogRead(TRIMMER_SOGLIA_BASSA_PIN);
    trimmer_soglia_alta_bit = analogRead(TRIMMER_SOGLIA_ALTA_PIN);

  
    trimmer_soglia_bassa_volt = trimmer_soglia_bassa_bit * VREF / 1023.0 * (R1 + R2) / R2;
    trimmer_soglia_alta_volt = trimmer_soglia_alta_bit * VREF / 1023.0 * (R1 + R2) / R2;
    
    
    batt_servizi_volt = adc_servizi_bit * VREF / 1023.0 * (R1 + R2) / R2;
    batt_motore_volt = adc_motore_bit * VREF / 1023.0 * (R1 + R2) / R2;
    
    //debug2
    //Serialprint2(batt_servizi_volt); Serialprint2(" "); Serialprint2(Calibrazione_servizi);  Serialprint2(" ");  
    batt_servizi_volt = batt_servizi_volt * Calibrazione_servizi;
    //Serialprint2(batt_servizi_volt); Serialprint2(" ");

   //debug2
    //Serialprint2(batt_motore_volt); Serialprint2(" "); Serialprint2(Calibrazione_motore);  Serialprint2(" ");  
    batt_motore_volt = batt_motore_volt * Calibrazione_motore;
    //Serialprint2(batt_motore_volt); Serialprint2(" "); 


//    batt_servizi_volt = leggiTensione(ADC_BAT_SERVIZI_PIN, Calibrazione_servizi);
 //   batt_motore_volt  = leggiTensione(ADC_BAT_MOTORE_PIN, Calibrazione_motore);


  int stato2 ;
  stato2 = digitalRead(JUMPER_LEDD2);
  // Se il pulsante è premuto (pin collegato a GND), stato sarà LOW
  if (stato2 == LOW) {

    Serialprint2(F("⚙️ S_ALTA "));Serialprint2(trimmer_soglia_alta_volt);Serialprint2(F("; "));
    Serialprint2(F("⚙️ S_BASSA "));Serialprint2(trimmer_soglia_bassa_volt);Serialprint2(F("; "));
    Serialprint2(F("🔋 V_SERVIZ "));Serialprint2(batt_servizi_volt);Serialprint2(F("; "));
    Serialprint2(F("🚀 V_AVVIAM "));Serialprint2(batt_motore_volt);Serialprint2(F("; "));

  } else {

    Serialprint2ln(F("--------------------------"));
    Serialprint2(F("⚙️ Trimmer soglia alta "));Serialprint2(trimmer_soglia_alta_bit);Serialprint2(F(" BIT; "));Serialprint2(trimmer_soglia_alta_volt);Serialprint2ln(F(" VOLT; "));
    Serialprint2(F("⚙️ Trimmer soglia bassa "));Serialprint2(trimmer_soglia_bassa_bit);Serialprint2(F(" BIT; "));Serialprint2(trimmer_soglia_bassa_volt);Serialprint2ln(F(" VOLT; "));
    Serialprint2(F("🔋 Batteria servizi "));Serialprint2(adc_servizi_bit);Serialprint2(F(" BIT; "));Serialprint2(batt_servizi_volt);Serialprint2ln(F(" VOLT; "));
    Serialprint2(F("🚀 Batteria motore "));Serialprint2(adc_motore_bit);Serialprint2(F(" BIT; "));Serialprint2(batt_motore_volt);Serialprint2ln(F(" VOLT; "));

  }
    
}     


//*******************************************************************


void beep(int count) {
  for (int i = 0; i < count; i++) {
    if (Silenzioso){
      //Serialprint2(". ");
    } else {
      digitalWrite(BUZZER_PIN, HIGH);
      delay(100);
      digitalWrite(BUZZER_PIN, LOW);
      delay(100);
    }
  }
}






/*

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
*/
