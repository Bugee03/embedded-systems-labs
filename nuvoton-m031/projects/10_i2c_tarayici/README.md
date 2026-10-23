# 10: I2C Tarayıcı

## Bağlantı
Kartın **J9 (I2C)** konnektörü: 1 = GND, 2 = 3V3, 3 = **SCL (PE13)**, 4 = **SDA (PC8)**.
Kartta 10.2k pull-up dirençleri zaten var.

## Çalıştır
```bash
cd ~/nuvoton/projects/10_i2c_tarayici
make flash        # derle + karta yükle
make monitor      # seri port çıktısı (çıkış: Ctrl+A, K, Y)
```

## Beklenen sonuç
`Cihaz bulundu: 0x28` gibi bir liste yazar. BNO055 = **0x28/0x29**, LCD = **0x27/0x3F**.

## Kod nasıl çalışıyor?
- **Sensör ya da LCD çalışmıyorsa önce bunu çalıştır.** Cihaz bu listede yoksa sorun koddan değil kablolamadan kaynaklanıyordur.
- Her adres için START gönderilir, ardından `adres<<1 | 0` (yazma) gönderilir. Durum kodu **0x18** ise cihaz ACK vermiştir, yani o adreste bir cihaz vardır.
- I2C iki hatlıdır (SDA veri, SCL saat), hatlar open-drain çalışır ve **pull-up direnci şarttır**.
- 7-bit adres + 1 R/W biti = 8 bit. Nuvoton'un `I2C_ReadByteOneReg` gibi fonksiyonları adresi **kaydırmadan** alır (0x28). Kaydırma işlemini kendileri yapar.

## Kendin dene
- Hiç cihaz bulunamıyorsa SDA ile SCL'yi ters bağlamış olabilirsin, yer değiştir.

> Pinler Boardoza M032KG şemasına göredir (`~/nuvoton/Boardoza_M032KG_sema.pdf`). Bir pinin hangi fonksiyonlara (MFP) bağlanabildiğini görmek için:
> `grep -o "SYS_GP._MFP._P..MFP_[A-Z0-9_]*" ~/nuvoton/M031BSP/Library/StdDriver/inc/sys.h | grep PA5`
