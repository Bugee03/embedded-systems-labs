/*
 * 16 - MUZIK KUTUSU: 4 buton = 4 melodi (kesmelerle)  -- Boardoza M032KG
 *
 *   SW2 (PC12) -> 1. Beethoven - Ode to Joy (Nese Marsi)
 *   SW3 (PC11) -> 2. Twinkle Twinkle Little Star (Daha Dun Annemizin)
 *   SW4 (PC10) -> 3. Jingle Bells
 *   SW5 (PB1)  -> 4. Beethoven - Fur Elise
 *   Encoder cevir  -> tempo (hizli / yavas)
 *   Encoder bas    -> durdur
 *
 * Mantik:
 *   - Melodiyi TIMER0 kesmesi (1 ms) calar: notanin suresi dolunca bir sonraki
 *     notanin frekansini PWM'e yazar. main hic beklemez.
 *   - Buton kesmesi sadece "hangi melodi" degiskenini degistirir ve basa sarar.
 *     Bu yuzden muzik notanin ortasinda bile ANINDA degisir.
 *   - LED5..LED8 hangi melodinin caldigini, RGB LED notaya gore renk gosterir.
 *   - OLED (J9): melodi adi, calan nota (buyuk), tempo.
 *
 * Buzzer: PC9 (PWM1_CH3). Duty %50, frekans = nota.
 */
#include <stdio.h>
#include "NuMicro.h"
#include "pwm_duzelt.h"        /* PWM_ConfigOutputChannel duzeltmesi (bkz. ../ortak/pwm_duzelt.h) */
#include "oled.h"

/* ---------- Nota frekanslari (Hz) ---------- */
#define C4 262
#define D4 294
#define E4 330
#define F4 349
#define G4 392
#define GS4 415
#define A4 440
#define B4 494
#define C5 523
#define D5 587
#define DS5 622
#define E5 659
#define F5 698
#define G5 784
#define R  0          /* sus (rest) */

typedef struct { uint16_t f; uint8_t vurus; } nota_t;   /* vurus: 1 = sekizlik, 2 = dortluk, 4 = ikilik */

/* 1. Ode to Joy */
static const nota_t m1[] = {
    {E4,2},{E4,2},{F4,2},{G4,2},{G4,2},{F4,2},{E4,2},{D4,2},{C4,2},{C4,2},{D4,2},{E4,2},{E4,3},{D4,1},{D4,4},
    {E4,2},{E4,2},{F4,2},{G4,2},{G4,2},{F4,2},{E4,2},{D4,2},{C4,2},{C4,2},{D4,2},{E4,2},{D4,3},{C4,1},{C4,4},{R,4},
};
/* 2. Twinkle Twinkle */
static const nota_t m2[] = {
    {C4,2},{C4,2},{G4,2},{G4,2},{A4,2},{A4,2},{G4,4},{F4,2},{F4,2},{E4,2},{E4,2},{D4,2},{D4,2},{C4,4},
    {G4,2},{G4,2},{F4,2},{F4,2},{E4,2},{E4,2},{D4,4},{G4,2},{G4,2},{F4,2},{F4,2},{E4,2},{E4,2},{D4,4},{R,4},
};
/* 3. Jingle Bells */
static const nota_t m3[] = {
    {E4,2},{E4,2},{E4,4},{E4,2},{E4,2},{E4,4},{E4,2},{G4,2},{C4,3},{D4,1},{E4,4},{R,2},
    {F4,2},{F4,2},{F4,3},{F4,1},{F4,2},{E4,2},{E4,2},{E4,1},{E4,1},{E4,2},{D4,2},{D4,2},{E4,2},{D4,4},{G4,4},{R,4},
};
/* 4. Fur Elise (giris) */
static const nota_t m4[] = {
    {E5,1},{DS5,1},{E5,1},{DS5,1},{E5,1},{B4,1},{D5,1},{C5,1},{A4,3},{R,1},{C4,1},{E4,1},{A4,1},{B4,3},{R,1},
    {E4,1},{GS4,1},{B4,1},{C5,3},{R,1},{E4,1},{E5,1},{DS5,1},{E5,1},{DS5,1},{E5,1},{B4,1},{D5,1},{C5,1},{A4,3},{R,1},
    {C4,1},{E4,1},{A4,1},{B4,3},{R,1},{E4,1},{C5,1},{B4,1},{A4,4},{R,4},
};

typedef struct { const char *ad; const nota_t *n; uint32_t adet; } melodi_t;
static const melodi_t melodi[4] = {
    { "ODE TO JOY",   m1, sizeof(m1) / sizeof(m1[0]) },
    { "TWINKLE",      m2, sizeof(m2) / sizeof(m2[0]) },
    { "JINGLE BELLS", m3, sizeof(m3) / sizeof(m3[0]) },
    { "FUR ELISE",    m4, sizeof(m4) / sizeof(m4[0]) },
};

/* ---------- Kesmelerle paylasilan durum ---------- */
volatile int      g_secim = -1;          /* calan melodi (-1 = sus) */
volatile uint32_t g_index = 0;           /* calan nota */
volatile uint32_t g_kalan = 0;           /* notanin kalan suresi (ms) */
volatile int32_t  g_sekizlik_ms = 180;   /* tempo: bir sekizlik notanin suresi */
volatile uint32_t g_ms = 0;

static void ses_ver(uint32_t f)
{
    if (f) { PWM_Kanal_Ayarla(PWM1, 3, f, 50); PWM_EnableOutput(PWM1, PWM_CH_3_MASK); }
    else     PWM_DisableOutput(PWM1, PWM_CH_3_MASK);
}

static void rgb_nota(uint32_t f)          /* notaya gore renk */
{
    uint32_t k = f ? (f / 37) % 6 + 1 : 0;
    PC3 = (k == 1 || k == 4 || k == 6);   /* R */
    PC5 = (k == 2 || k == 4 || k == 5);   /* G */
    PC4 = (k == 3 || k == 5 || k == 6);   /* B */
}

static void led_secim(int s)
{
    PF8 = (s == 0); PF7 = (s == 1); PF6 = (s == 2); PF14 = (s == 3);
}

static void melodi_sec(int s)             /* kesmeden cagrilir */
{
    g_secim = s;
    g_index = 0;
    g_kalan = 0;                          /* timer bir sonraki ms'de ilk notayi baslatir */
    led_secim(s);
    if (s < 0) { ses_ver(0); rgb_nota(0); }
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

/* ---------- 1 ms: muzik motoru ---------- */
void TMR0_IRQHandler(void)
{
    TIMER_ClearIntFlag(TIMER0);
    g_ms++;
    if (g_secim < 0) return;

    if (g_kalan == 0)                                   /* yeni nota */
    {
        const melodi_t *m = &melodi[g_secim];
        const nota_t *n = &m->n[g_index];
        g_kalan = (uint32_t)n->vurus * (uint32_t)g_sekizlik_ms;
        ses_ver(n->f);
        rgb_nota(n->f);
        g_index = (g_index + 1) % m->adet;              /* sona gelince basa don */
    }
    else
    {
        g_kalan--;
        if (g_kalan == 25) ses_ver(0);                   /* notalar arasi kisa sessizlik */
    }
}

/* ---------- Butonlar ---------- */
void GPCDEF_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PC, BIT12)) { GPIO_CLR_INT_FLAG(PC, BIT12); melodi_sec(0); }
    if (GPIO_GET_INT_FLAG(PC, BIT11)) { GPIO_CLR_INT_FLAG(PC, BIT11); melodi_sec(1); }
    if (GPIO_GET_INT_FLAG(PC, BIT10)) { GPIO_CLR_INT_FLAG(PC, BIT10); melodi_sec(2); }
}

void GPABGH_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PB, BIT1)) { GPIO_CLR_INT_FLAG(PB, BIT1); melodi_sec(3); }
    if (GPIO_GET_INT_FLAG(PB, BIT7))                    /* encoder: tempo */
    {
        GPIO_CLR_INT_FLAG(PB, BIT7);
        g_sekizlik_ms += PB6 ? -15 : 15;
        if (g_sekizlik_ms < 60)  g_sekizlik_ms = 60;
        if (g_sekizlik_ms > 600) g_sekizlik_ms = 600;
    }
    if (GPIO_GET_INT_FLAG(PA, BIT7)) { GPIO_CLR_INT_FLAG(PA, BIT7); melodi_sec(-1); }   /* durdur */
}

static const char *nota_adi(uint32_t f)
{
    switch (f)
    {
        case C4: return "DO";  case D4: return "RE";  case E4: return "MI";  case F4: return "FA";
        case G4: return "SOL"; case GS4: return "SOL#"; case A4: return "LA"; case B4: return "SI";
        case C5: return "DO5"; case D5: return "RE5"; case DS5: return "RE#5"; case E5: return "MI5";
        case F5: return "FA5"; case G5: return "SOL5";
        default: return "-";
    }
}

int main(void)
{
    int son_secim = -2;
    uint32_t son_ekran = 0;
    char t[40];

    SYS_Init();
    UART_Open(UART0, 115200);

    GPIO_SetMode(PF, BIT8 | BIT7 | BIT6 | BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PC, BIT3 | BIT4 | BIT5, GPIO_MODE_OUTPUT);
    led_secim(-1); rgb_nota(0);

    PWM_Kanal_Ayarla(PWM1, 3, 1000, 50);
    PWM_Start(PWM1, PWM_CH_3_MASK);
    PWM_DisableOutput(PWM1, PWM_CH_3_MASK);

    GPIO_SetMode(PC, BIT12 | BIT11 | BIT10, GPIO_MODE_INPUT);
    GPIO_SetMode(PB, BIT1 | BIT7 | BIT6, GPIO_MODE_INPUT);
    GPIO_SetMode(PA, BIT7, GPIO_MODE_INPUT);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_64);
    GPIO_ENABLE_DEBOUNCE(PC, BIT12 | BIT11 | BIT10);
    GPIO_ENABLE_DEBOUNCE(PB, BIT1 | BIT7 | BIT6);
    GPIO_ENABLE_DEBOUNCE(PA, BIT7);
    GPIO_EnableInt(PC, 12, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 11, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 10, GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 1,  GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 7,  GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 7,  GPIO_INT_FALLING);

    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER0);
    NVIC_SetPriority(TMR0_IRQn, 0);
    NVIC_SetPriority(GPIO_PCPDPEPF_IRQn, 1);
    NVIC_SetPriority(GPIO_PAPB_IRQn, 1);
    NVIC_EnableIRQ(TMR0_IRQn);
    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
    NVIC_EnableIRQ(GPIO_PAPB_IRQn);
    TIMER_Start(TIMER0);

    oled_baslat();
    printf("\n16_muzik basladi. SW2..SW5 = 4 melodi, encoder = tempo, encoder bas = dur\n");

    while (1)
    {
        int s = g_secim;

        if (s != son_secim)
        {
            son_secim = s;
            printf("%s\n", s < 0 ? "Durdu" : melodi[s].ad);
        }

        if (g_ms - son_ekran >= 40)
        {
            son_ekran = g_ms;
            oled_temizle();
            if (s < 0)
            {
                yazi_orta(0, "MUZIK", 2);
                yazi_orta(20, "SEC", 3);
                kayan_yazi(48, "SW2 Ode to Joy  SW3 Twinkle  SW4 Jingle Bells  SW5 Fur Elise", 2);
            }
            else
            {
                const melodi_t *m = &melodi[s];
                uint32_t i = (g_index + m->adet - 1) % m->adet;     /* calan nota */
                kayan_yazi(0, m->ad, 2);                            /* melodi adi (uzunsa kayar) */
                yazi_orta(20, nota_adi(m->n[i].f), 3);
                snprintf(t, sizeof(t), "tempo %d", (int)(60000 / (g_sekizlik_ms * 2)));
                yazi_orta(52, t, 1);
            }
            oled_guncelle();
        }
    }
}
