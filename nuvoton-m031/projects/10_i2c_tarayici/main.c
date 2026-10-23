/*
 * 10 - I2C TARAYICI (bagli cihazlarin adreslerini bul)
 *
 * Yaptigi: 0x08..0x77 arasindaki tum adresleri dener, cevap veren
 *          (ACK donen) cihazlari listeler. 3 saniyede bir tekrarlar.
 *          Sensor/LCD calismiyorsa ILK bunu calistir.
 *
 * Bilinen adresler:  BNO055 = 0x28 (veya 0x29)
 *                    LCD (PCF8574) = 0x27 (veya 0x3F)
 *
 * Baglanti (Boardoza J9 konnektoru): 1=GND 2=3V3 3=SCL(PE13) 4=SDA(PC8)
 *   Kartta pull-up direncleri zaten var.
 */
#include <stdio.h>
#include "NuMicro.h"

static void delay_ms(uint32_t ms) { while (ms--) CLK_SysTickDelay(1000); }

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
    CLK_EnableModuleClock(I2C0_MODULE);                 /* I2C icin kaynak secimi yok, PCLK kullanir */
    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Boardoza J9: PC8 = I2C0_SDA, PE13 = I2C0_SCL (kartta 10.2k pull-up var) */
    SYS->GPC_MFPH = (SYS->GPC_MFPH & ~SYS_GPC_MFPH_PC8MFP_Msk) | SYS_GPC_MFPH_PC8MFP_I2C0_SDA;
    SYS->GPE_MFPH = (SYS->GPE_MFPH & ~SYS_GPE_MFPH_PE13MFP_Msk) | SYS_GPE_MFPH_PE13MFP_I2C0_SCL;
    SYS_LockReg();
}

/* Adrese START + (adres<<1 | W) gonderir; ACK gelirse 1 dondurur */
static int i2c_var_mi(uint8_t adr)
{
    uint32_t durum;

    I2C_START(I2C0);
    I2C_WAIT_READY(I2C0);

    I2C_SET_DATA(I2C0, (uint8_t)(adr << 1));            /* yazma biti = 0 */
    I2C_SET_CONTROL_REG(I2C0, I2C_CTL_SI);
    I2C_WAIT_READY(I2C0);
    durum = I2C_GET_STATUS(I2C0);                       /* 0x18 = adres + W gonderildi, ACK alindi */

    I2C_SET_CONTROL_REG(I2C0, I2C_CTL_STO_SI);          /* STOP */
    while (I2C0->CTL0 & I2C_CTL0_STO_Msk);

    return durum == 0x18;
}

int main(void)
{
    uint8_t a;
    int adet;

    SYS_Init();
    UART_Open(UART0, 115200);
    I2C_Open(I2C0, 100000);                             /* 100 kHz (standart mod) */

    printf("\n10_i2c_tarayici basladi\n");

    while (1)
    {
        adet = 0;
        printf("Taraniyor...\n");
        for (a = 0x08; a <= 0x77; a++)
        {
            if (i2c_var_mi(a))
            {
                printf("  Cihaz bulundu: 0x%02X\n", a);
                adet++;
            }
        }
        printf("Toplam %d cihaz.%s\n\n", adet, adet ? "" : " Kablolari / pull-up'lari kontrol et.");
        delay_ms(3000);
    }
}
