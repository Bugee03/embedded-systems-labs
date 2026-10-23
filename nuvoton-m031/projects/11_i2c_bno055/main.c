/*
 * 11 - I2C: BNO055 ile YONELIM (Euler acilari)
 *
 * Yaptigi: BNO055'in kimligini (CHIP_ID = 0xA0) kontrol eder, NDOF
 *          (sensor fuzyonu) moduna alir ve heading/roll/pitch'i
 *          200 ms'de bir derece cinsinden yazar.
 *
 * Baglanti (Boardoza J9): GND, 3V3 -> VIN, SCL = PE13, SDA = PC8
 *   ADR pini GND ise adres 0x28, 3.3V ise 0x29.
 *
 * DIKKAT: Nuvoton I2C fonksiyonlari 7-bit adresi KAYDIRMADAN alir (0x28).
 *         STM32 HAL'daki gibi (0x28 << 1) YAZMA.
 */
#include <stdio.h>
#include "NuMicro.h"

#define BNO_ADR        0x28
#define REG_CHIP_ID    0x00
#define REG_EUL_H_LSB  0x1A        /* heading, roll, pitch: 6 bayt */
#define REG_CALIB_STAT 0x35
#define REG_OPR_MODE   0x3D
#define MOD_CONFIG     0x00
#define MOD_NDOF       0x0C

static void delay_ms(uint32_t ms) { while (ms--) CLK_SysTickDelay(1000); }

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
    CLK_EnableModuleClock(I2C0_MODULE);
    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
    /* Boardoza J9: PC8 = I2C0_SDA, PE13 = I2C0_SCL (kartta 10.2k pull-up var) */
    SYS->GPC_MFPH = (SYS->GPC_MFPH & ~SYS_GPC_MFPH_PC8MFP_Msk) | SYS_GPC_MFPH_PC8MFP_I2C0_SDA;
    SYS->GPE_MFPH = (SYS->GPE_MFPH & ~SYS_GPE_MFPH_PE13MFP_Msk) | SYS_GPE_MFPH_PE13MFP_I2C0_SCL;
    SYS_LockReg();
}

/* 1/16 derece birimindeki degeri "123.4" seklinde yazdirir (float kullanmadan) */
static void derece_yaz(const char *ad, int16_t ham)
{
    int32_t onda = (int32_t)ham * 10 / 16;              /* 0.1 derece birimi */
    const char *isaret = onda < 0 ? "-" : "";
    if (onda < 0) onda = -onda;
    printf("%s=%s%d.%d  ", ad, isaret, (int)(onda / 10), (int)(onda % 10));
}

int main(void)
{
    uint8_t id, b[6], kal;

    SYS_Init();
    UART_Open(UART0, 115200);
    I2C_Open(I2C0, 100000);

    printf("\n11_i2c_bno055 basladi\n");
    delay_ms(700);                                      /* BNO055 acilis suresi ~650 ms */

    id = I2C_ReadByteOneReg(I2C0, BNO_ADR, REG_CHIP_ID);
    printf("CHIP_ID = 0x%02X (beklenen 0xA0)\n", id);
    if (id != 0xA0)
    {
        printf("BNO055 bulunamadi! 10_i2c_tarayici ile adresi kontrol et.\n");
        while (1);
    }

    I2C_WriteByteOneReg(I2C0, BNO_ADR, REG_OPR_MODE, MOD_CONFIG);
    delay_ms(25);
    I2C_WriteByteOneReg(I2C0, BNO_ADR, REG_OPR_MODE, MOD_NDOF);    /* fuzyon modu */
    delay_ms(25);

    while (1)
    {
        I2C_ReadMultiBytesOneReg(I2C0, BNO_ADR, REG_EUL_H_LSB, b, 6);
        kal = I2C_ReadByteOneReg(I2C0, BNO_ADR, REG_CALIB_STAT);

        /* Her deger 16-bit, once LSB. 16 LSB = 1 derece */
        derece_yaz("Heading", (int16_t)(b[1] << 8 | b[0]));
        derece_yaz("Roll",    (int16_t)(b[3] << 8 | b[2]));
        derece_yaz("Pitch",   (int16_t)(b[5] << 8 | b[4]));
        printf("Kalib(sys,gyr,acc,mag)=%d%d%d%d\n",
               (kal >> 6) & 3, (kal >> 4) & 3, (kal >> 2) & 3, kal & 3);

        delay_ms(200);
    }
}
