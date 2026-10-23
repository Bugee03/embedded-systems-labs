library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity gt2 is port(
                    a,b : in std_logic_vector(1 downto 0);
                    agtb : out std_logic
                  );
end gt2;

architecture logic of gt2 is 
begin 
  agtb <= '1' when unsigned(a) > unsigned(b) else '0';
end logic;


