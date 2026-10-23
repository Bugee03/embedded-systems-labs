/*
 * 06 - BUZZER ile NOTA / MELODI (PWM frekansi degistirerek)
 *
 * Yaptigi: Do-Re-Mi gamini, ardindan kisa bir melodi calar; tekrar eder.
 *          LED parlakliginin tersine burada DUTY sabit (%50), FREKANS degisir.
 *
 * Boardoza semasi: Buzzer BZ1 = PC9 (PWM1_CH3), NPN transistor (Q1) ile surulur.
 *   Karttaki buzzer pasif tiptir: hangi frekansi verirsen o notayi calar.
 */
#include <stdio.h>
#include "NuMicro.h"
#include "pwm_duzelt.h"        /* PWM_ConfigOutputChannel duzeltmesi (bkz. ../ortak/pwm_duzelt.h) */
#include "oled.h"             /* ../ortak/oled.c: OLED (J9), takili degilse sessizce atlanir */

#define BUZ_CH     3
#define BUZ_MASK   PWM_CH_3_MASK

/* Nota frekanslari (Hz), 4. oktav */
#define DO   262
#define RE   294
#define MI   330
#define FA   349
#define SOL  392
#define LA   440
#define SI   494
#define DO5  523
#define SUS  0

static void delay_ms(uint32_t ms) { while (ms--) CLK_SysTickDelay(1000); }

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    CLK_EnableModuleClock(PWM1_MODULE);
    CLK_SetModuleClock(PWM1_MODULE, CLK_CLKSEL2_PWM1SEL_PCLK1, 0);
    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* PC9 -> PWM1_CH3 */
    SYS->GPC_MFPH = (SYS->GPC_MFPH & ~SYS_GPC_MFPH_PC9MFP_Msk) | SYS_GPC_MFPH_PC9MFP_PWM1_CH3;

    SYS_LockReg();
}

/* Frekanstan nota adi (ekran icin) */
static const char *nota_adi(uint32_t f)
{
    switch (f)
    {
        case DO: return "DO";   case RE: return "RE";  case MI: return "MI";  case FA: return "FA";
        case SOL: return "SOL"; case LA: return "LA";  case SI: return "SI";  case DO5: return "DO5";
        default: return "--";
    }
}

/* f Hz'de 'ms' milisaniye cal. f = 0 ise sus. */
static void ton(uint32_t f, uint32_t ms)
{
    char hz[12];
    snprintf(hz, sizeof(hz), f ? "%u Hz" : "Passif Abbas!!!", (unsigned)f);
    oled_temizle();
    yazi_orta(0, hz, 2);
    yazi_orta(18, nota_adi(f), 3);
    oled_guncelle();                                    /* ~25 ms, notaya eklenir */

    if (f)
    {
        PWM_Kanal_Ayarla(PWM1, BUZ_CH, f, 50);   /* %50 duty = en yuksek ses */
        PWM_EnableOutput(PWM1, BUZ_MASK);
    }
    else
    {
        PWM_DisableOutput(PWM1, BUZ_MASK);
    }
    delay_ms(ms);
    PWM_DisableOutput(PWM1, BUZ_MASK);
    delay_ms(20);                                       /* notalar birbirine yapismasin */
}

static const uint16_t gam[] = { DO, RE, MI, FA, SOL, LA, SI, DO5 };

/* "Ali Baba'nin ciftligi" basi: nota, sure(ms) */
static const uint16_t melodi[][2] = {
    {DO, 300}, {DO, 300}, {DO, 300}, {SOL, 600},
    {LA, 300}, {LA, 300}, {SOL, 600},
    {MI, 300}, {MI, 300}, {RE, 300}, {RE, 300}, {DO, 900},
};

int main(void)
{
    uint32_t i;

    SYS_Init();
    UART_Open(UART0, 115200);

    PWM_Kanal_Ayarla(PWM1, BUZ_CH, 1000, 50);
    PWM_Start(PWM1, BUZ_MASK);
    PWM_DisableOutput(PWM1, BUZ_MASK);                  /* baslangicta sessiz */

    printf("\n06_buzzer basladi\n");
    oled_baslat();
    kayan_yazi(48, "Buzzer PC9 (PWM1_CH3): duty %50, frekans = nota", 2);

    while (1)
    {
        printf("PASSIF ABBAS BUYRUUUN!!!!\n");
        for (i = 0; i < sizeof(gam) / sizeof(gam[0]); i++)
            ton(gam[i], 250);

        delay_ms(500);

        printf("Melodi\n");
        for (i = 0; i < sizeof(melodi) / sizeof(melodi[0]); i++)
            ton(melodi[i][0], melodi[i][1]);

        ton(SUS, 1500);
    }
}
