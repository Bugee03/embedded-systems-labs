library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity tb_decoder is
end tb_decoder;

architecture behavior of tb_decoder is
  signal a : std_logic_vector(1 downto 0);
  signal y : std_logic_vector(3 downto 0);

  component decoder
    port (
      a : in  std_logic_vector(1 downto 0);
      y : out std_logic_vector(3 downto 0)
    );
  end component;

begin
  uut: decoder port map(a => a, y => y);

  stim_proc: process
  begin
    a <= "00"; wait for 10 ns;
    a <= "01"; wait for 10 ns;
    a <= "10"; wait for 10 ns;
    a <= "11"; wait for 10 ns;
    wait;
  end process;
end behavior;
