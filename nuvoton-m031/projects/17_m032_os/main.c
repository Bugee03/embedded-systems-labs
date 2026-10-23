/*
 * 17 - M032 OS: mini oyun konsolu  -- Boardoza M032KG + 0.91" OLED (128x32)
 *
 * Acilis animasyonu -> menu (encoder ile kayarak secilir) -> 5 uygulama:
 *   DINO        : Chrome dinozor oyunu (zipla: SW3/SW4/SW5 veya encoder'a bas)
 *   OSILOSKOP   : POT / LDR / NTC'nin canli grafigi (encoder: kaynak, bas: dondur)
 *   TERMOMETRE  : NTC ile gercek sicaklik (C), RGB renk sicakliga gore
 *   MUZIK       : 12 melodi (+ kendi sarkin) + ekolayzer + isik (encoder: sec, bas: cal/dur)
 *   ISIK SOVU   : RGB gokkusagi + kayan LED + ekranda dalga (POT3 = hiz)
 *
 * Kontroller:  Encoder cevir = sec / degistir    Encoder bas = gir / tamam
 *              SW2 = MENUYE DON                   SW3, SW4, SW5 = oyunda zipla
 *
 * Her sey kesmelerle calisir:
 *   TMR0_IRQHandler   : 1 ms zaman tabani + muzik motoru + bip suresi
 *   GPABGH_IRQHandler : encoder A (PB7), encoder dugme (PA7), SW5 (PB1)
 *   GPCDEF_IRQHandler : SW2 (PC12), SW3 (PC11), SW4 (PC10)
 *
 * Donanim (Boardoza semasi): LED5..8 = PF8 PF7 PF6 PF14, RGB R/G/B = PC3/PC5/PC4 (PWM1 ch2/ch0/ch1),
 *   buzzer = PC9 (PWM1 ch3), POT = PB3/PB4/PB5, NTC = PB2, LDR = PB15, OLED = J9 (PC8 SDA, PE13 SCL)
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "NuMicro.h"
#include "oled.h"
#include "pwm_duzelt.h"

/* ======================================================================
 *  1) KESMELER VE ZAMAN
 * ====================================================================== */
volatile uint32_t g_ms = 0;              /* 1 ms sayaci                         */
volatile int32_t  g_enc = 0;             /* encoder adimlari (main tuketir)     */
volatile uint32_t g_tus = 0;             /* basilan tuslar (bit maskesi)        */

#define T_SW2   0x01                     /* menuye don */
#define T_SW3   0x02
#define T_SW4   0x04
#define T_SW5   0x08
#define T_ENC   0x10                     /* encoder dugmesi */
#define T_ZIPLA (T_SW3 | T_SW4 | T_SW5 | T_ENC)

static uint32_t tus_al(void)             /* kesmenin biriktirdigi tuslari al ve sifirla */
{
    uint32_t t;
    __disable_irq(); t = g_tus; g_tus = 0; __enable_irq();
    return t;
}

static int32_t enc_al(void)
{
    int32_t e;
    __disable_irq(); e = g_enc; g_enc = 0; __enable_irq();
    return e;
}

static void bekle_ms(uint32_t ms) { uint32_t t = g_ms; while (g_ms - t < ms) { } }

/* ======================================================================
 *  2) SES: buzzer (PWM1 ch3) + muzik motoru (TIMER0 kesmesinde)
 * ====================================================================== */
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
#define G5 784
#define C6 1047
#define R  0

typedef struct { uint16_t f; uint8_t v; } nota_t;          /* v: sekizlik sayisi */

/* ----------------------------------------------------------------------
 *  SARKILAR: her sarki tek bir yazi (string). Yeni sarki eklemek icin
 *  asagidaki listeye bir satir ekle, baska hicbir yeri degistirme.
 *
 *  Yazim:  NOTA[#]OKTAV[/SURE]   bosluklarla ayrilir
 *    NOTA  : C D E F G A B  (Do Re Mi Fa Sol La Si),  R = sus
 *    #     : diyez (C#5, F#4 ...)
 *    OKTAV : 3..6  (4 = orta oktav, A4 = 440 Hz)
 *    /SURE : sekizlik sayisi: /1 sekizlik, /2 dortluk (varsayilan), /4 ikilik, /8 birlik
 *  Ornek: "E4 E4 F4 G4/4 R/2"
 * ---------------------------------------------------------------------- */
typedef struct { const char *ad; const char *notalar; } sarki_t;
static const sarki_t sarkilar[] = {
    { "ODE TO JOY",     "E4 E4 F4 G4 G4 F4 E4 D4 C4 C4 D4 E4 E4/3 D4/1 D4/4 "
                        "E4 E4 F4 G4 G4 F4 E4 D4 C4 C4 D4 E4 D4/3 C4/1 C4/4 R/4" },
    { "TETRIS",         "E5/2 B4/1 C5/1 D5/2 C5/1 B4/1 A4/2 A4/1 C5/1 E5/2 D5/1 C5/1 B4/3 C5/1 D5/2 E5/2 C5/2 A4/2 A4/4 R/2 "
                        "D5/3 F5/1 A5/2 G5/1 F5/1 E5/3 C5/1 E5/2 D5/1 C5/1 B4/2 B4/1 C5/1 D5/2 E5/2 C5/2 A4/2 A4/4 R/4" },
    { "NOKIA",          "E6/1 D6/1 F#5/2 G#5/2 C#6/1 B5/1 D5/2 E5/2 B5/1 A5/1 C#5/2 E5/2 A5/6 R/6" },
    { "HAPPY BIRTHDAY", "G4/1 G4/1 A4/2 G4/2 C5/2 B4/4 G4/1 G4/1 A4/2 G4/2 D5/2 C5/4 "
                        "G4/1 G4/1 G5/2 E5/2 C5/2 B4/2 A4/4 F5/1 F5/1 E5/2 C5/2 D5/2 C5/4 R/4" },
    { "TURK MARSI",     "B4/1 A4/1 G#4/1 A4/1 C5/4 D5/1 C5/1 B4/1 C5/1 E5/4 F5/1 E5/1 D#5/1 E5/1 "
                        "B5/1 A5/1 G#5/1 A5/1 B5/1 A5/1 G#5/1 A5/1 C6/4 A5/2 C6/2 B5/2 A5/2 G5/2 A5/2 "
                        "B5/2 A5/2 G5/2 A5/2 B5/2 A5/2 G5/2 F#5/2 E5/4 R/4" },
    { "FUR ELISE",      "E5/1 D#5/1 E5/1 D#5/1 E5/1 B4/1 D5/1 C5/1 A4/3 R/1 C4/1 E4/1 A4/1 B4/3 R/1 "
                        "E4/1 G#4/1 B4/1 C5/3 R/1 E4/1 E5/1 D#5/1 E5/1 D#5/1 E5/1 B4/1 D5/1 C5/1 A4/3 R/1 "
                        "C4/1 E4/1 A4/1 B4/3 R/1 E4/1 C5/1 B4/1 A4/4 R/4" },
    { "TWINKLE",        "C4 C4 G4 G4 A4 A4 G4/4 F4 F4 E4 E4 D4 D4 C4/4 G4 G4 F4 F4 E4 E4 D4/4 G4 G4 F4 F4 E4 E4 D4/4 R/4" },
    { "JINGLE BELLS",   "E4 E4 E4/4 E4 E4 E4/4 E4 G4 C4/3 D4/1 E4/4 R/2 "
                        "F4 F4 F4/3 F4/1 F4 E4 E4 E4/1 E4/1 E4 D4 D4 E4 D4/4 G4/4 R/4" },

    /* Ozgun parca: melankolik indie-pop havasi. Am -> Fmaj7 -> C -> E7, arpej (bas + akor notalari),
     * sonra ayni akorlar uzerine la minorde basit bir melodi. Buzzer tek ses caldigi icin akorlar
     * "parmakla tek tek" (arpej) calinir. */
    { "MELANKOLI (Am)", "A3/1 E4/1 A4/1 C5/1 E5/1 C5/1 A4/1 E4/1 "          /* Am    */
                        "F3/1 C4/1 F4/1 A4/1 E5/1 A4/1 F4/1 C4/1 "          /* Fmaj7 */
                        "C4/1 G4/1 C5/1 E5/1 G5/1 E5/1 C5/1 G4/1 "          /* C     */
                        "E3/1 B3/1 E4/1 G#4/1 D5/1 G#4/1 E4/1 B3/1 "        /* E7    */
                        "A3/1 E4/1 A4/1 C5/1 E5/1 C5/1 A4/1 E4/1 "
                        "F3/1 C4/1 F4/1 A4/1 E5/1 A4/1 F4/1 C4/1 "
                        "C4/1 G4/1 C5/1 E5/1 G5/1 E5/1 C5/1 G4/1 "
                        "E3/1 B3/1 E4/1 G#4/1 D5/1 G#4/1 E4/1 B3/1 "
                        "E5/2 D5/1 C5/1 B4/2 A4/2 "                         /* melodi: Am    */
                        "C5/2 A4/1 F4/1 E4/4 "                              /*         Fmaj7 */
                        "E4/1 G4/1 C5/2 E5/3 D5/1 "                         /*         C     */
                        "D5/2 B4/1 G#4/1 B4/4 "                             /*         E7    */
                        "E5/2 D5/1 C5/1 B4/2 A4/2 "
                        "C5/2 A4/1 F4/1 E5/4 "
                        "G5/2 E5/1 C5/1 D5/2 C5/1 B4/1 "
                        "B4/2 G#4/2 A4/6 R/2 "
                        "A3/1 E4/1 A4/1 C5/1 E5/4 R/4" },                     /* bitis */

    /* Akor ilerlemesi arpeji (sozsuz, vokal melodisi yok): D -> Em -> G -> D,
     * ara bolum F#m -> Em, sonra tekrar D -> Em -> G -> D. Akorlar telif kapsaminda degildir. */
    { "D-Em-G ARPEJ",     "D4/1 A4/1 D5/1 F#5/1 A5/1 F#5/1 D5/1 A4/1 E4/1 B4/1 E5/1 G5/1 B5/1 G5/1 E5/1 B4/1 G3/1 D4/1 G4/1 B4/1 D5/1 B4/1 G4/1 D4/1 D4/1 A4/1 D5/1 F#5/1 A5/1 F#5/1 D5/1 A4/1 "   /* giris / kita */
                          "D4/1 A4/1 D5/1 F#5/1 A5/1 F#5/1 D5/1 A4/1 E4/1 B4/1 E5/1 G5/1 B5/1 G5/1 E5/1 B4/1 G3/1 D4/1 G4/1 B4/1 D5/1 B4/1 G4/1 D4/1 D4/1 A4/1 D5/1 F#5/1 A5/1 F#5/1 D5/1 A4/1 "
                          "F#3/1 C#4/1 F#4/1 A4/1 C#5/1 A4/1 F#4/1 C#4/1 E4/1 B4/1 E5/1 G5/1 B5/1 G5/1 E5/1 B4/1 F#3/1 C#4/1 F#4/1 A4/1 C#5/1 A4/1 F#4/1 C#4/1 E4/1 B4/1 E5/1 G5/1 B5/1 G5/1 E5/1 B4/1 " /* ara bolum   */
                          "D4/1 A4/1 D5/1 F#5/1 A5/1 F#5/1 D5/1 A4/1 E4/1 B4/1 E5/1 G5/1 B5/1 G5/1 E5/1 B4/1 G3/1 D4/1 G4/1 B4/1 D5/1 B4/1 G4/1 D4/1 D4/1 A4/1 D5/1 F#5/1 A5/1 F#5/1 D5/1 A4/1 "   /* nakarat     */
                          "D4/1 A4/1 D5/1 F#5/1 A5/1 F#5/1 D5/1 A4/1 E4/1 B4/1 E5/1 G5/1 B5/1 G5/1 E5/1 B4/1 G3/1 D4/1 G4/1 B4/1 D5/1 B4/1 G4/1 D4/1 D4/1 A4/1 D5/1 F#5/1 A5/1 F#5/1 D5/1 A4/1 "
                          "D4/1 A4/1 D5/1 F#5/4 R/4" },

    /* Kullanicinin gonderdigi nota listesi (Arduino tone() taslagindan cevrildi: 730 ms -> /4, 365 ms -> /2) */
    { "CIGARETTE SMOKE", "A4/4 A4/4 G4/4 F4/4 F4/4 E4/4 D4/4 E4/4 "
                         "F4/4 G4/4 A4/4 G4/4 F4/4 E4/4 D4/4 C4/4 "
                         "D4/2 E4/2 F4/2 G4/2 A4/4 A4/4 G4/4 F4/4 "
                         "D5/4 C5/2 A4/4 G4/2 F4/2 G4/2 A4/4 D5/4 "
                         "C5/4 A4/2 G4/4 F4/2 E4/2 D4/4 R/6" },

    /* >>> KENDI SARKILARINI BURAYA EKLE (ad ekranda gorunur, en fazla ~20 harf) <<< */
    { "BENIM SARKIM",   "C4 D4 E4 F4 G4 A4 B4 C5/4 R/2 C5 B4 A4 G4 F4 E4 D4 C4/4 R/4" },
};
#define SARKI_SAYI ((int)(sizeof(sarkilar) / sizeof(sarkilar[0])))

typedef struct { const char *ad; const nota_t *n; uint32_t adet; } melodi_t;
static melodi_t melodiler[SARKI_SAYI];
static nota_t nota_havuzu[1200];

/* "D#5/2" gibi bir notayi frekansa cevirir; yazi isaretcisini ilerletir */
static int nota_coz(const char **p, nota_t *n)
{
    static const uint16_t oktav4[12] = { 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494 };
    static const int8_t harf_yarim[7] = { 9, 11, 0, 2, 4, 5, 7 };          /* A B C D E F G */
    const char *s = *p;
    int yarim, oktav = 4, sure = 2;
    uint32_t f;

    while (*s == ' ') s++;
    if (!*s) return 0;
    if (*s == 'R') { f = 0; s++; }
    else
    {
        if (*s < 'A' || *s > 'G') return 0;
        yarim = harf_yarim[*s - 'A']; s++;
        if (*s == '#') { yarim++; s++; }
        if (*s >= '0' && *s <= '9') oktav = *s++ - '0';
        if (yarim == 12) { yarim = 0; oktav++; }                            /* B# = C */
        f = oktav4[yarim];
        while (oktav > 4) { f *= 2; oktav--; }
        while (oktav < 4) { f /= 2; oktav++; }
    }
    if (*s == '/') { s++; sure = 0; while (*s >= '0' && *s <= '9') sure = sure * 10 + (*s++ - '0'); }
    while (*s && *s != ' ') s++;                                            /* taninmayan artiklari atla */
    n->f = (uint16_t)f; n->v = (uint8_t)(sure ? sure : 1);
    *p = s;
    return 1;
}

static void sarkilari_hazirla(void)
{
    int i; uint32_t k = 0;
    for (i = 0; i < SARKI_SAYI; i++)
    {
        const char *p = sarkilar[i].notalar;
        melodiler[i].ad = sarkilar[i].ad;
        melodiler[i].n = &nota_havuzu[k];
        melodiler[i].adet = 0;
        while (k < sizeof(nota_havuzu) / sizeof(nota_havuzu[0]) && nota_coz(&p, &nota_havuzu[k])) { k++; melodiler[i].adet++; }
        if (melodiler[i].adet == 0) { nota_havuzu[k].f = 0; nota_havuzu[k].v = 4; melodiler[i].adet = 1; k++; }
    }
}

volatile int      g_melodi = -1;         /* calan melodi, -1 = yok        */
volatile uint32_t g_nota_i = 0, g_nota_kalan = 0;
volatile uint16_t g_calan_f = 0;         /* su an calan frekans (ekran icin) */
volatile uint32_t g_bip_bitis = 0;       /* tek bip'in bitis zamani          */

static uint8_t rgb_r, rgb_g, rgb_b;      /* 0..100 */

/* RGB duty'yi CMR'ye yaz. Kirmizi (ch2) buzzer (ch3) ile ayni periyodu paylastigi icin
 * her zaman o anki PERIOD'a gore hesaplanir. %0'da cikis maskelenir. */
static void rgb_kanal(uint32_t ch, uint32_t d)
{
    uint32_t m = 1UL << ch;
    if (d == 0) { PWM1->MSK &= ~m; PWM1->MSKEN |= m; return; }
    PWM1->MSKEN &= ~m;
    PWM_SET_CMR(PWM1, ch, (d >= 100) ? PWM1->PERIOD[ch & ~1UL] : d * (PWM1->PERIOD[ch & ~1UL] + 1) / 100);
}

static void rgb(uint32_t r, uint32_t g, uint32_t b)
{
    rgb_r = (uint8_t)r; rgb_g = (uint8_t)g; rgb_b = (uint8_t)b;
    rgb_kanal(2, r); rgb_kanal(0, g); rgb_kanal(1, b);
}

/* Buzzer frekansi (kesmeden de cagrilir). f = 0 -> sus */
static void ses(uint32_t f)
{
    g_calan_f = (uint16_t)f;
    if (f == 0) { PWM_DisableOutput(PWM1, PWM_CH_3_MASK); return; }
    PWM_Kanal_Ayarla(PWM1, 3, f, 50);                    /* ch2+ch3 periyodu degisir... */
    rgb_kanal(2, rgb_r);                                 /* ...kirmiziyi yeni periyoda gore yeniden yaz */
    PWM_EnableOutput(PWM1, PWM_CH_3_MASK);
}

static void bip(uint32_t f, uint32_t ms)                 /* beklemeden kisa ses */
{
    if (g_melodi >= 0) return;                           /* muzik caliyorsa karisma */
    ses(f);
    g_bip_bitis = g_ms + ms;
}

static void muzik_cal(int i) { g_nota_i = 0; g_nota_kalan = 0; g_melodi = i; }
static void muzik_dur(void)  { g_melodi = -1; ses(0); }

void TMR0_IRQHandler(void)
{
    TIMER_ClearIntFlag(TIMER0);
    g_ms++;

    if (g_bip_bitis && g_ms >= g_bip_bitis) { g_bip_bitis = 0; if (g_melodi < 0) ses(0); }

    if (g_melodi >= 0)                                   /* muzik motoru */
    {
        if (g_nota_kalan == 0)
        {
            const melodi_t *m = &melodiler[g_melodi];
            const nota_t *n = &m->n[g_nota_i];
            g_nota_kalan = (uint32_t)n->v * 170;         /* sekizlik = 170 ms */
            ses(n->f);
            g_nota_i = (g_nota_i + 1) % m->adet;
        }
        else if (--g_nota_kalan == 25) ses(0);           /* notalar arasi kisa bosluk */
    }
}

/* ======================================================================
 *  3) BUTON / ENCODER KESMELERI
 * ====================================================================== */
void GPABGH_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PB, BIT7)) { GPIO_CLR_INT_FLAG(PB, BIT7); g_enc += PB6 ? 1 : -1; }   /* encoder A */
    if (GPIO_GET_INT_FLAG(PA, BIT7)) { GPIO_CLR_INT_FLAG(PA, BIT7); g_tus |= T_ENC; }
    if (GPIO_GET_INT_FLAG(PB, BIT1)) { GPIO_CLR_INT_FLAG(PB, BIT1); g_tus |= T_SW5; }
}

void GPCDEF_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PC, BIT12)) { GPIO_CLR_INT_FLAG(PC, BIT12); g_tus |= T_SW2; }
    if (GPIO_GET_INT_FLAG(PC, BIT11)) { GPIO_CLR_INT_FLAG(PC, BIT11); g_tus |= T_SW3; }
    if (GPIO_GET_INT_FLAG(PC, BIT10)) { GPIO_CLR_INT_FLAG(PC, BIT10); g_tus |= T_SW4; }
}

/* ======================================================================
 *  4) YARDIMCILAR: LED, ADC, renk
 * ====================================================================== */
static void ledler(uint32_t maske)                       /* bit0..3 = LED5..LED8 */
{
    PF8 = (maske >> 0) & 1; PF7 = (maske >> 1) & 1; PF6 = (maske >> 2) & 1; PF14 = (maske >> 3) & 1;
}

static uint32_t adc_oku(uint32_t ch)
{
    ADC_Open(ADC, ADC_ADCR_DIFFEN_SINGLE_END, ADC_ADCR_ADMD_SINGLE, 1UL << ch);
    ADC_CLR_INT_FLAG(ADC, ADC_ADF_INT);
    ADC_START_CONV(ADC);
    while (!ADC_GET_INT_FLAG(ADC, ADC_ADF_INT)) { }
    return ADC_GET_CONVERSION_DATA(ADC, ch);
}

/* 0..255 renk tekerlegi -> RGB (0..100) */
static void tekerlek(uint8_t p, uint32_t *r, uint32_t *g, uint32_t *b)
{
    if (p < 85)       { *r = (255 - p * 3) * 100 / 255; *g = p * 3 * 100 / 255; *b = 0; }
    else if (p < 170) { p -= 85;  *r = 0; *g = (255 - p * 3) * 100 / 255; *b = p * 3 * 100 / 255; }
    else              { p -= 170; *r = p * 3 * 100 / 255; *g = 0; *b = (255 - p * 3) * 100 / 255; }
}

/* NTC (VDD -[10.2k]- PB2 -[NTC 10k, B=3950]- GND) -> santigrat x10 */
static int32_t ntc_sicaklik_x10(uint32_t adc)
{
    float r, t;
    if (adc < 10 || adc > 4085) return -999;
    r = 10200.0f * (float)adc / (4095.0f - (float)adc);
    t = 1.0f / (1.0f / 298.15f + logf(r / 10000.0f) / 3950.0f) - 273.15f;
    return (int32_t)(t * 10.0f + (t >= 0 ? 0.5f : -0.5f));
}

/* ======================================================================
 *  5) ACILIS ANIMASYONU
 * ====================================================================== */
static void acilis(void)
{
    static const uint16_t jingle[] = { C5, E5, G5, C6 };
    int x, i;
    uint32_t r, g, b;

    for (x = 128; x >= 0; x -= 4)                        /* "M032" sagdan kayarak gelir */
    {
        oled_temizle();
        yazi(x + 29, 0, "M032", 3);
        tekerlek((uint8_t)(x * 2), &r, &g, &b); rgb(r / 3, g / 3, b / 3);
        ledler(1u << ((x / 8) % 4));
        oled_guncelle();
    }
    for (i = 0; i < 4; i++)                              /* jingle + LED doldurma */
    {
        ses(jingle[i]);
        ledler((1u << (i + 1)) - 1);
        oled_temizle();
        yazi(29, 0, "M032", 3);
        yazi_orta(25, i < 3 ? "BOARDOZA  KIT" : "HAZIR!", 1);
        oled_guncelle();
        bekle_ms(i < 3 ? 120 : 300);
    }
    ses(0); ledler(0); rgb(0, 0, 0);
}

/* ======================================================================
 *  6) MENU
 * ====================================================================== */
enum { UYG_DINO, UYG_OSIL, UYG_TERMO, UYG_MUZIK, UYG_ISIK, UYG_SAYI };
static const char *uyg_adi[UYG_SAYI]  = { "DINO", "OSILOSKOP", "TERMOMETRE", "MUZIK", "ISIK SOVU" };
static const char *uyg_ipucu[UYG_SAYI] = { "zipla: SW3-5 / bas", "canli grafik", "NTC -> santigrat",
                                           "sarkilar + isik", "gokkusagi + dalga" };

static void menu_ciz(int sec, int kayma)                 /* kayma: animasyon icin piksel kaydirma */
{
    int i;
    oled_temizle();
    for (i = -1; i <= 1; i++)                            /* secili + iki komsu (kayarken gorunur) */
    {
        int k = (sec + i + UYG_SAYI) % UYG_SAYI;
        int w = yazi_genislik(uyg_adi[k], 2);
        yazi((128 - w) / 2 + i * 128 + kayma, 4, uyg_adi[k], 2);
    }
    yazi(0, 8, "<", 1);
    yazi(122, 8, ">", 1);
    yazi_orta(24, uyg_ipucu[sec], 1);
    for (i = 0; i < UYG_SAYI; i++)                       /* sayfa noktalari */
        dolu_kutu(52 + i * 5, 21, i == sec ? 3 : 1, i == sec ? 2 : 1);
    oled_guncelle();
}

static int menu(int sec)
{
    uint32_t r, g, b, son = 0;
    uint8_t renk = 0;

    tus_al(); enc_al();
    for (;;)
    {
        int32_t e = enc_al();
        uint32_t t = tus_al();

        if (e != 0)                                      /* kayarak gecis animasyonu */
        {
            int yon = (e > 0) ? 1 : -1, k;
            bip(1500, 15);
            for (k = 1; k <= 4; k++) { menu_ciz(sec, -yon * k * 32); }
            sec = (sec + yon + UYG_SAYI) % UYG_SAYI;
            for (k = 3; k >= 0; k--) { menu_ciz(sec, yon * k * 32); }
        }
        if (t & (T_ENC | T_SW3 | T_SW4 | T_SW5)) { bip(2000, 60); return sec; }

        if (g_ms - son >= 30)                            /* menudeyken yavas renk gecisi */
        {
            son = g_ms;
            tekerlek(renk++, &r, &g, &b); rgb(r / 8, g / 8, b / 8);
            menu_ciz(sec, 0);
        }
    }
}

/* ======================================================================
 *  7) UYGULAMA: DINO
 * ====================================================================== */
static const char *dino_sekil[2][8] = {                  /* 8x8, '#' = piksel; 2 kare (bacaklar) */
    { "....###.", "....#.##", "....####", "#..###..", "######..", ".####...", "..#.#...", "..#.#..." },
    { "....###.", "....#.##", "....####", "#..###..", "######..", ".####...", "..#..#..", ".#...#.." },
};

static void dino_ciz(int x, int y, int kare)
{
    int i, j;
    for (j = 0; j < 8; j++)
        for (i = 0; i < 8; i++)
            if (dino_sekil[kare][j][i] == '#') piksel(x + i, y + j, 1);
}

static void kaktus_ciz(int x, int h)
{
    dolu_kutu(x + 2, 31 - h, 3, h);                      /* govde */
    dolu_kutu(x, 31 - h + 3, 2, 1); dolu_kutu(x, 31 - h + 1, 1, 3);           /* sol kol */
    dolu_kutu(x + 5, 31 - h + 4, 2, 1); dolu_kutu(x + 6, 31 - h + 2, 1, 3);   /* sag kol */
}

static uint32_t dino_rekor = 0;
static uint32_t tohum = 1;
static uint32_t rastgele(void) { tohum = tohum * 1103515245u + 12345u; return tohum >> 16; }

static void uyg_dino(void)
{
    #define ZEMIN 30
    int32_t y, vy;                                       /* dinozor ust kenari, x16 sabit nokta */
    int kx[3], kh[3], i, oyunda, kare = 0;
    uint32_t skor, hiz, son = 0, t;
    char s[24];

yeniden:
    y = (ZEMIN - 8) * 16; vy = 0; skor = 0; hiz = 32;    /* hiz: piksel/kare x16 */
    for (i = 0; i < 3; i++) { kx[i] = 140 + i * 70; kh[i] = 6 + (int)(rastgele() % 5); }
    oyunda = 0;
    rgb(0, 20, 0); ledler(0);

    for (;;)
    {
        if (oyunda && g_ms - son < 25) continue;         /* 40 kare/sn; tuslar kesmede birikir, kaybolmaz */
        t = tus_al();
        enc_al();
        if (t & T_SW2) { rgb(0, 0, 0); ledler(0); return; }

        if (!oyunda)                                     /* baslangic ekrani */
        {
            oled_temizle();
            dino_ciz(10, ZEMIN - 8, 0);
            dolu_kutu(0, 31, 128, 1);
            yazi(30, 4, "DINO", 2);
            snprintf(s, sizeof(s), "HI %lu", (unsigned long)dino_rekor);
            yazi(128 - yazi_genislik(s, 1), 0, s, 1);
            yazi(30, 21, "bas: basla", 1);
            oled_guncelle();
            if (t & T_ZIPLA) { oyunda = 1; tohum ^= g_ms; bip(1800, 40); son = g_ms; }
            continue;
        }
        son = g_ms;

        if ((t & T_ZIPLA) && y >= (ZEMIN - 8) * 16) { vy = -68; bip(1400, 30); }   /* zipla */
        vy += 8;                                         /* yercekimi */
        y += vy;
        if (y > (ZEMIN - 8) * 16) { y = (ZEMIN - 8) * 16; vy = 0; }

        for (i = 0; i < 3; i++)                          /* kaktuslar */
        {
            kx[i] -= (int)(hiz / 16);
            if (kx[i] < -8)
            {
                int en_sag = kx[0];
                if (kx[1] > en_sag) en_sag = kx[1];
                if (kx[2] > en_sag) en_sag = kx[2];
                kx[i] = (en_sag < 128 ? 128 : en_sag) + 45 + (int)(rastgele() % 50);
                kh[i] = 6 + (int)(rastgele() % 6);
            }
        }

        skor++;
        if (skor % 400 == 0) { hiz += 6; bip(2500, 80); ledler(0xF); }
        else if (skor % 400 == 40) ledler(0);
        if (hiz > 90) hiz = 90;
        kare = (skor / 4) & 1;

        /* carpisma: dinozor kutusu (x 11..16) ile kaktus govdesi/kollari */
        for (i = 0; i < 3; i++)
        {
            int dy = y / 16;
            if (kx[i] + 6 >= 11 && kx[i] <= 16 && dy + 7 >= 31 - kh[i])
            {
                int k;
                muzik_dur();
                rgb(100, 0, 0); ledler(0xF);
                if (skor > dino_rekor) dino_rekor = skor;
                for (k = 0; k < 3; k++) { ses(400 - k * 100); bekle_ms(120); }
                ses(0);
                oled_temizle();
                yazi_orta(0, "OYUN BITTI", 2);
                snprintf(s, sizeof(s), "SKOR %lu  REKOR %lu", (unsigned long)skor, (unsigned long)dino_rekor);
                yazi_orta(20, s, 1);
                oled_guncelle();
                bekle_ms(700);
                tus_al();
                while (!(t = tus_al())) { }
                if (t & T_SW2) { rgb(0, 0, 0); ledler(0); return; }
                goto yeniden;
            }
        }

        oled_temizle();
        for (i = 0; i < 128; i += 8) piksel((i - (int)(skor * hiz / 16)) & 127, 31, 1);   /* kayan zemin */
        dolu_kutu(0, 31, 128, 1);
        for (i = 0; i < 128; i += 13) piksel((i * 7 + 128 - (int)(skor * hiz / 16) % 128) % 128, 29 + (i & 1), 1);
        for (i = 0; i < 3; i++) if (kx[i] < 128) kaktus_ciz(kx[i], kh[i]);
        dino_ciz(10, y / 16, (y >= (ZEMIN - 8) * 16) ? kare : 0);
        snprintf(s, sizeof(s), "%05lu", (unsigned long)skor);
        yazi(98, 0, s, 1);
        oled_guncelle();
    }
}

/* ======================================================================
 *  8) UYGULAMA: OSILOSKOP
 * ====================================================================== */
static void uyg_osiloskop(void)
{
    static const char *ad[5]  = { "POT3", "POT4", "POT5", "LDR", "NTC" };
    static const uint8_t ch[5] = { 3, 4, 5, 15, 2 };
    uint8_t iz[128];
    int kaynak = 0, dondur = 0, i;
    uint32_t son = 0, v = 0;
    char s[24];

    memset(iz, 31, sizeof(iz));
    rgb(0, 0, 0);
    for (;;)
    {
        uint32_t t = tus_al();
        int32_t e = enc_al();
        if (t & T_SW2) { ledler(0); rgb(0, 0, 0); return; }
        if (e) { kaynak = (kaynak + (e > 0 ? 1 : 4)) % 5; memset(iz, 31, sizeof(iz)); bip(1500, 15); }
        if (t & T_ENC) { dondur = !dondur; bip(dondur ? 900 : 1800, 40); }

        if (g_ms - son < 20) continue;                   /* 50 ornek/sn */
        son = g_ms;

        if (!dondur)
        {
            v = adc_oku(ch[kaynak]);
            memmove(iz, iz + 1, 127);
            iz[127] = (uint8_t)(31 - v * 22 / 4095);     /* grafik alani: y 9..31 */
            ledler((1u << ((v * 5) / 4096)) - 1);         /* LED cubugu: deger seviyesi */
            rgb(0, v * 40 / 4095, (4095 - v) * 40 / 4095);
        }

        oled_temizle();
        snprintf(s, sizeof(s), "%-4s %4lu %lu.%02luV%s", ad[kaynak], (unsigned long)v,
                 (unsigned long)(v * 330 / 4095 / 100), (unsigned long)(v * 330 / 4095 % 100), dondur ? " ||" : "");
        yazi(0, 0, s, 1);
        for (i = 0; i < 128; i += 4) piksel(i, 20, 1);   /* orta cizgi (noktali) */
        for (i = 1; i < 128; i++)                        /* izi cizgiyle birlestir */
        {
            int a = iz[i - 1], b = iz[i], y;
            if (a > b) { int tmp = a; a = b; b = tmp; }
            for (y = a; y <= b; y++) piksel(i, y, 1);
        }
        oled_guncelle();
    }
}

/* ======================================================================
 *  9) UYGULAMA: TERMOMETRE
 * ====================================================================== */
static void uyg_termometre(void)
{
    int32_t ort = 0, t10, ilk = -9999;
    uint32_t son = 0, ldr;
    int i;
    char s[24];

    for (;;)
    {
        uint32_t t = tus_al();
        enc_al();
        if (t & T_SW2) { ledler(0); rgb(0, 0, 0); return; }
        if (g_ms - son < 150) continue;
        son = g_ms;

        ort = 0;
        for (i = 0; i < 8; i++) ort += (int32_t)adc_oku(2);
        t10 = ntc_sicaklik_x10((uint32_t)(ort / 8));
        ldr = adc_oku(15);
        if (ilk == -9999) ilk = t10;

        /* renk: <= 22 C mavi, 27 C yesil, >= 32 C kirmizi */
        {
            int32_t k = t10 - 220;                       /* 0..100 = 22..32 C */
            if (k < 0) k = 0;
            if (k > 100) k = 100;
            if (k < 50) rgb(0, (uint32_t)k * 2 * 40 / 100, (uint32_t)(100 - k * 2) * 40 / 100);
            else        rgb((uint32_t)(k - 50) * 2 * 40 / 100, (uint32_t)(100 - (k - 50) * 2) * 40 / 100, 0);
        }
        /* LED'ler: acilistaki sicakliga gore artis (her 0.5 C bir LED) -> NTC'ye dokun! */
        {
            int32_t fark = (t10 - ilk) / 5;
            ledler(fark <= 0 ? 0 : (fark >= 4 ? 0xF : (1u << fark) - 1));
        }

        oled_temizle();
        yazi(0, 0, "SICAKLIK", 1);
        snprintf(s, sizeof(s), "isik %lu%%", (unsigned long)((4095 - ldr) * 100 / 4095));
        yazi(128 - yazi_genislik(s, 1), 0, s, 1);
        if (t10 == -999) yazi_orta(12, "HATA", 2);
        else
        {
            int w;
            snprintf(s, sizeof(s), "%ld.%ld", (long)(t10 / 10), (long)((t10 < 0 ? -t10 : t10) % 10));
            w = yazi_genislik(s, 3);
            yazi((116 - w) / 2, 9, s, 3);
            cerceve((116 - w) / 2 + w + 3, 9, 4, 4);     /* derece isareti */
            yazi((116 - w) / 2 + w + 9, 9, "C", 2);
        }
        oled_guncelle();
    }
}

/* ======================================================================
 *  10) UYGULAMA: MUZIK
 * ====================================================================== */
static void uyg_muzik(void)
{
    uint8_t bar[16];
    int sec = 0, i, n = 0;
    uint32_t son = 0, r, g, b;
    uint16_t son_f = 0;

    memset(bar, 0, sizeof(bar));
    for (;;)
    {
        uint32_t t = tus_al();
        int32_t e = enc_al();
        if (t & T_SW2) { muzik_dur(); ledler(0); rgb(0, 0, 0); return; }
        if (e) { sec = (sec + (e > 0 ? 1 : SARKI_SAYI - 1)) % SARKI_SAYI; if (g_melodi >= 0) muzik_cal(sec); else bip(1500, 15); }
        if (t & (T_ENC | T_SW3 | T_SW4 | T_SW5)) { if (g_melodi >= 0) muzik_dur(); else muzik_cal(sec); }

        if (g_ms - son < 30) continue;
        son = g_ms;

        if (g_calan_f && g_calan_f != son_f)             /* yeni nota: bar ekle, isik */
        {
            uint32_t h = (g_calan_f > 260) ? (g_calan_f - 260) * 20 / 400 + 3 : 3;
            if (h > 22) h = 22;
            memmove(bar, bar + 1, 15);
            bar[15] = (uint8_t)h;
            tekerlek((uint8_t)(g_calan_f / 3), &r, &g, &b); rgb(r / 2, g / 2, b / 2);
            ledler((1u << ((h * 4) / 23 + 1)) - 1);
            n++;
        }
        son_f = g_calan_f;
        if (!g_calan_f) { for (i = 0; i < 16; i++) if (bar[i]) bar[i]--; }   /* sessizlikte barlar soner */

        oled_temizle();
        yazi(0, 0, g_melodi >= 0 ? ">" : "#", 1);
        yazi(8, 0, melodiler[sec].ad, 1);
        yazi(110, 0, g_melodi >= 0 ? "||" : "|>", 1);
        for (i = 0; i < 16; i++) if (bar[i]) dolu_kutu(i * 8 + 1, 32 - bar[i], 6, bar[i]);
        oled_guncelle();
        if (g_melodi < 0 && !g_calan_f) { ledler(0); rgb(0, 0, 0); }
    }
}

/* ======================================================================
 *  11) UYGULAMA: ISIK SOVU
 * ====================================================================== */
static const int8_t sinus[32] = { 0,2,4,6,7,9,10,11,11,11,10,9,7,6,4,2,0,-2,-4,-6,-7,-9,-10,-11,-11,-11,-10,-9,-7,-6,-4,-2 };

static void uyg_isik(void)
{
    uint32_t son = 0, r, g, b, faz = 0, led = 0, led_son = 0;
    int yon = 1, x;

    for (;;)
    {
        uint32_t t = tus_al(), hiz;
        enc_al();
        if (t & T_SW2) { ledler(0); rgb(0, 0, 0); return; }
        if (g_ms - son < 25) continue;
        son = g_ms;

        hiz = 1 + adc_oku(3) * 12 / 4095;                /* POT3 = hiz */
        faz += hiz;
        tekerlek((uint8_t)(faz & 255), &r, &g, &b);
        rgb(r, g, b);

        if (g_ms - led_son > 400 - hiz * 25)              /* knight rider */
        {
            led_son = g_ms;
            ledler(1u << led);
            if ((int)led + yon < 0 || led + yon > 3) yon = -yon;
            led += yon;
        }

        oled_temizle();
        for (x = 0; x < 128; x++)                        /* iki dalga, ters yonlerde */
        {
            piksel(x, 16 + sinus[(x / 2 + faz / 2) & 31], 1);
            piksel(x, 16 + sinus[(x / 3 - faz / 3 + 64) & 31] / 2, 1);
        }
        yazi(0, 0, "ISIK SOVU", 1);
        yazi(98, 25, "POT3", 1);
        oled_guncelle();
    }
}

/* ======================================================================
 *  12) KURULUM VE main
 * ====================================================================== */
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

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk | SYS_GPB_MFPH_PB15MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD | SYS_GPB_MFPH_PB15MFP_ADC0_CH15);
    SYS->GPB_MFPL = (SYS->GPB_MFPL & ~(SYS_GPB_MFPL_PB2MFP_Msk | SYS_GPB_MFPL_PB3MFP_Msk |
                                       SYS_GPB_MFPL_PB4MFP_Msk | SYS_GPB_MFPL_PB5MFP_Msk)) |
                    (SYS_GPB_MFPL_PB2MFP_ADC0_CH2 | SYS_GPB_MFPL_PB3MFP_ADC0_CH3 |
                     SYS_GPB_MFPL_PB4MFP_ADC0_CH4 | SYS_GPB_MFPL_PB5MFP_ADC0_CH5);
    SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC3MFP_Msk | SYS_GPC_MFPL_PC4MFP_Msk | SYS_GPC_MFPL_PC5MFP_Msk)) |
                    (SYS_GPC_MFPL_PC3MFP_PWM1_CH2 | SYS_GPC_MFPL_PC4MFP_PWM1_CH1 | SYS_GPC_MFPL_PC5MFP_PWM1_CH0);
    SYS->GPC_MFPH = (SYS->GPC_MFPH & ~SYS_GPC_MFPH_PC9MFP_Msk) | SYS_GPC_MFPH_PC9MFP_PWM1_CH3;
    SYS_LockReg();
}

static void donanim_baslat(void)
{
    /* LED'ler */
    GPIO_SetMode(PF, BIT8 | BIT7 | BIT6 | BIT14, GPIO_MODE_OUTPUT);
    ledler(0);

    /* Analog girisler */
    GPIO_SetMode(PB, BIT2 | BIT3 | BIT4 | BIT5 | BIT15, GPIO_MODE_INPUT);
    GPIO_DISABLE_DIGITAL_PATH(PB, BIT2 | BIT3 | BIT4 | BIT5 | BIT15);
    ADC_POWER_ON(ADC);

    /* PWM1: RGB (ch0..2) + buzzer (ch3) */
    PWM_Kanal_Ayarla(PWM1, 0, 1000, 0);
    PWM_Kanal_Ayarla(PWM1, 1, 1000, 0);
    PWM_Kanal_Ayarla(PWM1, 2, 1000, 0);
    PWM_Kanal_Ayarla(PWM1, 3, 1000, 50);
    PWM_EnableOutput(PWM1, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK);
    PWM_DisableOutput(PWM1, PWM_CH_3_MASK);
    PWM_Start(PWM1, PWM_CH_0_MASK | PWM_CH_1_MASK | PWM_CH_2_MASK | PWM_CH_3_MASK);

    /* Butonlar + encoder (kartta dis pull-up var) */
    GPIO_SetMode(PC, BIT12 | BIT11 | BIT10, GPIO_MODE_INPUT);
    GPIO_SetMode(PB, BIT1 | BIT7 | BIT6, GPIO_MODE_INPUT);
    GPIO_SetMode(PA, BIT7, GPIO_MODE_INPUT);
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_64);   /* ~1.7 ms */
    GPIO_ENABLE_DEBOUNCE(PC, BIT12 | BIT11 | BIT10);
    GPIO_ENABLE_DEBOUNCE(PB, BIT1 | BIT7 | BIT6);
    GPIO_ENABLE_DEBOUNCE(PA, BIT7);
    GPIO_EnableInt(PC, 12, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 11, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 10, GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 1,  GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 7,  GPIO_INT_FALLING);
    GPIO_EnableInt(PA, 7,  GPIO_INT_FALLING);

    /* TIMER0: 1 kHz */
    TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000);
    TIMER_EnableInt(TIMER0);

    NVIC_SetPriority(TMR0_IRQn, 0);                     /* zaman + muzik en yuksek oncelik */
    NVIC_SetPriority(GPIO_PAPB_IRQn, 1);
    NVIC_SetPriority(GPIO_PCPDPEPF_IRQn, 1);
    NVIC_EnableIRQ(TMR0_IRQn);
    NVIC_EnableIRQ(GPIO_PAPB_IRQn);
    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);
    TIMER_Start(TIMER0);
}

int main(void)
{
    int sec = 0;

    SYS_Init();
    UART_Open(UART0, 115200);
    sarkilari_hazirla();                                /* yazi -> nota dizisi (once, kesmeler acilmadan) */
    donanim_baslat();

    oled_yerli_32(1);                                   /* bu proje dogrudan 128x32 cizer */
    if (!oled_baslat()) printf("OLED bulunamadi (J9)! Isik ve ses yine calisir.\n");
    oled_parlaklik(255);

    printf("\nM032 OS basladi. Encoder: sec, bas: gir, SW2: menu\n");
    acilis();

    for (;;)
    {
        sec = menu(sec);
        printf("Uygulama: %s\n", uyg_adi[sec]);
        switch (sec)
        {
            case UYG_DINO:  uyg_dino();       break;
            case UYG_OSIL:  uyg_osiloskop();  break;
            case UYG_TERMO: uyg_termometre(); break;
            case UYG_MUZIK: uyg_muzik();      break;
            case UYG_ISIK:  uyg_isik();       break;
        }
        bip(1000, 40);
    }
}
