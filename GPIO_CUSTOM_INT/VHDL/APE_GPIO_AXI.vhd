----------------------------------------------------------------------------------
--! @file   APE_GPIO_AXI.vhd
--  Company: Gruppo 5
--! @author Alfonso, Pierluigi, Erasmo (APE)
--!
--! @date 20.06.2017 20:15:38
--!
--! @addtogroup APE_GPIO
--! @{
--! @addtogroup APE_GPIO_AXI
--! @{
--! @mainpage
--! @brief Periferica GPIO custom su bus AXI 4 Lite.
--!
--! @details
--!	<br>La periferica GPIO fornisce 5 registri di 32 bit. Mediante il
--!	Il parametro <b>width</b> si stabilisce quanti di questi 32 bit devono
--!	essere effettivamente utilizzati.
--!
--! <h3><b>REGISTRI:</b></h3>
--! <table>
--! <tr><th>Offset</th><th>Nome Registro</th><th>Funzione 								   </th></tr>
--! <tr><td>0x00</td><td>DATA</td><td>Registro dato della periferica                       </td></tr>
--! <tr><td>0x08</td><td>DIR </td><td>Registro Abilitazione Interrupt sul fronte di salita </td></tr>
--! <tr><td>0x0C</td><td>IERR</td><td>Registro Abilitazione Interrupt sul fronte di discesa</td></tr>
--! <tr><td>0x10</td><td>IERF</td><td>Registro Stato/Controllo interrupt                   </td></tr>
--! </table>
--!
--! - <br><b>DATA</b>: Acceduto sia in lettura che in scrittura all'offset 0x00. Contiene i dati da scrivere
--!       verso la periferica e provenienti dalla periferica.
--!       Il bit i-esimo di questo registro corrisponde all'i-esimo pin GPIO.
--!
--! - <br><b>DIR</b>:  Acceduto all'offset 0x04. Setta la direzione del buffer tristate:
--!       <br>Il bit i-esimo di questo registro corrisponde all'i-esimo pin GPIO.
--!         <br>'1': direzione lettura dal pin GPIO.
--!         <br>'0': direzione scrittura verso pin GPIO.
--!
--! - <br><b>IERR</b>: Acceduto all'offset 0x08Settando l'i-esimo bit a '1' si
--!					  abilita l'interrupt sul medesimo pin GPIO sul fronte di salita.
--!
--! - <br><b>IERF</b>: Acceduto all'offset 0x0C. Settando l'i-esimo bit a '1' si abilita l'interrupt sul medesimo pin GPIO sul fronte di discesa.
--!
--! - <br><b>ISR</b>:  Accedendo in lettura all'offset 0x10 viene riportato il registro ISR:
--!       ogni bit segnala l'eventuale interrupt pendente sul corrispondente pin GPIO.
--!
--! - <br><b>ICR</b>:  Accedendo in scrittura all'offset 0x10 si scrive sul registro ICR:
--!       scrivendo '1' su un bit, se presente un'interrupt pendente al corrispondente pin su ISR, questa viene
--!       azzerata. Scrivere '0' è ininfluente.
--!		  Il dato scritto su ICR viene cancellato al successivo colpo di clock,
--!       pertanto il registro è sempre nullo. Per fare ciò è stata aggiunta la riga "slv_reg4 <= (others => '0');"
--!		  nel PROCESS 4 della Xilinx.
--!
--!		  <br>Ad esempio si ha: ISR:"0x00000F01"
--!		  <br>Scrivendo: 		ICR:"0x00000111"
--!		  <br>Si ottiene: 		ISR:"0x00000E00"
--!
--!	La logica di gestione dei registri ICR e ISR e' implementata dal process <b>ICRISR_management</b>.
----------------------------------------------------------------------------------

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

--! la std_logic.misc permette di usare la funzione or_reduce
use ieee.std_logic_misc.all;

entity APE_GPIO_AXI is
	generic (
		-- Users to add parameters here
        width : natural := 4;
		-- User parameters ends
		-- Do not modify the parameters beyond this line

		-- Width of S_AXI data bus
		C_S_AXI_DATA_WIDTH	: integer	:= 32;
		-- Width of S_AXI address bus
		C_S_AXI_ADDR_WIDTH	: integer	:= 5
	);
	port (
		-- Users to add ports here
        pad : inout STD_LOGIC_VECTOR (width-1 downto 0);
        gpio_int : out std_logic;
		-- User ports ends
		-- Do not modify the ports beyond this line

		-- Global Clock Signal
		S_AXI_ACLK	: in std_logic;
		-- Global Reset Signal. This Signal is Active LOW
		S_AXI_ARESETN	: in std_logic;
		-- Write address (issued by master, acceped by Slave)
		S_AXI_AWADDR	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		-- Write channel Protection type. This signal indicates the
    		-- privilege and security level of the transaction, and whether
    		-- the transaction is a data access or an instruction access.
		S_AXI_AWPROT	: in std_logic_vector(2 downto 0);
		-- Write address valid. This signal indicates that the master signaling
    		-- valid write address and control information.
		S_AXI_AWVALID	: in std_logic;
		-- Write address ready. This signal indicates that the slave is ready
    		-- to accept an address and associated control signals.
		S_AXI_AWREADY	: out std_logic;
		-- Write data (issued by master, acceped by Slave)
		S_AXI_WDATA	: in std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		-- Write strobes. This signal indicates which byte lanes hold
    		-- valid data. There is one write strobe bit for each eight
    		-- bits of the write data bus.
		S_AXI_WSTRB	: in std_logic_vector((C_S_AXI_DATA_WIDTH/8)-1 downto 0);
		-- Write valid. This signal indicates that valid write
    		-- data and strobes are available.
		S_AXI_WVALID	: in std_logic;
		-- Write ready. This signal indicates that the slave
    		-- can accept the write data.
		S_AXI_WREADY	: out std_logic;
		-- Write response. This signal indicates the status
    		-- of the write transaction.
		S_AXI_BRESP	: out std_logic_vector(1 downto 0);
		-- Write response valid. This signal indicates that the channel
    		-- is signaling a valid write response.
		S_AXI_BVALID	: out std_logic;
		-- Response ready. This signal indicates that the master
    		-- can accept a write response.
		S_AXI_BREADY	: in std_logic;
		-- Read address (issued by master, acceped by Slave)
		S_AXI_ARADDR	: in std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
		-- Protection type. This signal indicates the privilege
    		-- and security level of the transaction, and whether the
    		-- transaction is a data access or an instruction access.
		S_AXI_ARPROT	: in std_logic_vector(2 downto 0);
		-- Read address valid. This signal indicates that the channel
    		-- is signaling valid read address and control information.
		S_AXI_ARVALID	: in std_logic;
		-- Read address ready. This signal indicates that the slave is
    		-- ready to accept an address and associated control signals.
		S_AXI_ARREADY	: out std_logic;
		-- Read data (issued by slave)
		S_AXI_RDATA	: out std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
		-- Read response. This signal indicates the status of the
    		-- read transfer.
		S_AXI_RRESP	: out std_logic_vector(1 downto 0);
		-- Read valid. This signal indicates that the channel is
    		-- signaling the required read data.
		S_AXI_RVALID	: out std_logic;
		-- Read ready. This signal indicates that the master can
    		-- accept the read data and response information.
		S_AXI_RREADY	: in std_logic
	);
end APE_GPIO_AXI;

architecture arch_imp of APE_GPIO_AXI is

	-- AXI4LITE signals
	signal axi_awaddr	: std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
	signal axi_awready	: std_logic;
	signal axi_wready	: std_logic;
	signal axi_bresp	: std_logic_vector(1 downto 0);
	signal axi_bvalid	: std_logic;
	signal axi_araddr	: std_logic_vector(C_S_AXI_ADDR_WIDTH-1 downto 0);
	signal axi_arready	: std_logic;
	signal axi_rdata	: std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
	signal axi_rresp	: std_logic_vector(1 downto 0);
	signal axi_rvalid	: std_logic;

	-- Example-specific design signals
	-- local parameter for addressing 32 bit / 64 bit C_S_AXI_DATA_WIDTH
	-- ADDR_LSB is used for addressing 32/64 bit registers/memories
	-- ADDR_LSB = 2 for 32 bits (n downto 2)
	-- ADDR_LSB = 3 for 64 bits (n downto 3)
	constant ADDR_LSB  : integer := (C_S_AXI_DATA_WIDTH/32)+ 1;
	constant OPT_MEM_ADDR_BITS : integer := 2;
	------------------------------------------------
	---- Signals for user logic register space example
	--------------------------------------------------
	---- Number of Slave Registers 5
	signal slv_reg0	:std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
	signal slv_reg1	:std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
	signal slv_reg2	:std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
	signal slv_reg3	:std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
	signal slv_reg4	:std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
	signal slv_reg_rden	: std_logic;
	signal slv_reg_wren	: std_logic;
	signal reg_data_out	:std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0);
	signal byte_index	: integer;
	signal aw_en	: std_logic;

    component gpio_array is generic(width : natural := 4);
        Port ( write : in    STD_LOGIC_VECTOR (width-1 downto 0);
               dir   : in    STD_LOGIC_VECTOR (width-1 downto 0);
               read  : out   STD_LOGIC_VECTOR (width-1 downto 0);
               pad   : inout STD_LOGIC_VECTOR (width-1 downto 0));
    end component;

    component edge_detector is
        Port ( s_in       : in  STD_LOGIC;
               clk        : in  STD_LOGIC;
               rising_en  : in  STD_LOGIC;
               falling_en : in  STD_LOGIC;
               reset_n    : in  STD_LOGIC;
               s_out      : out STD_LOGIC);
    end component;


--------------------------------------------------------------------------------------------------------------------------------|
--  SEGNALI UTENTE:                                                                                                             |
--------------------------------------------------------------------------------------------------------------------------------|

    --! Segnale che proviene dall'uscita read di gpio_array ed entra nel porto s_in componente edge_detector e su reg_data_out
	--! all'indirizzo b"000".
    signal periph_read      :std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0) := (others => '0');

    --! Segnale gestito dal process ICRISR_management e posto in reg_data_out del bus all'indirizzo b"100", corrisponde a ISR.
    signal periph_isr       :std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0) := (others => '0');

	--! Segnale di appoggio per periph_isr.
    signal temp_periph_isr  :std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0) := (others => '0');

	--! Segnale che abilita o meno la rilevazione dei fronti in base al registro DIR (slv_reg1).
	signal edge_and_dir     :std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0) := (others => '0');

	--! Segnale che proviene dall'output s_out dell'edge_detector.
	signal edge_detected    :std_logic_vector(C_S_AXI_DATA_WIDTH-1 downto 0) := (others => '0');

begin
	-- I/O Connections assignments

	S_AXI_AWREADY	<= axi_awready;
	S_AXI_WREADY	<= axi_wready;
	S_AXI_BRESP	<= axi_bresp;
	S_AXI_BVALID	<= axi_bvalid;
	S_AXI_ARREADY	<= axi_arready;
	S_AXI_RDATA	<= axi_rdata;
	S_AXI_RRESP	<= axi_rresp;
	S_AXI_RVALID	<= axi_rvalid;
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
	      slv_reg0 <= (others => '0');
	      slv_reg1 <= (others => '0');
	      slv_reg2 <= (others => '0');
	      slv_reg3 <= (others => '0');
	      slv_reg4 <= (others => '0');
	    else
	      loc_addr := axi_awaddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB);
	      -- ICR (slv_reg4) viene azzerato ad ogni colpo di clock. Se ci sono scritture
	      -- provenienti dal bus, allora vengono poste in ingresso su slv_reg4.
	      slv_reg4 <= (others => '0');
	      if (slv_reg_wren = '1') then
	        case loc_addr is
	          when b"000" =>
	            for byte_index in 0 to (C_S_AXI_DATA_WIDTH/8-1) loop
	              if ( S_AXI_WSTRB(byte_index) = '1' ) then
	                -- Respective byte enables are asserted as per write strobes
	                -- slave registor 0
	                slv_reg0(byte_index*8+7 downto byte_index*8) <= S_AXI_WDATA(byte_index*8+7 downto byte_index*8);
	              end if;
	            end loop;
	          when b"001" =>
	            for byte_index in 0 to (C_S_AXI_DATA_WIDTH/8-1) loop
	              if ( S_AXI_WSTRB(byte_index) = '1' ) then
	                -- Respective byte enables are asserted as per write strobes
	                -- slave registor 1
	                slv_reg1(byte_index*8+7 downto byte_index*8) <= S_AXI_WDATA(byte_index*8+7 downto byte_index*8);
	              end if;
	            end loop;
	          when b"010" =>
	            for byte_index in 0 to (C_S_AXI_DATA_WIDTH/8-1) loop
	              if ( S_AXI_WSTRB(byte_index) = '1' ) then
	                -- Respective byte enables are asserted as per write strobes
	                -- slave registor 2
	                slv_reg2(byte_index*8+7 downto byte_index*8) <= S_AXI_WDATA(byte_index*8+7 downto byte_index*8);
	              end if;
	            end loop;
	          when b"011" =>
	            for byte_index in 0 to (C_S_AXI_DATA_WIDTH/8-1) loop
	              if ( S_AXI_WSTRB(byte_index) = '1' ) then
	                -- Respective byte enables are asserted as per write strobes
	                -- slave registor 3
	                slv_reg3(byte_index*8+7 downto byte_index*8) <= S_AXI_WDATA(byte_index*8+7 downto byte_index*8);
	              end if;
	            end loop;
	          when b"100" =>
	            for byte_index in 0 to (C_S_AXI_DATA_WIDTH/8-1) loop
	              if ( S_AXI_WSTRB(byte_index) = '1' ) then
	                -- Respective byte enables are asserted as per write strobes
	                -- slave registor 4
	                slv_reg4(byte_index*8+7 downto byte_index*8) <= S_AXI_WDATA(byte_index*8+7 downto byte_index*8);
	              end if;
	            end loop;
	          when others =>
	            slv_reg0 <= slv_reg0;
	            slv_reg1 <= slv_reg1;
	            slv_reg2 <= slv_reg2;
	            slv_reg3 <= slv_reg3;
	            slv_reg4 <= slv_reg4;
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

	process (slv_reg0, slv_reg1, slv_reg2, slv_reg3, slv_reg4, axi_araddr, S_AXI_ARESETN, slv_reg_rden)
	variable loc_addr :std_logic_vector(OPT_MEM_ADDR_BITS downto 0);
	begin
	    -- Address decoding for reading registers
	    loc_addr := axi_araddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB);
	    case loc_addr is
	      when b"000" =>
	        reg_data_out <= periph_read;
	      when b"001" =>
	        reg_data_out <= slv_reg1;
	      when b"010" =>
	        reg_data_out <= slv_reg2;
	      when b"011" =>
	        reg_data_out <= slv_reg3;
	      when b"100" =>
	        reg_data_out <= periph_isr;
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

--------------------------------------------------------------------------------------------------------------------------------|
--  USER LOGIC:                                                                                                                |
--------------------------------------------------------------------------------------------------------------------------------|


    -- Il segnale gpio_int è ottenuto mediante la OR di tutti i bit del registro ISR (periph_isr).
    gpio_int <= or_reduce(periph_isr);

    -- edge_and_dir abilita a leggere o meno il fronte in base al registro DIR (slv_reg1).
    -- Il segnale edge_detected proviene dall'output dell'edge_detector.
    edge_and_dir <= edge_detected and slv_reg1;

    --! @brief Process di gestione dei registri ISR e ICR.
    --! @details Il registro ISR (periph_isr) è gestito in base ai valori di ICR (slv_reg4) e di edge_and_dir:
    --! <br>Se periph_isr(i) = '1', allora è posto a '0' se slv_reg4(i) = '1'.
    --! <br>Se periph_isr(i) = '0', allora è posto a '1' se edge_and_dir(i)= '1'.
    ICRISR_management: process(periph_isr,slv_reg4,edge_and_dir,S_AXI_ACLK) is
    begin

    if (rising_edge (S_AXI_ACLK)) then
        if ( S_AXI_ARESETN = '0' ) then
            temp_periph_isr <= (others => '0');
        else
            for k in width-1 downto 0 loop
                if(periph_isr(k) = '1') then

                    temp_periph_isr(k) <= periph_isr(k) and (not slv_reg4(k));

                else --se periph_isr(k) = '0'

                    temp_periph_isr(k) <= periph_isr(k) or (edge_and_dir(k));

                end if;
            end loop;
        end if;
    end if;

    end process;

    -- feedback del segnale di appoggio
    periph_isr <= temp_periph_isr;

    --! @brief Array di edge_detector per rilevare il rising/falling edge del registro DATA (slv_reg0).
    --! @details Il segnale edge_detected(i) in uscita dal componente è alto se si è presentato su s_in
	--! un fronte per cui se era settato il relativo bit di abilitazione.
    edge_detector_array : for i in width-1 downto 0 generate
       edge_detector_inst : edge_detector port map(
            s_in        =>  periph_read(i),
            clk         =>  S_AXI_ACLK,
            rising_en   =>  slv_reg2(i),
            falling_en  =>  slv_reg3(i),
            reset_n     =>  S_AXI_ARESETN,
            s_out       =>  edge_detected(i)
            );
    end generate;

    --! @brief Componente gpio_array di lunghezza width.
	--! @details L'uscita read e' mappata sul segnale periph_read,
    --!        inserito poi nel componente edge_detector_array per rilevare i fronti di salita e discesa
    --!	       provenienti dall'esterno.
    gpio_array_inst : gpio_array generic map(width) port map(
        read    =>  periph_read(width-1 downto 0),
        write   =>  slv_reg0(width-1 downto 0),
        dir     =>  slv_reg1(width-1 downto 0),
        pad     =>  pad );

end arch_imp;
--! @}
--! @}
