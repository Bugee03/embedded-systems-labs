/*
 * 22 - BLUETOOTH KUMANDA: telefon ile tum karti yonet  -- Boardoza M032KG
 *
 * Donanim:
 *   MDBT42TV-AT -> J11 (UART4, 9600): modul RX -> PC7 (TXD), modul TX -> PC6 (RXD), PD -> GND, 3V3, GND
 *   Phoenix24 halka -> J15: PE6 = DIN, PE7 = CIN (+ besleme)
 *   OLED 0.91" -> J9          RGB, buzzer, LED5..8, butonlar, encoder, NTC, LDR, potlar: kartta
 *
 * Telefon: Android "Serial Bluetooth Terminal" / iOS "Bluefruit Connect" -> M032KG-01 cihazina baglan.
 *
 * KOMUTLAR (buyuk/kucuk harf fark etmez, Enter ile):
 *   YARDIM                 komut listesi
 *   <herhangi bir yazi>    OLED'de buyuk harflerle kayar  (ornek: Merhaba hocam!)
 *   HALKA 0..8             halka efekti (0 = kapali)  |  HALKA OTO  (8 sn'de bir degisir)
 *   RENK r g b             halka + RGB LED sabit renk (0..255)   ornek: RENK 255 0 128
 *   PARLAK 1..31           halka parlakligi
 *   MUZIK 1..4 | MUZIK DUR
 *   BIP <Hz>               kisa ses
 *   LED5..LED8 ON|OFF
 *   SICAKLIK?  ISIK?  POT?  DURUM?
 *   CANLI ON|OFF           her saniye sensor degerlerini telefona gonder
 *
 * Kart -> telefon olaylari:  SW3 = sonraki efekt, SW4 = onceki efekt, SW5 = muzik ac/kapa,
 *   SW2 = halka kapat, encoder = parlaklik; hepsi telefona "OLAY: ..." olarak bildirilir.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "NuMicro.h"
#include "oled.h"
#include "pwm_duzelt.h"

#define BT_UART  UART4
#define BT_BAUD  9600
#define N_LED    24

/* =========================== zaman, ses, muzik =========================== */
volatile uint32_t g_ms = 0;

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
#define R  0
typedef struct { uint16_t f; uint8_t v; } nota_t;
static const nota_t m1[] = { {E4,2},{E4,2},{F4,2},{G4,2},{G4,2},{F4,2},{E4,2},{D4,2},{C4,2},{C4,2},{D4,2},{E4,2},{E4,3},{D4,1},{D4,4},
    {E4,2},{E4,2},{F4,2},{G4,2},{G4,2},{F4,2},{E4,2},{D4,2},{C4,2},{C4,2},{D4,2},{E4,2},{D4,3},{C4,1},{C4,4},{R,4} };
static const nota_t m2[] = { {C4,2},{C4,2},{G4,2},{G4,2},{A4,2},{A4,2},{G4,4},{F4,2},{F4,2},{E4,2},{E4,2},{D4,2},{D4,2},{C4,4},{R,4} };
static const nota_t m3[] = { {E4,2},{E4,2},{E4,4},{E4,2},{E4,2},{E4,4},{E4,2},{G4,2},{C4,3},{D4,1},{E4,4},{R,4} };
static const nota_t m4[] = { {E5,1},{DS5,1},{E5,1},{DS5,1},{E5,1},{B4,1},{D5,1},{C5,1},{A4,3},{R,1},{C4,1},{E4,1},{A4,1},{B4,3},{R,1},
    {E4,1},{GS4,1},{B4,1},{C5,3},{R,1},{E4,1},{E5,1},{DS5,1},{E5,1},{DS5,1},{E5,1},{B4,1},{D5,1},{C5,1},{A4,3},{R,4} };
typedef struct { const char *ad; const nota_t *n; uint32_t adet; } melodi_t;
static const melodi_t melodiler[4] = {
    { "ODE TO JOY", m1, sizeof(m1) / sizeof(nota_t) }, { "TWINKLE", m2, sizeof(m2) / sizeof(nota_t) },
    { "JINGLE BELLS", m3, sizeof(m3) / sizeof(nota_t) }, { "FUR ELISE", m4, sizeof(m4) / sizeof(nota_t) } };

volatile int g_melodi = -1;
volatile uint32_t g_nota_i, g_nota_kalan, g_bip_bitis;
static uint8_t rgb_r, rgb_g, rgb_b;      /* kart RGB, 0..100 */

static void rgb_kanal(uint32_t ch, uint32_t d)          /* kirmizi (ch2) buzzer (ch3) ile ayni periyotta */
{
    uint32_t m = 1UL << ch;
    if (d == 0) { PWM1->MSK &= ~m; PWM1->MSKEN |= m; return; }
    PWM1->MSKEN &= ~m;
    PWM_SET_CMR(PWM1, ch, (d >= 100) ? PWM1->PERIOD[ch & ~1UL] : d * (PWM1->PERIOD[ch & ~1UL] + 1) / 100);
}
static void kart_rgb(uint32_t r, uint32_t g, uint32_t b) { rgb_r = r; rgb_g = g; rgb_b = b; rgb_kanal(2, r); rgb_kanal(0, g); rgb_kanal(1, b); }

static void ses(uint32_t f)
{
    if (f == 0) { PWM_DisableOutput(PWM1, PWM_CH_3_MASK); return; }
    PWM_Kanal_Ayarla(PWM1, 3, f, 50);
    rgb_kanal(2, rgb_r);
    PWM_EnableOutput(PWM1, PWM_CH_3_MASK);
}
static void bip(uint32_t f, uint32_t ms) { if (g_melodi >= 0) return; ses(f); g_bip_bitis = g_ms + ms; }

void TMR0_IRQHandler(void)
{
    TIMER_ClearIntFlag(TIMER0);
    g_ms++;
    if (g_bip_bitis && g_ms >= g_bip_bitis) { g_bip_bitis = 0; if (g_melodi < 0) ses(0); }
    if (g_melodi >= 0)
    {
        if (g_nota_kalan == 0)
        {
            const melodi_t *m = &melodiler[g_melodi];
            g_nota_kalan = (uint32_t)m->n[g_nota_i].v * 170;
            ses(m->n[g_nota_i].f);
            g_nota_i = (g_nota_i + 1) % m->adet;
        }
        else if (--g_nota_kalan == 25) ses(0);
    }
}

/* =========================== Bluetooth (UART4) =========================== */
#define RB 256
static volatile uint8_t  rb[RB];
static volatile uint16_t rb_yaz, rb_oku;

void UART46_IRQHandler(void)
{
    while (!UART_GET_RX_EMPTY(BT_UART))
    {
        uint8_t c = UART_READ(BT_UART);
        uint16_t n = (rb_yaz + 1) % RB;
        if (n != rb_oku) { rb[rb_yaz] = c; rb_yaz = n; }
    }
}

static void bt(const char *s) { UART_Write(BT_UART, (const uint8_t *)s, strlen(s)); }
static void bt_satir(const char *s) { bt(s); bt("\r\n"); printf("BT< %s\n", s); }

static char satir[80];
static uint16_t satir_n;
static uint32_t son_bayt_ms;

static int satir_al(void)               /* Enter ile ya da 200 ms sessizlikle satir tamamlanir */
{
    while (rb_oku != rb_yaz)
    {
        uint8_t c = rb[rb_oku];
        rb_oku = (rb_oku + 1) % RB;
        son_bayt_ms = g_ms;
        if (c == '\r' || c == '\n') { if (satir_n) { satir[satir_n] = 0; satir_n = 0; return 1; } }
        else if (c >= 32 && c < 127 && satir_n < sizeof(satir) - 1) satir[satir_n++] = (char)c;
    }
    if (satir_n && g_ms - son_bayt_ms > 200) { satir[satir_n] = 0; satir_n = 0; return 1; }
    return 0;
}

/* =========================== LED halka (APA102, PE6/PE7) =========================== */
typedef struct { uint8_t r, g, b; } renk_t;
static renk_t led[N_LED];
static uint8_t parlaklik = 20;
static int efekt = 1, oto = 0;
static renk_t sabit = {0, 0, 0};
static const char *efekt_adi[] = { "KAPALI", "GOKKUSAGI", "YILDIZ", "NEFES", "POLIS", "YILDIZLAR", "DOLDUR", "ATES", "RENK" };
#define EF_SAYI 9

static void bayt(uint8_t b)
{
    int i;
    for (i = 7; i >= 0; i--) { PE6 = (b >> i) & 1; PE7 = 1; __NOP(); __NOP(); PE7 = 0; }
}
static void halka_goster(void)
{
    int i;
    for (i = 0; i < 4; i++) bayt(0);
    for (i = 0; i < N_LED; i++) { bayt(0xE0 | parlaklik); bayt(led[i].b); bayt(led[i].g); bayt(led[i].r); }
    for (i = 0; i < 4; i++) bayt(0xFF);
}
static renk_t tekerlek(uint8_t p)
{
    renk_t c;
    if (p < 85)       { c.r = 255 - p * 3; c.g = p * 3;       c.b = 0; }
    else if (p < 170) { p -= 85;  c.r = 0;  c.g = 255 - p * 3; c.b = p * 3; }
    else              { p -= 170; c.r = p * 3; c.g = 0;        c.b = 255 - p * 3; }
    return c;
}
static renk_t olcek(renk_t c, uint32_t k) { renk_t o = { (uint8_t)(c.r * k / 255), (uint8_t)(c.g * k / 255), (uint8_t)(c.b * k / 255) }; return o; }
static uint32_t tohum = 99;
static uint32_t rastgele(void) { tohum = tohum * 1103515245u + 12345u; return tohum >> 16; }

static void efekt_kare(uint32_t t)
{
    int i;
    switch (efekt)
    {
    case 0: for (i = 0; i < N_LED; i++) led[i] = (renk_t){0, 0, 0}; break;
    case 1: for (i = 0; i < N_LED; i++) led[i] = tekerlek((uint8_t)(i * 256 / N_LED + t * 2)); break;
    case 2: { int b = (int)((t / 2) % N_LED);
              for (i = 0; i < N_LED; i++) led[i] = olcek(led[i], 140);
              led[b] = tekerlek((uint8_t)(t / 3)); led[(b + 12) % N_LED] = tekerlek((uint8_t)(t / 3 + 128)); } break;
    case 3: { uint32_t f = (t * 4) % 512, k = f < 256 ? f : 511 - f;
              for (i = 0; i < N_LED; i++) led[i] = olcek(tekerlek((uint8_t)(t / 16)), k * k / 255); } break;
    case 4: { int faz = (int)((t / 6) % 8);
              for (i = 0; i < N_LED; i++) { int sol = i < 12, yan = (faz < 4) ? (sol && !(faz & 1)) : (!sol && !(faz & 1));
                  led[i] = yan ? (sol ? (renk_t){255, 0, 0} : (renk_t){0, 0, 255}) : (renk_t){0, 0, 0}; } } break;
    case 5: { for (i = 0; i < N_LED; i++) { led[i] = olcek(led[i], 200); if (led[i].b < 12) led[i] = (renk_t){0, 0, 12}; }
              if (rastgele() % 3 == 0) led[rastgele() % N_LED] = (renk_t){255, 255, 255}; } break;
    case 6: { uint32_t a = (t / 2) % (N_LED * 2); renk_t c = tekerlek((uint8_t)((t / (N_LED * 4)) * 50));
              for (i = 0; i < N_LED; i++) { int d = a < N_LED ? (i <= (int)a) : (i > (int)(a - N_LED)); led[i] = d ? c : (renk_t){0, 0, 0}; } } break;
    case 7: for (i = 0; i < N_LED; i++) { uint32_t k = 120 + rastgele() % 136; led[i] = (renk_t){(uint8_t)k, (uint8_t)(k * (rastgele() % 60) / 255), 0}; } break;
    case 8: for (i = 0; i < N_LED; i++) led[i] = sabit; break;
    }
}

/* Kartin kendi isiklari: efekt kartta da gorunsun (halka takili olmasa bile).
 *   RGB LED  = halkadaki EN PARLAK LED'in rengi (parlaklik ayariyla olceklenir)
 *   LED5..8  = halkanin 4 noktasi (0, 6, 12, 18. LED) yaniyorsa yanar */
static void kart_isiklari(void)
{
    int i, en = 0; uint32_t enp = 0;
    for (i = 0; i < N_LED; i++)
    {
        uint32_t p = (uint32_t)led[i].r + led[i].g + led[i].b;
        if (p > enp) { enp = p; en = i; }
    }
    kart_rgb(led[en].r * 100 / 255 * parlaklik / 31,
             led[en].g * 100 / 255 * parlaklik / 31,
             led[en].b * 100 / 255 * parlaklik / 31);
#define YANIK(k) (((uint32_t)led[k].r + led[k].g + led[k].b) > 120)
    PF8 = YANIK(0); PF7 = YANIK(6); PF6 = YANIK(12); PF14 = YANIK(18);
#undef YANIK
}

/* =========================== sensorler =========================== */
static uint32_t adc_oku(uint32_t ch)
{
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, 1UL << ch);
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
    ADC_START_CONV(ADC);
    while (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT)) { }
    return ADC_GET_CONVERSION_DATA(ADC, ch);
}
static int32_t sicaklik_x10(void)
{
    uint32_t a = 0; int i; float r, t;
    for (i = 0; i < 8; i++) a += adc_oku(2);
    a /= 8;
    if (a < 10 || a > 4085) return -999;
    r = 10200.0f * a / (4095.0f - a);
    t = 1.0f / (1.0f / 298.15f + logf(r / 10000.0f) / 3950.0f) - 273.15f;
    return (int32_t)(t * 10.0f + 0.5f);
}
static uint32_t isik_yuzde(void) { return (4095 - adc_oku(15)) * 100 / 4095; }

/* =========================== OLED =========================== */
static char mesaj[80] = "Telefondan yaz: YARDIM";
static void ekran(void)
{
    char s[32];
    int32_t t = sicaklik_x10();
    oled_temizle();
    snprintf(s, sizeof(s), "%s%s", efekt_adi[efekt], oto ? "*" : "");
    yazi(0, 0, s, 1);
    if (t != -999) { snprintf(s, sizeof(s), "%ld.%ldC", (long)(t / 10), (long)(t % 10)); yazi(128 - yazi_genislik(s, 1), 0, s, 1); }
    if (g_melodi >= 0) yazi(70, 0, "#", 1);
    kayan_yazi(12, mesaj, 2);
    oled_guncelle();
}

/* =========================== komutlar =========================== */
static void durum_gonder(void)
{
    char s[96]; int32_t t = sicaklik_x10();
    snprintf(s, sizeof(s), "SICAKLIK=%ld.%ldC ISIK=%%%lu POT=%lu/%lu/%lu", (long)(t / 10), (long)(t % 10),
             (unsigned long)isik_yuzde(), (unsigned long)adc_oku(3), (unsigned long)adc_oku(4), (unsigned long)adc_oku(5));
    bt_satir(s);
}

static void yardim(void)
{
    bt_satir("=== M032 BT KUMANDA ===");
    bt_satir("<yazi>: OLED'de kayar");
    bt_satir("EFEKT 0-7 | EFEKT OTO (RGB+LED)");
    bt_satir("RENK r g b (0-255)");
    bt_satir("PARLAK 1-31");
    bt_satir("MUZIK 1-4 | MUZIK DUR");
    bt_satir("BIP <Hz>");
    bt_satir("LED5..LED8 ON|OFF");
    bt_satir("SICAKLIK? ISIK? POT? DURUM?");
    bt_satir("CANLI ON|OFF");
}

static int canli = 0;

static void komut(char *ham)
{
    char b[80], *k[5]; int n = 0; uint32_t i; char s[64];
    for (i = 0; ham[i] && i < sizeof(b) - 1; i++) b[i] = (char)toupper((unsigned char)ham[i]);
    b[i] = 0;
    printf("BT> %s\n", ham);
    for (k[n] = strtok(b, " "); k[n] && n < 4; k[++n] = strtok(NULL, " ")) { }
    if (n == 0) return;

    if (!strcmp(k[0], "YARDIM") || !strcmp(k[0], "?") || !strcmp(k[0], "HELP")) { yardim(); return; }

    if (!strcmp(k[0], "EFEKT")) k[0] = "HALKA";              /* takma ad */
    if (!strcmp(k[0], "HALKA") && n >= 2 && !strcmp(k[1], "0"))
    { efekt = 0; oto = 0; kart_rgb(0, 0, 0); PF8 = PF7 = PF6 = PF14 = 0; bt_satir("OK KAPALI"); return; }
    if (!strcmp(k[0], "HALKA") && n >= 2)
    {
        if (!strcmp(k[1], "OTO")) { oto = 1; bt_satir("OK HALKA OTO"); return; }
        if (isdigit((unsigned char)k[1][0]) && atoi(k[1]) < EF_SAYI) { efekt = atoi(k[1]); oto = 0;
            snprintf(s, sizeof(s), "OK HALKA %s", efekt_adi[efekt]); bt_satir(s); bip(1500, 30); return; }
    }
    if (!strcmp(k[0], "RENK") && n >= 4)
    {
        sabit = (renk_t){ (uint8_t)atoi(k[1]), (uint8_t)atoi(k[2]), (uint8_t)atoi(k[3]) };
        efekt = 8; oto = 0;
        kart_rgb(sabit.r * 100 / 255, sabit.g * 100 / 255, sabit.b * 100 / 255);
        bt_satir("OK RENK"); return;
    }
    if (!strcmp(k[0], "PARLAK") && n >= 2)
    {
        int p = atoi(k[1]); if (p < 1) p = 1; if (p > 31) p = 31;
        parlaklik = (uint8_t)p; snprintf(s, sizeof(s), "OK PARLAK %d", p); bt_satir(s); return;
    }
    if (!strcmp(k[0], "MUZIK") && n >= 2)
    {
        if (!strcmp(k[1], "DUR")) { g_melodi = -1; ses(0); bt_satir("OK MUZIK DUR"); return; }
        if (atoi(k[1]) >= 1 && atoi(k[1]) <= 4) { g_nota_i = 0; g_nota_kalan = 0; g_melodi = atoi(k[1]) - 1;
            snprintf(s, sizeof(s), "OK MUZIK %s", melodiler[g_melodi].ad); bt_satir(s); return; }
    }
    if (!strcmp(k[0], "BIP")) { bip(n >= 2 && atoi(k[1]) > 0 ? (uint32_t)atoi(k[1]) : 2000, 200); bt_satir("OK BIP"); return; }
    if (!strncmp(k[0], "LED", 3) && k[0][3] >= '5' && k[0][3] <= '8' && n >= 2)
    {
        int on = !strcmp(k[1], "ON") || !strcmp(k[1], "AC") || !strcmp(k[1], "1");
        switch (k[0][3]) { case '5': PF8 = on; break; case '6': PF7 = on; break; case '7': PF6 = on; break; default: PF14 = on; }
        snprintf(s, sizeof(s), "OK %s %s", k[0], on ? "ON" : "OFF"); bt_satir(s); return;
    }
    if (!strcmp(k[0], "SICAKLIK?") || !strcmp(k[0], "SICAKLIK"))
    { int32_t t = sicaklik_x10(); snprintf(s, sizeof(s), "SICAKLIK = %ld.%ld C", (long)(t / 10), (long)(t % 10)); bt_satir(s); return; }
    if (!strcmp(k[0], "ISIK?") || !strcmp(k[0], "ISIK"))
    { snprintf(s, sizeof(s), "ISIK = %%%lu", (unsigned long)isik_yuzde()); bt_satir(s); return; }
    if (!strcmp(k[0], "POT?") || !strcmp(k[0], "POT"))
    { snprintf(s, sizeof(s), "POT = %lu %lu %lu", (unsigned long)adc_oku(3), (unsigned long)adc_oku(4), (unsigned long)adc_oku(5)); bt_satir(s); return; }
    if (!strcmp(k[0], "DURUM?") || !strcmp(k[0], "DURUM")) { durum_gonder(); return; }
    if (!strcmp(k[0], "CANLI") && n >= 2) { canli = !strcmp(k[1], "ON"); bt_satir(canli ? "OK CANLI ON" : "OK CANLI OFF"); return; }

    /* komut degil -> mesaj olarak OLED'e */
    strncpy(mesaj, ham, sizeof(mesaj) - 1);
    mesaj[sizeof(mesaj) - 1] = 0;
    bip(2200, 60);
    bt_satir("OK EKRANDA");
}

/* =========================== butonlar / encoder =========================== */
volatile uint32_t g_tus;
volatile int32_t g_enc;
void GPABGH_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PB, BIT7)) { GPIO_CLR_INT_FLAG(PB, BIT7); g_enc += PB6 ? 1 : -1; }
    if (GPIO_GET_INT_FLAG(PB, BIT1)) { GPIO_CLR_INT_FLAG(PB, BIT1); g_tus |= 8; }
    if (GPIO_GET_INT_FLAG(PA, BIT7)) { GPIO_CLR_INT_FLAG(PA, BIT7); g_tus |= 16; }
}
void GPCDEF_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PC, BIT12)) { GPIO_CLR_INT_FLAG(PC, BIT12); g_tus |= 1; }
    if (GPIO_GET_INT_FLAG(PC, BIT11)) { GPIO_CLR_INT_FLAG(PC, BIT11); g_tus |= 2; }
    if (GPIO_GET_INT_FLAG(PC, BIT10)) { GPIO_CLR_INT_FLAG(PC, BIT10); g_tus |= 4; }
}

/* =========================== kurulum =========================== */
void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
    CLK_EnableModuleClock(UART4_MODULE);
    CLK_SetModuleClock(UART4_MODULE, CLK_CLKSEL3_UART4SEL_HIRC, CLK_CLKDIV4_UART4(1));
    CLK_EnableModuleClock(TMR0_MODULE);
    CLK_SetModuleClock(TMR0_MODULE, CLK_CLKSEL1_TMR0SEL_HIRC, 0);
    CLK_EnableModuleClock(PWM1_MODULE);
    CLK_SetModuleClock(PWM1_MODULE, CLK_CLKSEL2_PWM1SEL_PCLK1, 0);
    CLK_EnableModuleClock(ADC_MODULE);
    CLK_SetModuleClock(ADC_MODULE, CLK_CLKSEL2_ADCSEL_PCLK1, CLK_CLKDIV0_ADC(1));
    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk | SYS_GPB_MFPH_PB15MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD | SYS_GPB_MFPH_PB15MFP_ADC0_CH15);
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk | SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk)) |
                    (SYS_GPB_MFPL_PB2MFP_ADC0_CH2 | SYS_GPB_MFPL_PB3MFP_ADC0_CH3 | SYS_GPB_MFPL_PB4MFP_ADC0_CH4 | SYS_GPB_MFPL_PB5MFP_ADC0_CH5);
    SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC3MFP_Msk | SYS_GPC_MFPL_PC4MFP_Msk | SYS_GPC_MFPL_PC5MFP_Msk |
                                       SYS_GPC_MFPL_PC6MFP_Msk | SYS_GPC_MFPL_PC7MFP_Msk)) |
                    (SYS_GPC_MFPL_PC3MFP_PWM1_CH2 | SYS_GPC_MFPL_PC4MFP_PWM1_CH1 | SYS_GPC_MFPL_PC5MFP_PWM1_CH0 |
                     SYS_GPC_MFPL_PC6MFP_UART4_RXD | SYS_GPC_MFPL_PC7MFP_UART4_TXD);
    SYS->GPC_MFPH = (SYS->GPC_MFPH & ~SYS_GPC_MFPH_PC9MFP_Msk) | SYS_GPC_MFPH_PC9MFP_PWM1_CH3;
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPL_PE6MFP_Msk | SYS_GPE_MFPL_PE7MFP_Msk);
    SYS_LockReg();
}

static void donanim(void)
{
    GPIO_SetMode(PE, BIT6 | BIT7, GPIO_MODE_OUTPUT); PE6 = 0; PE7 = 0;
    GPIO_SetMode(PF, BIT8 | BIT7 | BIT6 | BIT14, GPIO_MODE_OUTPUT);
    GPIO_SetMode(PB, BIT2 | BIT3 | BIT4 | BIT5 | BIT15, GPIO_MODE_INPUT);
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT2 | BIT3 | BIT4 | BIT5 | BIT15);
    ADC_POWER_ON(ADC);

    PWM_Kanal_Ayarla(PWM1, 0, 1000, 0);
    PWM_Kanal_Ayarla(PWM1, 1, 1000, 0);
    PWM_Kanal_Ayarla(PWM1, 2, 1000, 0);
    PWM_Kanal_Ayarla(PWM1, 3, 1000, 50);
    PWM_EnableOutput(PWM1, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK);
    PWM_DisableOutput(PWM1, PWM_CH_3_MASK);
    PWM_Start(PWM1, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);

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
    GPIO_EnableInt(PB, 1, GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 7, GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 7, GPIO_INT_FALLING);

    UART_Open(BT_UART, BT_BAUD);
    UART_EnableInt(BT_UART, UART_INTEN_RDAIEN_Msk);

    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER0);
    NVIC_SetPriority(TMR0_IRQn, 0);
    NVIC_EnableIRQ(TMR0_IRQn);
    NVIC_EnableIRQ(UART46_IRQn);
    NVIC_EnableIRQ(GPIO_PAPB_IRQn);
    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
    TIMER_Start(TIMER0);
}

int main(void)
{
    uint32_t t = 0, son_kare = 0, son_ekran = 0, son_canli = 0, son_oto = 0;
    char s[48];

    SYS_Init();
    UART_Open(UART0, 115200);
    donanim();
    oled_yerli_32(1);
    oled_baslat();
    printf("\n22_bt_kumanda basladi. Telefondan M032KG-01'e baglan, YARDIM yaz.\n");
    bt_satir("M032 BT KUMANDA hazir. YARDIM yaz.");
    bip(1800, 80);

    for (;;)
    {
        uint32_t tus; int32_t e;

        if (satir_al()) komut(satir);

        __disable_irq(); tus = g_tus; g_tus = 0; e = g_enc; g_enc = 0; __enable_irq();
        if (tus & 2) { efekt = efekt % (EF_SAYI - 1) + 1; oto = 0; snprintf(s, sizeof(s), "OLAY: SW3 -> %s", efekt_adi[efekt]); bt_satir(s); bip(1500, 30); }
        if (tus & 4) { efekt = (efekt + EF_SAYI - 3) % (EF_SAYI - 1) + 1; oto = 0; snprintf(s, sizeof(s), "OLAY: SW4 -> %s", efekt_adi[efekt]); bt_satir(s); bip(1300, 30); }
        if (tus & 8) { if (g_melodi >= 0) { g_melodi = -1; ses(0); } else { g_nota_i = 0; g_nota_kalan = 0; g_melodi = 0; }
                       bt_satir(g_melodi >= 0 ? "OLAY: SW5 -> MUZIK" : "OLAY: SW5 -> MUZIK DUR"); }
        if (tus & 1) { efekt = 0; oto = 0; kart_rgb(0, 0, 0); PF8 = PF7 = PF6 = PF14 = 0; bt_satir("OLAY: SW2 -> KAPALI"); }
        if (tus & 16) { oto = !oto; bt_satir(oto ? "OLAY: ENCODER -> OTO" : "OLAY: ENCODER -> OTO KAPALI"); }
        if (e)
        {
            int p = (int)parlaklik + (int)e * 2; if (p < 1) p = 1; if (p > 31) p = 31;
            parlaklik = (uint8_t)p; snprintf(s, sizeof(s), "OLAY: PARLAK %d", p); bt_satir(s);
        }

        if (oto && g_ms - son_oto > 8000) { son_oto = g_ms; efekt = efekt % 7 + 1; }

        if (g_ms - son_kare >= 15)
        {
            son_kare = g_ms; t++;
            efekt_kare(t);
            halka_goster();                       /* halka takiliysa orada da gorunur */
            if (efekt != 0) kart_isiklari();      /* efekt 0 iken LED5..8 ve RGB elle (LEDx / RENK) yonetilir */
        }
        if (g_ms - son_ekran >= 40) { son_ekran = g_ms; ekran(); }
        if (canli && g_ms - son_canli >= 1000) { son_canli = g_ms; durum_gonder(); }
    }
}
