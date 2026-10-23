# 13: I2C OLED Ekran (SSD1306, 128x64)

## Bağlantı
Kartın **J9 (I2C)** konnektörü: GND → GND, VCC → 3V3, **SCL → PE13**, **SDA → PC8**.
Adres **0x3C** (10_i2c_tarayici ile bulundu).

## Çalıştır
```bash
cd ~/nuvoton/projects/13_i2c_oled
make flash
make monitor      # "OLED hazir." yazmalı
```

## Beklenen sonuç
Üstte `POT3`, ortada değeri **büyük** yazıyla ve altında ince bir çubuk görünür. En altta diğer değerler (P4, P5, NTC, LDR, sayaç) **kayan yazı** olarak akar.
**POT4 (PB4) ekranın parlaklığını ayarlar.**

## Kod nasıl çalışıyor?
- OLED sürücüsü `../ortak/oled.c` dosyasındadır (büyük yazı, kayan yazı, parlaklık). Aynı sürücüyü 01–08 de kullanır.
- OLED'in kendi RAM'i 8 **sayfadan** oluşur, her sayfa 128 bayttır. Her bayt **dikey 8 pikseldir** (bit0 en üstteki piksel). 8 × 8 = 64 satır.
- Kod RAM'de aynı yapıda bir **frame buffer** (`fb[8][128]`, 1 KB) tutar. Önce buffer'a çizer (`piksel`, `yazi`, `cerceve`), sonra `oled_guncelle()` ile buffer'ın tamamını I2C üzerinden gönderir.
- I2C paketinin ilk baytı **kontrol baytıdır**: `0x00` gelen baytların komut olduğunu, `0x40` ise veri (piksel) olduğunu söyler.
- Başlatma dizisi: ekranı kapat → 64 satır → şarj pompası (`8D 14`) → sayfa modu → ayna ayarları → kontrast → ekranı aç (`AF`).
- Font 5×7 pikseldir, her karakter 6 piksel genişliğinde yer kaplar. `olcek = 2` verilirse her piksel 2×2 çizilir ve 12×16'lık büyük yazı elde edilir.
- `I2C_Open(I2C0, 400000)`: OLED 400 kHz (fast mode) destekler, ekran daha hızlı yenilenir.

## Kendin dene
- Encoder (07) ile bir menü yap: çevirince seçili satır ters renk olsun.
- NTC değerini sıcaklığa çevirip °C olarak göster.
- Ekranda hiçbir şey yoksa: adres 0x3D olabilir. 1.3" ekransa (SH1106) `OLED_KOLON_KAYMA`'yı 2 yap.

> Not: `12_i2c_lcd`, 16x2 karakter LCD (PCF8574, adres 0x27/0x3F) içindir. Sende bağlı olan ekran OLED olduğu için bu projeyi kullan.
