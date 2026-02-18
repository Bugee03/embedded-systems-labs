library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity signal_demo is
    port (
        clk : in std_logic;
        done : out std_logic;
        s5, s6 : out std_logic_vector(3 downto 0)
    );
end;

architecture behavior of signal_demo is
    signal s1, s2, s3, s4 : std_logic_vector(3 downto 0) := "0011";
    signal u1, u2, u3, u4, u5 : unsigned(3 downto 0);
begin

    process(clk)
    begin
        if rising_edge(clk) then
            -- type mismatch examples (would error if uncommented)
            -- u1 <= s1; -- ❌
            -- u2 <= 5;  -- ❌

            -- correct conversions
            u1 <= unsigned(s1);
            u2 <= to_unsigned(5, 4);
            s2 <= std_logic_vector(u1);
            s3 <= std_logic_vector(to_unsigned(5, 4));

            -- arithmetic with unsigned
            u4 <= u2 + u1;
            u5 <= u2 + 1;

            -- illegal: std_logic_vector + something
            -- s5 <= s2 + s1; -- ❌
            -- s6 <= s2 + 1;  -- ❌

            -- legal with conversions:
            s5 <= std_logic_vector(unsigned(s2) + unsigned(s1));
            s6 <= std_logic_vector(unsigned(s2) + to_unsigned(1, 4));

            done <= '1';  -- just to know when sim is done
        end if;
    end process;
end;
