/*
 * 08 - UART: BLUETOOTH MODUL ile HABERLESME + OLED  -- Boardoza M032KG
 *
 * Yaptigi:
 *   - Iki kaynaktan KOMUT kabul eder:
 *       BT : Bluetooth modulu (telefon)  -> UART4 (J11)
 *       PC : Mac'teki terminal (make monitor) -> UART0
 *   - Komutlar (satir sonu Enter ile):
 *       1 veya AC     -> LED5 yak
 *       0 veya KAPAT  -> LED5 sondur
 *       ?             -> LED durumunu gonder
 *       baska her sey -> mesaj olarak OLED'de gosterilir
 *   - PC'de yazilan her satir Bluetooth'a da gonderilir (telefonda gorunur),
 *     BT'den gelen her satir PC terminaline de yazilir.
 *   - OLED: baslik, LED durumu ve son 3 mesaj (kaynagiyla: BT> / PC>)
 *
 * Baglanti (Boardoza):
 *   Bluetooth modulu -> J11 (UART4):  J11.2 TXD (PC7) -> modul RX
 *                                      J11.1 RXD (PC6) <- modul TX
 *                                      J11.3 GND,  modul 3.3V -> J14 (3V3)
 *   OLED -> J9 (I2C0): SCL = PE13, SDA = PC8
 *   LED  -> LED5 (PF8, kartta)
 *
 * Not: Modulun baud hizi farkli olabilir (9600 / 115200). BT_BAUD'u ona gore ayarla.
 */
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "NuMicro.h"
#include "oled.h"

#define BT_BAUD   9600
#define BT_UART   UART4
#define LED       PF8

/* --- BT'den kesmeyle gelen baytlar icin halka (ring) tampon --- */
#define RB_BOYUT  64
static volatile uint8_t  rb[RB_BOYUT];
static volatile uint32_t rb_yaz = 0, rb_oku = 0;

/* --- Satir biriktirme (her kaynak icin ayri) --- */
#define SATIR_MAX 32
typedef struct { char buf[SATIR_MAX]; uint32_t n; } satir_t;
static satir_t bt_satir, pc_satir;

/* --- Son 3 mesaj: en yenisi buyuk ve kayarak, oncekiler kucuk --- */
#define GECMIS 3
static char gecmis[GECMIS][40];
static int  oled_var = 0;

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);                /* Mac (printf) */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
    CLK_EnableModuleClock(UART4_MODULE);                /* Bluetooth    */
    CLK_SetModuleClock(UART4_MODULE, CLK_CLKSEL3_UART4SEL_HIRC, CLK_CLKDIV4_UART4(1));
    CLK_EnableModuleClock(I2C0_MODULE);                 /* OLED         */
    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
    /* PC6 = UART4_RXD, PC7 = UART4_TXD (J11) */
    SYS->GPC_MFPL = (SYS->GPC_MFPL & ~(SYS_GPC_MFPL_PC6MFP_Msk | SYS_GPC_MFPL_PC7MFP_Msk)) |
                    (SYS_GPC_MFPL_PC6MFP_UART4_RXD | SYS_GPC_MFPL_PC7MFP_UART4_TXD);
    /* PC8 = I2C0_SDA, PE13 = I2C0_SCL (J9) */
    SYS->GPC_MFPH = (SYS->GPC_MFPH & ~SYS_GPC_MFPH_PC8MFP_Msk) | SYS_GPC_MFPH_PC8MFP_I2C0_SDA;
    SYS->GPE_MFPH = (SYS->GPE_MFPH & ~SYS_GPE_MFPH_PE13MFP_Msk) | SYS_GPE_MFPH_PE13MFP_I2C0_SCL;

    SYS_LockReg();
}

/* UART4 ve UART6 ayni kesmeyi paylasir */
void UART46_IRQHandler(void)
{
    while (!UART_GET_RX_EMPTY(BT_UART))         /* FIFO bosalana kadar oku */
    {
        uint8_t c = UART_READ(BT_UART);
        uint32_t sonraki = (rb_yaz + 1) % RB_BOYUT;
        if (sonraki != rb_oku) { rb[rb_yaz] = c; rb_yaz = sonraki; }   /* doluysa at */
    }
}

static void bt_yaz(const char *s)
{
    while (*s) UART_Write(BT_UART, (const uint8_t *)s++, 1);
}

/* Hem telefona hem Mac'e cevap */
static void cevap(const char *s)
{
    bt_yaz(s); bt_yaz("\r\n");
    printf("   -> %s\n", s);
}

static void ekran_ciz(void)
{
    char eski[96];
    if (!oled_var) return;

    oled_temizle();
    yazi(0, 0, "LED5", 2);                        /* ust: LED durumu (buyuk) */
    yazi(56, 0, LED ? "ACIK" : "KAPALI", 2);

    /* orta: en son mesaj BUYUK; ekrana sigmazsa kayar */
    kayan_yazi(20, gecmis[GECMIS - 1], 3);

    /* alt: onceki iki mesaj (kucuk) */
    snprintf(eski, sizeof(eski), "%s | %s", gecmis[GECMIS - 3], gecmis[GECMIS - 2]);
    eski[21] = 0;                                 /* 21 karakter = ekran genisligi */
    yazi(0, 52, eski, 1);

    oled_guncelle();
}

static void gecmise_ekle(const char *kaynak, const char *s)
{
    int i;
    for (i = 0; i < GECMIS - 1; i++) strcpy(gecmis[i], gecmis[i + 1]);   /* yukari kaydir */
    snprintf(gecmis[GECMIS - 1], sizeof(gecmis[0]), "%s%s", kaynak, s);
}

/* Bir satir tamamlandiginda cagrilir */
static void komut_isle(const char *kaynak, char *s)
{
    char buyuk[SATIR_MAX];
    uint32_t i;

    for (i = 0; s[i] && i < SATIR_MAX - 1; i++) buyuk[i] = (char)toupper((unsigned char)s[i]);
    buyuk[i] = 0;

    gecmise_ekle(kaynak, s);

    if (!strcmp(buyuk, "1") || !strcmp(buyuk, "AC"))         { LED = 1; cevap("LED ACIK"); }
    else if (!strcmp(buyuk, "0") || !strcmp(buyuk, "KAPAT")) { LED = 0; cevap("LED KAPALI"); }
    else if (!strcmp(buyuk, "?"))                            { cevap(LED ? "LED: 1" : "LED: 0"); }
}

/* Gelen her karakteri satira ekler; Enter gelince satiri isler */
static void karakter_al(satir_t *st, const char *kaynak, uint8_t c)
{
    if (c == '\r' || c == '\n')
    {
        if (st->n == 0) return;                   /* bos satir (\r\n'nin ikinci yarisi) */
        st->buf[st->n] = 0;
        komut_isle(kaynak, st->buf);
        st->n = 0;
    }
    else if ((c == 8 || c == 127) && st->n)       /* backspace */
    {
        st->n--;
    }
    else if (c >= 32 && c < 127 && st->n < SATIR_MAX - 1)
    {
        st->buf[st->n++] = (char)c;
    }
}

int main(void)
{
    SYS_Init();
    UART_Open(UART0, 115200);
    UART_Open(BT_UART, BT_BAUD);
    I2C_Open(I2C0, 400000);

    GPIO_SetMode(PF, BIT8, GPIO_MODE_OUTPUT);
    LED = 0;

    UART_EnableInt(BT_UART, UART_INTEN_RDAIEN_Msk); /* "veri geldi" kesmesi */
    NVIC_EnableIRQ(UART46_IRQn);                    /* UART4 ve UART6 ortak IRQ */

    CLK_SysTickDelay(100000);                       /* OLED acilis */
    oled_var = oled_baslat();

    printf("\n08_uart_bluetooth basladi (BT baud=%d, OLED %s)\n", BT_BAUD, oled_var ? "var" : "YOK");
    printf("Komutlar: 1 / AC, 0 / KAPAT, ?  (Enter ile). Baska yazi OLED'e gider.\n");
    bt_yaz("M032 hazir. 1=LED ac, 0=LED kapat, ?=durum\r\n");

    gecmise_ekle("", "Komut bekleniyor: 1 / 0 / ?");

    while (1)
    {
        ekran_ciz();                              /* ~25 ms; kayan yazi bu sayede ilerler */

        /* 1) Bluetooth'tan gelenler (kesme ring buffer'a yazdi) */
        while (rb_oku != rb_yaz)
        {
            uint8_t c = rb[rb_oku];
            rb_oku = (rb_oku + 1) % RB_BOYUT;
            if (c == '\n' || c == '\r') { if (bt_satir.n) printf("BT> %.*s\n", (int)bt_satir.n, bt_satir.buf); }
            karakter_al(&bt_satir, "BT>", c);
        }

        /* 2) Mac terminalinden gelenler: ekrana geri yaz (echo), BT'ye ilet, komut olarak isle */
        if (!UART_GET_RX_EMPTY(UART0))
        {
            uint8_t c = UART_READ(UART0);
            if (c == '\r') c = '\n';
            if (c == '\n') printf("\n"); else UART_Write(UART0, &c, 1);
            UART_Write(BT_UART, c == '\n' ? (const uint8_t *)"\r\n" : &c, c == '\n' ? 2 : 1);
            karakter_al(&pc_satir, "PC>", c);
        }
    }
}
