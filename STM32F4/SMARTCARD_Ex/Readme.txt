

@page SMARTCARD_EX   
 

@par Example Description 

Questo esempio descrive la trasmissione UART in modalità Smartcard tra due boards 
All'inizio del main program l'HAL-Init() è chiamata per resettare tutte le periferiche, inizializzare la flash interface, e Systick.
La funzione SystemClock:config() è usata per configurare il clock del sistema /SYSCLK) per eseguire a 25 MHz.
La Transmitter board è in sleep mode finchè l'user button non è premuto. 
La pressione del bottone, gestita con interrupt, farà partire la trasmissione con la seocnda board per poi mettersi in attesa di recezione.
La Receiver Board è in Sleep Mode finchè non viene svegliata da un interrupt per la ricezione.
Dopo aver ricevuto, trasmetterà un carattere verso la Receiver Board.
I LED della board sono utilizzati per monitorare lo stato dell'esecuzione:
- LED 6 (Blu) è ON quando il processo di trasmissione è completo
- LED 5 (Rosso) è ON quando è stato ricevuto il messaggio che ci si aspettava
- LED 4 (Verde) è ON quando il processo di ricezione del carattere inviato dalla controparte è completo
- LED 3 (Arancione) è utilizzatoper indicare che il codice è in esecuzione sulla transmitter board. 
	Quando si spegne indica che è inizato il processo d'invio e di ricezione e quando si riaccende indica che al board è pronta per inviare di nuovo il messaggio .

L'UART  in modalità Smartcard è stato configurato come segue:
    - BaudRate = 115200 baud  
    - Word Length = 8 Bits (7 data bit + 1 parity bit)
    - 1.5 Stop Bit
    - bit di parità pari
    - Hardware flow control disabled (RTS and CTS signals)
Il canale è Half Duplex in modalità Smartcard sul pin PA2.


