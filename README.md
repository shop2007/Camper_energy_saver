# Camper Energy Saver

**Camper Energy Saver** è un sistema elettronico pensato per ottimizzare l'uso dell'energia elettrica a bordo di un camper. Il progetto permette di monitorare in tempo reale corrente e tensione della batteria, proteggere i carichi in caso di sovracorrente, inviare notifiche via SMS e testare l’impianto tramite un menù guidato.

---

## ⚙️ Funzionalità principali

- Lettura di corrente e tensione con sensore LEM
- Gestione soglia corrente per protezione sovraccarichi
- Controllo automatico di un relè di scollegamento carico
- Display LCD I2C 16x2 per visualizzazione locale
- Integrazione con modulo GSM SIM800 per comandi e notifiche SMS
- Comandi remoti: stato, reset, attivazione test, gestione rubrica
- Menu di collaudo guidato (test 1.A, 1.B ecc.)
- Dati persistenti su EEPROM (rubrica, soglie, contatori)
- Watchdog per sicurezza operativa

---

## 🧰 Componenti hardware richiesti

- **Arduino Uno / Nano (ATmega328)**
- **Sensore di corrente LEM** (es. HASS50)
- **Modulo SIM800L GSM**
- **Display LCD 16x2 con interfaccia I2C**
- **Relè 5V**
- **Trimmer** per lettura tensione batteria
- **Resistenze di pull-up/pull-down** dove richiesto
- **Eventuale pulsante** per menu manuale/test
- **Alimentazione 12V camper**

---

## 🖼️ Schema elettrico

<p align="center">
  <img src="immagini/schema_camper_energy_saver.png" alt="Schema elettrico" width="600">
</p>

*(Assicurati che l'immagine sia presente nella cartella `immagini/` del repository.)*

---

## 📸 Foto prototipo montato

<p align="center">
  <img src="immagini/foto_montaggio_reale.jpg" alt="Montaggio reale" width="600">
</p>

---

## 🧪 Menù di test e collaudo

Il sistema include un menù test guidato, accessibile da seriale o via SMS:

- **1.A** – Verifica accensione display
- **1.B** – Verifica lettura corrente
- **1.C** – Verifica tensione batteria
- **2.A** – Test attivazione relè
- **2.B** – Verifica invio SMS test

I comandi disponibili via seriale includono:  
`AVANTI`, `RIPETI`, `SALTA`, `ESEGUI`  
oppure selezione diretta con `1.A`, `2.B`, ecc.

---

## 📱 Comandi SMS

| Comando             | Descrizione                                           |
|---------------------|-------------------------------------------------------|
| `STATO`             | Ricevi stato attuale (corrente, tensione, ecc.)       |
| `RESET`             | Azzera i contatori di tempo e corrente                |
| `TESTRELE`          | Attiva il relè per pochi secondi per test            |
| `PIN1234`           | Autenticazione (sostituire 1234 con il PIN impostato) |
| `HELP`              | Elenco comandi disponibili                            |
| `AGGIUNGI +39xxxxxx`| Aggiunge numero alla rubrica (solo admin)            |

---

## 🔧 Installazione

1. **Clona il progetto**:
   ```bash
   git clone https://github.com/shop2007/Camper_energy_saver.git
