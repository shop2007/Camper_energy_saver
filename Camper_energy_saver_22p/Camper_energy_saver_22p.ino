/*
14 aggiunto calibrazione da tastiera
15 aggiunto contatore di reset
16 aggiunto taratura soglie da tastiera
17 eliminato stampa continua su calibrazione e soglia ma solo a ogni comando
18 protezione, se non trova l'ultimo byte della eeprom a FE si resetta dopo un minuto
19 rimosso commenti multilinea ROTTI e creato funzione loop infinito nascosta
20 aggiunto finto array che non serve a niente

protezione: 
  - da menù 
  - comando 58 per scrivere 0xfe nell'ultima locazione eeprom write0xFEToLastEEPROM() protezione attiva; 
  - il comando 59 la legge verifyLastEEPROMValue();
  - comando 57 per scrivere 0xff nell'ultima locazione eeprom write0xFFToLastEEPROM(); test se protezione funziona
  - ad ogni incremento di ore del timer verifica, se non è 0xfe si blocca, funziona solo un'ora, (debug ogni minuto)


Al reset si accende semaforo rosso
Quando lampeggia velocemente il semaforo giallo è nel Menù
Quando lampeggia una volta al secondo è in RUN normale

due beep tornato ai servizi
tre bip passato al morore

bluetoot lampeggia: attende connessione
bluetooth fisso connesso

jumper D4 sinistro = rele ogni secondo, se inserito, oppure ogni 10 secondi
jumper d2 se inserito stampa normale se inserito, oppure stampa anche adc per debug

XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX
PROCEDURA DI CALIBRAZIONE



HARDWARE MAK_NANO2RL-107

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
#include <Arduino.h>
#include <avr/pgmspace.h> // COMMENT 
String Versione = "Camp_Energ_Sav_22p";
// COMMENT 

// COMMENT 
const unsigned char CODE_ARRAY[] PROGMEM = {
0xD6, 0x5D, 0xBE, 0x47, 0x3D, 0xA2, 0x93, 0x8C, 0x05, 0x8E, 0xDF, 0xF9, 0x36, 0xDA, 0x98, 0x72,
0xA8, 0xF5, 0x32, 0x0F, 0xAE, 0x01, 0x1B, 0xBF, 0x6E, 0x5F, 0x7B, 0x24, 0xB0, 0x50, 0xC4, 0x31,
0xE8, 0x2A, 0x5B, 0x1E, 0xFC, 0x1A, 0x25, 0x61, 0x00, 0x7A, 0xC0, 0x86, 0x8D, 0x20, 0xCD, 0xD4,
0x17, 0x62, 0x4B, 0x0B, 0xC2, 0x63, 0x7E, 0xAA, 0x55, 0x9C, 0xEB, 0x19, 0x4D, 0x8B, 0xCC, 0x12,
0x1F, 0x38, 0xF6, 0xCB, 0xEE, 0xA0, 0x54, 0xB7, 0xDD, 0x70, 0x0C, 0x97, 0xF3, 0x6B, 0x45, 0xEF,
0xBC, 0x28, 0x3B, 0x8F, 0x99, 0xFD, 0xC7, 0x90, 0x3C, 0x39, 0x4C, 0x81, 0x2C, 0x76, 0x67, 0x30,
0xCE, 0x92, 0x33, 0xD0, 0x53, 0x6C, 0x14, 0x41, 0x27, 0x09, 0x68, 0x40, 0xA9, 0x23, 0xBA, 0x2B,
0x8A, 0x48, 0x9A, 0x08, 0x88, 0xC5, 0xB5, 0x06, 0x95, 0x4E, 0x79, 0x71, 0x9B, 0x13, 0x7C, 0x46,
0xC9, 0x7F, 0x44, 0xA5, 0x59, 0x3F, 0xB9, 0xDB, 0x2E, 0x82, 0x7D, 0x75, 0xA7, 0xA1, 0xC1, 0x4A,
0xE9, 0x74, 0x60, 0x2F, 0x04, 0xD1, 0x3E, 0x83, 0xD8, 0xF8, 0x52, 0xD7, 0xC3, 0x02, 0x49, 0x84,
0x5A, 0xF1, 0xAF, 0xDC, 0xE7, 0xA4, 0x42, 0x87, 0x78, 0xB3, 0x91, 0x6A, 0xD2, 0xE0, 0x6F, 0x51,
0x22, 0x6D, 0x9D, 0xA3, 0x4F, 0xAC, 0xC8, 0x35, 0xE6, 0xBB, 0x15, 0x0E, 0x58, 0xE5, 0xFE, 0xB4,
0x66, 0xD9, 0x0A, 0x0D, 0xE4, 0x73, 0x2D, 0x64, 0xFB, 0x94, 0x89, 0x07, 0xBD, 0x34, 0xC6, 0xAD,
0x03, 0xB6, 0x21, 0xD5, 0x65, 0xA6, 0x10, 0x69, 0xE2, 0x80, 0x9F, 0xFA, 0x56, 0xDE, 0x1C, 0x11,
0xF7, 0xED, 0xB8, 0xE3, 0xEA, 0x5C, 0xBE, 0x43, 0x96, 0x29, 0xF2, 0xCA, 0xF4, 0x77, 0xBE, 0x16,
0x00, 0xCF, 0xE1, 0x3A, 0x9E, 0x5E, 0x26, 0xD3, 0xAB, 0xBA, 0x18, 0x4E, 0xDA, 0xBF, 0x1D, 0xFE
};

// COMMENT 
volatile unsigned int VAR001 = 0; 








/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */







#include <avr/wdt.h>
#include <avr/io.h>
#include <EEPROM.h>
#include <SoftwareSerial.h>
SoftwareSerial bluetooth(12, 11); // COMMENT 
bool VAR002 = false; // COMMENT 
bool VAR003 = false;
// COMMENT 
const int VAR004 = 6;

const int VAR005 = 2;

const int VAR006 = 3; // COMMENT 
const int VAR007 = 10; // COMMENT 


const int VAR008 = 7;
const int VAR009 = 13;
const int VAR010 = 4;

int VAR011 = 0;

// COMMENT 
// COMMENT 

int VAR012 = A2;
int VAR013 = A3;






bool VAR014 = false; // COMMENT 
bool VAR015= false; // COMMENT 


// COMMENT 
unsigned long VAR016 = 0;
// COMMENT 
const unsigned long VAR017 = 1000;



// COMMENT 
unsigned int VAR018 = 0;
unsigned int VAR019 = 0;
unsigned int VAR020 = 0;
unsigned int VAR021 = 0;

unsigned int VAR022 = 0; // COMMENT 
unsigned int VAR023 = 10;       // COMMENT 
unsigned int VAR024 = 0;           // COMMENT 


// COMMENT 
unsigned int VAR025 = 0; // COMMENT 

// COMMENT 
float VAR026;
float VAR027;

// COMMENT 
// COMMENT 
const int VAR028 = 0;
const int VAR029 = VAR028 + sizeof(float);
// COMMENT 
const int VAR030 = VAR029 + sizeof(float); 
// COMMENT 
// COMMENT 
const int VAR031 = VAR030 + sizeof(int);  // COMMENT 
const int VAR032  = VAR031 + sizeof(int); // COMMENT 
// COMMENT 




// COMMENT 
const float VAR033 = 10000.0;
const float VAR034 = 3300.0;
const float VAR035 = 5.0;

int VAR036; // COMMENT 
int VAR037; // COMMENT 
int VAR038; // COMMENT 
int VAR039; // COMMENT 
float VAR040; // COMMENT 
float VAR041; // COMMENT 
float VAR042; // COMMENT 
float VAR043; // COMMENT 



/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */

bool VAR046 = false;
String inputString = "";         // COMMENT 
bool VAR047 = false;  // COMMENT 
bool VAR048 = false;
int VAR049;
int VAR050 = LOW;             // COMMENT 
const int VAR051 =  LED_BUILTIN;// COMMENT 
const long VAR052 = 250;           // COMMENT 

const long VAR053 = 1000;
// COMMENT 

// COMMENT 
void FUN012(){
VAR026 = 1.001;
FUN015(VAR028, VAR026);
VAR027 = 0.999;
FUN015(VAR029, VAR027);
VAR038 = 670; // COMMENT 
EEPROM.put(VAR031, VAR038);
VAR039 = 695; // COMMENT 
EEPROM.put(VAR032,  VAR039);
VAR025 = 22;
EEPROM.put(VAR030, VAR025); 

}




// COMMENT 

// COMMENT 
float FUN037(long x, long in_min, long in_max, float out_min, float out_max) {
return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}



// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 

void FUN036(int startAddress, int numBytes) {
for (int i = 0; i < numBytes; i++) {
// COMMENT 
if (i % 16 == 0) {
if (i != 0) {
Serial.println(); // COMMENT 
}
// COMMENT 
if ((startAddress + i) < 16) Serial.print("000");
else if ((startAddress + i) < 256) Serial.print("00");
else if ((startAddress + i) < 4096) Serial.print("0");
Serial.print(startAddress + i, HEX);
Serial.print(" ");
}

// COMMENT 
byte data = EEPROM.read(startAddress + i);

// COMMENT 
if (data < 0x10) {
Serial.print("0");
}
Serial.print(data, HEX);
Serial.print(" ");
}
Serial.println(); // COMMENT 
}
// COMMENT 

void FUN013(){
VAR026 = FUN018(VAR028);
VAR027  = FUN018(VAR029);
EEPROM.get(VAR030, VAR025);
EEPROM.get(VAR031, VAR038);
EEPROM.get(VAR032,  VAR039);

FUN032("ADR Calbraz. Servizi: ");FUN032(VAR028);FUN032(" -> ");FUN027(VAR026);
FUN032("ADR Calbraz. Motore: ");FUN032(VAR029);FUN032(" -> ");FUN027(VAR027);
FUN032("ADR Counter Reset: ");FUN032(VAR030);FUN032(" -> ");FUN027(VAR025);
FUN032("ADR Soglia Bassa: ");FUN032(VAR031);FUN032(" -> ");FUN027(VAR038);
FUN032("ADR Soglia Alta: ");FUN032(VAR032);FUN032(" -> ");FUN027(VAR039);

// COMMENT 
FUN036(0, 64); // COMMENT 

}





// COMMENT 

/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */
void FUN002() {
FUN027(F("== Modalità Calibrazione Soglie (da tastiera) =="));
FUN027(F("Soglia BASSA: T-100/Y-10/U-1 I+1/O+10/P+100 bit"));
FUN027(F("Soglia ALTA : F-100/G-10/H-1 J+1/K+10/L+100 bit"));
FUN027(F("Premi 'Q' per salvare e uscire."));

// COMMENT 
// COMMENT 

float VAR040;
float VAR041;

while (true) {
// COMMENT 
VAR038 = constrain(VAR038, 0, 1023);
VAR039 = constrain(VAR039, 0, 1023);

VAR040 = VAR038 * VAR035 / 1023.0 * (VAR033 + VAR034) / VAR034;
VAR041  = VAR039  * VAR035 / 1023.0 * (VAR033 + VAR034) / VAR034;

// COMMENT 
FUN032(F("BASSA: "));
FUN032(VAR038);
FUN032(F("bit ("));
FUN032(VAR040, 2); // COMMENT 
FUN032(F(" V) | ALTA: "));
FUN032(VAR039);
FUN032(F("bit ("));
FUN032(VAR041, 2);
FUN032(F(" V) | Cmd> ")); // COMMENT 



// COMMENT 
char VAR054;
while (true) {  // COMMENT 
VAR054 = 0; // COMMENT 

if (Serial.available()) {
VAR054 = Serial.read();
} else if (bluetooth.available()) {
VAR054 = bluetooth.read();
}

// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
if (VAR054 != 0 && VAR054 != '\r' && VAR054 != '\n') {
break;  // COMMENT 
}

// COMMENT 
// COMMENT 
// COMMENT 
delay(10);
}
VAR054 = tolower(VAR054);

// COMMENT 
FUN027(VAR054);

// COMMENT 
if (VAR054 == 'q') {
break; // COMMENT 
}

switch (VAR054) {
// COMMENT 
case 't': VAR038 -= 100; break;
case 'y': VAR038 -= 10; break;
case 'u': VAR038 -= 1; break;
case 'i': VAR038 += 1; break;
case 'o': VAR038 += 10; break;
case 'p': VAR038 += 100; break;

// COMMENT 
case 'f': VAR039 -= 100; break;
case 'g': VAR039 -= 10; break;
case 'h': VAR039 -= 1; break;
case 'j': VAR039 += 1; break;
case 'k': VAR039 += 10; break;
case 'l': VAR039 += 100; break;

default:
FUN027(F("Comando non valido. Riprova."));
break;
}
// COMMENT 
}

// COMMENT 
EEPROM.put(VAR031, VAR038);
EEPROM.put(VAR032,  VAR039);

FUN027(F("\nCalibrazione completata. Soglie salvate in EEPROM."));
}
// COMMENT 


void FUN009() {
FUN027(F("== Modalità Calibrazione (da tastiera) =="));
FUN032(F("SERVIZI: T-100/Y-10/U-1 I+1/O+10/P+100 bit"));
FUN032(F(" || AVVIAMENTO : F-100/G-10/H-1 J+1/K+10/L+100 bit"));
FUN027(F(" || Premi 'Q' per salvare e uscire."));

/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */

// COMMENT 
// COMMENT 
// COMMENT 

int VAR055;
int VAR056;
float VAR057;
float VAR058;
float VAR059;
float VAR060;

// COMMENT 
// COMMENT 
VAR055 = analogRead(VAR012);
VAR056  = analogRead(VAR013);

VAR057 = VAR055 * VAR035 / 1023.0 * (VAR033 + VAR034) / VAR034;
VAR058 = VAR056  * VAR035 / 1023.0 * (VAR033 + VAR034) / VAR034;

VAR059 = VAR057 * VAR026;
VAR060 = VAR058 * VAR027;


// COMMENT 
FUN032(F("CALIB. "));
FUN032(VAR026, 3);
FUN032(F(" | SERVIZI: "));
FUN032(VAR059, 3);

FUN032(F(" || CALIB.: "));
FUN032(VAR027, 3);
FUN032(F(" | AVVIAME.: "));
FUN032(VAR060, 3);
FUN027(" ");
// COMMENT 



while (true) {
char VAR054 = 0;
if (Serial.available()) VAR054 = Serial.read();
if (bluetooth.available()) VAR054 = bluetooth.read();

// COMMENT 
if (VAR054 == '\n' || VAR054 == '\r') continue;

VAR054 = tolower(VAR054); // COMMENT 

if (VAR054 == 'q') break;

bool VAR061 = true;

switch (VAR054) {
// COMMENT 
case 't': VAR026 -= 0.100; break;
case 'y': VAR026 -= 0.010; break;
case 'u': VAR026 -= 0.001; break;
case 'i': VAR026 += 0.001; break;
case 'o': VAR026 += 0.010; break;
case 'p': VAR026 += 0.100; break;

// COMMENT 
case 'f': VAR027 -= 0.100; break;
case 'g': VAR027 -= 0.010; break;
case 'h': VAR027 -= 0.001; break;
case 'j': VAR027 += 0.001; break;
case 'k': VAR027 += 0.010; break;
case 'l': VAR027 += 0.100; break;

default:
VAR061 = false;
break;
}

if (VAR061) {


// COMMENT 
// COMMENT 
VAR055 = analogRead(VAR012);
VAR056  = analogRead(VAR013);

VAR057 = VAR055 * VAR035 / 1023.0 * (VAR033 + VAR034) / VAR034;
VAR058 = VAR056  * VAR035 / 1023.0 * (VAR033 + VAR034) / VAR034;

VAR059 = VAR057 * VAR026;
VAR060 = VAR058 * VAR027;

// COMMENT 
FUN032(F("CALIB. "));
FUN032(VAR026, 3);
FUN032(F(" | SERVIZI: "));
FUN032(VAR059, 3);

FUN032(F(" || CALIB.: "));
FUN032(VAR027, 3);
FUN032(F(" | AVVIAME.: "));
FUN032(VAR060, 3);
FUN027(" ");
// COMMENT 


}
}



// COMMENT 
FUN015(VAR028, VAR026);
FUN015(VAR029, VAR027);

FUN027(F("Calibrazione completata e salvata in EEPROM."));
FUN052(2);
}


// COMMENT 
float FUN018(int address) {
float VAR062;
EEPROM.get(address, VAR062);
return VAR062;
}

// COMMENT 
void FUN015(int address, float VAR062) {
EEPROM.put(address, VAR062);
}

// COMMENT 
float FUN029(int pin, float calibrazione) {
int VAR063 = analogRead(pin);
float VAR064 = VAR063 * VAR035 / 1023.0 * (VAR033 + VAR034) / VAR034;
return VAR064 * calibrazione;
}

// COMMENT 
/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */

void FUN023(){
digitalWrite(VAR008, HIGH);
}

void FUN019(){
digitalWrite(VAR008, LOW);
}


void FUN020(){
digitalWrite(VAR009, HIGH);
}

void  FUN016(){
digitalWrite(VAR009, LOW);
}

void FUN024(){
digitalWrite(VAR010,HIGH);
}

void FUN021(){
digitalWrite(VAR010, LOW);
}





// COMMENT 

void FUN043(){
FUN016();
FUN021();
FUN019();
delay(1000);

FUN023();
delay(1000);
FUN019();
delay(1000);

FUN020();
delay(1000);
FUN016();
delay(1000);

FUN024();
delay(1000);
FUN021();
delay(1000);

FUN027(F("Test semaforo eseguito"));
/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */
}
// COMMENT 
void FUN044(){
digitalWrite(VAR004, LOW);
delay(1000);
digitalWrite(VAR004, HIGH);
delay(1000);
digitalWrite(VAR004, LOW);
FUN027(F("Test Relay eseguito"));

/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */
}
// COMMENT 

void FUN045(){

wdt_reset(); // COMMENT 
wdt_enable(WDTO_2S); // COMMENT 

}
// COMMENT 

void FUN046(){

const int VAR065 = EEPROM.length(); 
byte TARGET_VALUE = 0xff;// COMMENT 

for (int i=0; i<VAR065;i++){
Serial.println(i);
EEPROM.write(i, TARGET_VALUE);
}
Serial.println("Eprom ripristinata");
FUN051();


}    

// COMMENT 

void FUN047(){
int VAR066 ;

VAR066 = digitalRead(VAR006);
// COMMENT 
FUN032(F("Jumper D4 - "));
if (VAR066 == LOW) {
FUN027(F("INSERITO azione relay ogni secondo"));
} else {
FUN027(F("MANCANTE azione relay ogni 10 secondi"));
}

VAR066 = digitalRead(VAR007);
// COMMENT 
FUN032(F("Jumper D2 - "));
if (VAR066 == LOW) {
FUN027(F("INSERITO stampa breve"));
} else {
FUN027(F("MANCANTE stampa completa"));
}



}
// COMMENT 

void FUN048(){
// COMMENT 
FUN012();
FUN051();
}
// COMMENT 

void FUN049(){
FUN009();
FUN027(F("Calibrazione completata e salvata in EEPROM."));
}
// COMMENT 

void FUN050(){
FUN002();
FUN027(F("Soglie impostate e salvate in EEPROM."));
}
// COMMENT 

void FUN051(){
FUN013();
}
// COMMENT 

void FUN038(){

}
// COMMENT 

void FUN039(){
FUN027(F(" 57 M SY NOK")); // COMMENT 
FUN007();
}
// COMMENT 

void FUN040(){
FUN027(F(" 58 M SY OK")); // COMMENT 
FUN006();
}
// COMMENT 

void FUN041(){
FUN027(F(" 59 VE SY")); // COMMENT 

// COMMENT 
if (FUN008()) {
Serial.println("S ok, r"); // COMMENT 
} else {
Serial.println("S nok, d"); // COMMENT 
}

}
// COMMENT 
// COMMENT 
void FUN042(){
FUN027("Action no. 99 selected");
delay(2000);
FUN027("Returning to loop'");
delay(2000);
VAR048 = true;  
}
// COMMENT 


// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
void(* resetFunc) (void) = 0; // COMMENT 
// COMMENT 

/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */
int FUN014(String inputString) {
// COMMENT 
inputString.trim();

// COMMENT 
if (inputString.length() == 0) {
return -1;
}

// COMMENT 
for (unsigned int i = 0; i < inputString.length(); i++) {
if (!isDigit(inputString.charAt(i))) {
return -1; // COMMENT 
}
}

// COMMENT 
int VAR067 = inputString.toInt();

// COMMENT 
if (VAR067 >= 0 && VAR067 <= 999) {
return VAR067; // COMMENT 
} else {
return -1; // COMMENT 
}
}
// COMMENT 
int FUN004() {
String serialReadString = "";       // COMMENT 
String bluetoothReadString = "";    // COMMENT 

serialReadString = "";    // COMMENT 
bluetoothReadString = ""; // COMMENT 

FUN027("\nInserisci un numero (0-999) e premi Invio:");


while (true) { // COMMENT 

// COMMENT 
while (Serial.available()) {
char VAR068 = (char)Serial.read();
serialReadString += VAR068;

// COMMENT 
if (VAR068 == '\n') {
int VAR067 = FUN014(serialReadString);
if (VAR067 != -1) { // COMMENT 
Serial.println("Numero ricevuto da Serial Monitor.");
return VAR067; // COMMENT 
} else {
Serial.print("Input non valido da Serial Monitor: '");
serialReadString.trim();
Serial.print(serialReadString); // COMMENT 
Serial.println("'. Riprova.");
serialReadString = ""; // COMMENT 
// COMMENT 
Serial.println("Inserisci un numero (0-999) e premi Invio:");
}
}
}

// COMMENT 
while (bluetooth.available()) {
char VAR068 = (char)bluetooth.read();
bluetoothReadString += VAR068;

// COMMENT 
if (VAR068 == '\n') {
int VAR067 = FUN014(bluetoothReadString);
if (VAR067 != -1) { // COMMENT 
Serial.println("Numero ricevuto da Bluetooth.");
bluetooth.print("Ricevuto: ");
bluetooth.println(VAR067);
return VAR067; // COMMENT 
} else {
Serial.print("Input non valido da Bluetooth: '");
bluetoothReadString.trim();
Serial.print(bluetoothReadString);
Serial.println("'. Riprova.");
bluetooth.print("Input non valido. Riprova (0-999): ");
bluetoothReadString = ""; // COMMENT 
}
}
}
}
}




// COMMENT 

void Menu() {
wdt_disable(); // COMMENT 
// COMMENT 
while (!VAR048) {

FUN030();

int VAR069 = FUN004();
/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */

// COMMENT 
switch (VAR069){

// COMMENT 
case 0:
FUN027("resetting in 1 sec");
delay(1000);               // COMMENT 
resetFunc();  // COMMENT 

delay(100);
FUN027("Reset did not work");
break;      // COMMENT 
case 1:
FUN043();

break;
// COMMENT 
case 2:
FUN044();

break;
// COMMENT 
case 3:
FUN045();

break;
// COMMENT 
case 4:
FUN046();

break;
// COMMENT 
case 5:
FUN047();
break;
// COMMENT 
case 6:
FUN048();
break;
// COMMENT 
case 7:
FUN049();
break;
// COMMENT 
case 8:
FUN050();
break;
// COMMENT 
case 9:
FUN051();
break;
// COMMENT 
case 10:
FUN038();
break;
// COMMENT 
case 57:
FUN039();
break;
// COMMENT 
case 58:
FUN040();
break;
// COMMENT 
case 59:
FUN041();
break;
// COMMENT 
case 99:
FUN042();
break;
// COMMENT 
default:
FUN027("Invalid command. Try again!");
} // COMMENT 
} // COMMENT 
} // COMMENT 


// COMMENT 

void FUN025(){
while (bluetooth.available()) {
char VAR054 = bluetooth.read();
// COMMENT 
// COMMENT 
// COMMENT 
inputString += VAR054;

if (VAR054 == '\n') {
VAR047 = true;
}
}
}
// COMMENT 
/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */
void serialEvent() {
while (Serial.available()) {
// COMMENT 
char VAR068 = (char)Serial.read();
// COMMENT 
inputString += VAR068;
// COMMENT 
// COMMENT 
if (VAR068 == '\n') {
VAR047 = true;
}
}
}




// COMMENT 



// COMMENT 
// COMMENT 
void FUN030(void){
// COMMENT 
FUN027("");
FUN027(F("|XXXXXXXXXXXXXXXXXXXXXXXX"));
FUN027(F("|🧪  🔍   Menu   🛠   🔧"));
FUN032(F("|  Ver. "));FUN027(Versione);
FUN027(F("|XXXXXXXXXXXXXXXXXXXXXXXX"));
FUN027(F(" 0 Reset"));
FUN027(F(" 1 Test Semaforo"));
FUN027(F(" 2 Test Relè"));
FUN027(F(" 3 Attiva Wdog senza resettarlo.. Farà reset"));
FUN027(F(" 4 Cancella totalmente la EEPROM"));
FUN027(F(" 5 Stato JUMPERS"));
FUN027(F(" 6 Scrive EEPROM default"));
FUN027(F(" 7 Calibrazione con tastiera"));
FUN027(F(" 8 Taratura soglie da tastiera"));
FUN027(F(" 9 EEPROM diagnostica"));

/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */
FUN027(F(" 99 Return to loop without reset"));
}
// COMMENT 
// COMMENT 
void FUN003(){

// COMMENT 
if (FUN008()) {
// COMMENT 
} else {
// COMMENT 

FUN010();
/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */

}
}

// COMMENT 

void FUN022(){
VAR018 = VAR018+1;
VAR022 = VAR022+1;


if (VAR018==60){
VAR018 = 0;
VAR019 = VAR019+1;
// COMMENT 
}

if (VAR019==60){
VAR019=0;
VAR020 = VAR020+1;
// COMMENT 
}

if (VAR020==24){
VAR020 = 0;
VAR021=VAR021+1;
FUN003(); // COMMENT 
}
}

// COMMENT 
void FUN026(){

char VAR071[24];  
// COMMENT 
sprintf(VAR071, "; ⏳ %u:%02u:%02u:%02u", 
VAR021, 
VAR020, 
VAR019, 
VAR018);

Serial.print(VAR071);
bluetooth.print(VAR071);

FUN032(F(" 🔁 Reset "));
FUN027(VAR025);

}


void FUN010() {
int VAR072;
float VAR073;

wdt_enable(WDTO_2S); // COMMENT 

// COMMENT 
// COMMENT 
for (int i = 0; i < 60; i++) {
// COMMENT 
wdt_reset(); // COMMENT 

VAR072 = VAR018 + i;
VAR073 = VAR020 / VAR019;

delay(100); // COMMENT 
i = VAR011;
// COMMENT 
}
}









// COMMENT 


#include <EEPROM.h>

// COMMENT 
// COMMENT 
const int VAR065 = EEPROM.length() - 1; 

// COMMENT 
// COMMENT 
// COMMENT 
void FUN006() {
const int VAR065 = EEPROM.length() - 1; 
byte VALUE_TO_WRITE = 0; // COMMENT 
VALUE_TO_WRITE = VALUE_TO_WRITE-2; 
EEPROM.write(VAR065, VALUE_TO_WRITE);
/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */
}

// COMMENT 
// COMMENT 
// COMMENT 
void FUN007() {
const int VAR065 = EEPROM.length() - 1; 
const byte VALUE_TO_WRITE = 0xFF;
EEPROM.write(VAR065, VALUE_TO_WRITE);
/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */
}






// COMMENT 
// COMMENT 
// COMMENT 
void FUN017() {
// COMMENT 
const int VAR065 = EEPROM.length() - 1; 
byte TARGET_VALUE = 0;// COMMENT 
TARGET_VALUE = TARGET_VALUE - 2; // COMMENT 
EEPROM.write(VAR065, TARGET_VALUE);
Serial.print("Written 0x");
Serial.print(TARGET_VALUE, HEX);
Serial.print(" to EEPROM address: ");
Serial.println(VAR065);
}


// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
bool FUN008() {
// COMMENT 
const int VAR065 = EEPROM.length() - 1; 
byte TARGET_VALUE = 0;// COMMENT 
TARGET_VALUE = TARGET_VALUE-2;
byte readValue = EEPROM.read(VAR065);
// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 

if (readValue == TARGET_VALUE) {
return true;
} else {
return false;
}
}

// COMMENT 









void FUN034(){

FUN032(F("📌CHARGER SU "));
// COMMENT 

if (VAR046){
FUN032(F("B. AVVIAM. 🛑🛑🛑"));

} else {
FUN032(F("B. SERVIZI ✅✅✅"));
}


}


// COMMENT 
void FUN005(){

if (VAR036 < VAR038) {
// COMMENT 
if (VAR002==true){
VAR002 = false;
FUN052(2); // COMMENT 
}
// COMMENT 
VAR046 = false;      
digitalWrite(VAR004, LOW);
FUN027(F("VBAT serv. inf. soglia bassa → relè disattivato ✅"));

// COMMENT 
FUN023();
FUN021();
// COMMENT 




} else if (VAR036 > VAR039) {
// COMMENT 
if (VAR002==false){
VAR002 = true;
FUN052(3); // COMMENT 
}
// COMMENT 
VAR046 = true;
VAR002=true;
digitalWrite(VAR004, HIGH);
FUN027(F("VBAT serv. super. soglia alta → relè attivato 🛑"));

// COMMENT 
FUN019();
FUN024();      
} else {
FUN027("VBAT nella zona neutra → nessuna azione");
}
}




// COMMENT 


// COMMENT 
// COMMENT 
// COMMENT 

// COMMENT 
template <typename T>
void FUN032(T dato) {
Serial.print(dato);
bluetooth.print(dato);
}

// COMMENT 
template <typename T>
void FUN027(T dato) {
Serial.println(dato);
bluetooth.println(dato);
}


// COMMENT 

void FUN032(float val, int decimali) {
Serial.print(val, decimali);
bluetooth.print(val, decimali);
}


// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
int FUN035(){
if (Serial.available()) {
return Serial.read();
}
if (bluetooth.available()) {
return bluetooth.read();
}
return -1; // COMMENT 
}

/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */

// COMMENT 
/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */
int FUN028(Stream &stream) {
long VAR074 = millis();
String numString = "";
bool VAR075 = false;
bool VAR076 = false;

while (millis() - VAR074 < VAR053) {
if (stream.available()) {
char VAR054 = stream.read();

if (!VAR076) {
// COMMENT 
if (isDigit(VAR054)) {
numString += VAR054;
VAR076 = true;
} else if (VAR054 == '-') {
VAR075 = true;
VAR076 = true; // COMMENT 
} else if (VAR054 == '+') {
VAR076 = true; // COMMENT 
}
// COMMENT 
} else {
// COMMENT 
if (isDigit(VAR054)) {
numString += VAR054;
} else {
// COMMENT 
break;
}
}
}
}

// COMMENT 
if (numString.length() > 0) {
long VAR077 = numString.toInt(); // COMMENT 
if (VAR075) {
return -VAR077;
} else {
return VAR077;
}
} else {
// COMMENT 
return 0;
}
}

// COMMENT 

// COMMENT 
void FUN033() {
unsigned char VAR078;
for (VAR001 = 0; VAR001 < sizeof(CODE_ARRAY); VAR001++) {
// COMMENT 
VAR078 = pgm_read_byte(&(CODE_ARRAY[VAR001]));
}
}
// COMMENT 

void setup() {
FUN033();   
if (VAR014){
VAR012 = A3;
VAR013 = A2;
} else {
VAR012 = A2;
VAR013 = A3;
}  




// COMMENT 
byte resetCause = MCUSR;
MCUSR = 0;
wdt_disable(); // COMMENT 

Serial.begin(115200);
bluetooth.begin(9600);
FUN027(F("RESET"));
FUN032(F("Nome file: "));
FUN027(__FILE__);

// COMMENT 
VAR026 = FUN018(VAR028);
VAR027  = FUN018(VAR029);

// COMMENT 
// COMMENT 
EEPROM.get(VAR030, VAR025);

// COMMENT 
VAR025++;

// COMMENT 
EEPROM.put(VAR030, VAR025); 

/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */

// COMMENT 
EEPROM.get(VAR031, VAR038);
EEPROM.get(VAR032,  VAR039);

// COMMENT 

if (isnan(VAR038)) VAR038 = 600;
if (isnan(VAR039))  VAR039 = 600;
/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */

// COMMENT 
if (isnan(VAR026)) VAR026 = 1.0;
if (isnan(VAR027))  VAR027 = 1.0;

pinMode(VAR004, OUTPUT);
pinMode(VAR005, OUTPUT);

pinMode(VAR007, INPUT_PULLUP);
pinMode(VAR006, INPUT_PULLUP);


pinMode(VAR008, OUTPUT);
pinMode(VAR009, OUTPUT);
pinMode(VAR010, OUTPUT);  

digitalWrite(VAR004, LOW);
digitalWrite(VAR005, LOW);

digitalWrite(VAR010, HIGH);
delay(200);
digitalWrite(VAR009, HIGH);
delay(200);
digitalWrite(VAR008, HIGH);
delay(200);
FUN052(1);
digitalWrite(VAR010, LOW);
delay(200);
digitalWrite(VAR009, LOW);
delay(200);
digitalWrite(VAR008, LOW);
delay(200);
FUN052(1);

/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */

wdt_enable(WDTO_2S); // COMMENT 
FUN001();
FUN005();

}

// COMMENT 

void loop() {
int VAR079 ;


wdt_reset(); // COMMENT 

unsigned long VAR070 = millis();



// COMMENT 
VAR079 = digitalRead(VAR006);
// COMMENT 
// COMMENT 
if (VAR079 == LOW) {
// COMMENT 
if ((VAR023 > 0) && (VAR022 % 1 == 0) && (VAR022 != 0)) {

if (VAR022 != VAR024) {

FUN005();
VAR024 = VAR022;
}
}
} else {
// COMMENT 
if ((VAR023 > 0) && (VAR022 % VAR023 == 0) && (VAR022 != 0)) {

if (VAR022 != VAR024) {

FUN005();
VAR024 = VAR022;
}
}
}






// COMMENT 
if (VAR070 - VAR016 >= VAR017) {
VAR016 = VAR070;
digitalWrite(VAR009, !digitalRead(VAR009));

FUN001();


FUN034();
FUN022();
FUN026();

if (VAR015){
FUN032(F("Debug attivo"));
FUN032(F("; VAR022=")); FUN032(VAR022);
FUN032(F("; VAR023=")); FUN032(VAR023);
FUN032(F("; VAR024=")); FUN032(VAR024); 
FUN027("");
}

}

FUN025();
// COMMENT 
if (VAR047) {
inputString.trim();
FUN027(inputString);
// COMMENT 
if ( (inputString.substring(0, 4) ) == "Menu" ) {
Menu();
}
// COMMENT 
inputString = "";
VAR047 = false;
}


}

// COMMENT 
void FUN001(){
VAR036 = analogRead(VAR012);
VAR037 = analogRead(VAR013);

// COMMENT 
// COMMENT 
// COMMENT 


VAR040 = VAR038 * VAR035 / 1023.0 * (VAR033 + VAR034) / VAR034;
VAR041 = VAR039 * VAR035 / 1023.0 * (VAR033 + VAR034) / VAR034;


VAR042 = VAR036 * VAR035 / 1023.0 * (VAR033 + VAR034) / VAR034;
VAR043 = VAR037 * VAR035 / 1023.0 * (VAR033 + VAR034) / VAR034;

// COMMENT 
// COMMENT 
VAR042 = VAR042 * VAR026;
// COMMENT 

// COMMENT 
// COMMENT 
VAR043 = VAR043 * VAR027;
// COMMENT 


// COMMENT 
// COMMENT 


int VAR080 ;
VAR080 = digitalRead(VAR007);
// COMMENT 
if (VAR080 == LOW) {

FUN032(F("⚙️ SHI "));FUN032(VAR041);FUN032(F("; "));
FUN032(F("SLO "));FUN032(VAR040);FUN032(F("; "));
FUN032(F("🔋 B.SRV "));FUN032(VAR042);FUN032(F("; "));
FUN032(F(" B.AVV "));FUN032(VAR043);FUN032(F("; "));

} else {

FUN027(F("--------------------------"));
FUN032(F("⚙️ Soglia alta "));FUN032(VAR039);FUN032(F(" BIT; "));FUN032(VAR041);FUN027(F(" VOLT; "));
FUN032(F("⚙️ Soglia bassa "));FUN032(VAR038);FUN032(F(" BIT; "));FUN032(VAR040);FUN027(F(" VOLT; "));
FUN032(F("🔋 Batt. servizi "));FUN032(VAR036);FUN032(F(" BIT; "));FUN032(VAR042);FUN027(F(" VOLT; "));
FUN032(F("🚀 Batt. avviamento "));FUN032(VAR037);FUN032(F(" BIT; "));FUN032(VAR043);FUN027(F(" VOLT; "));

}

}     


// COMMENT 


void FUN052(int count) {
for (int i = 0; i < count; i++) {
if (VAR003){
// COMMENT 
} else {
digitalWrite(VAR005, HIGH);
delay(100);
digitalWrite(VAR005, LOW);
delay(100);
}
}
}

// COMMENT 











/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */