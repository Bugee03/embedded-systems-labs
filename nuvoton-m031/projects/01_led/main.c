/*
 * 01 - LED YAK / SONDUR (GPIO cikis)  -- Boardoza M032KG kiti
 *
 * Yaptigi: Karttaki 4 kirmizi LED'i (LED5..LED8) 500 ms'de bir
 *          birlikte yakip sondurur, seri porta durumu yazar.
 *
 * Boardoza semasi (LEDS): LED5 = PF8, LED6 = PF7, LED7 = PF6, LED8 = PF14
 * OLED (J9) takiliysa: buyuk "ACIK/KAPALI" + kayan aciklama yazisi.
 *   Pin --[680R]--|>|-- GND  ->  1 = yanar, 0 = soner (active-high)
 */
#include <stdio.h>
#include "NuMicro.h"
#include "oled.h"             /* ../ortak/oled.c: OLED (J9), takili degilse sessizce atlanir */

#define LED_PORT  PF
#define LED_BITS  (BIT8 | BIT7 | BIT6 | BIT14)   /* LED5..LED8 */

static void delay_ms(uint32_t ms)
{
    while (ms--) CLK_SysTickDelay(1000);
}

void SYS_Init(void)
{
    SYS_UnlockReg();                                        /* korumali register'lari ac */

    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);                /* dahili 48 MHz osilator    */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);                    /* printf icin UART0         */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    SYS_LockReg();
}

int main(void)
{
    SYS_Init();
    UART_Open(UART0, 115200);

    /* GPIO icin clock/MFP gerekmez: pinler varsayilan olarak GPIO'dur */
    GPIO_SetMode(LED_PORT, LED_BITS, GPIO_MODE_OUTPUT);

    printf("\n01_led basladi\n");
    oled_baslat();
    kayan_yazi(48, "01 LED: LED5-LED8 (PF8 PF7 PF6 PF14) 500 ms yanip soner", 2);

    while (1)
    {
        int adim;

        LED_PORT->DOUT |= LED_BITS;         /* 4 LED'i birden yak    */
        printf("LED: 1\n");
        for (adim = 0; adim < 10; adim++)   /* 500 ms bekle, bu arada ekrani guncelle */
        {
            oled_temizle();
            yazi_orta(0, "LED", 2);
            yazi_orta(18, "ACIK", 3);
            oled_guncelle();                /* ~25 ms (I2C) */
            delay_ms(25);
        }

        LED_PORT->DOUT &= ~LED_BITS;        /* 4 LED'i birden sondur */
        printf("LED: 0\n");
        for (adim = 0; adim < 10; adim++)
        {
            oled_temizle();
            yazi_orta(0, "LED", 2);
            yazi_orta(18, "KAPALI", 3);
            oled_guncelle();
            delay_ms(25);
        }

        /* Tek LED icin: PF8 = 1; / PF8 = 0; / PF8 ^= 1; (toggle) */
    }
}
