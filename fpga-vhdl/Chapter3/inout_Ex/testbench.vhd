library ieee;
use ieee.std_logic_1164.all;

entity testbench is end;

architecture sim of testbench is
    signal clk   : std_logic := '0';
    signal done  : std_logic;
    signal s5, s6: std_logic_vector(3 downto 0);
begin

    uut: entity work.signal_demo
        port map (
            clk => clk,
            done => done,
            s5 => s5,
            s6 => s6
        );

    clk_process : process
    begin
        while done = '0' loop
            clk <= '0';
            wait for 10 ns;
            clk <= '1';
            wait for 10 ns;
        end loop;
        wait;
    end process;

end;
