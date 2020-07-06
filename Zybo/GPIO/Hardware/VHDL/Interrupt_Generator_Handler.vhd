--! @file Interrupt_Generator_Handler.vhd
--! @authors <b> Giorgio Farina<b> <giorgio.fari96@gmail.com> <br>
--!			 <b> Luca Giamattei<b>  <lgiamattei@gmail.com> <br>
--!			 <b> Gabriele Previtera<b>  <gabrieleprevitera@gmail.com> <br>
--! @date 11/06/2020
--!  
--! @addtogroup myGPIO_Interrupt_Selector
--! @{
--! @addtogroup Interrupt_Generator_Handler
--! @{

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

--! @brief Periferica che implementa la logica di sollevamento del segnale di interrupt di un segnale 
--! di ingresso per un GPIO, in base a una delle modalità scelte in sede di configurazione dall'utente.
--!
--! @details
--! La periferica implementa la logica per un solo segnale di ingresso, e in base ai bit MODE e EDGE
--! si seleziona una delle tre possibili modalità di funzionamento.
--! <b>Nota</b>: in questa periferica non si tiene conto del bit <b>GIES</b> e del pin di <b>PIE</b>
--! per semplificare la logica, pertanto è necessario tener presente questi bit prima di porre il 
--! segnale di pin in ingresso alla periferica che genera l'interrupt. Pertanto nel seguito del 
--! documento quando si farà riferimento a pin di ingresso si intederà quello che viene dalla
--! top-level che usa il componente in questione e non al pin fisico.
--! 
--! Le tre possibili modalità di funzionamento sono:
--!	    <ul>
--!         <li>Interrupt sul livello del pin, fin quando il pin in ingresso è '1', verranno
--!	            generate interruzioni consecutive sulla linea;
--!	        </li>
--!	        <li>Interrupt sul fronte di salita, quando c'è una variazione '0' ->'1' del 
--!	            pin di ingresso verrà generata un'interruzione sulla linea;</li>
--!	        <li>Interrupt sul fronte di discesa, quando c'è una variazione '1' ->'0' del 
--!             pin di ingresso verrà generata un'interruzione sulla linea.</li>
--!	    </ul>
--! <h4>Process di aggiornamento del bit di interrupt</h4>
--! La logica è stata implementata utilizzando una FSM, il cui obiettivo principale è
--! tener traccia della delle variazioni di stato quando il pin è configurato per
--! essere sensibile sul fronte. 
--! In base alla modalità le logiche per cui si alza il bit d'interrupt sono le seguenti:
--! - Modalità a <b>livelli</b>: se il GPIO_INOUT è alto e MODE = '0' 
--!     viene generto il segnale di interrupt;
--! - Modalità sensibile sul fronte di <b>salita</b>: se il GPIO_INOUT era basso e è diventato 
--!     alto e (MODE and EDGE) = '1' viene generto il segnale di interrupt;
--! - Modalità sensibile sul fronte di <b>discesa</b>: se il GPIO_INOUT era alto e è diventato 
--!     basse e (MODE and not EDGE) = '1' viene generto il segnale di interrupt.
--! <br>
--! Inoltre nella FSM lo stato del bit di interrupt varia soltanto nei seguenti casi:
--! -   Si alza quando si verifica la condizione di interrupt scelta dall'utente;
--! -   Si abbassa quando IACK viene alzato.
--! 
entity Interrupt_Generator_Handler is
  Port (
        GPIO_INOUT: in std_logic;   --! GPIO read value
        IACK:       in std_logic;   --! Clear Interrupt
        CLK:        in std_logic;   --! Clock
        RESET:      in std_logic;   --! Reset
        MODE:       in std_logic;   --! Interrupt mode : '0' level, '1' edge
        EDGE:       in std_logic;   --! EDGE : '0' falling edge, '1' rising edge
        INT:        out std_logic   --! Interrupt
    );
end Interrupt_Generator_Handler;

architecture Behavioral of Interrupt_Generator_Handler is
    signal state:           std_logic := '0';
    signal int_tmp:         std_logic := '0';
    signal next_state:      std_logic := '0';
    signal next_int_tmp:    std_logic := '0';
begin 
    -- process di aggiornamento dello stato
    process(CLK, RESET) begin   
        if RESET='0' then
            state   <= '0';
            int_tmp <= '0';
            next_state  <= '0';
            next_int_tmp <= '0';
        elsif rising_edge(CLK) then
            state   <= next_state;
            int_tmp <= next_int_tmp;
        end if;
    end process;
    
    -- process di determinazione dell'uscita e dello stato prossimo 
    process(GPIO_INOUT, MODE, EDGE, IACK, state, int_tmp) begin
        -- mantengo lo stato precedente se non è aggiornato
        next_int_tmp <= int_tmp;
         
        -- logica che alza il segnale di interrupt in base alla modalità di funzionamento

        -- rising edge
        if ((((not state) and GPIO_INOUT ) and MODE) and EDGE) = '1' then
            next_int_tmp <= '1';
        -- falling edge
        elsif ((state and (not GPIO_INOUT ) and MODE) and (not EDGE)) = '1' then
            next_int_tmp <= '1';
        -- level
        elsif MODE = '0' then
            next_int_tmp <= GPIO_INOUT;
        end if;
        
        -- Reset del segnale di interrupt
        if IACK = '1' then
            next_int_tmp <= '0';
        end if;
        
        -- se è variato il livello del pin in ingresso, allora aggiorna lo stato
        -- in modo tale da poter capire se si verificano altre variazioni 
        next_state <= GPIO_INOUT;
            
    end process;
    
    -- il segnale di uscita segue il valore di next_int_tmp
    INT <= next_int_tmp;

end Behavioral;
