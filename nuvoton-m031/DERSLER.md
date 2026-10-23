# Nuvoton M031: Konu Projeleri

Her konu `projects/` altında ayrı, numaralı bir klasörde. Her klasörde şunlar var:
- `main.c`: bol yorumlu, tek başına çalışan kod
- `README.md`: bağlantı şeması, beklenen sonuç, kodun açıklaması ve kendin deneyebileceğin değişiklikler

Hepsi aynı şekilde çalıştırılır:
```bash
cd ~/nuvoton/projects/01_led
make flash        # derle + karta yükle
make monitor      # seri port (çıkış: Ctrl+A, K, Y)
```

## Konular (kolaydan zora)

| # | Klasör | Konu | Öğrettiği |
|---|---|---|---|
| 00 | `00_pin_bulucu` | Pin bulucu | Her pini sırayla yakar (LED arama) |
| 01 | `01_led` | LED yak/söndür | GPIO çıkış, `PF8 = 1`, delay |
| 02 | `02_buton` | 4 buton → 4 LED | GPIO giriş, iki ayrı GPIO kesmesi, debounce |
| 03 | `03_adc` | 3 pot + NTC + LDR | ADC çok kanallı tarama, mV hesabı |
| 04 | `04_timer` | Timer kesmesi | 1 ms zaman tabanı, non-blocking kod |
| 05 | `05_pwm_led` | RGB LED rengi (3 pot) | PWM duty, ADC → PWM |
| 06 | `06_buzzer` | Nota / melodi | PWM frekansı |
| 07 | `07_encoder` | Rotary encoder | A/B faz mantığı, GPIO kesmesi |
| 08 | `08_uart_bluetooth` | Bluetooth (MDBT42) | UART4 (J11), RX kesmesi, ring buffer |
| 09 | `09_spi_phoenix_led` | Adreslenebilir LED | SPI master, LED çerçevesi |
| 10 | `10_i2c_tarayici` | I2C tarayıcı | I2C adres/ACK, hata ayıklama |
| 11 | `11_i2c_bno055` | BNO055 yönelim | I2C register okuma/yazma |
| 12 | `12_i2c_lcd` | 16x2 LCD | I2C + PCF8574, 4-bit LCD sürme |
| 13 | `13_i2c_oled` | OLED 128x64 (SSD1306, 0x3C) | I2C, frame buffer, font, canlı sensör ekranı |
| 14 | `14_kesme` | Her buton bir iş yapar | Çoklu kesme kaynağı, öncelik, ISR kuralları |
| 15 | `15_refleks_oyunu` | Refleks oyunu | Durum makinesi, kesmeyle ms hassasiyetinde ölçüm |
| 16 | `16_muzik` | 4 buton = 4 melodi | Timer kesmesiyle müzik çalma, anında melodi değişimi |
| 18 | `18_led_halka_sov` | Phoenix24 LED halkası: 8 efektli şov | APA102 bit-bang, efekt motoru |
| 19 | `19_halka_tarayici` | Halka hangi pinde? (tarama) | Bağlantı hatası bulma |
| 20 | `20_halka_oyunu` | **Cyclone oyunu** (halkada ışığı hedefte durdur) | Kesmeyle adil zamanlama, oyun durumu |
| 17 | `17_m032_os` | **Mini oyun konsolu** (Dino, osiloskop, termometre, müzik, ışık şovu) | Hepsi bir arada: menü, kesmeler, PWM paylaşımı, NTC °C |

## Kart: Boardoza M032KG Educational Development Kit

- Çip: **M032KG8AE** (256K flash / 64K RAM), pyOCD hedefi `m032kg8ae`
- Şema: `~/nuvoton/Boardoza_M032KG_sema.pdf` (Boardoza GitHub'dan)
- Projelerin çoğu **karttaki hazır parçaları** kullanır, kablo gerekmez.

## Pin haritası (Boardoza şemasından)

> Her pinin register bitleri (MODE, DOUT/PIN, MFP alanı ve değeri, kesme): **`PINOUT.md`**

| Parça | Pin | Not | Proje |
|---|---|---|---|
| LED5 / LED6 / LED7 / LED8 | PF8 / PF7 / PF6 / PF14 | 1 = yanar | 01, 02, 04, 07, 08 |
| Buton SW2 / SW3 / SW4 / SW5 | PC12 / PC11 / PC10 / PB1 | dış pull-up, basınca 0 | 02 |
| Potansiyometreler | PB3 / PB4 / PB5 | ADC CH3 / CH4 / CH5 | 03, 05 |
| NTC (sıcaklık) | PB2 | ADC CH2 | 03 |
| LDR (ışık) | PB15 | ADC CH15 | 03 |
| RGB LED R / G / B | PC3 / PC5 / PC4 | PWM1 CH2 / CH0 / CH1 | 05 |
| Buzzer | PC9 | PWM1 CH3 | 06 |
| Encoder A / B / düğme | PB7 / PB6 / PA7 | dış pull-up | 07 |
| UART konnektörü J11 | RX = PC6, TX = PC7 | UART4 | 08 (Bluetooth) |
| SPI konnektörü J10 | MOSI = PA0, MISO = PA1, CLK = PA2, SS = PA3 | SPI0 | 09 |
| I2C konnektörü J9 | SDA = PC8, SCL = PE13 | I2C0, pull-up kartta | 10, 11, 12, 13 |
| DC motor (DRV8837) | IN1 = PA5, IN2 = PA4, nSLEEP = PD15 | | – |
| ESP-01M (Wi-Fi) | RX = PD0, TX = PD1 | JP1 ile güç | – |
| Seri port (VCOM) | RX = PB12, TX = PB13 | UART0 → Mac | hepsi |

> Bir pinin hangi fonksiyonu alabildiğini görmek için:
> `grep -o "SYS_GP._MFP._P..MFP_[A-Z0-9_]*" ~/nuvoton/M031BSP/Library/StdDriver/inc/sys.h | grep PC9`

## OLED ekran (J9) tüm projelerde

OLED (0.91", 128×32) takılıysa 01–08 ve 13 numaralı projeler ekrana **büyük yazıyla** yazar. Takılı değilse projeler aynen çalışır, ekran kısmı sessizce atlanır.

Ekran düzeni (128×64):
- Üstte **başlık** (12×16 px yazı)
- Ortada **ana değer** (18×24 px, 07'de 24×32 px)
- Altta **kayan yazı**: ekrana sığmayan açıklama ya da değerler sağdan sola akar

Sürücü `projects/ortak/oled.c` dosyasında. Kendi projende kullanmak için:
```make
EXTRA_SRCS = ../ortak/oled.c
CFLAGS    += -I../ortak
include ../../mk/m031.mk
```
```c
#include "oled.h"
oled_baslat();                         // I2C, pinler, ekran: hepsini kendisi yapar
oled_temizle();
yazi_orta(0, "BASLIK", 2);             // olcek 2 = 12x16 px
yazi_orta(18, "1234", 3);              // olcek 3 = 18x24 px (4 = 24x32)
kayan_yazi(48, "uzun bir yazi ...", 2);// sigmazsa kayar
oled_parlaklik(255);                   // 0..255
oled_guncelle();                       // ekrana gonder (~25 ms), kayan yazi bu sırada ilerler
```

## Her programda aynı olan kalıp

```
SYS_UnlockReg()
  ├─ HIRC 48 MHz aç → HCLK = HIRC
  ├─ 1) CLK_EnableModuleClock(XXX_MODULE) + CLK_SetModuleClock(...)
  ├─ 2) SYS->GPx_MFPx = ... (pin → çevre birimi)
SYS_LockReg()
3) XXX_Open(...)  /  NVIC_EnableIRQ(...)
```
Bir şey çalışmıyorsa büyük ihtimalle bu üç adımdan biri eksiktir.

## M031'e özgü tuzaklar (sınavda/mülakatta sorulabilir)

| Konu | Doğrusu |
|---|---|
| Dahili pull-up | `GPIO_SetPullCtl` **yok**. Onun yerine `GPIO_SetMode(..., GPIO_MODE_QUASI)` kullanılır ve pine 1 yazılır |
| PA/PB kesme fonksiyonu | `GPABGH_IRQHandler` (NVIC: `GPIO_PAPB_IRQn`) |
| PC–PF kesme fonksiyonu | `GPCDEF_IRQHandler` (NVIC: `GPIO_PCPDPEPF_IRQn`) |
| UART4/UART6 kesmesi | `UART46_IRQHandler` (ortak), UART1/3 için `UART13_IRQHandler` |
| UART kesmesini açmak | `UART_EnableInt` NVIC'i açmaz, ayrıca `NVIC_EnableIRQ(UART46_IRQn)` gerekir |
| I2C adresi | 7-bit, **kaydırmadan** (0x28). STM32 HAL'daki gibi `<<1` yapılmaz |
| Gecikme | `CLK_SysTickDelay(us)` en fazla ~349 ms bekler, daha uzunu için döngü kullanılır |
| Float | M0'da FPU yok, tam sayıyla hesap yapılır (`raw*3300/4095`) |
| ISR adı yanlışsa | Derleme hata **vermez**, kesme sessizce çalışmaz |

## Önerilen sıra (yarın)
1. `01_led` → kart, yükleme ve seri port çalışıyor mu? (**test edildi, çalışıyor**)
2. `02_buton` → `03_adc` → `04_timer`
3. `05_pwm_led` → `06_buzzer` → `07_encoder`
4. `08_uart_bluetooth` → `09_spi_phoenix_led`
5. `10_i2c_tarayici` (sensör ve LCD'yi tak, adresleri gör) → `11_i2c_bno055` → `12_i2c_lcd`
