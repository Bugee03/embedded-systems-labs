/*
 * 09 - SPI ile ADRESLENEBILIR LED SERIDI (Phoenix LED / APA102 tipi)
 *
 * Yaptigi: N adet LED'de once kayan bir isik, sonra gokkusagi efekti gosterir.
 *
 * Baglanti:
 *   Boardoza J10 (SPI) pin 4 = MOSI (PA0) -> LED DI (Data In)
 *   Boardoza J10 (SPI) pin 2 = CLK  (PA2) -> LED CI (Clock In)
 *   5V / GND -> LED seridi (GND'ler ORTAK olmali)
 *
 * Cerceve yapisi:
 *   [0x00 0x00 0x00 0x00]                      baslangic (32 bit sifir)
 *   her LED icin [0xE0 | parlaklik(0-31), B, G, R]
 *   [0xFF 0xFF 0xFF 0xFF]                      bitis
 *
 * Not: Yarin verilen LED'in veri sirasi farkliysa (ornegin R,G,B), led_gonder() icindeki sirayi degistir.
 */
#include <stdio.h>
#include "NuMicro.h"

#define N_LED       8
#define PARLAKLIK   4          /* 0..31; dusuk tut, LED'ler cok parlak */

typedef struct { uint8_t r, g, b; } renk_t;
static renk_t led[N_LED];

static void delay_ms(uint32_t ms) { while (ms--) CLK_SysTickDelay(1000); }

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    CLK_EnableModuleClock(SPI0_MODULE);
    CLK_SetModuleClock(SPI0_MODULE, CLK_CLKSEL2_SPI0SEL_PCLK1, 0);
    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* PA0 = MOSI, PA2 = CLK (MISO ve SS kullanilmiyor) */
    SYS->GPA_MFPL = (SYS->GPA_MFPL & ~(SYS_GPA_MFPL_PA0MFP_Msk | SYS_GPA_MFPL_PA2MFP_Msk)) |
                    (SYS_GPA_MFPL_PA0MFP_SPI0_MOSI | SYS_GPA_MFPL_PA2MFP_SPI0_CLK);
    SYS_LockReg();
}

static void spi_tx(uint8_t b)
{
    SPI_WRITE_TX(SPI0, b);
    while (SPI_IS_BUSY(SPI0));          /* bayt gidene kadar bekle */
}

static void led_gonder(void)
{
    int i;
    for (i = 0; i < 4; i++) spi_tx(0x00);                   /* baslangic */
    for (i = 0; i < N_LED; i++)
    {
        spi_tx(0xE0 | PARLAKLIK);
        spi_tx(led[i].b);
        spi_tx(led[i].g);
        spi_tx(led[i].r);
    }
    for (i = 0; i < 4; i++) spi_tx(0xFF);                   /* bitis */
}

/* 0..255 arasi bir degeri gokkusagi rengine cevirir */
static renk_t tekerlek(uint8_t p)
{
    renk_t c;
    if (p < 85)       { c.r = 255 - p * 3; c.g = p * 3;       c.b = 0; }
    else if (p < 170) { p -= 85;  c.r = 0;  c.g = 255 - p * 3; c.b = p * 3; }
    else              { p -= 170; c.r = p * 3; c.g = 0;        c.b = 255 - p * 3; }
    return c;
}

int main(void)
{
    int i, adim, tur;

    SYS_Init();
    UART_Open(UART0, 115200);

    /* Master, mod 0 (CPOL=0, CPHA=0), 8 bit, 2 MHz */
    SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, 2000000);

    printf("\n09_spi_phoenix_led basladi (%d LED)\n", N_LED);

    while (1)
    {
        printf("Kayan isik\n");
        for (tur = 0; tur < 3; tur++)
            for (adim = 0; adim < N_LED; adim++)
            {
                for (i = 0; i < N_LED; i++) led[i] = (renk_t){0, 0, 0};
                led[adim] = (renk_t){255, 0, 0};
                led_gonder();
                delay_ms(80);
            }

        printf("Gokkusagi\n");
        for (adim = 0; adim < 256 * 2; adim++)
        {
            for (i = 0; i < N_LED; i++)
                led[i] = tekerlek((uint8_t)(i * 256 / N_LED + adim));
            led_gonder();
            delay_ms(10);
        }
    }
}
