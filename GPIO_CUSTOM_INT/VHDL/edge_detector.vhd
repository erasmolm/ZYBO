----------------------------------------------------------------------------------
--! @file   edge_detector.vhd
--  Company: Gruppo 5
--! @author Alfonso, Pierluigi, Erasmo (APE)
--!
--! @date 20.06.2017 20:15:38
--!
--! @addtogroup APE_GPIO
--! @{
--! @addtogroup edge_detector
--! @{
--!
--! @brief Componente che rileva fronti sia di salita che di discesa di un segnale.
--!
--! @details In base a quali fronti sono stati abilitati per la detection (rising e/o falling),
--!          il componente restituisce un'uscita alta quando questi si presentano sul segnale in
--!          ingresso. Si noti che l'uscita commuta sul fronte di <b>discesa</b> del clock.
--!          Questo perchè, nell'ottica di dover inserire il componente in un contesto piu' ampio,
--!          permette di evitare che un eventuale process campioni il segnale mentre sta variando.
--!          Il comportamente e' realizzato mediante due flip-flop in cascata come riportato nello schema.
--! @image html edge_detector.png
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;



entity edge_detector is
    Port ( s_in : in STD_LOGIC;--! Ingresso per il segnale di cui si vogliono registrare i fronti.
           clk : in STD_LOGIC;--! Ingresso per il segnale di clock.
           rising_en : in STD_LOGIC;--! Ingresso per abilitare la detection dei fronti di salita.
           falling_en : in STD_LOGIC;--! Ingresso per abilitare la detection dei fronti di discesa.
           reset_n : in STD_LOGIC;--! Reset asincrono in logica negata.
           s_out : out STD_LOGIC);--! Uscita del componente.
end edge_detector;

architecture Behavioral of edge_detector is

signal q1 : STD_LOGIC;
signal q2 : STD_LOGIC;

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
--! @}
--! @}
