----------------------------------------------------------------------------------
-- Company: Gruppo 5
-- Engineer: De Gregorio, Fezza, La Montagna
-- 
-- Create Date: 20.06.2017 20:15:38
-- Design Name: 
-- Module Name: gpio_pad - DataFlow
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

entity gpio_pad is
    Port ( write : in STD_LOGIC;
           dir : in STD_LOGIC;
           read : out STD_LOGIC;
           pad : inout STD_LOGIC);
end gpio_pad;

architecture DataFlow of gpio_pad is

begin

with dir select pad <=
    'Z' when '1',
    write when others;
    
read <= pad;

end DataFlow;
