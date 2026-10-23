# STM32 · NUCLEO‑F401RE

Cortex‑M4 labs, mostly **register level** (no HAL) so every bit in RCC / GPIO / TIM is explicit.

| Lab | Topic | Pins |
|---|---|---|
| [`01_gpio_blinky`](01_gpio_blinky) | LED blink via RCC → MODER → BSRR (atomic set/reset) | LD2 = PA5 |
| [`labs/pwm`](labs/pwm) | bare‑metal PWM, $f_{PWM}=f_{TIM}/((PSC+1)(ARR+1))$, ~1 MHz 50 % | TIM1_CH1 = PA8 |
| [`labs/01_pwm_rc_adc_uart`](labs/01_pwm_rc_adc_uart) | PWM → RC low‑pass → ADC → UART; duty 25/50/75 % → 0.825 / 1.65 / 2.475 V | TIM3_CH1 = PA6, ADC1_IN0 = PA0, USART2 (ST‑LINK VCP) |

Tools: STM32CubeIDE or `arm-none-eabi-gcc` + `st-flash`. Next: UART printf, SPI master, ADC single channel, EXTI, DMA.
