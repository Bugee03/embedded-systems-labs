/*
 * 15 - REFLEKS OYUNU (kesmelerle reaksiyon suresi olcumu)  -- Boardoza M032KG
 *
 * Nasil oynanir:
 *   1. Herhangi bir butona (SW2..SW5) bas -> "HAZIRLAN", RGB kirmizi.
 *   2. 1-4 saniye arasi rastgele bir anda tum LED'ler + RGB yesil yanar ve bip calar.
 *   3. O anda HERHANGI bir butona olabildigince hizli bas.
 *   4. Ekranda reaksiyon suren (ms) ve en iyi skorun gorunur.
 *   Isik yanmadan basarsan: "ERKEN!" (kalin bir ses ile).
 *
 * Kullanilan kesmeler:
 *   TMR0_IRQHandler   : 1 ms zaman tabani, rastgele bekleme bitince isigi yakar
 *   GPCDEF_IRQHandler : SW2 (PC12), SW3 (PC11), SW4 (PC10)
 *   GPABGH_IRQHandler : SW5 (PB1)
 * Butona basilan an KESMENIN ICINDE kaydedilir -> olcum main dongusunden bagimsiz, hassas.
 */
#include <stdio.h>
#include "NuMicro.h"
#include "pwm_duzelt.h"        /* PWM_ConfigOutputChannel duzeltmesi (bkz. ../ortak/pwm_duzelt.h) */
#include "oled.h"

enum durum_t { BOSTA, BEKLE, YANDI, SONUC, ERKEN };

volatile uint32_t g_ms = 0;
volatile enum durum_t g_durum = BOSTA;
volatile uint32_t g_yanma_ani = 0;       /* isik bu ms'de yanacak / yandi */
volatile uint32_t g_basma_ani = 0;       /* butona basildigi ms */
volatile uint32_t g_tohum = 12345;       /* rastgele sayi tohumu */

static void delay_ms(uint32_t ms) { uint32_t t = g_ms; while (g_ms - t < ms); }

static uint32_t rastgele(void)            /* basit LCG */
{
    g_tohum = g_tohum * 1103515245u + 12345u;
    return g_tohum >> 8;
}

static void ledler(int acik)
{
    PF8 = PF7 = PF6 = PF14 = (acik != 0);
}

static void rgb(int r, int g, int b) { PC3 = r; PC5 = g; PC4 = b; }

static void ses(uint32_t hz, uint32_t ms)
{
    PWM_Kanal_Ayarla(PWM1, 3, hz, 50);
    PWM_EnableOutput(PWM1, PWM_CH_3_MASK);
    delay_ms(ms);
    PWM_DisableOutput(PWM1, PWM_CH_3_MASK);
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
    CLK_EnableModuleClock(PWM1_MODULE);
    CLK_SetModuleClock(PWM1_MODULE, CLK_CLKSEL2_PWM1SEL_PCLK1, 0);
    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
    SYS->GPC_MFPH = (SYS->GPC_MFPH & ~SYS_GPC_MFPH_PC9MFP_Msk) | SYS_GPC_MFPH_PC9MFP_PWM1_CH3;
    SYS_LockReg();
}

void TMR0_IRQHandler(void)
{
    TIMER_ClearIntFlag(TIMER0);
    g_ms++;
    if (g_durum == BEKLE && g_ms >= g_yanma_ani)       /* rastgele sure doldu: ISIK! */
    {
        ledler(1);
        rgb(0, 1, 0);
        g_durum = YANDI;
    }
}

/* Her iki GPIO kesmesi de buraya gelir */
static void butona_basildi(void)
{
    g_tohum ^= g_ms;                                   /* insan zamanlamasi = iyi rastgelelik */
    switch (g_durum)
    {
        case BOSTA:
        case SONUC:
        case ERKEN:  g_basma_ani = g_ms; g_durum = BEKLE;
                     g_yanma_ani = 0xFFFFFFFF;         /* main ayarlayacak */
                     break;
        case BEKLE:  g_durum = ERKEN; break;           /* isik yanmadan basti */
        case YANDI:  g_basma_ani = g_ms; g_durum = SONUC; break;
    }
}

void GPCDEF_IRQHandler(void)
{
    uint32_t f = PC->INTSRC & (BIT10 | BIT11 | BIT12);
    PC->INTSRC = f;                                     /* hepsini temizle */
    if (f) butona_basildi();
}

void GPABGH_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PB, BIT1)) { GPIO_CLR_INT_FLAG(PB, BIT1); butona_basildi(); }
}

int main(void)
{
    enum durum_t son = (enum durum_t)-1;
    uint32_t sure = 0, en_iyi = 0xFFFFFFFF, deneme = 0, son_ekran = 0;
    char t[24];

    SYS_Init();
    UART_Open(UART0, 115200);

    GPIO_SetMode(PF, BIT8 | BIT7 | BIT6 | BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT3 | BIT4 | BIT5, GPIO_MODE_OUTPUT);
    ledler(0); rgb(0, 0, 0);

    PWM_Kanal_Ayarla(PWM1, 3, 1000, 50);
    PWM_Start(PWM1, PWM_CH_3_MASK);
    PWM_DisableOutput(PWM1, PWM_CH_3_MASK);

    GPIO_SetMode(PC, BIT12 | BIT11 | BIT10, GPIO_MODE_INPUT);
    GPIO_SetMode(PB, BIT1, GPIO_MODE_INPUT);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_64);   /* ~1.7 ms: olcumu bozmaz */
    GPIO_ENABLE_DEBOUNCE(PC, BIT12 | BIT11 | BIT10);
    GPIO_ENABLE_DEBOUNCE(PB, BIT1);
    GPIO_EnableInt(PC, 12, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 11, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 10, GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 1,  GPIO_INT_FALLING);

    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER0);
    NVIC_SetPriority(TMR0_IRQn, 0);                     /* zaman tabani en yuksek oncelik */
    NVIC_EnableIRQ(TMR0_IRQn);
    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
    NVIC_EnableIRQ(GPIO_PAPB_IRQn);
    TIMER_Start(TIMER0);

    oled_baslat();
    printf("\n15_refleks_oyunu basladi. Baslamak icin bir butona bas.\n");

    while (1)
    {
        enum durum_t d = g_durum;

        if (d != son)                                   /* durum degisti */
        {
            son = d;
            switch (d)
            {
                case BOSTA: break;
                case BEKLE:
                    ledler(0); rgb(1, 0, 0);
                    g_yanma_ani = g_ms + 1000 + rastgele() % 3000;   /* 1..4 s sonra */
                    printf("Hazirlan...\n");
                    break;
                case YANDI:
                    ses(2000, 30);
                    break;
                case SONUC:
                    sure = g_basma_ani - g_yanma_ani;
                    deneme++;
                    if (sure < en_iyi) en_iyi = sure;
                    ledler(0); rgb(0, 0, 1);
                    printf("Reaksiyon: %u ms  (en iyi %u ms, %u. deneme)\n",
                           (unsigned)sure, (unsigned)en_iyi, (unsigned)deneme);
                    break;
                case ERKEN:
                    ledler(0); rgb(1, 0, 1);
                    printf("ERKEN basildi!\n");
                    ses(200, 300);
                    break;
            }
        }

        if (g_ms - son_ekran >= 40)
        {
            son_ekran = g_ms;
            oled_temizle();
            switch (d)
            {
                case BOSTA:
                    yazi_orta(0, "REFLEKS", 2);
                    yazi_orta(22, "BASLA?", 2);
                    kayan_yazi(48, "Bir butona bas, LED'ler yaninca hemen tekrar bas!", 2);
                    break;
                case BEKLE:
                    yazi_orta(0, "BEKLE", 2);
                    yazi_orta(20, "...", 3);
                    kayan_yazi(48, "Isik yanmadan basma!", 2);
                    break;
                case YANDI:
                    yazi_orta(18, "BAS!", 4);
                    kayan_yazi(48, "", 2);
                    break;
                case SONUC:
                    snprintf(t, sizeof(t), "%u ms", (unsigned)sure);
                    yazi_orta(0, sure < 250 ? "SUPER!" : (sure < 400 ? "IYI" : "YAVAS"), 2);
                    yazi_orta(18, t, 3);
                    snprintf(t, sizeof(t), "En iyi: %u ms  Deneme: %u", (unsigned)en_iyi, (unsigned)deneme);
                    kayan_yazi(48, t, 2);
                    break;
                case ERKEN:
                    yazi_orta(0, "HATA", 2);
                    yazi_orta(18, "ERKEN!", 3);
                    kayan_yazi(48, "Tekrar denemek icin bir butona bas", 2);
                    break;
            }
            oled_guncelle();
        }
    }
}
