# MSP430 ↔ STM32 · serial buses

Pairs of programs for the same bus: the **MSP430** side (`*_test.c`, direct USCI registers) and the **STM32** side (`*_dataSending.c`, HAL).

| Bus | MSP430 (`#include <msp430.h>`) | STM32 (HAL) |
|---|---|---|
| I²C | [`I2C_test.c`](peripherals_i2c_spi_uart/I2C_test.c) — UCB0 | [`I2C_dataSending.c`](peripherals_i2c_spi_uart/I2C_dataSending.c) — I2C1 |
| SPI | [`SPI_test.c`](peripherals_i2c_spi_uart/SPI_test.c) — UCB0 | [`SPI_dataSending.c`](peripherals_i2c_spi_uart/SPI_dataSending.c) — SPI1 |
| UART | [`UART_test.c`](peripherals_i2c_spi_uart/UART_test.c) — UCA0 | [`UART_dataSending.c`](peripherals_i2c_spi_uart/UART_dataSending.c) — USART2 |

Comments in the code are in Russian. Tools: Code Composer Studio (MSP430), STM32CubeIDE (STM32).
Next: GPIO, timers and ADC labs on MSP‑EXP430FR4133.
