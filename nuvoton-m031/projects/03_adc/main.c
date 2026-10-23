/*
 * 03 - ADC: 3 POTANSIYOMETRE + NTC + LDR  -- Boardoza M032KG
 *
 * Yaptigi: Karttaki 5 analog kaynagi tek seferde okuyup ham degeri
 *          (0..4095) ve milivoltu 300 ms'de bir seri porta yazar.
 *
 * Boardoza semasi:
 *   Pot'lar  : PB3 (ADC0_CH3), PB4 (ADC0_CH4), PB5 (ADC0_CH5)
 *   NTC      : PB2 (ADC0_CH2)   VDD -[10.2k]- PB2 -[NTC 10k]- GND  -> isinca gerilim DUSER
 *   LDR      : PB15 (ADC0_CH15) VDD -[10.2k]- PB15 -[LDR]- GND    -> isik artinca gerilim DUSER
 */
#include <stdio.h>
#include "NuMicro.h"
#include "oled.h"             /* ../ortak/oled.c: OLED (J9), takili degilse sessizce atlanir */

#define VREF_MV    3300
#define KANALLAR   (BIT2 | BIT3 | BIT4 | BIT5 | BIT15)
#define MV(x)      ((unsigned)((x) * VREF_MV / 4095))

static void delay_ms(uint32_t ms) { while (ms--) CLK_SysTickDelay(1000); }

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* 1) ADC clock'u */
    CLK_EnableModuleClock(ADC_MODULE);
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL2_ADCSEL_PCLK1, CLK_CLKDIV0_ADC(1));
    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* 2) Pinleri analog yap: giris -> MFP = ADC -> dijital yolu kapat */
    GPIO_SetMode(PB, KANALLAR, GPIO_MODE_INPUT);
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk |
                                       SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk)) |
                    (SYS_GPB_MFPL_PB2MFP_ADC0_CH2 | SYS_GPB_MFPL_PB3MFP_ADC0_CH3 |
                     SYS_GPB_MFPL_PB4MFP_ADC0_CH4 | SYS_GPB_MFPL_PB5MFP_ADC0_CH5);
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~SYS_GPB_MFPH_PB15MFP_Msk) | SYS_GPB_MFPH_PB15MFP_ADC0_CH15;
    GPIO_DISABLE_DIGITAL_PATH(PB, KANALLAR);    /* kacak akimi onler */

    SYS_LockReg();
}

int main(void)
{
    uint32_t p3, p4, p5, ntc, ldr;

    SYS_Init();
    UART_Open(UART0, 115200);

    /* 3) ADC'yi ac: "single cycle scan" = bir START ile secili tum kanallar sirayla okunur */
    ADC_POWER_ON(ADC);
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE_CYCLE, KANALLAR);

    printf("\n03_adc basladi. Potlari cevir, LDR'yi elinle kapat, NTC'ye dokun.\n");
    oled_baslat();

    while (1)
    {
        static uint32_t sayfa_sayac = 0;
        char tum[80], deger[8];
        const char *ad[5]; uint32_t v[5]; uint32_t sayfa;
        ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
        ADC_START_CONV(ADC);
        while (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));    /* tum kanallar bitene kadar bekle */

        p3  = ADC_GET_CONVERSION_DATA(ADC, 3);           /* 12-bit: 0..4095 */
        p4  = ADC_GET_CONVERSION_DATA(ADC, 4);
        p5  = ADC_GET_CONVERSION_DATA(ADC, 5);
        ntc = ADC_GET_CONVERSION_DATA(ADC, 2);
        ldr = ADC_GET_CONVERSION_DATA(ADC, 15);

        /* M0'da FPU yok -> float yerine tam sayi ile mV */
        printf("POT3=%4u POT4=%4u POT5=%4u | NTC=%4u (%4u mV) | LDR=%4u (%4u mV)\n",
               (unsigned)p3, (unsigned)p4, (unsigned)p5,
               (unsigned)ntc, MV(ntc), (unsigned)ldr, MV(ldr));

        /* Ekran: her ~2 sn'de bir sonraki degeri BUYUK goster, altta hepsi kayar */
        ad[0] = "POT3"; v[0] = p3;  ad[1] = "POT4"; v[1] = p4;  ad[2] = "POT5"; v[2] = p5;
        ad[3] = "NTC";  v[3] = ntc; ad[4] = "LDR";  v[4] = ldr;
        sayfa = (sayfa_sayac++ / 7) % 5;

        snprintf(deger, sizeof(deger), "%u", (unsigned)v[sayfa]);
        snprintf(tum, sizeof(tum), "P3=%u P4=%u P5=%u NTC=%u LDR=%u",
                 (unsigned)p3, (unsigned)p4, (unsigned)p5, (unsigned)ntc, (unsigned)ldr);

        oled_temizle();
        yazi_orta(0, ad[sayfa], 2);
        yazi_orta(18, deger, 3);
        dolu_kutu(0, 43, (int)(v[sayfa] * 128 / 4095), 3);   /* ince cubuk */
        kayan_yazi(48, tum, 2);
        oled_guncelle();

        delay_ms(250);
    }
}
