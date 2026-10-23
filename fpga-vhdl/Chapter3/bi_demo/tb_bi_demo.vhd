library ieee;
use ieee.std_logic_1164.all;

entity tb_bi_demo is
end entity;

architecture sim of tb_bi_demo is
    signal dir      : std_logic := '0';
    signal sig_out  : std_logic := '0';
    signal sig_in   : std_logic;
    signal bi       : std_logic;
    signal ext_drv  : std_logic := 'Z';  -- внешний драйвер

begin
    -- Соединяем внешний драйвер и bi через разрешающую логику (как на физ. пине)
    bi <= ext_drv;

    uut: entity work.bi_demo
        port map (
            dir     => dir,
            sig_out => sig_out,
            sig_in  => sig_in,
            bi      => bi
        );

  process
begin
    -- bi как вход, внешний подаёт 1
    ext_drv <= '1';
    dir <= '0';
    wait for 10 ns;

    -- bi как выход, мы подаём 1 наружу
    dir <= '1';
    sig_out <= '1';  -- ← тут была разница!
    ext_drv <= 'Z';
    wait for 10 ns;

    -- снова как вход, внешний подаёт 0
    dir <= '0';
    ext_drv <= '0';
    wait for 10 ns;

    report "Simulation finished!" severity note;
    wait;
  end process;

end architecture;

