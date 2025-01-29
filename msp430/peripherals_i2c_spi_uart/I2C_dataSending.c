#include "stm32f4xx_hal.h"

I2C_HandleTypeDef hi2c1;

void I2C1_Init(void) {
    __HAL_RCC_I2C1_CLK_ENABLE(); // Включение тактирования I²C1
    __HAL_RCC_GPIOB_CLK_ENABLE(); // Включение тактирования GPIOB

    GPIO_InitTypeDef GPIO_InitStruct = {0};
    GPIO_InitStruct.Pin = GPIO_PIN_6 | GPIO_PIN_7; // SDA и SCL
    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD; // Альтернативная функция с открытым стоком
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

    hi2c1.Instance = I2C1;
    hi2c1.Init.ClockSpeed = 100000; // 100 кГц
    hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
    hi2c1.Init.OwnAddress1 = 0;
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    HAL_I2C_Init(&hi2c1);
}

void I2C1_Write(uint8_t address, uint8_t data) {
    HAL_I2C_Master_Transmit(&hi2c1, address << 1, &data, 1, HAL_MAX_DELAY);
}

void main() {
    HAL_Init();
    I2C1_Init();
    uint8_t address = 0x48; // Адрес ведомого устройства
    uint8_t data = 0x55;
    I2C1_Write(address, data); // Передача данных
    while (1);
}
