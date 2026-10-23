# 16: Müzik Kutusu: 4 buton = 4 melodi

## Çalıştır
```bash
cd ~/nuvoton/projects/16_muzik && make flash && make monitor
```

| Kontrol | Ne yapar |
|---|---|
| **SW2** | 1. Beethoven: Ode to Joy (Neşe Marşı) |
| **SW3** | 2. Twinkle Twinkle Little Star (Daha Dün Annemizin) |
| **SW4** | 3. Jingle Bells |
| **SW5** | 4. Beethoven: Für Elise |
| Encoder çevir | tempo (hızlı / yavaş) |
| Encoder bas | dur |

Butona bastığın an müzik **notanın ortasında bile** hemen değişir. LED5–LED8 hangi melodinin çaldığını gösterir, RGB LED notaya göre renk değiştirir. OLED'de melodi adı, çalan nota (büyük) ve tempo görünür.

## Kod nasıl çalışıyor?
- Her melodi bir **dizidir**: `{frekans, vuruş}`. Vuruş 1 sekizlik, 2 dörtlük, 4 ikilik notadır. `R` (0 Hz) sus demektir.
- **Müziği TIMER0 kesmesi çalar (1 ms'de bir):**
  - `g_kalan` (notanın kalan süresi) 0 olunca sıradaki notanın frekansını PWM'e yazar: `PWM_ConfigOutputChannel(PWM1, 3, frekans, 50)`.
  - Nota bitmeden 25 ms önce sesi keser, böylece aynı nota iki kez çalınınca ayrı duyulur.
  - Melodi bitince başa döner.
- **Buton kesmesi** sadece `g_secim`'i değiştirir ve `g_index = 0`, `g_kalan = 0` yapar. Timer bir sonraki milisaniyede yeni melodinin ilk notasını başlatır. İşte bu yüzden değişim anında olur.
- `main` hiç beklemez, sadece ekranı günceller. Uzun `delay` ile çalan bir koddan (06_buzzer) farkı budur: orada butona basılsa bile nota bitene kadar tepki verilemezdi.
- Tempo: bir sekizliğin süresi `g_sekizlik_ms`. Encoder ±15 ms değiştirir. Ekrandaki tempo, dakikadaki dörtlük nota sayısıdır.

## Kendin dene
- Kendi şarkını ekle: `m5[]` dizisi yaz ve encoder düğmesine bağla.
- Melodiyi ilerletmek yerine butonlarla piyano yap: basılı tutulan butonun notası çalsın (`GPIO_INT_BOTH_EDGE`).
