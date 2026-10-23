/*
 * 04 - TIMER KESMESI (1 ms'lik zaman tabani)
 *
 * Yaptigi: TIMER0 her 1 ms'de kesme uretir ve bir sayaci artirir (millis).
 *          main dongusu bu sayaci kullanarak beklemeden (non-blocking)
 *          LED'i 500 ms'de bir yakip sondurur ve her saniye sure yazar.
 *
 * Boardoza: LED5 = PF8 (karttaki LED)
 */
#include <stdio.h>
#include "NuMicro.h"
#include "oled.h"             /* ../ortak/oled.c: OLED (J9), takili degilse sessizce atlanir */

#define LED  PF8

volatile uint32_t g_ms = 0;         /* her 1 ms'de 1 artar */

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* TIMER0 clock'u: kaynak HIRC (48 MHz) */
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);
    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
    SYS_LockReg();
}

void TMR0_IRQHandler(void)
{
    if (TIMER_GetIntFlag(TIMER0))
    {
        TIMER_ClearIntFlag(TIMER0);
        g_ms++;
    }
}

int main(void)
{
    uint32_t son_led = 0, son_yazi = 0;

    SYS_Init();
    UART_Open(UART0, 115200);
    GPIO_SetMode(PF, BIT8, GPIO_MODE_OUTPUT);

    /* f = 1000 Hz -> periyot 1 ms. TIMER_Open prescaler ve CMP'yi kendisi hesaplar:
     * f_kesme = f_clk / ((PSC+1) * CMP)  ->  48 MHz / (1 * 48000) = 1 kHz */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);

    printf("\n04_timer basladi\n");
    oled_baslat();
    kayan_yazi(48, "TIMER0 1 kHz kesme -> g_ms sayaci, LED5 her 500 ms", 2);

    while (1)
    {
        uint32_t simdi = g_ms;

        if (simdi - son_led >= 500)          /* 500 ms gectiyse */
        {
            son_led = simdi;
            LED ^= 1;
        }

        if (simdi - son_yazi >= 1000)        /* 1 s gectiyse */
        {
            son_yazi = simdi;
            printf("Gecen sure: %u s\n", (unsigned)(simdi / 1000));
        }

        {   /* Ekran: buyuk DD:SS, kayan yazi kendi kendine ilerler */
            static uint32_t son_ekran = 0;
            char sure[8];
            if (simdi - son_ekran >= 40)    /* ~25 kare/sn */
            {
                son_ekran = simdi;
                snprintf(sure, sizeof(sure), "%02u:%02u",
                         (unsigned)(simdi / 60000 % 100), (unsigned)(simdi / 1000 % 60));
                oled_temizle();
                yazi_orta(0, "SURE", 2);
                yazi_orta(18, sure, 3);
                if (LED) dolu_kutu(120, 0, 8, 8);    /* LED durumu, sag ust */
                oled_guncelle();
            }
        }

        /* Burada baska isler de yapilabilir; hicbir yerde bekleme yok */
    }
}
