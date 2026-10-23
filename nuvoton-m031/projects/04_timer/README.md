# 04: Timer Kesmesi (1 ms zaman tabanı, non-blocking)

## Bağlantı
Harici bağlantı gerekmez. LED5 = **PF8** kullanılır.

## Çalıştır
```bash
cd ~/nuvoton/projects/04_timer
make flash        # derle + karta yükle
make monitor      # seri port çıktısı (çıkış: Ctrl+A, K, Y)
```

## Beklenen sonuç
LED 500 ms'de bir yanıp söner. Seri portta her saniye `Gecen sure: N s` yazar.

## Kod nasıl çalışıyor?
- TIMER0 her 1 ms'de bir kesme üretir ve `g_ms` sayacını artırır (Arduino'daki `millis()` gibi).
- Formül: **f_kesme = f_clk / ((PSC+1) × CMP)**. 48 MHz / (1 × 48000) = 1 kHz. `TIMER_Open(TIMER0, TIMER_PERIODIC_MODE, 1000)` PSC ve CMP değerlerini kendisi hesaplar.
- M031 timer'ı 24-bit sayaç ve 8-bit prescaler'dan oluşur.
- main içinde `delay` kullanılmaz. `simdi - son >= 500` kontrolüyle birden fazla iş aynı anda yürütülür (**non-blocking**). `delay` kullanılsaydı işlemci beklerken başka bir iş yapamazdı.
- `simdi - son` yazımı, sayaç taşsa bile (overflow) doğru sonuç verir.

## Kendin dene
- İkinci bir LED'i 300 ms'de bir yak. Çakışmadan ikisinin birlikte çalıştığını gör.
- Timer'ı 2 Hz'e ayarla ve LED'i doğrudan ISR'nin içinde toggle et.

> Pinler Boardoza M032KG şemasına göredir (`~/nuvoton/Boardoza_M032KG_sema.pdf`). Bir pinin hangi fonksiyonlara (MFP) bağlanabildiğini görmek için:
> `grep -o "SYS_GP._MFP._P..MFP_[A-Z0-9_]*" ~/nuvoton/M031BSP/Library/StdDriver/inc/sys.h | grep PA5`
