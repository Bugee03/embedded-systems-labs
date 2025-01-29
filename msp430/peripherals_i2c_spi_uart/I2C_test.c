#include <msp430.h>

void I2C_init() {
    UCB0CTL1 |= UCSWRST; // Сброс I²C
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC; // Ведущий, I²C, синхронный режим
    UCB0BRW = 10; // Установка скорости передачи
    UCB0I2CSA = 0x48; // Адрес ведомого устройства (например, датчик температуры)
    P1SEL |= BIT6 | BIT7; // Настройка выводов (SDA, SCL)
    P1SEL2 |= BIT6 | BIT7;
    UCB0CTL1 &= ~UCSWRST; // Запуск I²C
}

void I2C_write(uint8_t data) {
    UCB0CTL1 |= UCTR + UCTXSTT; // Режим передачи, старт-сигнал
    while (!(IFG2 & UCB0TXIFG)); // Ожидание готовности
    UCB0TXBUF = data; // Передача данных
    while (UCB0CTL1 & UCTXSTT); // Ожидание завершения передачи
    UCB0CTL1 |= UCTXSTP; // Стоп-сигнал
    while (UCB0CTL1 & UCTXSTP); // Ожидание завершения стопа
}

void main() {
    WDTCTL = WDTPW | WDTHOLD; // Остановка WDT
    I2C_init();
    I2C_write(0x55); // Отправка данных
    while (1);
}
