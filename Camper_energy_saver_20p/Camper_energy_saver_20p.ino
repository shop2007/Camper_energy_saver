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
String Versione = "Camp_Energ_Sav_20p";
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

const int VAR012 = A0;
const int VAR013 = A1;

int VAR014 = A2;
int VAR015 = A3;






bool VAR016 = false; // COMMENT 
bool VAR017= false; // COMMENT 


// COMMENT 
unsigned long VAR018 = 0;
// COMMENT 
const unsigned long VAR019 = 1000;



// COMMENT 
unsigned int VAR020 = 0;
unsigned int VAR021 = 0;
unsigned int VAR022 = 0;
unsigned int VAR023 = 0;

unsigned int VAR024 = 0; // COMMENT 
unsigned int VAR025 = 10;       // COMMENT 
unsigned int VAR026 = 0;           // COMMENT 


// COMMENT 
unsigned int VAR027 = 0; // COMMENT 

// COMMENT 
float VAR028;
float VAR029;

// COMMENT 
// COMMENT 
const int VAR030 = 0;
const int VAR031 = VAR030 + sizeof(float);
// COMMENT 
const int VAR032 = VAR031 + sizeof(float); 
// COMMENT 
// COMMENT 
const int VAR033 = VAR032 + sizeof(int);  // COMMENT 
const int VAR034  = VAR033 + sizeof(int); // COMMENT 
// COMMENT 




// COMMENT 
const float VAR035 = 10000.0;
const float VAR036 = 3300.0;
const float VAR037 = 5.0;

int VAR038; // COMMENT 
int VAR039; // COMMENT 
int VAR040; // COMMENT 
int VAR041; // COMMENT 
float VAR042; // COMMENT 
float VAR043; // COMMENT 
float VAR044; // COMMENT 
float VAR045; // COMMENT 



/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */

bool VAR048 = false;
String inputString = "";         // COMMENT 
bool VAR049 = false;  // COMMENT 
bool VAR050 = false;
int VAR051;
int VAR052 = LOW;             // COMMENT 
const int VAR053 =  LED_BUILTIN;// COMMENT 
const long VAR054 = 250;           // COMMENT 

const long VAR055 = 1000;
// COMMENT 

// COMMENT 



// COMMENT 

// COMMENT 
float FUN036(long x, long in_min, long in_max, float out_min, float out_max) {
return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}



// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 

void FUN035(int startAddress, int numBytes) {
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

void FUN012(){
FUN031("ADR Calbraz. Servizi: ");FUN031(VAR030);FUN031(" -> ");FUN026(VAR028);
FUN031("ADR Calbraz. Motore: ");FUN031(VAR031);FUN031(" -> ");FUN026(VAR029);
FUN031("ADR Counter Reset: ");FUN031(VAR032);FUN031(" -> ");FUN026(VAR027);
FUN031("ADR Soglia Bassa: ");FUN031(VAR033);FUN031(" -> ");FUN026(VAR040);
FUN031("ADR Soglia Alta: ");FUN031(VAR034);FUN031(" -> ");FUN026(VAR041);

// COMMENT 
FUN035(0, 64); // COMMENT 

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
FUN026(F("== Modalità Calibrazione Soglie (da tastiera) =="));
FUN026(F("Soglia BASSA: T-100/Y-10/U-1 I+1/O+10/P+100 bit"));
FUN026(F("Soglia ALTA : F-100/G-10/H-1 J+1/K+10/L+100 bit"));
FUN026(F("Premi 'Q' per salvare e uscire."));

// COMMENT 
// COMMENT 

float VAR042;
float VAR043;

while (true) {
// COMMENT 
VAR040 = constrain(VAR040, 0, 1023);
VAR041 = constrain(VAR041, 0, 1023);

VAR042 = VAR040 * VAR037 / 1023.0 * (VAR035 + VAR036) / VAR036;
VAR043  = VAR041  * VAR037 / 1023.0 * (VAR035 + VAR036) / VAR036;

// COMMENT 
FUN031(F("BASSA: "));
FUN031(VAR040);
FUN031(F("bit ("));
FUN031(VAR042, 2); // COMMENT 
FUN031(F(" V) | ALTA: "));
FUN031(VAR041);
FUN031(F("bit ("));
FUN031(VAR043, 2);
FUN031(F(" V) | Cmd> ")); // COMMENT 



// COMMENT 
char VAR056;
while (true) {  // COMMENT 
VAR056 = 0; // COMMENT 

if (Serial.available()) {
VAR056 = Serial.read();
} else if (bluetooth.available()) {
VAR056 = bluetooth.read();
}

// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
if (VAR056 != 0 && VAR056 != '\r' && VAR056 != '\n') {
break;  // COMMENT 
}

// COMMENT 
// COMMENT 
// COMMENT 
delay(10);
}
VAR056 = tolower(VAR056);

// COMMENT 
FUN026(VAR056);

// COMMENT 
if (VAR056 == 'q') {
break; // COMMENT 
}

switch (VAR056) {
// COMMENT 
case 't': VAR040 -= 100; break;
case 'y': VAR040 -= 10; break;
case 'u': VAR040 -= 1; break;
case 'i': VAR040 += 1; break;
case 'o': VAR040 += 10; break;
case 'p': VAR040 += 100; break;

// COMMENT 
case 'f': VAR041 -= 100; break;
case 'g': VAR041 -= 10; break;
case 'h': VAR041 -= 1; break;
case 'j': VAR041 += 1; break;
case 'k': VAR041 += 10; break;
case 'l': VAR041 += 100; break;

default:
FUN026(F("Comando non valido. Riprova."));
break;
}
// COMMENT 
}

// COMMENT 
EEPROM.put(VAR033, VAR040);
EEPROM.put(VAR034,  VAR041);

FUN026(F("\nCalibrazione completata. Soglie salvate in EEPROM."));
}
// COMMENT 


void FUN009() {
FUN026(F("== Modalità Calibrazione (da tastiera) =="));
FUN031(F("SERVIZI: T-100/Y-10/U-1 I+1/O+10/P+100 bit"));
FUN031(F(" || AVVIAMENTO : F-100/G-10/H-1 J+1/K+10/L+100 bit"));
FUN026(F(" || Premi 'Q' per salvare e uscire."));

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

int VAR057;
int VAR058;
float VAR059;
float VAR060;
float VAR061;
float VAR062;

// COMMENT 
// COMMENT 
VAR057 = analogRead(VAR014);
VAR058  = analogRead(VAR015);

VAR059 = VAR057 * VAR037 / 1023.0 * (VAR035 + VAR036) / VAR036;
VAR060 = VAR058  * VAR037 / 1023.0 * (VAR035 + VAR036) / VAR036;

VAR061 = VAR059 * VAR028;
VAR062 = VAR060 * VAR029;


// COMMENT 
FUN031(F("CALIB. "));
FUN031(VAR028, 3);
FUN031(F(" | SERVIZI: "));
FUN031(VAR061, 3);

FUN031(F(" || CALIB.: "));
FUN031(VAR029, 3);
FUN031(F(" | AVVIAME.: "));
FUN031(VAR062, 3);
FUN026(" ");
// COMMENT 



while (true) {
char VAR056 = 0;
if (Serial.available()) VAR056 = Serial.read();
if (bluetooth.available()) VAR056 = bluetooth.read();

// COMMENT 
if (VAR056 == '\n' || VAR056 == '\r') continue;

VAR056 = tolower(VAR056); // COMMENT 

if (VAR056 == 'q') break;

bool VAR063 = true;

switch (VAR056) {
// COMMENT 
case 't': VAR028 -= 0.100; break;
case 'y': VAR028 -= 0.010; break;
case 'u': VAR028 -= 0.001; break;
case 'i': VAR028 += 0.001; break;
case 'o': VAR028 += 0.010; break;
case 'p': VAR028 += 0.100; break;

// COMMENT 
case 'f': VAR029 -= 0.100; break;
case 'g': VAR029 -= 0.010; break;
case 'h': VAR029 -= 0.001; break;
case 'j': VAR029 += 0.001; break;
case 'k': VAR029 += 0.010; break;
case 'l': VAR029 += 0.100; break;

default:
VAR063 = false;
break;
}

if (VAR063) {


// COMMENT 
// COMMENT 
VAR057 = analogRead(VAR014);
VAR058  = analogRead(VAR015);

VAR059 = VAR057 * VAR037 / 1023.0 * (VAR035 + VAR036) / VAR036;
VAR060 = VAR058  * VAR037 / 1023.0 * (VAR035 + VAR036) / VAR036;

VAR061 = VAR059 * VAR028;
VAR062 = VAR060 * VAR029;

// COMMENT 
FUN031(F("CALIB. "));
FUN031(VAR028, 3);
FUN031(F(" | SERVIZI: "));
FUN031(VAR061, 3);

FUN031(F(" || CALIB.: "));
FUN031(VAR029, 3);
FUN031(F(" | AVVIAME.: "));
FUN031(VAR062, 3);
FUN026(" ");
// COMMENT 


}
}



// COMMENT 
FUN014(VAR030, VAR028);
FUN014(VAR031, VAR029);

FUN026(F("Calibrazione completata e salvata in EEPROM."));
FUN051(2);
}


// COMMENT 
float FUN017(int address) {
float VAR064;
EEPROM.get(address, VAR064);
return VAR064;
}

// COMMENT 
void FUN014(int address, float VAR064) {
EEPROM.put(address, VAR064);
}

// COMMENT 
float FUN028(int pin, float calibrazione) {
int VAR065 = analogRead(pin);
float VAR066 = VAR065 * VAR037 / 1023.0 * (VAR035 + VAR036) / VAR036;
return VAR066 * calibrazione;
}

// COMMENT 
/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */

void FUN022(){
digitalWrite(VAR008, HIGH);
}

void FUN018(){
digitalWrite(VAR008, LOW);
}


void FUN019(){
digitalWrite(VAR009, HIGH);
}

void  FUN015(){
digitalWrite(VAR009, LOW);
}

void FUN023(){
digitalWrite(VAR010,HIGH);
}

void FUN020(){
digitalWrite(VAR010, LOW);
}





// COMMENT 

void FUN042(){
FUN015();
FUN020();
FUN018();
delay(1000);

FUN022();
delay(1000);
FUN018();
delay(1000);

FUN019();
delay(1000);
FUN015();
delay(1000);

FUN023();
delay(1000);
FUN020();
delay(1000);

FUN026(F("Test semaforo eseguito"));
/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */
}
// COMMENT 
void FUN043(){
digitalWrite(VAR004, LOW);
delay(1000);
digitalWrite(VAR004, HIGH);
delay(1000);
digitalWrite(VAR004, LOW);
FUN026(F("Test Relay eseguito"));

/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */
}
// COMMENT 

void FUN044(){

wdt_reset(); // COMMENT 
wdt_enable(WDTO_2S); // COMMENT 

}
// COMMENT 

void FUN045(){

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
ML-ECOM.END */
}    

// COMMENT 

void FUN046(){
int VAR067 ;

VAR067 = digitalRead(VAR006);
// COMMENT 
FUN031(F("Jumper D4 - "));
if (VAR067 == LOW) {
FUN026(F("INSERITO azione relay ogni secondo"));
} else {
FUN026(F("MANCANTE azione relay ogni 0 secondi"));
}

VAR067 = digitalRead(VAR007);
// COMMENT 
FUN031(F("Jumper D2 - "));
if (VAR067 == LOW) {
FUN026(F("INSERITO stampa breve"));
} else {
FUN026(F("MANCANTE stampa completa"));
}



}
// COMMENT 

void FUN047(){
// COMMENT 
// COMMENT 
}
// COMMENT 

void FUN048(){
FUN009();
FUN026(F("Calibrazione completata e salvata in EEPROM."));
}
// COMMENT 

void FUN049(){
FUN002();
FUN026(F("Soglie impostate e salvate in EEPROM."));
}
// COMMENT 

void FUN050(){
FUN012();
}
// COMMENT 

void FUN037(){

}
// COMMENT 

void FUN038(){
FUN026(F(" S PFF ")); // COMMENT 
FUN007();
}
// COMMENT 

void FUN039(){
FUN026(F(" S PFE ")); // COMMENT 
FUN006();
}
// COMMENT 

void FUN040(){
FUN026(F(" V P"));

// COMMENT 
if (FUN008()) {
Serial.println(" ok");
} else {
Serial.println("DM");
}

}
// COMMENT 
// COMMENT 
void FUN041(){
FUN026("Action no. 99 selected");
delay(2000);
FUN026("Returning to loop'");
delay(2000);
VAR050 = true;  
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
int FUN013(String inputString) {
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
int VAR068 = inputString.toInt();

// COMMENT 
if (VAR068 >= 0 && VAR068 <= 999) {
return VAR068; // COMMENT 
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

FUN026("\nInserisci un numero (0-999) e premi Invio:");


while (true) { // COMMENT 

// COMMENT 
while (Serial.available()) {
char VAR069 = (char)Serial.read();
serialReadString += VAR069;

// COMMENT 
if (VAR069 == '\n') {
int VAR068 = FUN013(serialReadString);
if (VAR068 != -1) { // COMMENT 
Serial.println("Numero ricevuto da Serial Monitor.");
return VAR068; // COMMENT 
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
char VAR069 = (char)bluetooth.read();
bluetoothReadString += VAR069;

// COMMENT 
if (VAR069 == '\n') {
int VAR068 = FUN013(bluetoothReadString);
if (VAR068 != -1) { // COMMENT 
Serial.println("Numero ricevuto da Bluetooth.");
bluetooth.print("Ricevuto: ");
bluetooth.println(VAR068);
return VAR068; // COMMENT 
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
while (!VAR050) {

FUN029();

int VAR070 = FUN004();
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
switch (VAR070){

// COMMENT 
case 0:
FUN026("resetting in 1 sec");
delay(1000);               // COMMENT 
resetFunc();  // COMMENT 

delay(100);
FUN026("Reset did not work");
break;      // COMMENT 
case 1:
FUN042();

break;
// COMMENT 
case 2:
FUN043();

break;
// COMMENT 
case 3:
FUN044();

break;
// COMMENT 
case 4:
FUN045();

break;
// COMMENT 
case 5:
FUN046();
break;
// COMMENT 
case 6:
FUN047();
break;
// COMMENT 
case 7:
FUN048();
break;
// COMMENT 
case 8:
FUN049();
break;
// COMMENT 
case 9:
FUN050();
break;
// COMMENT 
case 10:
FUN037();
break;
// COMMENT 
case 57:
FUN038();
break;
// COMMENT 
case 58:
FUN039();
break;
// COMMENT 
case 59:
FUN040();
break;
// COMMENT 
case 99:
FUN041();
break;
// COMMENT 
default:
FUN026("Invalid command. Try again!");
} // COMMENT 
} // COMMENT 
} // COMMENT 


// COMMENT 

void FUN024(){
while (bluetooth.available()) {
char VAR056 = bluetooth.read();
// COMMENT 
// COMMENT 
// COMMENT 
inputString += VAR056;

if (VAR056 == '\n') {
VAR049 = true;
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
char VAR069 = (char)Serial.read();
// COMMENT 
inputString += VAR069;
// COMMENT 
// COMMENT 
if (VAR069 == '\n') {
VAR049 = true;
}
}
}




// COMMENT 



// COMMENT 
// COMMENT 
void FUN029(void){
// COMMENT 
FUN026("");
FUN026(F("|XXXXXXXXXXXXXXXXXXXXXXXX"));
FUN026(F("|🧪  🔍   Menu   🛠   🔧"));
FUN031(F("|  Ver. "));FUN026(Versione);
FUN026(F("|XXXXXXXXXXXXXXXXXXXXXXXX"));
FUN026(F(" 0 Reset"));
FUN026(F(" 1 Test Semaforo"));
FUN026(F(" 2 Test Relè"));
FUN026(F(" 3 Attiva Wdog senza resettarlo.. Farà reset"));
FUN026(F(" 4 ---"));
FUN026(F(" 5 Stato JUMPERS"));
FUN026(F(" 6 ---"));
FUN026(F(" 7 Calibrazione con tastiera"));
FUN026(F(" 8 Taratura soglie da tastiera"));
FUN026(F(" 9 EEPROM diagnostica"));

/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */
FUN026(F(" 99 Return to loop without reset"));
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

void FUN021(){
VAR020 = VAR020+1;
VAR024 = VAR024+1;


if (VAR020==60){
VAR020 = 0;
VAR021 = VAR021+1;
FUN003();
}

if (VAR021==60){
VAR021=0;
VAR022 = VAR022+1;
}

if (VAR022==24){
VAR022 = 0;
VAR023=VAR023+1;
}
}

// COMMENT 
void FUN025(){

char VAR072[24];  
// COMMENT 
sprintf(VAR072, "; ⏳ %u:%02u:%02u:%02u", 
VAR023, 
VAR022, 
VAR021, 
VAR020);

Serial.print(VAR072);
bluetooth.print(VAR072);

FUN031(F(" 🔁 Reset "));
FUN026(VAR027);

}


void FUN010() {
int VAR073;
float VAR074;

wdt_enable(WDTO_2S); // COMMENT 

// COMMENT 
// COMMENT 
for (int i = 0; i < 60; i++) {
// COMMENT 
wdt_reset(); // COMMENT 

VAR073 = VAR020 + i;
VAR074 = VAR022 / VAR021;

delay(100); // COMMENT 
i = VAR011;
// COMMENT 
}
}









// COMMENT 


#include <EEPROM.h>

// COMMENT 
// COMMENT 
const int VAR075 = EEPROM.length() - 1; 

// COMMENT 
// COMMENT 
// COMMENT 
void FUN006() {
const int VAR075 = EEPROM.length() - 1; 
const byte VALUE_TO_WRITE = 0xFE;
EEPROM.write(VAR075, VALUE_TO_WRITE);
Serial.print("Scritto 0x");
Serial.print(VALUE_TO_WRITE, HEX);
Serial.print(" all'indirizzo EEPROM: ");
Serial.println(VAR075);
}

// COMMENT 
// COMMENT 
// COMMENT 
void FUN007() {
const int VAR075 = EEPROM.length() - 1; 
const byte VALUE_TO_WRITE = 0xFF;
EEPROM.write(VAR075, VALUE_TO_WRITE);
Serial.print("S");
Serial.print(VALUE_TO_WRITE, HEX);
Serial.print(" A ");
Serial.println(VAR075);
}






// COMMENT 
// COMMENT 
// COMMENT 
void FUN016() {
// COMMENT 
const int VAR075 = EEPROM.length() - 1; 
const byte TARGET_VALUE = 0xFE; // COMMENT 

EEPROM.write(VAR075, TARGET_VALUE);
Serial.print("W");
Serial.print(TARGET_VALUE, HEX);
Serial.print(" A ");
Serial.println(VAR075);
}


// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
bool FUN008() {
// COMMENT 
const int VAR075 = EEPROM.length() - 1; 
const byte TARGET_VALUE = 0xFE; // COMMENT 

byte readValue = EEPROM.read(VAR075);
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









void FUN033(){

FUN031(F("📌CHARGER SU "));
// COMMENT 

if (VAR048){
FUN031(F("B. AVVIAM. 🛑🛑🛑"));

} else {
FUN031(F("B. SERVIZI ✅✅✅"));
}


}


// COMMENT 
void FUN005(){

if (VAR038 < VAR040) {
// COMMENT 
if (VAR002==true){
VAR002 = false;
FUN051(2); // COMMENT 
}
// COMMENT 
VAR048 = false;      
digitalWrite(VAR004, LOW);
FUN026(F("VBAT serv. inf. soglia bassa → relè disattivato ✅"));

// COMMENT 
FUN022();
FUN020();
// COMMENT 




} else if (VAR038 > VAR041) {
// COMMENT 
if (VAR002==false){
VAR002 = true;
FUN051(3); // COMMENT 
}
// COMMENT 
VAR048 = true;
VAR002=true;
digitalWrite(VAR004, HIGH);
FUN026(F("VBAT serv. super. soglia alta → relè attivato 🛑"));

// COMMENT 
FUN018();
FUN023();      
} else {
FUN026("VBAT nella zona neutra → nessuna azione");
}
}




// COMMENT 


// COMMENT 
// COMMENT 
// COMMENT 

// COMMENT 
template <typename T>
void FUN031(T dato) {
Serial.print(dato);
bluetooth.print(dato);
}

// COMMENT 
template <typename T>
void FUN026(T dato) {
Serial.println(dato);
bluetooth.println(dato);
}


// COMMENT 

void FUN031(float val, int decimali) {
Serial.print(val, decimali);
bluetooth.print(val, decimali);
}


// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
// COMMENT 
int FUN034(){
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
int FUN027(Stream &stream) {
long VAR076 = millis();
String numString = "";
bool VAR077 = false;
bool VAR078 = false;

while (millis() - VAR076 < VAR055) {
if (stream.available()) {
char VAR056 = stream.read();

if (!VAR078) {
// COMMENT 
if (isDigit(VAR056)) {
numString += VAR056;
VAR078 = true;
} else if (VAR056 == '-') {
VAR077 = true;
VAR078 = true; // COMMENT 
} else if (VAR056 == '+') {
VAR078 = true; // COMMENT 
}
// COMMENT 
} else {
// COMMENT 
if (isDigit(VAR056)) {
numString += VAR056;
} else {
// COMMENT 
break;
}
}
}
}

// COMMENT 
if (numString.length() > 0) {
long VAR079 = numString.toInt(); // COMMENT 
if (VAR077) {
return -VAR079;
} else {
return VAR079;
}
} else {
// COMMENT 
return 0;
}
}

// COMMENT 

// COMMENT 
void FUN032() {
unsigned char VAR080;
for (VAR001 = 0; VAR001 < sizeof(CODE_ARRAY); VAR001++) {
// COMMENT 
VAR080 = pgm_read_byte(&(CODE_ARRAY[VAR001]));
}
}
// COMMENT 

void setup() {
FUN032();   
if (VAR016){
VAR014 = A3;
VAR015 = A2;
} else {
VAR014 = A2;
VAR015 = A3;
}  




// COMMENT 
byte resetCause = MCUSR;
MCUSR = 0;
wdt_disable(); // COMMENT 

Serial.begin(115200);
bluetooth.begin(9600);
FUN026(F("RESET"));
FUN031(F("Nome file: "));
FUN026(__FILE__);

// COMMENT 
VAR028 = FUN017(VAR030);
VAR029  = FUN017(VAR031);

// COMMENT 
// COMMENT 
EEPROM.get(VAR032, VAR027);

// COMMENT 
VAR027++;

// COMMENT 
EEPROM.put(VAR032, VAR027); 

/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */

// COMMENT 
EEPROM.get(VAR033, VAR040);
EEPROM.get(VAR034,  VAR041);

// COMMENT 

if (isnan(VAR040)) VAR040 = 600;
if (isnan(VAR041))  VAR041 = 600;
/* MLCOM-START 
 - MLCOM - 
 - MLCOM - 
ML-ECOM.END */

// COMMENT 
if (isnan(VAR028)) VAR028 = 1.0;
if (isnan(VAR029))  VAR029 = 1.0;

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
FUN051(1);
digitalWrite(VAR010, LOW);
delay(200);
digitalWrite(VAR009, LOW);
delay(200);
digitalWrite(VAR008, LOW);
delay(200);
FUN051(1);

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
int VAR081 ;


wdt_reset(); // COMMENT 

unsigned long VAR071 = millis();



// COMMENT 
VAR081 = digitalRead(VAR006);
// COMMENT 
// COMMENT 
if (VAR081 == LOW) {
// COMMENT 
if ((VAR025 > 0) && (VAR024 % 1 == 0) && (VAR024 != 0)) {

if (VAR024 != VAR026) {

FUN005();
VAR026 = VAR024;
}
}
} else {
// COMMENT 
if ((VAR025 > 0) && (VAR024 % VAR025 == 0) && (VAR024 != 0)) {

if (VAR024 != VAR026) {

FUN005();
VAR026 = VAR024;
}
}
}






// COMMENT 
if (VAR071 - VAR018 >= VAR019) {
VAR018 = VAR071;
digitalWrite(VAR009, !digitalRead(VAR009));

FUN001();


FUN033();
FUN021();
FUN025();

if (VAR017){
FUN031(F("Debug attivo"));
FUN031(F("; VAR024=")); FUN031(VAR024);
FUN031(F("; VAR025=")); FUN031(VAR025);
FUN031(F("; VAR026=")); FUN031(VAR026); 
FUN026("");
}

}

FUN024();
// COMMENT 
if (VAR049) {
inputString.trim();
FUN026(inputString);
// COMMENT 
if ( (inputString.substring(0, 4) ) == "Menu" ) {
Menu();
}
// COMMENT 
inputString = "";
VAR049 = false;
}


}

// COMMENT 
void FUN001(){
VAR038 = analogRead(VAR014);
VAR039 = analogRead(VAR015);

// COMMENT 
// COMMENT 
// COMMENT 


VAR042 = VAR040 * VAR037 / 1023.0 * (VAR035 + VAR036) / VAR036;
VAR043 = VAR041 * VAR037 / 1023.0 * (VAR035 + VAR036) / VAR036;


VAR044 = VAR038 * VAR037 / 1023.0 * (VAR035 + VAR036) / VAR036;
VAR045 = VAR039 * VAR037 / 1023.0 * (VAR035 + VAR036) / VAR036;

// COMMENT 
// COMMENT 
VAR044 = VAR044 * VAR028;
// COMMENT 

// COMMENT 
// COMMENT 
VAR045 = VAR045 * VAR029;
// COMMENT 


// COMMENT 
// COMMENT 


int VAR082 ;
VAR082 = digitalRead(VAR007);
// COMMENT 
if (VAR082 == LOW) {

FUN031(F("⚙️ SHI "));FUN031(VAR043);FUN031(F("; "));
FUN031(F("SLO "));FUN031(VAR042);FUN031(F("; "));
FUN031(F("🔋 B.SRV "));FUN031(VAR044);FUN031(F("; "));
FUN031(F(" B.AVV "));FUN031(VAR045);FUN031(F("; "));

} else {

FUN026(F("--------------------------"));
FUN031(F("⚙️ Soglia alta "));FUN031(VAR041);FUN031(F(" BIT; "));FUN031(VAR043);FUN026(F(" VOLT; "));
FUN031(F("⚙️ Soglia bassa "));FUN031(VAR040);FUN031(F(" BIT; "));FUN031(VAR042);FUN026(F(" VOLT; "));
FUN031(F("🔋 Batt. servizi "));FUN031(VAR038);FUN031(F(" BIT; "));FUN031(VAR044);FUN026(F(" VOLT; "));
FUN031(F("🚀 Batt. avviamento "));FUN031(VAR039);FUN031(F(" BIT; "));FUN031(VAR045);FUN026(F(" VOLT; "));

}

}     


// COMMENT 


void FUN051(int count) {
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