--! @file myuart_AXI.vhd 
--! @authors <b> Giorgio Farina<b> <giorgio.fari96@gmail.com> <br> 
--!    <b> Luca Giamattei<b>  <lgiamattei@gmail.com> <br> 
--!    <b> Gabriele Previtera<b>  <gabrieleprevitera@gmail.com> <br> 
--! @date 12/06/2020 
--! 
--! @addtogroup myUart_AXI 
--! @{ 
 
library ieee; 
use ieee.std_logic_1164.all; 
use ieee.numeric_std.all; 
 
--! @brief Periferica AXI4 Lite che implementa un UART pilotabile da processing-system, con la  
--! possibilità di abilitare l'interrupt in lettura scegliendone la modalità di funzionameto tra le tre 
--! disponibili: 
--!    <ul> 
--!     <li>Interrupt sul livello del pin, fin quando il pin in ingresso è '1', verranno 
--!      generate interruzioni consecutive sulla linea; 
--!     </li> 
--!     <li>Interrupt sul fronte di salita, quando c'è una variazione '0' ->'1' del  
--!      pin di ingresso verrà generata un'interruzione sulla linea;</li> 
--!     <li>Interrupt sul fronte di discesa, quando c'è una variazione '1' ->'0' del  
--!      pin di ingresso verrà generata un'interruzione sulla linea;</li> 
--!    </ul> 
--! @details 
--! 
--! <h4>Registri interni del device</h4> 
--! Per ogni registro viene indicato:  
--!    <ul> 
--!     <li>Modalità di lettura 
--!      <ul> 
--!       <li>R: sola lettura</li> 
--!       <li>W: sola scrittura</li> 
--!       <li>R/W: lettura scrittura</li> 
--!      </ul>  
--!     </li> 
--!     <li>L'offset: rispetto al base address della periferica, per accedere al registro</li> 
--!    </ul>   
--! <br> 
--! Il device possiede i seguenti registri:  
--!  - <b>MODE</b> (R/W, offset +0x0): consente di impostare i singoli pin del device come ingressi o uscite; solo i 
--!    GPIO_width bit meno significativi del registro hanno significato, agire sui restanti bit non produce 
--!    nessun effetto; Il valore che i singoli pin possono 
--!    assumere sono: 
--!   - '1': il pin viene configurato come pin di uscita; 
--!   - '0': il pin viene configurato come pin di ingresso; 
--!  
--!  - <b>WRITE</b> (R/W, offset +0x4): consente di imporre un valore ai pin del device, qualora essi siano configurati 
--!    come uscite; solo i GPIO_width bit meno significativi del hanno significato, agire sui restanti bit non produce 
--!    nessun effetto; 
--!  - <b>READ</b> (R, offset +0x8): consente di leggere il valore dei pin del device, indipendentemente dal mode;  
--!    solo i GPIO_width bit meno significativi del registro hanno significato, gli altri saranno fissi a 
--!    zero; 
--!  - <b>GIES</b> (Global Interrupt Enable/Status, R/W, offset 0xC): Consente di abilitare/disabilitare gli interrupt 
--!    globali della periferica; sono utilizzati solo i due bit meno significativi del registro: 
--!      - IE (bit 0): interrupt enable, abilita gli interrupt, può essere scritto e letto; se posto ad '1' 
--!     la periferica potrà generare interrupt a seconda di come viene configurata la modalità di interrupt 
--!     per il singolo bit; se posto a '0' il device non genererà mai interruzioni; 
--!   - IS (bit 1): interrupt status, settato dalla periferica ad '1' nel caso in cui la periferica abbia 
--!     generato interrupt; settato a '0' quando tutte le interruzioni sono servite; 
--!     replica del segnale "interrupt" diretto verso il processing-system, che segnala al GIC il  
--!     il sollevamento di un'interruzione. 
--!  - <b>PIE</b> (Pin Interrupt Enable, R/W, offset 0x10): consente di abilitare/disabilitare gli interrupt per i 
--!    singoli pin. Con GIES(0)='1' e MODE(n)='0' (interrupt globale abilitati e il pin  n-esimo è 
--!    configurato come input), se PIE(n)='1' allora il device ha generato un interrupt verso il 
--!    processing-system in seguito alla varizione dello stato del pin (variazione configurabile usando 
--!    i registri IRQ_MODE e IRQ_EDGE), mentre, se PIE(n)='0' non verrà generata nessuna interruzione; 
--!  - <b>IRQ</b> (Interrupt Request, R, offset 0x14): IRQ(n)='1' indica che il pin-ennessimo è la sorgente di interruzione;  
--!    i singoli bit del registro sono pilotati dall'Interrupt_Generator_Handler, che in base alla configurazione, 
--!    scelta in fase di setup, alza il segnale di interrupt sulla specifica linea; 
--!    la or-reduce di tale registro costituisce il flag "interrupt" (IS) di GIES. 
--!  - <b>IACK</b> (Interrupt Ack, W, offset 0x18): imponento IACK(n)='1' è possibile segnalare al device che 
--!    l'interruzione generata dal in n-esimo è stata servita; i bit IRQ(n) e IACK(n) saranno resettato automaticamente, 
--!    permettendo di gestire interrupt consecutive quando la linea funziona con un interrupt sul livello del pin. 
--!  - <b>IRQ_MODE</b> (Interrupt Mode, R/W, offset 0x1C): seleziona la modalità con vengono generate le interruzioni per singolo 
--!    pin, quando GIES(1) = '1' e MODE(n)='0' e PIE(n) = '1' se IRQ_MODE(n)='0' il dispositivo generara interruzzioni 
--!    consecutive (vengono abbassate da IACK(n)) fin quando READ(n) = '1', invece se IRQ_MODE(n)='1' il dispositivo 
--!    genera interruzzioni solo quando c'è una variazione del clock, il cui fronte è configurabile utilizzando il  
--!    registro IRQ_EDGE(n). 
--!  - <b>IRQ_EDGE</b> (Interrupt EDGE, R/W, offset 0x1F): indica il fronte su cui è sensibile il dispositivo per la  
--!    generazione dell'interrupt; tale bit assume significato nel sollevamento dell'eccezzione solo se  
--!    il pin è configurato in modo tale da sollevare interruzioni sulla variazione del fronte, 
--!    ovvero se GIES(1) = '1' e MODE(n)='0' e PIE(n) = '1' e IRQ_MODE(n) '1'; 
--!    quando il dispositivo è sensibile sul fronte se IRQ_EDGE(n) = '0' l'interrupt si alza quando si verifica un  
--!    fronte di discesa; 
--!    quando il dispositivo è sensibile sul fronte e IRQ_EDGE(n) = '1' l'interrupt si alza quando si verifica un  
--!    fronte di salita; 
--! <br> 
--! 
--! 
--! 
--! <h4>Riepilogo modalità di interruzzione in base alla configurazione dei registri</h4> 
--! <b>Nota:</b> - indica don't care. 
--! <table> 
--! <tr><th>GIES(1)</th><th>MODE(n)</th><th>PIE(n)</th><th>IRQ_MODE(n)</th><th>IRQ_EDGE(n)</th><th>Brief</th></tr> 
--! <tr><td>0</td><td>-</td><td>-</td><td>-</td><td>-</td><td>Interrupt disabilitato</td></tr> 
--! <tr><td>1</td><td>1</td><td>-</td><td>-</td><td>-</td><td>Interrupt disabilitato</td></tr> 
--! <tr><td>1</td><td>0</td><td>0</td><td>-</td><td>-</td><td>Interrupt disabilitato</td></tr> 
--! <tr><td>1</td><td>0</td><td>1</td><td>0</td><td>-</td><td>Interrupt sul livello del pin n-esimo</td></tr> 
--! <tr><td>1</td><td>0</td><td>1</td><td>1</td><td>0</td><td>Interrupt sul fronte di discesa del pin n-esimo</td></tr> 
--! <tr><td>1</td><td>0</td><td>1</td><td>1</td><td>1</td><td>Interrupt sul fornte di salita del pin n-esimo</td></tr> 
--! </table> 
--! 
--! 
--! 
--! <h4>Process di scrittura dei registri della periferica</h4> 
--! Il process che implementa la logica di scrittura dei registri è stato modificato in modo da ottenere 
--! il seguente indirizzamento: 
--! <table> 
--! <tr><th>Indirizzo</th><th>Offset</th><th>Registro</th></tr> 
--! <tr><td>b"0000"</td><td>0x00</td><td>MODE</td></tr> 
--! <tr><td>b"0001"</td><td>0x04</td><td>WRITE</td></tr> 
--! <tr><td>b"0011"</td><td>0x0C</td><td>GIES(*)</td></tr> 
--! <tr><td>b"0100"</td><td>0x10</td><td>PIE</td></tr> 
--! <tr><td>b"0110"</td><td>0x18</td><td>IACK(**)</td></tr> 
--! <tr><td>b"0111"</td><td>0x1C</td><td>IRQ_MODE(**)</td></tr> 
--! <tr><td>b"1000"</td><td>0x1F</td><td>IRQ_EDGE(**)</td></tr> 
--! </table> 
--! (*) La scrittura ha effetto solo sul bit zero del registro;<br> 
--! (**)  La scrittura su IACK è fittizzia, nel senso che appena si smette di indirizzare il registro, 
--! esso assume valore zero;<br> 
--! 
--! 
--! <h4>Process di lettura dei registri della periferica</h4> 
--! Il process che implementa la logica di lettura dei registri è stato modificato in modo da ottenere 
--! il seguente indirizzamento: 
--! <table> 
--! <tr><th>Indirizzo</th><th>Offset</th><th>Registro</th></tr> 
--! <tr><td>b"0000"</td><td>0x00</td><td>MODE</td></tr> 
--! <tr><td>b"0001"</td><td>0x04</td><td>WRITE</td></tr> 
--! <tr><td>b"0010"</td><td>0x08</td><td>READ(*)</td></tr> 
--! <tr><td>b"0011"</td><td>0x0C</td><td>GIES(**)</td></tr> 
--! <tr><td>b"0100"</td><td>0x10</td><td>PIE</td></tr> 
--! <tr><td>b"0101"</td><td>0x14</td><td>IRQ</td></tr> 
--! <tr><td>b"0110"</td><td>0x18</td><td>IACK(***)</td></tr> 
--! <tr><td>b"0111"</td><td>0x1C</td><td>IRQ_MODE</td></tr> 
--! <tr><td>b"1000"</td><td>0x1F</td><td>IRQ_EDGE</td></tr> 
--! </table> 
--! (*) Il registro READ è direttamente connesso alla porta GPIO_inout<br> 
--! (**) Il bit 2 di GIES è il flag "interrupt", che vale '1' nel caso in cui la periferica abbia generato 
--! interrupt ancora non gestiti.<br> 
--! (***) Viene letto sempre zero, dal momento che la scrittura su tale registro è fittizzia. 
--! 
--! 
 
entity myUart_v1_0_S00_AXI is 
 generic ( 
  -- Users to add parameters here 
 
  -- User parameters ends 
  -- Do not modify the parameters beyond this line 
 
  -- Width of S_AXI data bus 
  C_S_AXI_DATA_WIDTH : integer := 32; 
  -- Width of S_AXI address bus 
  C_S_AXI_ADDR_WIDTH : integer := 4 
 ); 
 port ( 
  -- Users to add ports here 
        interrupt_1 : out std_logic; 
        interrupt_2 : out std_logic; 
         
         
        pin_tx : out std_logic; 
        pin_rx : in std_logic; 
         
  -- User ports ends 
  -- Do not modify the ports beyond this line 
 
  -- Global Clock Signal 
  S_AXI_ACLK : in std_logic; 
  -- Global Reset Signal. This Signal is Active LOW 
  S_AXI_ARESETN : in std_logic; 
  -- Write address (issued by master, acceped by Slave) 
  S_AXI_AWADDR : in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0); 
  -- Write channel Protection type. This signal indicates the 
      -- privilege and security level of the transaction, and whether 
      -- the transaction is a data access or an instruction access. 
  S_AXI_AWPROT : in std_logic_vector(2 downto 0); 
  -- Write address valid. This signal indicates that the master signaling 
      -- valid write address and control information. 
  S_AXI_AWVALID : in std_logic; 
  -- Write address ready. This signal indicates that the slave is ready 
      -- to accept an address and associated control signals. 
  S_AXI_AWREADY : out std_logic; 
  -- Write data (issued by master, acceped by Slave)  
  S_AXI_WDATA : in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0); 
  -- Write strobes. This signal indicates which byte lanes hold 
      -- valid data. There is one write strobe bit for each eight 
      -- bits of the write data bus.     
  S_AXI_WSTRB : in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0); 
  -- Write valid. This signal indicates that valid write 
      -- data and strobes are available. 
  S_AXI_WVALID : in std_logic; 
  -- Write ready. This signal indicates that the slave 
      -- can accept the write data. 
  S_AXI_WREADY : out std_logic; 
  -- Write response. This signal indicates the status 
      -- of the write transaction. 
  S_AXI_BRESP : out std_logic_vector(1 downto 0); 
  -- Write response valid. This signal indicates that the channel 
      -- is signaling a valid write response. 
  S_AXI_BVALID : out std_logic; 
  -- Response ready. This signal indicates that the master 
      -- can accept a write response. 
  S_AXI_BREADY : in std_logic; 
  -- Read address (issued by master, acceped by Slave) 
  S_AXI_ARADDR : in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0); 
  -- Protection type. This signal indicates the privilege 
      -- and security level of the transaction, and whether the 
      -- transaction is a data access or an instruction access. 
  S_AXI_ARPROT : in std_logic_vector(2 downto 0); 
  -- Read address valid. This signal indicates that the channel 
      -- is signaling valid read address and control information. 
  S_AXI_ARVALID : in std_logic; 
  -- Read address ready. This signal indicates that the slave is 
      -- ready to accept an address and associated control signals. 
  S_AXI_ARREADY : out std_logic; 
  -- Read data (issued by slave) 
  S_AXI_RDATA : out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0); 
  -- Read response. This signal indicates the status of the 
      -- read transfer. 
  S_AXI_RRESP : out std_logic_vector(1 downto 0); 
  -- Read valid. This signal indicates that the channel is 
      -- signaling the required read data. 
  S_AXI_RVALID : out std_logic; 
  -- Read ready. This signal indicates that the master can 
      -- accept the read data and response information. 
  S_AXI_RREADY : in std_logic 
 ); 
end myUart_v1_0_S00_AXI; 
 
architecture arch_imp of myUart_v1_0_S00_AXI is 
 
 -- AXI4LITE signals 
 signal axi_awaddr : std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0); 
 signal axi_awready : std_logic; 
 signal axi_wready : std_logic; 
 signal axi_bresp : std_logic_vector(1 downto 0); 
 signal axi_bvalid : std_logic; 
 signal axi_araddr : std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0); 
 signal axi_arready : std_logic; 
 signal axi_rdata : std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0); 
 signal axi_rresp : std_logic_vector(1 downto 0); 
 signal axi_rvalid : std_logic; 
 
 -- Example-specific design signals 
 -- local parameter for addressing 32 bit / 64 bit C_S_AXI_DATA_WIDTH 
 -- ADDR_LSB is used for addressing 32/64 bit registers/memories 
 -- ADDR_LSB = 2 for 32 bits (n downto 2) 
 -- ADDR_LSB = 3 for 64 bits (n downto 3) 
 constant ADDR_LSB  : integer := (C_S_AXI_DATA_WIDTH/32)+ 1; 
 constant OPT_MEM_ADDR_BITS : integer := 1; 
 ------------------------------------------------ 
 ---- Signals for user logic register space example 
 signal DBOUT_tmp :std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0); 
 signal DBIN_tmp  :std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0); 
 signal state_reg :std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0); 
 signal TBE_tmp      :std_logic; 
 signal IRQ_T        :std_logic; 
 signal control_reg :std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0); 
 
 -------------------------------------------------- 
 ---- Number of Slave Registers 4 
 signal slv_reg1 :std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0); 
 signal slv_reg3 :std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0); 
 signal slv_reg_rden : std_logic; 
 signal slv_reg_wren : std_logic; 
 signal reg_data_out :std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0); 
 signal byte_index : integer; 
 signal aw_en : std_logic; 
  
 component uart_se is 
    Port (  
  TXD  : out std_logic   := '1'; 
     RXD  : in  std_logic;      
     CLK  : in  std_logic;        --Master Clock 
  DBIN  : in  std_logic_vector (7 downto 0); --Data Bus in 
  DBOUT : out std_logic_vector (7 downto 0); --Data Bus out 
  RDA : inout std_logic;      --Read Data Available 
  TBE : inout std_logic  := '1';   --Transfer Bus Empty 
  RD  : in  std_logic;     --Read Strobe 
  WR  : in  std_logic;     --Write Strobe 
  PE  : out std_logic;     --Parity Error Flag 
  FE  : out std_logic;     --Frame Error Flag 
  OE  : out std_logic;     --Overwrite Error Flag 
  RST1  : in  std_logic := '0'); --Master Reset 
     end component; 
  
 
begin 
 -- I/O Connections assignments 
 
 S_AXI_AWREADY <= axi_awready; 
 S_AXI_WREADY <= axi_wready; 
 S_AXI_BRESP <= axi_bresp; 
 S_AXI_BVALID <= axi_bvalid; 
 S_AXI_ARREADY <= axi_arready; 
 S_AXI_RDATA <= axi_rdata; 
 S_AXI_RRESP <= axi_rresp; 
 S_AXI_RVALID <= axi_rvalid; 
 -- Implement axi_awready generation 
 -- axi_awready is asserted for one S_AXI_ACLK clock cycle when both 
 -- S_AXI_AWVALID and S_AXI_WVALID are asserted. axi_awready is 
 -- de-asserted when reset is low. 
 
 process (S_AXI_ACLK) 
 begin 
   if rising_edge(S_AXI_ACLK) then  
     if S_AXI_ARESETN = '0' then 
       axi_awready <= '0'; 
       aw_en <= '1'; 
     else 
       if (axi_awready = '0' and S_AXI_AWVALID = '1' and S_AXI_WVALID = '1' and aw_en = '1') then 
         -- slave is ready to accept write address when 
         -- there is a valid write address and write data 
         -- on the write address and data bus. This design  
         -- expects no outstanding transactions.  
            axi_awready <= '1'; 
            aw_en <= '0'; 
         elsif (S_AXI_BREADY = '1' and axi_bvalid = '1') then 
            aw_en <= '1'; 
            axi_awready <= '0'; 
       else 
         axi_awready <= '0'; 
       end if; 
     end if;
end if; 
 end process; 
 
 -- Implement axi_awaddr latching 
 -- This process is used to latch the address when both  
 -- S_AXI_AWVALID and S_AXI_WVALID are valid.  
 
 process (S_AXI_ACLK) 
 begin 
   if rising_edge(S_AXI_ACLK) then  
     if S_AXI_ARESETN = '0' then 
       axi_awaddr <= (others => '0'); 
     else 
       if (axi_awready = '0' and S_AXI_AWVALID = '1' and S_AXI_WVALID = '1' and aw_en = '1') then 
         -- Write Address latching 
         axi_awaddr <= S_AXI_AWADDR; 
       end if; 
     end if; 
   end if;                    
 end process;  
 
 -- Implement axi_wready generation 
 -- axi_wready is asserted for one S_AXI_ACLK clock cycle when both 
 -- S_AXI_AWVALID and S_AXI_WVALID are asserted. axi_wready is  
 -- de-asserted when reset is low.  
 
 process (S_AXI_ACLK) 
 begin 
   if rising_edge(S_AXI_ACLK) then  
     if S_AXI_ARESETN = '0' then 
       axi_wready <= '0'; 
     else 
       if (axi_wready = '0' and S_AXI_WVALID = '1' and S_AXI_AWVALID = '1' and aw_en = '1') then 
           -- slave is ready to accept write data when  
           -- there is a valid write address and write data 
           -- on the write address and data bus. This design  
           -- expects no outstanding transactions.            
           axi_wready <= '1'; 
       else 
         axi_wready <= '0'; 
       end if; 
     end if; 
   end if; 
 end process;  
 
 -- Implement memory mapped register select and write logic generation 
 -- The write data is accepted and written to memory mapped registers when 
 -- axi_awready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted. Write strobes are used to 
 -- select byte enables of slave registers while writing. 
 -- These registers are cleared when reset (active low) is applied. 
 -- Slave register write enable is asserted when valid address and data are available 
 -- and the slave is ready to accept the write address and write data. 
 slv_reg_wren <= axi_wready and S_AXI_WVALID and axi_awready and S_AXI_AWVALID ; 
 
 process (S_AXI_ACLK) 
 variable loc_addr :std_logic_vector(OPT_MEM_ADDR_BITS downto 0);  
 begin 
   if rising_edge(S_AXI_ACLK) then  
     if S_AXI_ARESETN = '0' then 
       DBIN_tmp <= (others => '0'); 
       slv_reg1 <= (others => '0'); 
       control_reg <= (others => '0'); 
       slv_reg3 <= (others => '0'); 
     else 
        control_reg(3) <= '0'; 
      
       loc_addr := axi_awaddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB); 
       if (slv_reg_wren = '1') then 
         case loc_addr is 
           when b"00" => 
             for byte_index in 0 to (C_S_AXI_DATA_WIDTH/8-1) loop 
               if ( S_AXI_WSTRB(byte_index) = '1' ) then 
                 -- Respective byte enables are asserted as per write strobes                    
                 -- slave registor 0 
                 DBIN_tmp(byte_index*8+7 downto byte_index*8) <= S_AXI_WDATA(byte_index*8+7 downto byte_index*8); 
               end if; 
             end loop; 
           when b"01" => 
             for byte_index in 0 to (C_S_AXI_DATA_WIDTH/8-1) loop 
               if ( S_AXI_WSTRB(byte_index) = '1' ) then 
                 -- Respective byte enables are asserted as per write strobes                    
                 -- slave registor 1 
                 slv_reg1(byte_index*8+7 downto byte_index*8) <= S_AXI_WDATA(byte_index*8+7 downto byte_index*8); 
               end if; 
             end loop; 
           when b"10" => 
             for byte_index in 0 to (C_S_AXI_DATA_WIDTH/8-1) loop 
               if ( S_AXI_WSTRB(byte_index) = '1' ) then 
                 -- Respective byte enables are asserted as per write strobes                    
                 -- slave registor 2 
                 control_reg(byte_index*8+7 downto byte_index*8) <= S_AXI_WDATA(byte_index*8+7 downto byte_index*8); 
               end if; 
             end loop; 
           when b"11" => 
             for byte_index in 0 to (C_S_AXI_DATA_WIDTH/8-1) loop 
               if ( S_AXI_WSTRB(byte_index) = '1' ) then 
                 -- Respective byte enables are asserted as per write strobes                    
                 -- slave registor 3 
                 slv_reg3(byte_index*8+7 downto byte_index*8) <= S_AXI_WDATA(byte_index*8+7 downto byte_index*8); 
               end if; 
             end loop; 
           when others => 
             DBIN_tmp <= DBIN_tmp; 
             slv_reg1 <= slv_reg1; 
             control_reg <= control_reg; 
             slv_reg3 <= slv_reg3; 
         end case; 
       end if; 
     end if; 
   end if;                    
 end process;  
 
 -- Implement write response logic generation 
 -- The write response and response valid signals are asserted by the slave  
 -- when axi_wready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted.   
 -- This marks the acceptance of address and indicates the status of  
 -- write transaction. 
 
 process (S_AXI_ACLK) 
 begin 
   if rising_edge(S_AXI_ACLK) then  
     if S_AXI_ARESETN = '0' then 
       axi_bvalid  <= '0'; 
       axi_bresp   <= "00"; --need to work more on the responses 
     else 
       if (axi_awready = '1' and S_AXI_AWVALID = '1' and axi_wready = '1' and S_AXI_WVALID = '1' and axi_bvalid = '0'  ) then 
         axi_bvalid <= '1'; 
         axi_bresp  <= "00";  
       elsif (S_AXI_BREADY = '1' and axi_bvalid = '1') then   --check if bready is asserted while bvalid is high) 
         axi_bvalid <= '0';                                 -- (there is a possibility that bready is always asserted high) 
       end if; 
     end if; 
   end if;                    
 end process;  
 
 -- Implement axi_arready generation 
 -- axi_arready is asserted for one S_AXI_ACLK clock cycle when 
 -- S_AXI_ARVALID is asserted. axi_awready is  
 -- de-asserted when reset (active low) is asserted.  
 -- The read address is also latched when S_AXI_ARVALID is  
 -- asserted. axi_araddr is reset to zero on reset assertion. 
 
 process (S_AXI_ACLK) 
 begin 
   if rising_edge(S_AXI_ACLK) then  
     if S_AXI_ARESETN = '0' then 
       axi_arready <= '0'; 
       axi_araddr  <= (others => '1'); 
     else 
       if (axi_arready = '0' and S_AXI_ARVALID = '1') then 
         -- indicates that the slave has acceped the valid read address 
         axi_arready <= '1'; 
         -- Read Address latching  
         axi_araddr  <= S_AXI_ARADDR;            
       else 
         axi_arready <= '0'; 
       end if; 
     end if; 
   end if;                    
 end process;  
 
 -- Implement axi_arvalid generation 
 -- axi_rvalid is asserted for one S_AXI_ACLK clock cycle when both  
 -- S_AXI_ARVALID and axi_arready are asserted. The slave registers  
 -- data are available on the axi_rdata bus at this instance. The  
 -- assertion of axi_rvalid marks the validity of read data on the  
 -- bus and axi_rresp indicates the status of read transaction.axi_rvalid  
 -- is deasserted on reset (active low). axi_rresp and axi_rdata are  
 -- cleared to zero on reset (active low).   
 process (S_AXI_ACLK) 
 begin 
   if rising_edge(S_AXI_ACLK) then 
     if S_AXI_ARESETN = '0' then 
       axi_rvalid <= '0'; 
       axi_rresp  <= "00"; 
     else 
       if (axi_arready = '1' and S_AXI_ARVALID = '1' and axi_rvalid = '0') then 
         -- Valid read data is available at the read data bus 
         axi_rvalid <= '1'; 
         axi_rresp  <= "00"; -- 'OKAY' response 
       elsif (axi_rvalid = '1' and S_AXI_RREADY = '1') then 
         -- Read data is accepted by the master 
         axi_rvalid <= '0'; 
       end if;             
     end if; 
   end if; 
 end process; 
 
 -- Implement memory mapped register select and read logic generation 
 -- Slave register read enable is asserted when valid address is available 
 -- and the slave is ready to accept the read address. 
 slv_reg_rden <= axi_arready and S_AXI_ARVALID and (not axi_rvalid) ; 
 
 process (DBIN_tmp, slv_reg1, control_reg, slv_reg3, axi_araddr, S_AXI_ARESETN, slv_reg_rden) 
 variable loc_addr :std_logic_vector(OPT_MEM_ADDR_BITS downto 0); 
 begin 
     -- Address decoding for reading registers 
     loc_addr := axi_araddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB); 
     case loc_addr is 
       when b"00" => 
         reg_data_out <= DBIN_tmp; 
       when b"01" => 
         reg_data_out <= DBOUT_tmp; 
       when b"10" => 
         reg_data_out <= control_reg; 
       when b"11" => 
         reg_data_out <= state_reg; 
       when others => 
         reg_data_out  <= (others => '0'); 
     end case; 
 end process;  
 
 -- Output register or memory read data 
 process( S_AXI_ACLK ) is 
 begin 
   if (rising_edge (S_AXI_ACLK)) then 
     if ( S_AXI_ARESETN = '0' ) then 
       axi_rdata  <= (others => '0'); 
     else 
       if (slv_reg_rden = '1') then 
         -- When there is a valid read address (S_AXI_ARVALID) with  
         -- acceptance of read address by the slave (axi_arready),  
         -- output the read dada  
         -- Read address mux 
           axi_rdata <= reg_data_out;     -- register read data 
       end if;    
     end if; 
   end if; 
 end process; 
 
 
 -- Add user logic here 
     
     uart_inst:  uart_se  
            Port map(  
                TXD  => pin_tx, 
                RXD  => pin_rx,   
                CLK  => S_AXI_ACLK,    --Master Clock 
                DBIN    => DBIN_tmp(7 downto 0),                   --Data Bus in 
                DBOUT  => DBOUT_tmp(7 downto 0),                --Data Bus out 
                RDA  => state_reg(0),       --Read Data Available 
                TBE     => state_reg(1),          --Transfer Bus Empty 
                RD  => control_reg(0),       --Read Strobe 
                WR  => control_reg(1),        --Write Strobe 
                PE  => state_reg(2),                --Parity Error Flag 
                FE  => state_reg(3),             --Frame Error Flag 
                OE  => state_reg(4),             --Overwrite Error Flag 
                RST1    => S_AXI_ARESETN);                     --Master Reset 
     
    process (state_reg(1), control_reg(3), control_reg(2)) 
    begin 
    if rising_edge(state_reg(1)) then 
        IRQ_T <= '1' ; 
    end if; 
     
    if control_reg(3) = '1' then 
        IRQ_T <= '0'; 
    end if; 
    end process; 
     
    interrupt_1 <= state_reg(0) and control_reg(2); 
    interrupt_2 <= IRQ_T and control_reg(2); 
     
 -- User logic ends 
 
end arch_imp; 
 
--! @}