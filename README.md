# Camper Energy Saver

**Camper Energy Saver** è un sistema di gestione automatica della ricarica solare nei camper. Il dispositivo controlla la tensione della **batteria servizi** e, quando questa è completamente carica, **connette al charger solare** anche la **batteria di avviamento**, allo scopo di mantenerla completamente carica durante le soste prolungate.
Si possono impostare due soglie che controllano la batteria servizi, quella alta quando al charger solare verrà aggiunta la batteria motore, e quella bassa , quando la stessa viene staccata, per non sottrarre energia alla batteria servizi.

Lo stato del sistema viene trasmesso in tempo reale tramite **Bluetooth (HC-05)** a uno smartphone Android tramite l’app gratuita **“Bluetooth Terminal”**.

---

## ⚙️ Funzionalità principali

- Monitoraggio continuo della **tensione batteria servizi**
- Connessione automatica della carica solare alla batteria motore:
  - Priorità alla batteria servizi
  - Connessione della batteria motore solo quando la batteria servizi è completamente carica
- Segnalazione dello stato via **Bluetooth (HC-05)**
- Compatibilità con l'app Android **Bluetooth Terminal**
- Configurazione soglie tramite cavetto usb (e Arduino IDE) oppure da bluetooth terminal

---

## 🧰 Componenti hardware richiesti

- **Arduino Uno o Nano (ATmega328)**
- **Modulo relè 12V o 5V** (singolo o doppio)
- **Modulo Bluetooth HC-05**
- **Partitore resistivo** per misurare la tensione della batteria servizi
- **Diodi o relè di potenza** per instradare la carica
- **Alimentazione 12V** dal circuito servizi del camper

Al reset si accende semaforo rosso
Quando lampeggia velocemente il semaforo giallo è nel Menù
Quando lampeggia una volta al secondo è in RUN normale

******************************************
## 🧰 PROCEDURA DI CALIBRAZIONE 
- Entrare nel Menu (Nota1) digitando "Menu" ("M" maiuscola ed "u" senza accento)
- Lanciare "7 Calibrazione con tastiera"
- Leggere con un tester preciso la tensione della batteria Servizi
- Agire sulla calibrazione con i tasti t,y,u,i,o,p in modo che La tensione letta su SERVIZI sia uguale alla tensione reale (T-100/Y-10/U-1 I+1/O+10/P+100 bit)
- Ripetere per tensione della batteria AVVIAMENTO con i tasti f,g,h,j,k,l (F-100/G-10/H-1 J+1/K+10/L+100 bit) 
- Salvare uscendo con Q (maiuscolo)
- Resettare con 0

******************************************
## 🧰 MODIFICA DELLE SOGLIE DI INTERVENTO
- Entrare nel Menu (Nota1) digitando "Menu" ("M" maiuscola ed "u" senza accento)
- Lanciare ""8 Taratura soglie da tastiera""
- Modificare la sogia bassa con i tasti t,y,u,i,o,p (T-100/Y-10/U-1 I+1/O+10/P+100 bit)
- Modificare la sogia alta con i tasti f,g,h,j,k,l (F-100/G-10/H-1 J+1/K+10/L+100 bit) 
- Salvare uscendo con Q (maiuscolo)
- Resettare con 0

******************************************
## 🧰 PONTICELLI
- Posizione D4 : default NON INSERITO=10 secondi, INSERITO=1 secondo; Determina il periodo di verifica tensione batteria servizi e azionamento relè
- Posizione D2 : default INSERITO=stampa normale, NON INSERITO=stampa dettagliata per debug  
---

## 🔌 Schema di funzionamento


