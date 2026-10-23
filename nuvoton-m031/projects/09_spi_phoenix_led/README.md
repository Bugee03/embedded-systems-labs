# 09: SPI ile Adreslenebilir LED (Phoenix / APA102 tipi)

## Bağlantı
Kartın **J10 (SPI)** konnektörü: pin 4 = **MOSI (PA0) → LED DI**, pin 2 = **CLK (PA2) → LED CI**
LED şeridine 5V ve GND (J6). **GND'ler ortak olmalı.**

## Çalıştır
```bash
cd ~/nuvoton/projects/09_spi_phoenix_led
make flash        # derle + karta yükle
make monitor      # seri port çıktısı (çıkış: Ctrl+A, K, Y)
```

## Beklenen sonuç
Önce kırmızı bir ışık LED'ler boyunca kayar, sonra gökkuşağı efekti gelir.

## Kod nasıl çalışıyor?
- SPI senkron bir protokoldür: CLK hattındaki her vuruşta MOSI hattından bir bit gider. Bu LED'ler sadece veri alır, bu yüzden MISO ve SS kullanılmaz.
- `SPI_Open(SPI0, SPI_MASTER, SPI_MODE_0, 8, 2000000)`: master, mod 0 (CPOL=0, CPHA=0), 8 bit, 2 MHz.
- Çerçeve yapısı:
  - Başlangıç: 4 × `0x00`
  - Her LED için: `0xE0 | parlaklık(0-31)`, `B`, `G`, `R`
  - Bitiş: 4 × `0xFF`
- LED sayısını `N_LED` ile ayarla. Parlaklığı düşük tut, bu LED'ler çok parlak ve çok akım çeker.
- Renkler yanlış çıkıyorsa (kırmızı yerine mavi gibi) `led_gonder()` içindeki B/G/R sırasını değiştir.

## Kendin dene
- Potansiyometre ile yanan LED sayısını ayarla (bar göstergesi).
- Encoder ile rengi değiştir.

> Pinler Boardoza M032KG şemasına göredir (`~/nuvoton/Boardoza_M032KG_sema.pdf`). Bir pinin hangi fonksiyonlara (MFP) bağlanabildiğini görmek için:
> `grep -o "SYS_GP._MFP._P..MFP_[A-Z0-9_]*" ~/nuvoton/M031BSP/Library/StdDriver/inc/sys.h | grep PA5`
