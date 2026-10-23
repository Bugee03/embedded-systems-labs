/*
 * ortak/oled.h - SSD1306 128x64 I2C OLED surucusu (Boardoza J9: PC8 SDA, PE13 SCL)
 *
 * Kullanim (projenin Makefile'inda include satirindan ONCE):
 *   EXTRA_SRCS = ../ortak/oled.c
 *   CFLAGS    += -I../ortak
 *
 * Akis: oled_baslat() -> [oled_temizle() -> yazi()/cizim -> oled_guncelle()] dongude
 *
 * oled_baslat() her seyi kendisi yapar: I2C0 clock'u, PC8/PE13 pin ayari,
 * I2C_Open(400 kHz) ve ekran baslatma. OLED takili degilse diger fonksiyonlar
 * hicbir sey yapmaz, yani proje OLED'siz de calisir.
 *
 * Kucuk ekran icin:
 *   - olcek 2 (12x16 px) / 3 (18x24) / 4 (24x32) ile buyuk yazi
 *   - kayan_yazi(): ekrana sigmayan yazi saga-sola kayar (her oled_guncelle()'de ilerler)
 *   - oled_parlaklik(0..255): kontrast / parlaklik
 *
 * Ekran duzeni (128x64), projelerde ortak:
 *   y  0..15  baslik   (olcek 2)
 *   y 18..41  ana deger (olcek 3)
 *   y 48..63  kayan yazi (olcek 2)
 */
#ifndef OLED_H
#define OLED_H
#include <stdint.h>

#define OLED_ADR          0x3C     /* bazi moduller 0x3D */
#define OLED_KOLON_KAYMA  0        /* SSD1306 = 0, SH1106 (1.3") = 2 */

/* Ekran yuksekligi: Boardoza kitindeki 0.91" ekran = 32 (128x32). 0.96" ekran icin 64 yap.
 * 32 iken eski projeler (64 satira gore cizilmis) otomatik olarak dikeyde yariya sikistirilir.
 * Yeni projeler oled_yerli_32(1) cagirip dogrudan 128x32 koordinatlariyla cizebilir. */
#ifndef OLED_YUKSEKLIK
#define OLED_YUKSEKLIK    32
#endif

int  oled_baslat(void);                                  /* 1 = OK, 0 = cevap yok */
int  oled_var_mi(void);
void oled_yerli_32(int acik);                            /* 1: 128x32 koordinatlarla ciz (sikistirma yok) */
void oled_parlaklik(uint8_t deger);                      /* 0 = en sonuk, 255 = en parlak */
void oled_temizle(void);                                 /* sadece buffer'i siler */
void oled_guncelle(void);                                /* buffer -> ekran       */
void piksel(int x, int y, int renk);
void yazi(int x, int y, const char *s, int olcek);       /* olcek 1: 6x8, 2: 12x16 */
void cerceve(int x, int y, int w, int h);
void dolu_kutu(int x, int y, int w, int h);

int  yazi_genislik(const char *s, int olcek);            /* piksel */
void yazi_orta(int y, const char *s, int olcek);         /* yatayda ortala */

/* Kayan yazi: y satirinda, olcek boyutunda. Yazi ekrana sigiyorsa ortalanir,
 * sigmiyorsa her oled_guncelle() cagrisinda 'hiz' piksel sola kayar.
 * Ayni yaziyla tekrar cagirmak kaymayi sifirlamaz; metni her dongude guncelleyebilirsin. */
void kayan_yazi(int y, const char *s, int olcek);
void kayan_yazi_hiz(int piksel_adim);                    /* varsayilan 3 */

#endif
