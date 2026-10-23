#include "stm32f4xx_hal.h"

UART_HandleTypeDef huart2;

void UART2_Init(void) {
    __HAL_RCC_USART2_CLK_ENABLE(); // Включение тактирования UART2
    __HAL_RCC_GPIOA_CLK_ENABLE(); // Включение тактирования GPIOA

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_2 | GPIO_PIN_3; // TX и RX
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF7_USART2;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

    huart2.Instance = USART2;
    huart2.Init.BaudRate = 9600;
    huart2.Init.WordLength = UART_WORDLENGTH_8B;
    huart2.Init.StopBits = UART_STOPBITS_1;
    huart2.Init.Parity = UART_PARITY_NONE;
    huart2.Init.Mode = UART_MODE_TX_RX;
    huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart2.Init.OverSampling = UART_OVERSAMPLING_16;
    HAL_UART_Init(&huart2);
}

void UART2_Write(char *data) {
    HAL_UART_Transmit(&huart2, (uint8_t *)data, strlen(data), HAL_MAX_DELAY);
}

void main() {
    HAL_Init();
    UART2_Init();
    UART2_Write("Hello, UART!\n");
    while (1);
}
