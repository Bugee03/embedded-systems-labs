library ieee;
use ieee.std_logic_1164.all;

entity shift_demo is
    port (
        a     : in  std_logic_vector(7 downto 0);
        rot_r : out std_logic_vector(7 downto 0);
        rot_l : out std_logic_vector(7 downto 0);
        log_r : out std_logic_vector(7 downto 0);
        arith_r : out std_logic_vector(7 downto 0)
    );
end entity;

architecture behavior of shift_demo is
begin
    -- Rotate right by 3
    rot_r <= a(2 downto 0) & a(7 downto 3);

    -- Rotate left by 3
    rot_l <= a(4 downto 0) & a(7 downto 5);

    -- Logical shift right by 3 (insert 0s)
    log_r <= "000" & a(7 downto 3);

    -- Arithmetic shift right by 3 (insert MSB)
    arith_r <= a(7) & a(7) & a(7) & a(7 downto 3);
end architecture;

