----------------------------------------------------------------------------------
--! @file   gpio_pad.vhd
--  Company: Gruppo 5
--! @author Alfonso, Pierluigi, Erasmo (APE)
--!
--! @date 20.06.2017 20:15:38
--!
--! @addtogroup APE_GPIO
--! @{
--! @addtogroup gpio_pad
--! @{
--!
--! @brief Componente gpio singolo.
--! @details Rappresenta il comportamento di un singolo
--!          pad gpio input-output, pilotato dalla logica tristate mediante il segnale in ingresso
--!          <b>dir</b>:
--!          - Se pari a '1', il valore in ingresso a pad viene riportato sull'uscita <b>read</b>.
--!          - Se pari a '0', il valore in ingresso a <b>write</b> viene riportato sul <b>pad</b> e su <b>read</b>.
----------------------------------------------------------------------------------

library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

--! Entity gpio_pad
entity gpio_pad is
    Port ( write : in STD_LOGIC;--! Porto di scrittura dal PS verso l'esterno.
           dir : in STD_LOGIC;--! Porto di selezione della direzione.
           read : out STD_LOGIC; --!Porto di lettura dall'esterno verso al PS.
           pad : inout STD_LOGIC);--!Pad input-output.
end gpio_pad;

architecture DataFlow of gpio_pad is

begin

--!In base al valore di <b>dir</b>:
--! - Se pari a '1', il valore in ingresso a pad viene riportato sull'uscita <b>read</b>.
--! - Se pari a '0', il valore in ingresso a <b>write</b> viene riportato sul <b>pad</b> e su <b>read</b>.
with dir select pad <=
    'Z' when '1',
    write when others;

read <= pad;

end DataFlow;
--! @}
--! @}
