/*
 * 05 - PWM: RGB LED rengini 3 POT ile ayarla  -- Boardoza M032KG
 *
 * Yaptigi: Her potansiyometre RGB LED'in bir renginin parlakligini
 *          (%0..%100 duty) belirler. Potlari cevirerek her rengi karistir.
 *
 * Boardoza semasi:
 *   RGB LED (NPN transistorle surulur, 1 = yanar):
 *     Kirmizi = PC3 (PWM1_CH2)   Yesil = PC5 (PWM1_CH0)   Mavi = PC4 (PWM1_CH1)
 *   Pot'lar: PB3 -> kirmizi, PB4 -> yesil, PB5 -> mavi
 */
#include <stdio.h>
#include "NuMicro.h"
#include "pwm_duzelt.h"        /* PWM_ConfigOutputChannel duzeltmesi (bkz. ../ortak/pwm_duzelt.h) */
#include "oled.h"             /* ../ortak/oled.c: OLED (J9), takili degilse sessizce atlanir */

#define PWM_FREKANS  1000      /* 1 kHz: goz titremeyi gormez */
#define CH_R  2
#define CH_G  0
#define CH_B  1

static void delay_ms(uint32_t ms) { while (ms--) CLK_SysTickDelay(1000); }

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* PWM1 clock'u = PCLK1 (48 MHz) */
    CLK_EnableModuleClock(PWM1_MODULE);
    CLK_SetModuleClock(PWM1_MODULE, CLK_CLKSEL2_PWM1SEL_PCLK1, 0);

    CLK_EnableModuleClock(ADC_MODULE);
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL2_ADCSEL_PCLK1, CLK_CLKDIV0_ADC(1));
    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* PC3/PC4/PC5 -> PWM1 kanallari */
    SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC3MFP_Msk | SYS_GPC_MFPL_PC4MFP_Msk | SYS_GPC_MFPL_PC5MFP_Msk)) |
                    (SYS_GPC_MFPL_PC3MFP_PWM1_CH2 | SYS_GPC_MFPL_PC4MFP_PWM1_CH1 | SYS_GPC_MFPL_PC5MFP_PWM1_CH0);

    /* PB3/PB4/PB5 -> ADC */
    GPIO_SetMode(PB, BIT3 | BIT4 | BIT5, GPIO_MODE_INPUT);
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB3MFP_Msk | SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk)) |
                    (SYS_GPB_MFPL_PB3MFP_ADC0_CH3 | SYS_GPB_MFPL_PB4MFP_ADC0_CH4 | SYS_GPB_MFPL_PB5MFP_ADC0_CH5);
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT3 | BIT4 | BIT5);

    SYS_LockReg();
}

int main(void)
{
    uint32_t r, g, b, er = 999, eg = 999, eb = 999;
    const uint32_t maske = PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK;

    SYS_Init();
    UART_Open(UART0, 115200);

    ADC_POWER_ON(ADC);
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE_CYCLE, BIT3 | BIT4 | BIT5);

    /* 3 kanal: 1 kHz, %0 duty ile baslat */
    PWM_Kanal_Ayarla(PWM1, CH_R, PWM_FREKANS, 0);
    PWM_Kanal_Ayarla(PWM1, CH_G, PWM_FREKANS, 0);
    PWM_Kanal_Ayarla(PWM1, CH_B, PWM_FREKANS, 0);
    PWM_EnableOutput(PWM1, maske);
    PWM_Start(PWM1, maske);

    printf("\n05_pwm_led basladi. Potlari cevir: PB3=R  PB4=G  PB5=B\n");
    oled_baslat();
    kayan_yazi(48, "RGB LED: PB3=Kirmizi PB4=Yesil PB5=Mavi", 2);

    while (1)
    {
        ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
        ADC_START_CONV(ADC);
        while (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT));

        /* 0..4095 -> %0..100 */
        r = ADC_GET_CONVERSION_DATA(ADC, 3) * 100 / 4095;
        g = ADC_GET_CONVERSION_DATA(ADC, 4) * 100 / 4095;
        b = ADC_GET_CONVERSION_DATA(ADC, 5) * 100 / 4095;

        /* Duty = CMR / (PERIOD+1). Fonksiyon yuzdeden kendisi hesaplar */
        PWM_Kanal_Ayarla(PWM1, CH_R, PWM_FREKANS, r);
        PWM_Kanal_Ayarla(PWM1, CH_G, PWM_FREKANS, g);
        PWM_Kanal_Ayarla(PWM1, CH_B, PWM_FREKANS, b);

        if (r != er || g != eg || b != eb)
        {
            er = r; eg = g; eb = b;
            printf("R=%%%3u  G=%%%3u  B=%%%3u\n", (unsigned)r, (unsigned)g, (unsigned)b);
        }
        delay_ms(10);

        {   /* Ekran: 3 satir, her renk icin harf + cubuk + yuzde */
            const char *harf[3] = { "R", "G", "B" };
            uint32_t yuzde[3] = { r, g, b };
            char t[6];
            int i;
            oled_temizle();
            for (i = 0; i < 3; i++)
            {
                int y = i * 16;
                yazi(0, y, harf[i], 2);
                cerceve(16, y + 1, 80, 13);
                dolu_kutu(18, y + 3, (int)(yuzde[i] * 76 / 100), 9);
                snprintf(t, sizeof(t), "%u", (unsigned)yuzde[i]);
                yazi(100, y + 3, t, 1);
            }
            oled_guncelle();
        }
    }
}
