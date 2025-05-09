# Camper_energy_saver
Quando la batteria della cellula abitativa supera i 13,6V caricata tramite i pannelli solari, mi attiva un relè che mi fa passare la corrente oramai in esubero verso la batteria motore, mantenendola così sempre in carica.
sketch completo e aggiornato, con:

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
	
	
 Esempio output seriale ogni 60s:
----------
VBAT (ADC): 734 → 12.00 V
Soglia bassa (A1): 620
Soglia alta (A0): 720
VBAT nella zona neutra → nessuna azione
Contatore reset watchdog attuale: 3	
