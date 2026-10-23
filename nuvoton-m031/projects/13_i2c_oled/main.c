/*
 * 13 - I2C: OLED EKRAN (SSD1306, 128x64)  -- Boardoza M032KG
 *
 * Yaptigi: POT3 degerini BUYUK yazar + cubuk grafik; diger degerler altta
 *          kayan yazi olarak akar. POT4 ekranin parlakligini ayarlar.
 *
 * Baglanti (Boardoza J9): GND, VCC -> 3V3, SCL = PE13, SDA = PC8
 *   Adres 0x3C (10_i2c_tarayici ile bulundu). Bazi moduller 0x3D kullanir.
 *   1.3" ekran (SH1106) ise OLED_KOLON_KAYMA'yi 2 yap.
 *
 * Mantik:
 *   - RAM'de 128x64 bitlik bir "frame buffer" tutulur (8 sayfa x 128 bayt).
 *     Her bayt dikey 8 pikseldir (bit0 = ust).
 *   - Once buffer'a cizilir, sonra oled_guncelle() ile tamami I2C'den gonderilir.
 *   - I2C'de her paketin ilk bayti "kontrol bayti"dir: 0x00 = komut, 0x40 = veri.
 */
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "oled.h"             /* ../ortak/oled.c: OLED surucusu */

#define KANALLAR          (BIT2 | BIT3 | BIT4 | BIT5 | BIT15)

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
    CLK_EnableModuleClock(ADC_MODULE);
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL2_ADCSEL_PCLK1, CLK_CLKDIV0_ADC(1));
    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Boardoza J9: PC8 = I2C0_SDA, PE13 = I2C0_SCL */
    SYS->GPC_MFPH = (SYS->GPC_MFPH & ~SYS_GPC_MFPH_PC8MFP_Msk) | SYS_GPC_MFPH_PC8MFP_I2C0_SDA;
    SYS->GPE_MFPH = (SYS->GPE_MFPH & ~SYS_GPE_MFPH_PE13MFP_Msk) | SYS_GPE_MFPH_PE13MFP_I2C0_SCL;

    /* Analog girisler: PB2 NTC, PB3/4/5 pot, PB15 LDR */
    GPIO_SetMode(PB, KANALLAR, GPIO_MODE_INPUT);
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk |
                                       SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk)) |
                    (SYS_GPB_MFPL_PB2MFP_ADC0_CH2 | SYS_GPB_MFPL_PB3MFP_ADC0_CH3 |
                     SYS_GPB_MFPL_PB4MFP_ADC0_CH4 | SYS_GPB_MFPL_PB5MFP_ADC0_CH5);
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~SYS_GPB_MFPH_PB15MFP_Msk) | SYS_GPB_MFPH_PB15MFP_ADC0_CH15;
    GPIO_DISABLE_DIGITAL_PATH(PB, KANALLAR);

    SYS_LockReg();
}

int main(void)
{
    char satir[24], uzun[80];
    uint32_t p3, p4, p5, ntc, ldr, sayac = 0;

    SYS_Init();
    UART_Open(UART0, 115200);
    I2C_Open(I2C0, 400000);                           /* OLED 400 kHz'i destekler */

    ADC_POWER_ON(ADC);
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE_CYCLE, KANALLAR);

    printf("\n13_i2c_oled basladi (adres 0x%02X)\n", OLED_ADR);
    delay_ms(100);                                    /* OLED acilis */
    if (!oled_baslat())
    {
        printf("OLED cevap vermiyor! 10_i2c_tarayici ile adresi kontrol et.\n");
        while (1);
    }
    printf("OLED hazir.\n");

    while (1)
    {
        ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
        ADC_START_CONV(ADC);
        while (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));
        p3  = ADC_GET_CONVERSION_DATA(ADC, 3);
        p4  = ADC_GET_CONVERSION_DATA(ADC, 4);
        p5  = ADC_GET_CONVERSION_DATA(ADC, 5);
        ntc = ADC_GET_CONVERSION_DATA(ADC, 2);
        ldr = ADC_GET_CONVERSION_DATA(ADC, 15);

        /* POT4 = ekran parlakligi (0..255) */
        oled_parlaklik((uint8_t)(p4 >> 4));

        oled_temizle();
        yazi_orta(0, "POT3", 2);
        snprintf(satir, sizeof(satir), "%u", (unsigned)p3);
        yazi_orta(18, satir, 3);                      /* buyuk deger */
        dolu_kutu(0, 43, (int)(p3 * 128 / 4095), 3);  /* ince cubuk */

        snprintf(uzun, sizeof(uzun), "P4(parlaklik)=%u P5=%u NTC=%u LDR=%u #%u",
                 (unsigned)p4, (unsigned)p5, (unsigned)ntc, (unsigned)ldr, (unsigned)sayac++);
        kayan_yazi(48, uzun, 2);                      /* sigmayan yazi kayar */

        oled_guncelle();
        delay_ms(20);
    }
}
