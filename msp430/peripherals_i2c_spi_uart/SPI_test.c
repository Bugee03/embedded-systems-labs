#include <msp430.h>

void SPI_init() {
    UCB0CTL1 |= UCSWRST; // Сброс модуля SPI
    UCB0CTL0 = UCCKPH | UCMSB | UCMST | UCSYNC; // Настройка: ведущий, синхронный режим
    UCB0CTL1 = UCSSEL_2 | UCSWRST; // Выбор источника тактов (SMCLK)
    UCB0BR0 = 0x02; // Скорость SPI
    UCB0BR1 = 0;
    P1SEL |= BIT5 | BIT6 | BIT7; // Настройка выводов (MOSI, MISO, SCLK)
    P1SEL2 |= BIT5 | BIT6 | BIT7;
    UCB0CTL1 &= ~UCSWRST; // Запуск SPI
}

void SPI_write(uint8_t data) {
    while (!(IFG2 & UCB0TXIFG)); // Ожидание готовности к передаче
    UCB0TXBUF = data; // Передача данных
}

uint8_t SPI_read() {
    while (!(IFG2 & UCB0RXIFG)); // Ожидание данных
    return UCB0RXBUF; // Чтение данных
}

void main() {
    WDTCTL = WDTPW | WDTHOLD; // Остановка WDT
    SPI_init();
    SPI_write(0x55); // Отправка байта
    uint8_t received = SPI_read(); // Чтение ответа
    while (1);
}
