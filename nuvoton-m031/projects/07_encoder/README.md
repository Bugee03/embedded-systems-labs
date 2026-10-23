# 07: Rotary Encoder (GPIO kesmesi)

## Bağlantı
Harici bağlantı gerekmez. Karttaki encoder: **A = PB7, B = PB6, düğme = PA7** (dış pull-up ve RC filtre kartta var).
LED5–LED8 (PF8, PF7, PF6, PF14) pozisyonu gösterir.

## Çalıştır
```bash
cd ~/nuvoton/projects/07_encoder
make flash        # derle + karta yükle
make monitor      # seri port çıktısı (çıkış: Ctrl+A, K, Y)
```

## Beklenen sonuç
Çevirdikçe `Pozisyon: N` yazar ve LED5 → LED8 arasında yanan LED kayar. Encoder'ın düğmesine basınca sıfırlanır.

## Kod nasıl çalışıyor?
- Encoder'ın A ve B kanalları 90° faz farkıyla kare dalga üretir.
- A kanalı 1 → 0 düştüğü anda B'nin seviyesine bakılır: **B = 1 ise +1, B = 0 ise −1**.
- M031/M032'de STM32'deki gibi donanımsal encoder modu (QEI) yoktur, bu yüzden GPIO kesmesi kullanılır.
- A (PB7) ve düğme (PA7) aynı kesmeye düşer: `GPABGH_IRQHandler`. Hangisinin tetiklediği `GPIO_GET_INT_FLAG` ile anlaşılır.
- Debounce kısa tutulur (~0,4 ms). Uzun olursa hızlı çevirmede adım kaçırılır.
- Yön ters çıkıyorsa `++`/`--` yerlerini değiştir.

## Kendin dene
- Encoder ile 05'teki LED parlaklığını ayarla (her adım ±%5).
- Encoder ile buzzer'ın frekansını değiştir.

> Pinler Boardoza M032KG şemasına göredir (`~/nuvoton/Boardoza_M032KG_sema.pdf`). Bir pinin hangi fonksiyonlara (MFP) bağlanabildiğini görmek için:
> `grep -o "SYS_GP._MFP._P..MFP_[A-Z0-9_]*" ~/nuvoton/M031BSP/Library/StdDriver/inc/sys.h | grep PA5`
