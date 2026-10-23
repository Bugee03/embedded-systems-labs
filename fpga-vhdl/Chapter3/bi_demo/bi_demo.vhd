library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all; 

entity bi_demo is
  port(
        dir : in std_logic;
        sig_out : in std_logic; 
        sig_in : out std_logic;
        bi : inout std_logic
      );
end bi_demo;




architecture behavior of bi_demo is
begin 
--if dir (1) out ot bi else "z or 0" off
  bi <= sig_out when dir = '1' else 'Z';


  sig_in <= bi;
end architecture;



