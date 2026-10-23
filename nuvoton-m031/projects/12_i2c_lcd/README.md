# 12: I2C: 16x2 LCD (PCF8574 adaptörlü)

## Bağlantı
Kartın **J9 (I2C)** konnektörü: 1 = GND, 2 = 3V3, 3 = **SCL (PE13)**, 4 = **SDA (PC8)**. LCD'nin VCC'si: çoğu LCD 5V ister, bu durumda J6'daki 5V'a bağla.

## Çalıştır
```bash
cd ~/nuvoton/projects/12_i2c_lcd
make flash        # derle + karta yükle
make monitor      # seri port çıktısı (çıkış: Ctrl+A, K, Y)
```

## Beklenen sonuç
1. satırda `Merhaba M031`, 2. satırda `Sure: N sn` yazar ve her saniye güncellenir.

## Kod nasıl çalışıyor?
- LCD'nin arkasındaki PCF8574 bir I2C → 8-bit port dönüştürücüdür. Bitleri şöyle bağlıdır: P0 = RS, P1 = RW, P2 = EN, P3 = arka ışık, P4–P7 = LCD D4–D7.
- LCD **4-bit modda** sürülür: her bayt iki parça hâlinde gider (önce üst 4 bit, sonra alt 4 bit). Her parça **EN darbesiyle** (1 → 0) LCD'ye kilitlenir.
- RS = 0 ise komut, RS = 1 ise karakter gönderilir.
- Başlatma dizisi datasheet'te sabittir: 0x30 ×3 → 0x20 (4-bit mod) → 0x28 → 0x0C → 0x06 → 0x01.
- Satır adresleri: 1. satır `0x80`, 2. satır `0xC0` (`0x80 | 0x40`).
- **Ekran yanıyor ama yazı yoksa: adaptördeki mavi kontrast potunu çevir!** Bu en sık karşılaşılan sorundur.
- Hiçbir şey olmuyorsa 10_i2c_tarayici ile adresi kontrol et (0x27 ya da 0x3F) ve `LCD_ADR`'ı ona göre değiştir.

## Kendin dene
- 03'teki ADC değerini LCD'de göster.
- 11'deki BNO055 açılarını LCD'de göster.

> Pinler Boardoza M032KG şemasına göredir (`~/nuvoton/Boardoza_M032KG_sema.pdf`). Bir pinin hangi fonksiyonlara (MFP) bağlanabildiğini görmek için:
> `grep -o "SYS_GP._MFP._P..MFP_[A-Z0-9_]*" ~/nuvoton/M031BSP/Library/StdDriver/inc/sys.h | grep PA5`
