/*
 * 18 - LED HALKA SOVU: Phoenix24 (24 x APA102) -- Boardoza M032KG
 *
 * Baglanti: J15 -> PE6 = DATA, PE7 = CLOCK (yazilimla surulur), halkanin 5V/GND'si -> J6
 *
 * 8 efekt, her 8 saniyede bir kendiliginden degisir:
 *   GOKKUSAGI, KUYRUKLU YILDIZ, NEFES, POLIS, YILDIZLAR, DOLDUR, SAAT, ATES
 *
 * Kontroller:
 *   Encoder cevir : sonraki / onceki efekt (otomatik gecis durur)
 *   Encoder bas   : otomatik gecisi tekrar ac
 *   POT3 (PB3)    : hiz          POT4 (PB4) : parlaklik
 *   OLED (J9)     : efekt adi     LED5..8    : efekt numarasi     Buzzer: efekt degisince bip
 */
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"
#include "oled.h"
#include "pwm_duzelt.h"

#define N_LED    24
#define DATA     PE6
#define SAAT_PIN PE7

typedef struct { uint8_t r, g, b; } renk_t;
static renk_t led[N_LED];
static uint8_t parlaklik = 8;            /* APA102 global parlaklik 1..31 */

volatile uint32_t g_ms = 0;
volatile int32_t  g_enc = 0;
volatile uint32_t g_enc_bas = 0;
volatile uint32_t g_bip_bitis = 0;

/* ---------------- kesmeler ---------------- */
void TMR0_IRQHandler(void)
{
    TIMER_ClearIntFlag(TIMER0);
    g_ms++;
    if (g_bip_bitis && g_ms >= g_bip_bitis) { g_bip_bitis = 0; PWM_DisableOutput(PWM1, PWM_CH_3_MASK); }
}

void GPABGH_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PB, BIT7)) { GPIO_CLR_INT_FLAG(PB, BIT7); g_enc += PB6 ? 1 : -1; }
    if (GPIO_GET_INT_FLAG(PA, BIT7)) { GPIO_CLR_INT_FLAG(PA, BIT7); g_enc_bas = 1; }
}

static void bip(uint32_t f, uint32_t ms)
{
    PWM_Kanal_Ayarla(PWM1, 3, f, 50);
    PWM_EnableOutput(PWM1, PWM_CH_3_MASK);
    g_bip_bitis = g_ms + ms;
}

/* ---------------- APA102 surme (bit-bang) ---------------- */
static void bayt_gonder(uint8_t b)
{
    int i;
    for (i = 7; i >= 0; i--)
    {
        DATA = (b >> i) & 1;
        SAAT_PIN = 1;
        __NOP(); __NOP();
        SAAT_PIN = 0;
    }
}

static void halka_goster(void)
{
    int i;
    for (i = 0; i < 4; i++) bayt_gonder(0x00);                 /* baslangic cercevesi */
    for (i = 0; i < N_LED; i++)
    {
        bayt_gonder(0xE0 | parlaklik);
        bayt_gonder(led[i].b);
        bayt_gonder(led[i].g);
        bayt_gonder(led[i].r);
    }
    for (i = 0; i < 4; i++) bayt_gonder(0xFF);                 /* bitis cercevesi */
}

/* ---------------- yardimcilar ---------------- */
static renk_t tekerlek(uint8_t p)                              /* 0..255 -> gokkusagi */
{
    renk_t c;
    if (p < 85)       { c.r = 255 - p * 3; c.g = p * 3;       c.b = 0; }
    else if (p < 170) { p -= 85;  c.r = 0;  c.g = 255 - p * 3; c.b = p * 3; }
    else              { p -= 170; c.r = p * 3; c.g = 0;        c.b = 255 - p * 3; }
    return c;
}

static renk_t olcekle(renk_t c, uint32_t k)                    /* k: 0..255 */
{
    renk_t o = { (uint8_t)(c.r * k / 255), (uint8_t)(c.g * k / 255), (uint8_t)(c.b * k / 255) };
    return o;
}

static void hepsi(renk_t c) { int i; for (i = 0; i < N_LED; i++) led[i] = c; }

static uint32_t tohum = 12345;
static uint32_t rastgele(void) { tohum = tohum * 1103515245u + 12345u; return tohum >> 16; }

static uint32_t adc_oku(uint32_t ch)
{
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, 1UL << ch);
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
    ADC_START_CONV(ADC);
    while (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT)) { }
    return ADC_GET_CONVERSION_DATA(ADC, ch);
}

/* ---------------- efektler (her cagri = 1 kare, t = zaman sayaci) ---------------- */
static void ef_gokkusagi(uint32_t t)
{
    int i;
    for (i = 0; i < N_LED; i++) led[i] = tekerlek((uint8_t)(i * 256 / N_LED + t * 2));
}

static void ef_kuyruklu(uint32_t t)
{
    int i, bas = (int)((t / 2) % N_LED);
    renk_t c = tekerlek((uint8_t)(t / 3));
    for (i = 0; i < N_LED; i++) led[i] = olcekle(led[i], 140);  /* kuyruk yavasca soner */
    led[bas] = c;
    led[(bas + N_LED / 2) % N_LED] = tekerlek((uint8_t)(t / 3 + 128));   /* karsi tarafta ikinci yildiz */
}

static void ef_nefes(uint32_t t)
{
    uint32_t f = (t * 4) % 512, k = f < 256 ? f : 511 - f;      /* ucgen dalga 0..255..0 */
    hepsi(olcekle(tekerlek((uint8_t)(t / 16)), k * k / 255));    /* karesel: daha dogal nefes */
}

static void ef_polis(uint32_t t)
{
    int i, faz = (int)((t / 6) % 8);
    renk_t kir = {255, 0, 0}, mav = {0, 0, 255}, yok = {0, 0, 0};
    for (i = 0; i < N_LED; i++)
    {
        int sol = i < N_LED / 2;
        int yan = (faz < 4) ? (sol && (faz & 1) == 0) : (!sol && (faz & 1) == 0);
        led[i] = yan ? (sol ? kir : mav) : yok;
    }
}

static void ef_yildizlar(uint32_t t)
{
    int i;
    renk_t taban = {0, 0, 12};
    (void)t;
    for (i = 0; i < N_LED; i++)
    {
        led[i] = olcekle(led[i], 200);
        if (led[i].b < taban.b) led[i] = taban;
    }
    if (rastgele() % 3 == 0) { renk_t b = {255, 255, 255}; led[rastgele() % N_LED] = b; }
}

static void ef_doldur(uint32_t t)
{
    uint32_t adim = (t / 2) % (N_LED * 2);
    renk_t c = tekerlek((uint8_t)((t / (N_LED * 4)) * 50));
    int i;
    for (i = 0; i < N_LED; i++)
    {
        int dolu = adim < N_LED ? (i <= (int)adim) : (i > (int)(adim - N_LED));
        led[i] = dolu ? c : (renk_t){0, 0, 0};
    }
}

static void ef_saat(uint32_t t)
{
    int i;
    uint32_t sn = g_ms / 1000, dk = sn / 60;
    (void)t;
    for (i = 0; i < N_LED; i++) led[i] = (i % 6 == 0) ? (renk_t){10, 10, 10} : (renk_t){0, 0, 0};   /* 4 isaret */
    led[(dk * N_LED / 60) % N_LED] = (renk_t){0, 0, 255};                           /* dakika: mavi */
    led[(sn % 60) * N_LED / 60] = (renk_t){255, 0, 0};                              /* saniye: kirmizi */
    led[((g_ms % 1000) * N_LED / 1000)] = olcekle((renk_t){0, 255, 0}, 80);        /* milisaniye: yesil kosu */
}

static void ef_ates(uint32_t t)
{
    int i;
    (void)t;
    for (i = 0; i < N_LED; i++)
    {
        uint32_t k = 120 + rastgele() % 136;
        led[i] = (renk_t){ (uint8_t)k, (uint8_t)(k * (rastgele() % 60) / 255), 0 };
    }
}

typedef void (*efekt_f)(uint32_t);
static const efekt_f efektler[] = { ef_gokkusagi, ef_kuyruklu, ef_nefes, ef_polis, ef_yildizlar, ef_doldur, ef_saat, ef_ates };
static const char *efekt_adi[]  = { "GOKKUSAGI", "YILDIZ", "NEFES", "POLIS", "YILDIZLAR", "DOLDUR", "SAAT", "ATES" };
#define EF_SAYI 8

/* ---------------- kurulum ---------------- */
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
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB3MFP_Msk | SYS_GPB_MFPL_PB4MFP_Msk)) |
                    (SYS_GPB_MFPL_PB3MFP_ADC0_CH3 | SYS_GPB_MFPL_PB4MFP_ADC0_CH4);
    SYS->GPE_MFPL = (SYS->GPE_MFPL & ~(SYS_GPE_MFPL_PE6MFP_Msk | SYS_GPE_MFPL_PE7MFP_Msk));   /* PE6/PE7 = GPIO */
    SYS->GPC_MFPH = (SYS->GPC_MFPH & ~SYS_GPC_MFPH_PC9MFP_Msk) | SYS_GPC_MFPH_PC9MFP_PWM1_CH3;
    SYS_LockReg();
}

int main(void)
{
    int ef = 0, oto = 1;
    uint32_t t = 0, son_kare = 0, son_degisim = 0, son_ekran = 0;
    char s[40];

    SYS_Init();
    UART_Open(UART0, 115200);

    GPIO_SetMode(PE, BIT6 | BIT7, GPIO_MODE_OUTPUT);
    DATA = 0; SAAT_PIN = 0;
    GPIO_SetMode(PF, BIT8 | BIT7 | BIT6 | BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, BIT3 | BIT4, GPIO_MODE_INPUT);
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT3 | BIT4);
    ADC_POWER_ON(ADC);

    PWM_Kanal_Ayarla(PWM1, 3, 2000, 50);
    PWM_Start(PWM1, PWM_CH_3_MASK);
    PWM_DisableOutput(PWM1, PWM_CH_3_MASK);

    GPIO_SetMode(PB, BIT7 | BIT6, GPIO_MODE_INPUT);
    GPIO_SetMode(PA, BIT7, GPIO_MODE_INPUT);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_64);
    GPIO_ENABLE_DEBOUNCE(PB, BIT7 | BIT6);
    GPIO_ENABLE_DEBOUNCE(PA, BIT7);
    GPIO_EnableInt(PB, 7, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 7, GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPIO_PAPB_IRQn);

    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER0);
    NVIC_EnableIRQ(TMR0_IRQn);
    TIMER_Start(TIMER0);

    oled_yerli_32(1);
    oled_baslat();
    printf("\n18_led_halka_sov basladi. Encoder: efekt, bas: otomatik, POT3: hiz, POT4: parlaklik\n");

    for (;;)
    {
        int32_t e;
        uint32_t hiz_ms;

        __disable_irq(); e = g_enc; g_enc = 0; __enable_irq();
        if (e)          { ef = (ef + (e > 0 ? 1 : EF_SAYI - 1)) % EF_SAYI; oto = 0; son_degisim = g_ms; bip(1500, 20); hepsi((renk_t){0,0,0}); }
        if (g_enc_bas)  { g_enc_bas = 0; oto = 1; son_degisim = g_ms; bip(2500, 60); }
        if (oto && g_ms - son_degisim > 8000)
        {
            ef = (ef + 1) % EF_SAYI; son_degisim = g_ms; bip(1000, 15); hepsi((renk_t){0,0,0});
            printf("Efekt: %s\n", efekt_adi[ef]);
        }

        hiz_ms = 5 + (4095 - adc_oku(3)) * 35 / 4095;          /* POT3: 5..40 ms/kare */
        parlaklik = (uint8_t)(1 + adc_oku(4) * 19 / 4095);     /* POT4: 1..20 (akim icin 31'e cikmiyoruz) */

        if (g_ms - son_kare >= hiz_ms)
        {
            son_kare = g_ms;
            t++;
            efektler[ef](t);
            halka_goster();
            PF8 = ((ef + 1) >> 0) & 1; PF7 = ((ef + 1) >> 1) & 1; PF6 = ((ef + 1) >> 2) & 1; PF14 = ((ef + 1) >> 3) & 1;
        }

        if (g_ms - son_ekran >= 100)
        {
            son_ekran = g_ms;
            oled_temizle();
            yazi_orta(0, efekt_adi[ef], 2);
            snprintf(s, sizeof(s), "%d/%d %s  P:%u", ef + 1, EF_SAYI, oto ? "OTO" : "EL ", parlaklik);
            yazi_orta(24, s, 1);
            dolu_kutu(0, 18, (int)(128 - (g_ms - son_degisim) * 128 / 8000 > 0 && oto ? 128 - (g_ms - son_degisim) * 128 / 8000 : 0), 2);
            oled_guncelle();
        }
    }
}
