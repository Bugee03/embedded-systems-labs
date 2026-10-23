/*
 * 07 - ROTARY ENCODER (GPIO kesmesi ile)  -- Boardoza M032KG
 *
 * Yaptigi: Karttaki encoder cevrildikce pozisyonu artirir/azaltir,
 *          encoder'in dugmesine basilinca sifirlar. Pozisyona gore
 *          LED5..LED8'den biri yanar. Degisiklikler seri porta yazilir.
 *
 * Boardoza semasi:
 *   A = PB7,  B = PB6,  dugme (S1) = PA7   (hepsinde dis pull-up + RC filtre var)
 *   LED5..LED8 = PF8, PF7, PF6, PF14
 *
 * Mantik: A kanali 1->0 dustugu anda B'ye bakilir.
 *   B = 1 ise bir yon (+1), B = 0 ise diger yon (-1).
 * M031/M032'de donanimsal encoder (QEI) yok, bu yuzden kesme kullaniyoruz.
 */
#include <stdio.h>
#include "NuMicro.h"
#include "oled.h"             /* ../ortak/oled.c: OLED (J9), takili degilse sessizce atlanir */

#define ENC_A   PB7
#define ENC_B   PB6
#define ENC_SW  PA7

volatile int32_t g_pos = 0;

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
    SystemCoreClockUpdate();
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
    SYS_LockReg();
}

/* PA/PB ortak kesmesi */
void GPABGH_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PB, BIT7))            /* A kanali dustu */
    {
        GPIO_CLR_INT_FLAG(PB, BIT7);
        if (ENC_B) g_pos++;                     /* yon B'nin seviyesinden */
        else       g_pos--;
    }
    if (GPIO_GET_INT_FLAG(PA, BIT7))            /* encoder dugmesi */
    {
        GPIO_CLR_INT_FLAG(PA, BIT7);
        g_pos = 0;
    }
}

static void led_goster(int32_t p)
{
    uint32_t i = (uint32_t)(((p % 4) + 4) % 4);  /* negatifte de 0..3 */
    PF8 = (i == 0); PF7 = (i == 1); PF6 = (i == 2); PF14 = (i == 3);
}

int main(void)
{
    int32_t son = 0;

    SYS_Init();
    UART_Open(UART0, 115200);

    GPIO_SetMode(PF, BIT8 | BIT7 | BIT6 | BIT14, GPIO_MODE_OUTPUT);
    led_goster(0);

    /* Kartta dis pull-up var -> normal giris */
    GPIO_SetMode(PB, BIT7 | BIT6, GPIO_MODE_INPUT);
    GPIO_SetMode(PA, BIT7, GPIO_MODE_INPUT);

    /* Kisa debounce: LIRC x 16 ~ 0.4 ms (uzun olursa hizli cevirmede adim kacar) */
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_16);
    GPIO_ENABLE_DEBOUNCE(PB, BIT7 | BIT6);
    GPIO_ENABLE_DEBOUNCE(PA, BIT7);

    GPIO_EnableInt(PB, 7, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 7, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPIO_PAPB_IRQn);

    printf("\n07_encoder basladi. Cevir / bas.\n");
    oled_baslat();
    kayan_yazi(48, "Cevir: +/-   Bas: sifirla   (A=PB7 B=PB6 SW=PA7)", 2);

    while (1)
    {
        int32_t p = g_pos;
        char t[12];
        if (p != son)
        {
            printf("Pozisyon: %d  (%s)\n", (int)p, p == 0 ? "sifir" : (p > son ? "+" : "-"));
            led_goster(p);
            son = p;
        }

        /* Ekran: pozisyon BUYUK (olcek 4) */
        snprintf(t, sizeof(t), "%d", (int)p);
        oled_temizle();
        yazi(0, 0, "POZ", 2);
        yazi_orta(14, t, 4);
        oled_guncelle();                     /* kesme sayar, ekran sadece gosterir */
    }
}
