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

---

## 🔌 Schema di funzionamento


