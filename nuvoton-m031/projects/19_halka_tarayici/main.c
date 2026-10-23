/*
 * 19 - LED HALKA TARAYICI: Phoenix24 halkasi J15'in hangi pinlerinde?
 *
 * J15'teki 10 pinin her (DATA, CLOCK) ikilisini sirayla dener (90 ikili):
 * her denemede 24 LED'e kirmizi/yesil/mavi gonderir ve 400 ms bekler.
 * Halka yandiginda OLED'de ve seri portta yazan ikili = dogru baglanti.
 *
 * ONCE: halkanin 5V ve GND'si J6'ya bagli olmali (J15'te besleme yok!)
 */
#include <stdio.h>
#include "NuMicro.h"
#include "oled.h"

typedef struct { GPIO_T *port; uint32_t bit; const char *ad; int j15; } pin_t;
static const pin_t pinler[10] = {
    { PE, 7,  "PE7",  1 }, { PE, 6,  "PE6",  2 }, { PE, 5,  "PE5",  3 }, { PE, 4,  "PE4",  4 },
    { PH, 8,  "PH8",  5 }, { PH, 9,  "PH9",  6 }, { PB, 10, "PB10", 7 }, { PB, 11, "PB11", 8 },
    { PB, 8,  "PB8",  9 }, { PB, 9,  "PB9", 10 },
};

static void yaz(const pin_t *p, int v)
{
    if (v) p->port->DOUT |= (1UL << p->bit); else p->port->DOUT &= ~(1UL << p->bit);
}

static void bayt(const pin_t *d, const pin_t *c, uint8_t b)
{
    int i;
    for (i = 7; i >= 0; i--) { yaz(d, (b >> i) & 1); yaz(c, 1); __NOP(); __NOP(); yaz(c, 0); }
}

static void kare(const pin_t *d, const pin_t *c, int renk)
{
    int i;
    for (i = 0; i < 4; i++) bayt(d, c, 0x00);
    for (i = 0; i < 24; i++)
    {
        bayt(d, c, 0xE0 | 6);
        bayt(d, c, renk == 2 ? 255 : 0);     /* B */
        bayt(d, c, renk == 1 ? 255 : 0);     /* G */
        bayt(d, c, renk == 0 ? 255 : 0);     /* R */
    }
    for (i = 0; i < 4; i++) bayt(d, c, 0xFF);
}

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
    SystemCoreClockUpdate();
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
    /* J15 pinlerini GPIO yap */
    SYS->GPE_MFPL &= ~(SYS_GPE_MFPL_PE4MFP_Msk | SYS_GPE_MFPL_PE5MFP_Msk | SYS_GPE_MFPL_PE6MFP_Msk | SYS_GPE_MFPL_PE7MFP_Msk);
    SYS->GPH_MFPH &= ~(SYS_GPH_MFPH_PH8MFP_Msk | SYS_GPH_MFPH_PH9MFP_Msk);
    SYS->GPB_MFPH &= ~(SYS_GPB_MFPH_PB8MFP_Msk | SYS_GPB_MFPH_PB9MFP_Msk | SYS_GPB_MFPH_PB10MFP_Msk | SYS_GPB_MFPH_PB11MFP_Msk);
    SYS_LockReg();
}

int main(void)
{
    int d, c, r, i;
    char s[32];

    SYS_Init();
    UART_Open(UART0, 115200);
    for (i = 0; i < 10; i++) { GPIO_SetMode(pinler[i].port, 1UL << pinler[i].bit, GPIO_MODE_OUTPUT); yaz(&pinler[i], 0); }
    oled_yerli_32(1);
    oled_baslat();
    printf("\n19_halka_tarayici: halka yandiginda yazan ikiliyi not et.\n");

    for (;;)
        for (d = 0; d < 10; d++)
            for (c = 0; c < 10; c++)
            {
                if (d == c) continue;
                snprintf(s, sizeof(s), "DATA=J15.%d %s", pinler[d].j15, pinler[d].ad);
                printf("%s  CLK=J15.%d %s\n", s, pinler[c].j15, pinler[c].ad);
                oled_temizle();
                yazi(0, 0, s, 1);
                snprintf(s, sizeof(s), "CLK =J15.%d %s", pinler[c].j15, pinler[c].ad);
                yazi(0, 10, s, 1);
                yazi(0, 24, "halka yaninca not et", 1);
                oled_guncelle();
                for (r = 0; r < 3; r++) { kare(&pinler[d], &pinler[c], r); CLK_SysTickDelay(130000); }
                kare(&pinler[d], &pinler[c], 3);         /* sondur (renk 3 = siyah) */
                for (i = 0; i < 10; i++) yaz(&pinler[i], 0);
            }
}
