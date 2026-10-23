# 01: LED Yak / Söndür (GPIO çıkış)

## Bağlantı
Harici bağlantı gerekmez. Karttaki **LED5–LED8** = **PF8, PF7, PF6, PF14** (1 = yanar).

## Çalıştır
```bash
cd ~/nuvoton/projects/01_led
make flash        # derle + karta yükle
make monitor      # seri port çıktısı (çıkış: Ctrl+A, K, Y)
```

## Beklenen sonuç
4 kırmızı LED 0,5 saniyede bir birlikte yanıp söner. Seri portta `LED: 1` / `LED: 0` satırları akar.

## Kod nasıl çalışıyor?
- `GPIO_SetMode(PF, BIT8|BIT7|BIT6|BIT14, GPIO_MODE_OUTPUT)` 4 pini birden çıkış yapar. GPIO için ayrıca clock ya da MFP ayarı gerekmez, çünkü pinler açılışta zaten GPIO'dur.
- `PF->DOUT |= maske` seçili pinleri 1 yapar, `&= ~maske` ise 0 yapar.
- Tek bir pin için Nuvoton'a özgü kısa yazım: `PF8 = 1;`, `PF8 ^= 1;` (toggle).
- `CLK_SysTickDelay(us)` en fazla yaklaşık 349 ms bekleyebilir (24-bit SysTick, 48 MHz). Bu yüzden `delay_ms()` beklemeyi 1 ms'lik parçalara böler.

## Kendin dene
- Kayan ışık yap: LED5 → LED6 → LED7 → LED8 → LED5 …
- Süreyi 100 ms yap ve farkı gör.

> Pinler Boardoza M032KG şemasına göredir (`~/nuvoton/Boardoza_M032KG_sema.pdf`). Bir pinin hangi fonksiyonlara (MFP) bağlanabildiğini görmek için:
> `grep -o "SYS_GP._MFP._P..MFP_[A-Z0-9_]*" ~/nuvoton/M031BSP/Library/StdDriver/inc/sys.h | grep PA5`
