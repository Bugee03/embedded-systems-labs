/*
 * 20 - HALKA OYUNU "CYCLONE"  -- Boardoza M032KG + Phoenix24 halka (J15: PE6 = DIN, PE7 = CIN)
 *
 * Nasil oynanir:
 *   - Halkada KIRMIZI isik doner, bir LED YESIL (hedef).
 *   - Isik hedefin USTUNDEYKEN herhangi bir butona bas (SW2..SW5 veya encoder'a bas).
 *   - Tam isabet: +10 puan, seviye atlar, isik HIZLANIR ve YON DEGISTIRIR.
 *   - 1 LED yakin: +3 puan (seviye ayni).  Iska: 1 can gider.
 *   - 3 can (LED5..LED7). Canlar bitince OYUN BITTI; bir butonla yeniden basla.
 *
 * Neden kesme? Isigin konumunu TIMER0 kesmesi ilerletir; butona basildigi AN konum
 * buton kesmesinin icinde kaydedilir. main o sirada halkayi ciziyor olsa bile olcum
 * milisaniye hassasiyetinde ve adildir.
 *
 * POT4 (PB4) = halka parlakligi.  OLED (J9) varsa: seviye, skor, rekor.
 */
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "oled.h"
#include "pwm_duzelt.h"

#define N_LED 24
#define DATA  PE6
#define SAAT  PE7

typedef struct { uint8_t r, g, b; } renk_t;
static renk_t led[N_LED];
static uint8_t parlaklik = 10;

/* ---------- kesmelerle paylasilanlar ---------- */
volatile uint32_t g_ms = 0;
volatile int      g_poz = 0;             /* donen isigin konumu (0..23)            */
volatile int      g_yon = 1;             /* +1 saat yonu, -1 ters                   */
volatile uint32_t g_adim_ms = 90;        /* isigin bir LED ilerleme suresi          */
volatile int      g_kosuyor = 0;         /* isik donuyor mu                          */
volatile int      g_durdu_poz = -1;      /* butona basildigi andaki konum (-1: yok)  */
volatile uint32_t g_tus = 0;             /* oyun disinda: herhangi bir tus basildi   */
volatile uint32_t g_bip_bitis = 0;

void TMR0_IRQHandler(void)
{
    static uint32_t sayac = 0;
    TIMER_ClearIntFlag(TIMER0);
    g_ms++;
    if (g_kosuyor && ++sayac >= g_adim_ms)
    {
        sayac = 0;
        g_poz = (g_poz + g_yon + N_LED) % N_LED;
    }
    if (g_bip_bitis && g_ms >= g_bip_bitis) { g_bip_bitis = 0; PWM_DisableOutput(PWM1, PWM_CH_3_MASK); }
}

static void basildi(void)                /* her iki GPIO kesmesinden cagrilir */
{
    if (g_kosuyor && g_durdu_poz < 0) { g_durdu_poz = g_poz; g_kosuyor = 0; }   /* konumu AN'inda dondur */
    g_tus = 1;
}

void GPCDEF_IRQHandler(void)
{
    uint32_t f = PC->INTSRC & (BIT10 | BIT11 | BIT12);
    PC->INTSRC = f;
    if (f) basildi();
}

void GPABGH_IRQHandler(void)
{
    uint32_t fb = PB->INTSRC & BIT1, fa = PA->INTSRC & BIT7;
    PB->INTSRC = fb; PA->INTSRC = fa;
    if (fb || fa) basildi();
}

/* ---------- ses ---------- */
static void bip(uint32_t f, uint32_t ms)
{
    PWM_Kanal_Ayarla(PWM1, 3, f, 50);
    PWM_EnableOutput(PWM1, PWM_CH_3_MASK);
    g_bip_bitis = g_ms + ms;
}
static void bekle(uint32_t ms) { uint32_t t = g_ms; while (g_ms - t < ms) { } }

/* ---------- halka (APA102 bit-bang) ---------- */
static void bayt(uint8_t b)
{
    int i;
    for (i = 7; i >= 0; i--) { DATA = (b >> i) & 1; SAAT = 1; __NOP(); __NOP(); SAAT = 0; }
}

static void goster(void)
{
    int i;
    for (i = 0; i < 4; i++) bayt(0x00);
    for (i = 0; i < N_LED; i++) { bayt(0xE0 | parlaklik); bayt(led[i].b); bayt(led[i].g); bayt(led[i].r); }
    for (i = 0; i < 4; i++) bayt(0xFF);
}

static void hepsi(renk_t c) { int i; for (i = 0; i < N_LED; i++) led[i] = c; }

static renk_t tekerlek(uint8_t p)
{
    renk_t c;
    if (p < 85)       { c.r = 255 - p * 3; c.g = p * 3;       c.b = 0; }
    else if (p < 170) { p -= 85;  c.r = 0;  c.g = 255 - p * 3; c.b = p * 3; }
    else              { p -= 170; c.r = p * 3; c.g = 0;        c.b = 255 - p * 3; }
    return c;
}

static uint32_t tohum = 7;
static uint32_t rastgele(void) { tohum = tohum * 1103515245u + 12345u; return tohum >> 16; }

static uint32_t adc_oku(uint32_t ch)
{
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, 1UL << ch);
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
    ADC_START_CONV(ADC);
    while (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT)) { }
    return ADC_GET_CONVERSION_DATA(ADC, ch);
}

static int mesafe(int a, int b)          /* halka uzerinde en kisa mesafe */
{
    int d = a > b ? a - b : b - a;
    return d > N_LED / 2 ? N_LED - d : d;
}

/* ---------- ekran + can LED'leri ---------- */
static uint32_t skor, rekor, seviye, can;

static void durum_goster(const char *mesaj)
{
    char s[32];
    PF8 = can >= 1; PF7 = can >= 2; PF6 = can >= 3; PF14 = 0;
    oled_temizle();
    snprintf(s, sizeof(s), "SEVIYE %lu", (unsigned long)seviye);
    yazi(0, 0, s, 1);
    snprintf(s, sizeof(s), "REKOR %lu", (unsigned long)rekor);
    yazi(128 - yazi_genislik(s, 1), 0, s, 1);
    snprintf(s, sizeof(s), "%lu", (unsigned long)skor);
    yazi_orta(9, mesaj ? mesaj : s, 2);
    {
        int i;
        for (i = 0; i < 3; i++) if (can > (uint32_t)i) dolu_kutu(2 + i * 8, 26, 5, 5); else cerceve(2 + i * 8, 26, 5, 5);
    }
    yazi(40, 25, "bas: durdur", 1);
    oled_guncelle();
}

/* ---------- kurulum ---------- */
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
    CLK_EnableModuleClock(ADC_MODULE);
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL2_ADCSEL_PCLK1, CLK_CLKDIV0_ADC(1));
    SystemCoreClockUpdate();
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~SYS_GPB_MFPL_PB4MFP_Msk) | SYS_GPB_MFPL_PB4MFP_ADC0_CH4;
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPL_PE6MFP_Msk | SYS_GPE_MFPL_PE7MFP_Msk);
    SYS->GPC_MFPH = (SYS->GPC_MFPH & ~SYS_GPC_MFPH_PC9MFP_Msk) | SYS_GPC_MFPH_PC9MFP_PWM1_CH3;
    SYS_LockReg();
}

static void donanim(void)
{
    GPIO_SetMode(PE, BIT6 | BIT7, GPIO_MODE_OUTPUT); DATA = 0; SAAT = 0;
    GPIO_SetMode(PF, BIT8 | BIT7 | BIT6 | BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, BIT4, GPIO_MODE_INPUT); GPIO_DISABLE_DIGITAL_PATH(PB, BIT4);
    ADC_POWER_ON(ADC);

    PWM_Kanal_Ayarla(PWM1, 3, 2000, 50);
    PWM_Start(PWM1, PWM_CH_3_MASK);
    PWM_DisableOutput(PWM1, PWM_CH_3_MASK);

    GPIO_SetMode(PC, BIT12 | BIT11 | BIT10, GPIO_MODE_INPUT);
    GPIO_SetMode(PB, BIT1, GPIO_MODE_INPUT);
    GPIO_SetMode(PA, BIT7, GPIO_MODE_INPUT);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_64);
    GPIO_ENABLE_DEBOUNCE(PC, BIT12 | BIT11 | BIT10);
    GPIO_ENABLE_DEBOUNCE(PB, BIT1);
    GPIO_ENABLE_DEBOUNCE(PA, BIT7);
    GPIO_EnableInt(PC, 12, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 11, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 10, GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 1,  GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 7,  GPIO_INT_FALLING);

    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER0);
    NVIC_SetPriority(TMR0_IRQn, 0);                      /* konum ilerletme en yuksek oncelik */
    NVIC_EnableIRQ(TMR0_IRQn);
    NVIC_EnableIRQ(GPIO_PAPB_IRQn);
    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
    TIMER_Start(TIMER0);
}

/* ---------- animasyonlar ---------- */
static void anim_basari(void)
{
    static const uint16_t f[] = { 1047, 1319, 1568, 2093 };
    int k, i;
    for (k = 0; k < 4; k++)
    {
        bip(f[k], 70);
        for (i = 0; i < N_LED; i++) led[i] = tekerlek((uint8_t)(i * 10 + k * 60));
        goster(); bekle(90);
    }
}

static void anim_iska(int hedef, int durdu)
{
    int k;
    bip(180, 350);
    for (k = 0; k < 6; k++)
    {
        hepsi((renk_t){0, 0, 0});
        led[hedef] = (renk_t){0, 255, 0};
        if (k & 1) led[durdu] = (renk_t){255, 0, 0};
        goster(); bekle(110);
    }
}

static void anim_bitti(void)
{
    static const uint16_t f[] = { 523, 392, 330, 262 };
    int k, i;
    for (k = 0; k < 4; k++)
    {
        bip(f[k], 220);
        for (i = 0; i < N_LED; i++) led[i] = (i <= (N_LED - 1) - k * 6) ? (renk_t){255, 0, 0} : (renk_t){0, 0, 0};
        goster(); bekle(250);
    }
}

/* ---------- oyun ---------- */
int main(void)
{
    int hedef;
    uint32_t son = 0, t = 0;

    SYS_Init();
    UART_Open(UART0, 115200);
    donanim();
    oled_yerli_32(1);
    oled_baslat();
    printf("\n20_halka_oyunu: isik yesil hedefin ustundeyken bir butona bas!\n");

yeni_oyun:
    skor = 0; seviye = 1; can = 3;
    g_adim_ms = 90; g_yon = 1;

    /* bekleme ekrani: halkada gokkusagi doner, bir tusla basla */
    g_tus = 0;
    while (!g_tus)
    {
        int i;
        parlaklik = (uint8_t)(1 + adc_oku(4) * 19 / 4095);
        for (i = 0; i < N_LED; i++) led[i] = tekerlek((uint8_t)(i * 256 / N_LED + t));
        t += 3; goster();
        durum_goster("BASLA?");
        bekle(15);
    }
    tohum ^= g_ms;
    bip(1500, 60);

    for (;;)
    {
        /* yeni tur: hedefi sec, isigi hedefin karsisindan baslat */
        hedef = (int)(rastgele() % N_LED);
        __disable_irq();
        g_poz = (hedef + N_LED / 2) % N_LED;
        g_durdu_poz = -1;
        g_kosuyor = 1;
        __enable_irq();
        durum_goster(NULL);
        printf("Seviye %lu, hiz %lu ms/LED, hedef %d\n", (unsigned long)seviye, (unsigned long)g_adim_ms, hedef);

        while (g_durdu_poz < 0)                          /* isik donerken ciz */
        {
            int p = g_poz, i;
            if (g_ms - son < 8) continue;
            son = g_ms;
            parlaklik = (uint8_t)(1 + adc_oku(4) * 19 / 4095);
            for (i = 0; i < N_LED; i++) led[i] = (renk_t){0, 0, 4};                         /* hafif mavi zemin */
            led[hedef] = (renk_t){0, 255, 0};                                                /* hedef */
            led[(p - g_yon + N_LED) % N_LED] = (renk_t){60, 0, 0};                           /* kuyruk */
            led[p] = (p == hedef) ? (renk_t){255, 255, 0} : (renk_t){255, 0, 0};            /* isik (hedefte sari) */
            goster();
        }

        {   /* sonuc */
            int d = mesafe(g_durdu_poz, hedef);
            if (d == 0)
            {
                skor += 10 * seviye; seviye++;
                if (g_adim_ms > 25) g_adim_ms -= 8;      /* hizlan */
                g_yon = -g_yon;                          /* yon degistir */
                printf("TAM ISABET! skor %lu\n", (unsigned long)skor);
                durum_goster("SUPER!");
                anim_basari();
            }
            else if (d == 1)
            {
                skor += 3;
                printf("Yakin! skor %lu\n", (unsigned long)skor);
                durum_goster("YAKIN!");
                bip(1200, 120);
                anim_iska(hedef, g_durdu_poz);
            }
            else
            {
                can--;
                printf("Iska (%d LED uzak). Kalan can: %lu\n", d, (unsigned long)can);
                durum_goster("ISKA!");
                anim_iska(hedef, g_durdu_poz);
                if (can == 0)
                {
                    if (skor > rekor) rekor = skor;
                    durum_goster("BITTI");
                    anim_bitti();
                    printf("OYUN BITTI. Skor %lu, rekor %lu\n", (unsigned long)skor, (unsigned long)rekor);
                    bekle(800);
                    goto yeni_oyun;
                }
            }
            bekle(300);
        }
    }
}
