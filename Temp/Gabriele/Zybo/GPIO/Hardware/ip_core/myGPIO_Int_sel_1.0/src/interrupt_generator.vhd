

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity Interrupt_Generator is
  Port (
    GPIO_INOUT: in std_logic;
    IACK:       in std_logic;
    CLK:        in std_logic;
    RESET:      in std_logic;
    MODE:       in std_logic;
    EDGE:       in std_logic;
    INT:        out std_logic
   );
end Interrupt_Generator;


architecture Behavioral of interrupt_generator is
    signal state: std_logic := '0';
    signal int_tmp: std_logic := '0';
    signal next_state: std_logic := '0';
    signal next_int_tmp: std_logic := '0';
begin 
    process(CLK, RESET) begin   
        if RESET='0' then
            state <= '0';
            int_tmp <= '0';

        elsif rising_edge(CLK) then
            state <= next_state;
            int_tmp<= next_int_tmp;
        end if;
    end process;
        
    process(GPIO_INOUT, MODE, EDGE, IACK, state, int_tmp) begin
        next_int_tmp <= int_tmp;
         
        if ((((not state) and GPIO_INOUT ) and MODE) and EDGE) = '1' then
            next_int_tmp<= '1';
                    
        elsif ((state and (not GPIO_INOUT ) and MODE) and (not EDGE)) = '1' then
            next_int_tmp <= '1';
        
        elsif MODE = '0' then
            next_int_tmp <= GPIO_INOUT;
        end if;
            
        if IACK = '1' then
            next_int_tmp <= '0';
        end if;
            
        next_state<= GPIO_INOUT;
            
    end process;
    
    INT <= next_int_tmp;



end Behavioral;
