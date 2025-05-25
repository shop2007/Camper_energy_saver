# Camper Energy Saver

**Camper Energy Saver** è un sistema di gestione automatica della ricarica solare nei camper. Il dispositivo controlla la tensione della **batteria servizi** e, quando questa è completamente carica, **commuta il caricatore solare** verso la **batteria di avviamento**, evitando lo scaricamento di quest'ultima durante le soste prolungate.

Lo stato del sistema viene trasmesso in tempo reale tramite **Bluetooth (HC-05)** a uno smartphone Android tramite l’app gratuita **“Bluetooth Terminal”**.

---

## ⚙️ Funzionalità principali

- Monitoraggio continuo della **tensione batteria servizi**
- Commutazione automatica della carica solare:
  - Priorità alla batteria servizi
  - Passaggio alla batteria motore quando la batteria servizi è carica
- Segnalazione dello stato via **Bluetooth (HC-05)**
- Compatibilità con l'app Android **Bluetooth Terminal**
- Configurazione soglie e pin semplice tramite codice

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
nota: i trimmer sono usati per calibrare e poi torneranno a fare la funzione di soglia alta e soglia bassa
- Entra nel Menu
- lancia 6 Calibrazione
- Leggere con un tester preciso la tensione Servizi
- Regolare il trimmer di destra RV1 in modo che La tensione letta su Calib SRV sia uguale alla tensione reale
- Ripetere per tensione motore
- Salvare uscendo con Q (maiuscolo)
- Resettare con 0
- A questo punto occorre tarare i due trimmer per la soglia desiderata 
  Trimmer RV2 sinistra = soglia bassa
  Trimmer RV1 destra = soglia alta

---

## 🔌 Schema di funzionamento


