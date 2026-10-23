library ieee;
use  ieee.std_logic_1164.all;


entity tb_eq1 is
  end tb_eq1;

architecture behavior of tb_eq1 is
  signal i0, i1 : std_logic;
  signal eq     : std_logic;


component eq1 port(
                    i0,i1 : in std_logic;
                    eq    : out std_logic
                  );
end component;

begin
  uut : eq1 port map(i0 => i0, i1 =>i1, eq => eq);
  stim_proc: process
  begin 
    i0 <= '0'; i1  <=  '0'; wait for 10 ns;
    i0 <= '0'; i1  <=  '1'; wait for 20 ns;
    i0 <= '1'; i1  <=  '0'; wait for 10 ns;
    i0 <= '1'; i1  <=  '1'; wait for 10 ns;
    wait;
  end process;
end behavior;

  
