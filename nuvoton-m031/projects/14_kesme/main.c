/*
 * 14 - KESMELER (INTERRUPT): her buton bir is yapar  -- Boardoza M032KG
 *
 * Bu projede main dongusu butonlari HIC okumaz. Her sey kesmelerle olur:
 *
 *   Kaynak            Kesme fonksiyonu      Ne yapar
 *   ---------------   -------------------   ----------------------------------------
 *   TIMER0 (1 kHz)    TMR0_IRQHandler       zaman sayaci + kayan isik + bip suresi
 *   SW2  (PC12)       GPCDEF_IRQHandler     kayan isigi BASLAT / DURDUR
 *   SW3  (PC11)       GPCDEF_IRQHandler     BIP sesi
 *   SW4  (PC10)       GPCDEF_IRQHandler     RGB LED'in rengini degistir
 *   SW5  (PB1)        GPABGH_IRQHandler     sayaclari SIFIRLA
 *   Encoder A (PB7)   GPABGH_IRQHandler     kayan isigin HIZINI degistir
 *   Encoder SW (PA7)  GPABGH_IRQHandler     kayan isigin YONUNU degistir
 *
 * main dongusu sadece ekrani ve seri portu gunceller.
 * OLED (J9) takiliysa son olay buyuk yazilir, kesme sayilari altta kayar.
 */
#include <stdio.h>
#include "NuMicro.h"
#include "pwm_duzelt.h"        /* PWM_ConfigOutputChannel duzeltmesi (bkz. ../ortak/pwm_duzelt.h) */
#include "oled.h"

/* ---- Kesmeler ile main arasinda paylasilan degiskenler: hepsi volatile ---- */
volatile uint32_t g_ms = 0;              /* TIMER0: her 1 ms'de +1                */
volatile int      g_isik_acik = 0;       /* kayan isik calisiyor mu               */
volatile int      g_yon = 1;             /* +1 / -1                               */
volatile int32_t  g_hiz_ms = 150;        /* kayan isik adim suresi                */
volatile uint32_t g_bip_bitis = 0;       /* bu ms'de buzzer susar (0 = calmiyor)  */
volatile uint32_t g_renk = 0;            /* RGB renk indeksi                      */
volatile uint32_t g_bip_istek = 0;       /* SW3 -> main: bip cal                  */
volatile uint32_t g_olay = 0;            /* son olay (ekran icin)                 */
volatile uint32_t g_sayac[7];            /* her kaynak icin kesme sayisi          */

enum { O_YOK, O_SW2, O_SW3, O_SW4, O_SW5, O_ENC, O_ENCSW };
static const char *olay_adi[] = { "HAZIR", "SW2 ISIK", "SW3 BIP", "SW4 RENK", "SW5 SIFIR", "ENC HIZ", "ENC YON" };

/* LED5..LED8 = PF8, PF7, PF6, PF14 */
static void led_yaz(int i)
{
    PF8 = (i == 0); PF7 = (i == 1); PF6 = (i == 2); PF14 = (i == 3);
}

/* RGB: R = PC3, G = PC5, B = PC4 (burada PWM degil, sadece ac/kapa) */
static void rgb_yaz(uint32_t r)
{
    static const uint8_t renk[][3] = {   /* R G B */
        {0,0,0}, {1,0,0}, {0,1,0}, {0,0,1}, {1,1,0}, {0,1,1}, {1,0,1}, {1,1,1}
    };
    r %= 8;
    PC3 = renk[r][0]; PC5 = renk[r][1]; PC4 = renk[r][2];
}

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);
    CLK_EnableModuleClock(PWM1_MODULE);                 /* buzzer */
    CLK_SetModuleClock(PWM1_MODULE, CLK_CLKSEL2_PWM1SEL_PCLK1, 0);
    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
    SYS->GPC_MFPH = (SYS->GPC_MFPH & ~SYS_GPC_MFPH_PC9MFP_Msk) | SYS_GPC_MFPH_PC9MFP_PWM1_CH3;
    SYS_LockReg();
}

/* ================= KESME FONKSIYONLARI (kisa tut!) ================= */

/* 1 ms'de bir */
void TMR0_IRQHandler(void)
{
    static int adim = 0;
    static uint32_t son_adim = 0;

    TIMER_ClearIntFlag(TIMER0);
    g_ms++;
    g_sayac[0]++;

    if (g_isik_acik && g_ms - son_adim >= (uint32_t)g_hiz_ms)   /* kayan isik */
    {
        son_adim = g_ms;
        adim = (adim + g_yon + 4) % 4;
        led_yaz(adim);
    }

    if (g_bip_bitis && g_ms >= g_bip_bitis)                     /* bip suresi doldu */
    {
        PWM_DisableOutput(PWM1, PWM_CH_3_MASK);
        g_bip_bitis = 0;
    }
}

/* PC..PF pinleri: SW2, SW3, SW4 */
void GPCDEF_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PC, BIT12))
    {
        GPIO_CLR_INT_FLAG(PC, BIT12);
        g_isik_acik = !g_isik_acik;
        if (!g_isik_acik) led_yaz(-1);          /* hepsini sondur */
        g_sayac[O_SW2]++; g_olay = O_SW2;
    }
    if (GPIO_GET_INT_FLAG(PC, BIT11))
    {
        GPIO_CLR_INT_FLAG(PC, BIT11);
        g_bip_istek = 1;                        /* PWM ayari main'de yapilir */
        g_sayac[O_SW3]++; g_olay = O_SW3;
    }
    if (GPIO_GET_INT_FLAG(PC, BIT10))
    {
        GPIO_CLR_INT_FLAG(PC, BIT10);
        g_renk = (g_renk + 1) % 8;
        rgb_yaz(g_renk);
        g_sayac[O_SW4]++; g_olay = O_SW4;
    }
}

/* PA/PB pinleri: SW5, encoder A, encoder dugmesi */
void GPABGH_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PB, BIT1))
    {
        GPIO_CLR_INT_FLAG(PB, BIT1);
        uint32_t i;
        for (i = 0; i < 7; i++) g_sayac[i] = 0;
        g_olay = O_SW5;
    }
    if (GPIO_GET_INT_FLAG(PB, BIT7))            /* encoder A dustu */
    {
        GPIO_CLR_INT_FLAG(PB, BIT7);
        g_hiz_ms += PB6 ? -10 : 10;             /* yon: B kanali */
        if (g_hiz_ms < 20)   g_hiz_ms = 20;
        if (g_hiz_ms > 1000) g_hiz_ms = 1000;
        g_sayac[O_ENC]++; g_olay = O_ENC;
    }
    if (GPIO_GET_INT_FLAG(PA, BIT7))
    {
        GPIO_CLR_INT_FLAG(PA, BIT7);
        g_yon = -g_yon;
        g_sayac[O_ENCSW]++; g_olay = O_ENCSW;
    }
}

/* ================= main ================= */
int main(void)
{
    uint32_t son_olay = 99, son_ekran = 0;
    char buyuk[16], kayan[96];

    SYS_Init();
    UART_Open(UART0, 115200);

    /* Cikislar: LED'ler + RGB */
    GPIO_SetMode(PF, BIT8 | BIT7 | BIT6 | BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT3 | BIT4 | BIT5, GPIO_MODE_OUTPUT);
    led_yaz(-1); rgb_yaz(0);

    /* Buzzer PWM'i hazirla ama sessiz basla */
    PWM_Kanal_Ayarla(PWM1, 3, 2000, 50);
    PWM_Start(PWM1, PWM_CH_3_MASK);
    PWM_DisableOutput(PWM1, PWM_CH_3_MASK);

    /* Girisler (kartta dis pull-up var) + debounce */
    GPIO_SetMode(PC, BIT12 | BIT11 | BIT10, GPIO_MODE_INPUT);
    GPIO_SetMode(PB, BIT1 | BIT7 | BIT6, GPIO_MODE_INPUT);
    GPIO_SetMode(PA, BIT7, GPIO_MODE_INPUT);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_64);   /* ~1.7 ms */
    GPIO_ENABLE_DEBOUNCE(PC, BIT12 | BIT11 | BIT10);
    GPIO_ENABLE_DEBOUNCE(PB, BIT1 | BIT7 | BIT6);
    GPIO_ENABLE_DEBOUNCE(PA, BIT7);

    /* GPIO kesmeleri: dusen kenar (basinca) */
    GPIO_EnableInt(PC, 12, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 11, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 10, GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 1,  GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 7,  GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 7,  GPIO_INT_FALLING);

    /* TIMER0: 1 kHz periyodik kesme */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER0);

    /* Oncelik: timer en onemli (0 = en yuksek, M0'da 0..3) */
    NVIC_SetPriority(TMR0_IRQn, 0);
    NVIC_SetPriority(GPIO_PCPDPEPF_IRQn, 1);
    NVIC_SetPriority(GPIO_PAPB_IRQn, 1);

    NVIC_EnableIRQ(TMR0_IRQn);
    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
    NVIC_EnableIRQ(GPIO_PAPB_IRQn);
    TIMER_Start(TIMER0);

    oled_baslat();
    printf("\n14_kesme basladi\n");
    printf("SW2=isik  SW3=bip  SW4=renk  SW5=sifirla  Encoder=hiz  Encoder bas=yon\n");

    while (1)
    {
        /* SW3 istegi: bip baslat (bitirmeyi timer kesmesi yapar) */
        if (g_bip_istek)
        {
            g_bip_istek = 0;
            PWM_EnableOutput(PWM1, PWM_CH_3_MASK);
            g_bip_bitis = g_ms + 80;            /* 80 ms */
        }

        /* Yeni olay varsa seri porta yaz */
        if (g_olay != son_olay)
        {
            son_olay = g_olay;
            printf("Olay: %-10s  hiz=%d ms  yon=%+d  renk=%u\n",
                   olay_adi[son_olay], (int)g_hiz_ms, g_yon, (unsigned)g_renk);
        }

        /* Ekran ~25 kare/sn */
        if (g_ms - son_ekran >= 40)
        {
            son_ekran = g_ms;
            oled_temizle();
            yazi_orta(0, "KESME", 2);
            snprintf(buyuk, sizeof(buyuk), "%s", olay_adi[son_olay]);
            yazi_orta(20, buyuk, 2);
            snprintf(buyuk, sizeof(buyuk), "%dms %s", (int)g_hiz_ms, g_yon > 0 ? ">>" : "<<");
            yazi_orta(36, buyuk, 1);
            snprintf(kayan, sizeof(kayan), "SW2:%u SW3:%u SW4:%u ENC:%u ENCSW:%u  TMR0:%u",
                     (unsigned)g_sayac[O_SW2], (unsigned)g_sayac[O_SW3], (unsigned)g_sayac[O_SW4],
                     (unsigned)g_sayac[O_ENC], (unsigned)g_sayac[O_ENCSW], (unsigned)g_sayac[0]);
            kayan_yazi(48, kayan, 2);
            oled_guncelle();
        }
    }
}
