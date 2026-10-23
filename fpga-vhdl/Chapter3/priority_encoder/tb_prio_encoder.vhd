library ieee;
use ieee.std_logic_1164.all;

entity tb_prio_encoder is
end tb_prio_encoder;

architecture test of tb_prio_encoder is

  signal r     : std_logic_vector(4 downto 1);
  signal pcode : std_logic_vector(2 downto 0);

  -- подключение testbench к модулю
  component prio_encoder
    port (
      r     : in  std_logic_vector(4 downto 1);
      pcode : out std_logic_vector(2 downto 0)
    );
  end component;

begin

  uut: prio_encoder port map (
    r => r,
    pcode => pcode
  );

  stim_proc: process
  begin
    -- Тест 1: ничего не активно
    r <= "0000";
    wait for 10 ns;

    -- Тест 2: r(1) активен
    r <= "0001";
    wait for 10 ns;

    -- Тест 3: r(2) активен
    r <= "0010";
    wait for 10 ns;

    -- Тест 4: r(3) активен
    r <= "0100";
    wait for 10 ns;

    -- Тест 5: r(4) активен
    r <= "1000";
    wait for 10 ns;

    -- Тест 6: r(1), r(2), r(4) активны — приоритет у r(4)
    r <= "1011";
    wait for 10 ns;

    -- Тест 7: только r(3)
    r <= "0100";
    wait for 10 ns;

    wait;
  end process;

end test;

