library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;


entity tb_gate_level_grater_than is 
  end tb_gate_level_grater_than;

architecture behavior of tb_gate_level_grater_than is 
  signal a,b : std_logic_vector (1 downto 0);
  signal agtb : std_logic;

  component gt2
    port(
          a,b : in std_logic_vector (1 downto 0);
          agtb : out std_logic
        );
  end component;

begin 
  uut: gt2 port map(a => a ,b => b,agtb => agtb);
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

