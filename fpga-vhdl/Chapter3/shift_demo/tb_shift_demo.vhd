library ieee;
use ieee.std_logic_1164.all;

entity tb_shift_demo is
end entity;

architecture sim of tb_shift_demo is
    signal a        : std_logic_vector(7 downto 0) := "10110011";
    signal rot_r    : std_logic_vector(7 downto 0);
    signal rot_l    : std_logic_vector(7 downto 0);
    signal log_r    : std_logic_vector(7 downto 0);
    signal arith_r  : std_logic_vector(7 downto 0);
begin
    uut: entity work.shift_demo
        port map (
            a       => a,
            rot_r   => rot_r,
            rot_l   => rot_l,
            log_r   => log_r,
            arith_r => arith_r
        );

    process
    begin
        wait for 10 ns;
        assert false report "Simulation finished!" severity failure;
    end process;
end architecture;

