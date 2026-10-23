/*
 * 21 - ESP-01M TEST / KOPRU  -- Boardoza M032KG
 *
 * Mac (UART0, 115200) <-> M032 <-> ESP-01M (UART3, ESP_BAUD)
 * Acilista ESP'yi resetler, "AT" gonderir ve cevabi yazar. Sonra seffaf kopru:
 * make monitor'de yazdigin AT komutlari dogrudan ESP'ye gider.
 *
 * Boardoza semasi: ESP TX -> PD0 (UART3_RXD), ESP RX <- PD1 (UART3_TXD),
 *                  IO0 = PD2, EN = PD13, RST = PD3
 * DIKKAT: ESP'nin beslemesi JP1 lehim koprusunden gelir ve varsayilan olarak ACIKTIR!
 */
#include <stdio.h>
#include <string.h>
#include "NuMicro.h"

#ifndef ESP_BAUD
#define ESP_BAUD 115200
#endif

static void bekle_ms(uint32_t ms) { while (ms--) CLK_SysTickDelay(1000); }

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
    CLK_EnableModuleClock(UART3_MODULE);
    CLK_SetModuleClock(UART3_MODULE, CLK_CLKSEL3_UART3SEL_HIRC, CLK_CLKDIV4_UART3(1));
    SystemCoreClockUpdate();
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
    SYS->GPD_MFPL = (SYS->GPD_MFPL & ~(SYS_GPD_MFPL_PD0MFP_Msk | SYS_GPD_MFPL_PD1MFP_Msk |
                                       SYS_GPD_MFPL_PD2MFP_Msk | SYS_GPD_MFPL_PD3MFP_Msk)) |
                    (SYS_GPD_MFPL_PD0MFP_UART3_RXD | SYS_GPD_MFPL_PD1MFP_UART3_TXD);
    SYS->GPD_MFPH &= ~SYS_GPD_MFPH_PD13MFP_Msk;
    SYS_LockReg();
}

static void esp_yaz(const char *s) { UART_Write(UART3, (const uint8_t *)s, strlen(s)); }

/* 'ms' boyunca ESP'den geleni Mac'e aktar */
static void esp_dinle(uint32_t ms)
{
    while (ms--)
    {
        uint32_t k;
        for (k = 0; k < 48; k++)                         /* ~1 ms boyunca FIFO'yu bosalt */
        {
            while (!UART_GET_RX_EMPTY(UART3)) { uint8_t c = UART_READ(UART3); UART_Write(UART0, &c, 1); }
            CLK_SysTickDelay(20);
        }
    }
}

int main(void)
{
    SYS_Init();
    UART_Open(UART0, 115200);
    UART_Open(UART3, ESP_BAUD);

    GPIO_SetMode(PD, BIT2 | BIT3 | BIT13, GPIO_MODE_OUTPUT);
    PD2 = 1;                 /* IO0 = 1: normal acilis (0 olsaydi flash moduna girerdi) */
    PD13 = 1;                /* EN  = 1: cip etkin */
    PD3 = 0;                 /* RST: kisa reset darbesi */
    bekle_ms(50);
    PD3 = 1;

    printf("\n21_esp_test: ESP resetlendi, acilis mesajlari:\n");
    esp_dinle(2000);
    printf("\n>>> AT\n");
    esp_yaz("AT\r\n");
    esp_dinle(500);
    printf("\n>>> AT+GMR (surum)\n");
    esp_yaz("AT+GMR\r\n");
    esp_dinle(800);
    printf("\n--- Kopru acik: AT komutu yaz + Enter ---\n");

    for (;;)
    {
        if (!UART_GET_RX_EMPTY(UART0))
        {
            uint8_t c = UART_READ(UART0);
            if (c == '\r') { esp_yaz("\r\n"); printf("\n"); }
            else if (c != '\n') { UART_Write(UART3, &c, 1); UART_Write(UART0, &c, 1); }
        }
        while (!UART_GET_RX_EMPTY(UART3)) { uint8_t c = UART_READ(UART3); UART_Write(UART0, &c, 1); }
    }
}
