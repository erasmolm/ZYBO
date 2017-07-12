----------------------------------------------------------------------------------
--! @file   gpio_array.vhd
--  Company: Gruppo 5
--! @author Alfonso, Pierluigi, Erasmo (APE)
--!
--! @date 20.06.2017 20:15:38
--!
--! @addtogroup APE_GPIO
--! @{
--! @addtogroup gpio_array
--! @{
--!
--! @brief Array di pad gpio di lunghezza configurabile.
--! @details Questo componente e' la versione replicata del componente gpio_pad
--!          per un numero di volte pari a <b>width</b>.
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

--! Entity gpio_array
entity gpio_array is generic(width : natural := 4);
    Port ( write : in STD_LOGIC_VECTOR (width-1 downto 0);
           dir : in STD_LOGIC_VECTOR (width-1 downto 0);
           read : out STD_LOGIC_VECTOR (width-1 downto 0);
           pad : inout STD_LOGIC_VECTOR (width-1 downto 0));
end gpio_array;

architecture Structural of gpio_array is

component gpio_pad is generic (width : natural := 4);
    Port ( write : in STD_LOGIC;
           dir : in STD_LOGIC;
           read : out STD_LOGIC;
           pad : inout STD_LOGIC);
end component;

begin
    --! Istanza gpio_pad
    gpio_pad_inst: for k in width-1 downto 0 generate
    gpio_inst: gpio_pad generic map(width) port map(
          write=>write(k),
          dir=>dir(k),
          read=>read(k),
          pad=>pad(k));
     end generate;

end Structural;
--! @}
--! @}
