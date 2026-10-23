/*
 * 00 - PIN BULUCU: karttaki LED'ler hangi pinde?
 *
 * Her GPIO pinini sirayla 2 saniye boyunca yakip sondurur (4 kez)
 * ve seri porta pinin adini yazar. 'make monitor' ile izle;
 * bir LED yanip sondugunde ekranda yazan pin o LED'in pinidir.
 *
 * Atlanan pinler: PB12/PB13 (UART0 = seri port), PF0/PF1 (SWD = programlayici)
 */
#include <stdio.h>
#include "NuMicro.h"

static void delay_ms(uint32_t ms) { while (ms--) CLK_SysTickDelay(1000); }

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

int main(void)
{
    GPIO_T *port[] = { PA, PB, PC, PD, PE, PF, PG, PH };
    const char ad[] = "ABCDEFGH";
    uint32_t p, n, k;

    SYS_Init();
    UART_Open(UART0, 115200);

    while (1)
    {
        printf("\n=== PIN BULUCU basliyor ===\n");
        for (p = 0; p < 8; p++)
        {
            for (n = 0; n < 16; n++)
            {
                if (p == 1 && (n == 12 || n == 13)) continue;   /* UART0 */
                if (p == 5 && (n == 0 || n == 1))   continue;   /* SWD   */

                printf("Simdi: P%c%u\n", ad[p], (unsigned)n);
                GPIO_SetMode(port[p], 1u << n, GPIO_MODE_OUTPUT);
                for (k = 0; k < 4; k++)
                {
                    port[p]->DOUT ^= (1u << n);
                    delay_ms(250);
                    port[p]->DOUT ^= (1u << n);
                    delay_ms(250);
                }
                GPIO_SetMode(port[p], 1u << n, GPIO_MODE_INPUT);
            }
        }
    }
}
