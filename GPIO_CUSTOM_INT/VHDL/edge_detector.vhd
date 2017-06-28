----------------------------------------------------------------------------------
-- Company: Gruppo 5
-- Engineer: De Gregorio, Fezza, La Montagna
-- 
-- Create Date: 22.06.2017 12:44:08
-- Design Name: 
-- Module Name: edge_detector - Behavioral
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

entity edge_detector is
    Port ( s_in : in STD_LOGIC;
           clk : in STD_LOGIC;
           rising_en : in STD_LOGIC;
           falling_en : in STD_LOGIC;
           reset_n : in STD_LOGIC;
           s_out : out STD_LOGIC);
end edge_detector;

architecture Behavioral of edge_detector is

signal q1 : STD_LOGIC;
signal q2 : STD_LOGIC;
signal signal_rise : STD_LOGIC;
signal signal_fall : STD_LOGIC;

begin

process(clk,reset_n) is
begin
    if(reset_n = '0') then
        q1 <= '0';
        q2 <= '0';
    elsif(falling_edge(clk)) then
        q1 <= s_in;
        q2 <= q1;
    end if;
    
end process;

s_out <= (q1 and (not q2) and rising_en) or (q2 and (not q1) and falling_en);

end Behavioral;
