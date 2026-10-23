library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity comparator is
  port (
    A : in  std_logic_vector(1 downto 0);
    B : in  std_logic_vector(1 downto 0);
    GT : out std_logic
  );
end comparator;

architecture behavioral of comparator is
begin
  GT <= '1' when unsigned(A) > unsigned(B) else '0';
end behavioral;
