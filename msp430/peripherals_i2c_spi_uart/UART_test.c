#include <msp430.h>

void UART_init() {
    UCA0CTL1 |= UCSWRST; // Сброс UART
    UCA0CTL1 = UCSSEL_2; // Источник тактов — SMCLK
    UCA0BR0 = 104; // Скорость 9600 (для 1 МГц SMCLK)
    UCA0BR1 = 0;
    UCA0MCTL = UCBRS0; // Модульный делитель
    P1SEL |= BIT1 | BIT2; // RX и TX
    P1SEL2 |= BIT1 | BIT2;
    UCA0CTL1 &= ~UCSWRST; // Запуск UART
}

void UART_write(char c) {
    while (!(IFG2 & UCA0TXIFG)); // Ожидание готовности
    UCA0TXBUF = c; // Отправка символа
}

void main() {
    WDTCTL = WDTPW | WDTHOLD; // Остановка WDT
    UART_init();
    UART_write('A'); // Отправка символа 'A'
    while (1);
}
