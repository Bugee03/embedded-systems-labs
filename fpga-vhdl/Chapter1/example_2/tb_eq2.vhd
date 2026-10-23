library ieee;
use ieee.std_logic_1164.all;

entity tb_eq2 is
end tb_eq2;

architecture behavior of tb_eq2 is
  signal a, b  : std_logic_vector(1 downto 0);
  signal aeqb  : std_logic;

  component eq2
    port (
      a, b  : in  std_logic_vector(1 downto 0);
      aeqb  : out std_logic
    );
  end component;

begin
  uut: eq2 port map(a => a, b => b, aeqb => aeqb);

  stim_proc: process
  begin
    a <= "00"; b <= "00"; wait for 10 ns;
    a <= "00"; b <= "01"; wait for 10 ns;
    a <= "00"; b <= "10"; wait for 10 ns;
    a <= "00"; b <= "11"; wait for 10 ns;
    a <= "01"; b <= "00"; wait for 10 ns;
    a <= "01"; b <= "01"; wait for 10 ns;
    a <= "01"; b <= "10"; wait for 10 ns;
    a <= "01"; b <= "11"; wait for 10 ns;
    a <= "10"; b <= "00"; wait for 10 ns;
    a <= "10"; b <= "01"; wait for 10 ns;
    a <= "10"; b <= "10"; wait for 10 ns;
    a <= "10"; b <= "11"; wait for 10 ns;
    a <= "11"; b <= "00"; wait for 10 ns;
    a <= "11"; b <= "01"; wait for 10 ns;
    a <= "11"; b <= "10"; wait for 10 ns;
    a <= "11"; b <= "11"; wait for 10 ns;      
    wait;
  end process;
end behavior;
