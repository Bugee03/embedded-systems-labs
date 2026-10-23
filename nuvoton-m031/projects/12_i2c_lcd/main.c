/*
 * 12 - I2C: 16x2 LCD (PCF8574 I2C adaptorlu)
 *
 * Yaptigi: 1. satira "Merhaba M031", 2. satira saniye sayaci yazar.
 *
 * Baglanti (Boardoza J9): SCL = PE13, SDA = PC8, GND.  VCC: LCD 5V istiyorsa J6'daki 5V'a bagla
 *   Adres genelde 0x27, bazi modullerde 0x3F (10_i2c_tarayici ile bak).
 *   Yazi gorunmuyorsa adaptordeki mavi potu (kontrast) cevir!
 *
 * PCF8574 bitleri -> LCD:  P0=RS  P1=RW  P2=EN  P3=Arka isik  P4..P7=D4..D7
 * LCD 4-bit modda surulur: her bayt iki parca (once ust 4 bit, sonra alt 4 bit) gider.
 */
#include <stdio.h>
#include "NuMicro.h"

#define LCD_ADR  0x27
#define RS       0x01
#define EN       0x04
#define ISIK     0x08

static void delay_ms(uint32_t ms) { while (ms--) CLK_SysTickDelay(1000); }

void SYS_Init(void)
{
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));
    CLK_EnableModuleClock(I2C0_MODULE);
    SystemCoreClockUpdate();

    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);
    /* Boardoza J9: PC8 = I2C0_SDA, PE13 = I2C0_SCL (kartta 10.2k pull-up var) */
    SYS->GPC_MFPH = (SYS->GPC_MFPH & ~SYS_GPC_MFPH_PC8MFP_Msk) | SYS_GPC_MFPH_PC8MFP_I2C0_SDA;
    SYS->GPE_MFPH = (SYS->GPE_MFPH & ~SYS_GPE_MFPH_PE13MFP_Msk) | SYS_GPE_MFPH_PE13MFP_I2C0_SCL;
    SYS_LockReg();
}

/* 4 bitlik parcayi EN darbesiyle LCD'ye kilitler */
static void lcd_nibble(uint8_t veri)
{
    I2C_WriteByte(I2C0, LCD_ADR, veri | EN | ISIK);
    CLK_SysTickDelay(1);
    I2C_WriteByte(I2C0, LCD_ADR, (veri & ~EN) | ISIK);
    CLK_SysTickDelay(50);
}

static void lcd_gonder(uint8_t b, uint8_t rs)
{
    lcd_nibble((b & 0xF0) | rs);             /* ust 4 bit */
    lcd_nibble(((b << 4) & 0xF0) | rs);      /* alt 4 bit */
}

static void lcd_komut(uint8_t k) { lcd_gonder(k, 0);  if (k <= 0x02) delay_ms(2); }
static void lcd_harf(char c)     { lcd_gonder((uint8_t)c, RS); }
static void lcd_yazi(const char *s) { while (*s) lcd_harf(*s++); }
static void lcd_git(uint8_t satir, uint8_t sutun) { lcd_komut(0x80 | (satir ? 0x40 : 0x00) | sutun); }

static void lcd_baslat(void)
{
    delay_ms(50);                            /* LCD acilis bekleme */
    lcd_nibble(0x30); delay_ms(5);           /* datasheet'teki sabit baslatma dizisi */
    lcd_nibble(0x30); delay_ms(1);
    lcd_nibble(0x30);
    lcd_nibble(0x20);                        /* 4-bit moda gec */
    lcd_komut(0x28);                         /* 4-bit, 2 satir, 5x8 font */
    lcd_komut(0x0C);                         /* ekran acik, imlec kapali */
    lcd_komut(0x06);                         /* yazdikca imlec saga kaysin */
    lcd_komut(0x01);                         /* ekrani temizle */
}

int main(void)
{
    uint32_t sn = 0;
    char buf[17];

    SYS_Init();
    UART_Open(UART0, 115200);
    I2C_Open(I2C0, 100000);

    printf("\n12_i2c_lcd basladi (adres 0x%02X)\n", LCD_ADR);

    lcd_baslat();
    lcd_git(0, 0);
    lcd_yazi("Merhaba M031");

    while (1)
    {
        snprintf(buf, sizeof(buf), "Sure: %5u sn", (unsigned)sn);
        lcd_git(1, 0);
        lcd_yazi(buf);
        printf("%s\n", buf);
        sn++;
        delay_ms(1000);
    }
}
