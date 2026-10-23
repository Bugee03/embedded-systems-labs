/*
 * 02 - BUTONLAR (GPIO giris + kesme + donanimsal debounce)  -- Boardoza M032KG
 *
 * Yaptigi: Karttaki 4 butondan her biri kendi LED'ini yakar/sondurur:
 *            SW2 -> LED5    SW3 -> LED6    SW4 -> LED7    SW5 -> LED8
 *          Her basis seri porta yazilir.
 *
 * Boardoza semasi:
 *   Butonlar (dis 4.7k pull-up var, basinca 0): SW2=PC12 SW3=PC11 SW4=PC10 SW5=PB1
 *   LED'ler (1 = yanar):                         LED5=PF8 LED6=PF7 LED7=PF6 LED8=PF14
 */
#include <stdio.h>
#include "NuMicro.h"
#include "oled.h"             /* ../ortak/oled.c: OLED (J9), takili degilse sessizce atlanir */

volatile uint32_t g_basilan = 0;    /* ISR -> main: hangi butonlar basildi (bit maskesi) */
#define B_SW2  0x1
#define B_SW3  0x2
#define B_SW4  0x4
#define B_SW5  0x8

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
    SYS_LockReg();
}

/* PC..PF pinlerinin ORTAK kesmesi (SW2, SW3, SW4 burada) */
void GPCDEF_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PC, BIT12)) { GPIO_CLR_INT_FLAG(PC, BIT12); PF8  ^= 1; g_basilan |= B_SW2; }
    if (GPIO_GET_INT_FLAG(PC, BIT11)) { GPIO_CLR_INT_FLAG(PC, BIT11); PF7  ^= 1; g_basilan |= B_SW3; }
    if (GPIO_GET_INT_FLAG(PC, BIT10)) { GPIO_CLR_INT_FLAG(PC, BIT10); PF6  ^= 1; g_basilan |= B_SW4; }
}

/* PA/PB pinlerinin ORTAK kesmesi (SW5 = PB1 burada).
 * Isimler startup dosyasindakiyle birebir ayni olmali, yoksa kesme hic calismaz! */
void GPABGH_IRQHandler(void)
{
    if (GPIO_GET_INT_FLAG(PB, BIT1))  { GPIO_CLR_INT_FLAG(PB, BIT1);  PF14 ^= 1; g_basilan |= B_SW5; }
}

int main(void)
{
    SYS_Init();
    UART_Open(UART0, 115200);

    /* LED'ler cikis */
    GPIO_SetMode(PF, BIT8 | BIT7 | BIT6 | BIT14, GPIO_MODE_OUTPUT);
    PF8 = PF7 = PF6 = PF14 = 0;

    /* Butonlar giris. Kartta dis pull-up oldugu icin normal INPUT yeterli.
     * (Dis pull-up olmasaydi M031/M032'de GPIO_MODE_QUASI kullanilirdi.) */
    GPIO_SetMode(PC, BIT12 | BIT11 | BIT10, GPIO_MODE_INPUT);
    GPIO_SetMode(PB, BIT1, GPIO_MODE_INPUT);

    /* Donanimsal debounce: LIRC (38.4 kHz) x 256 ornek ~ 6.7 ms */
    GPIO_SET_DEBOUNCE_TIME(GPIO_DBCTL_DBCLKSRC_LIRC, GPIO_DBCTL_DBCLKSEL_256);
    GPIO_ENABLE_DEBOUNCE(PC, BIT12 | BIT11 | BIT10);
    GPIO_ENABLE_DEBOUNCE(PB, BIT1);

    /* 1 -> 0 gecisinde (basinca) kesme */
    GPIO_EnableInt(PC, 12, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 11, GPIO_INT_FALLING);
    GPIO_EnableInt(PC, 10, GPIO_INT_FALLING);
    GPIO_EnableInt(PB, 1,  GPIO_INT_FALLING);
    NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);     /* PC..PF */
    NVIC_EnableIRQ(GPIO_PAPB_IRQn);         /* PA, PB */

    printf("\n02_buton basladi. SW2..SW5'e bas.\n");
    oled_baslat();
    kayan_yazi(48, "SW2-SW5: her buton kendi LED'ini yakar/sondurur", 2);

    while (1)
    {
        uint32_t b;
        static const char *son = "--";
        int i;
        const int led[4] = { (int)PF8, (int)PF7, (int)PF6, (int)PF14 };

        __disable_irq();                    /* oku + sifirla, arada kesme girmesin */
        b = g_basilan;
        g_basilan = 0;
        __enable_irq();

        if (b & B_SW2) printf("SW2 -> LED5=%d\n", (int)PF8);
        if (b & B_SW3) printf("SW3 -> LED6=%d\n", (int)PF7);
        if (b & B_SW4) printf("SW4 -> LED7=%d\n", (int)PF6);
        if (b & B_SW5) printf("SW5 -> LED8=%d\n", (int)PF14);

        if (b & B_SW2) son = "SW2";
        if (b & B_SW3) son = "SW3";
        if (b & B_SW4) son = "SW4";
        if (b & B_SW5) son = "SW5";

        /* Ekran: ustte son basilan buton, ortada 4 LED kutusu */
        oled_temizle();
        yazi(0, 0, "SON:", 2);
        yazi(56, 0, son, 2);
        for (i = 0; i < 4; i++)
        {
            int x = 4 + i * 31;
            cerceve(x, 18, 26, 22);
            if (led[i]) dolu_kutu(x + 3, 21, 20, 16);
            else        yazi(x + 8, 22, (const char[]){ (char)('5' + i), 0 }, 2);
        }
        oled_guncelle();

        /* Kesmesiz (polling) yontem de olurdu:  if (PC12 == 0) { ... }  */
    }
}
