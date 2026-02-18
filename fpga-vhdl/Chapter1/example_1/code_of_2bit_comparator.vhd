library ieee;
use ieee.std_logic_1164.all;


entity eq2 is 
  port(
        a,b : in std_logic_vector(5 donwto 0);
        aeqb : out std_logic_vector(5 downto 0);
      );
end eq2;


architecture sop_arch of eq2 is
  signal e0,e1,e2,e3,e4: std_logic;

begin

  eq_bit_unit : entity  work.eq1(sop_arch) 
  port map(
  i0 => a(0);
  i1 => b(0);
  eq = e0;

  eq_bit_unit : entity  work.eq1(sop_arch) 
  port map(
  i0 => a(1);
  i1 => b(1);
  eq = e1;


  eq_bit_unit : entity  work.eq1(sop_arch) 
  port map(
  i0 => a(2);
  i1 => b(2);
  eq = e2;


  eq_bit_unit : entity  work.eq1(sop_arch) 
  port map(
  e0 => e1;
  e0 => e2;
  eq = e3;

  eq_bit_unit : entity  work.eq1(sop_arch) 
  port map(
  e1 => e3;
  e2 => e3;
  eq = e4;


  aeqb = e4 and e
end sop_arch;

 
