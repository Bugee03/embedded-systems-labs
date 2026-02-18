library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tb_comparator is
end tb_comparator;

architecture behavior of tb_comparator is
  signal A, B : std_logic_vector(1 downto 0);
  signal GT   : std_logic;

  component comparator
    port (
      A  : in  std_logic_vector(1 downto 0);
      B  : in  std_logic_vector(1 downto 0);
      GT : out std_logic
    );
  end component;
begin
  uut: comparator port map(A => A, B => B, GT => GT);

  stim_proc: process
  begin
    A <= "00"; B <= "00"; wait for 10 ns;
    A <= "10"; B <= "01"; wait for 10 ns;
    A <= "01"; B <= "11"; wait for 10 ns;
    A <= "11"; B <= "10"; wait for 10 ns;
    wait;
  end process;
end behavior;
