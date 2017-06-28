----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 20.06.2017 20:15:38
-- Design Name: 
-- Module Name: gpio_array - Structural
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

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
    for_inst: for k in width-1 downto 0 generate
    gpio_inst: gpio_pad generic map(width) port map(
          write=>write(k),
          dir=>dir(k),
          read=>read(k),
          pad=>pad(k));
     end generate;

end Structural;
