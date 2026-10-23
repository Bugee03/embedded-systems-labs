# 11: I2C: BNO055 ile Yönelim (Euler açıları)

## Bağlantı
Kartın **J9 (I2C)** konnektörü: 1 = GND, 2 = 3V3, 3 = **SCL (PE13)**, 4 = **SDA (PC8)**. BNO055'in VIN'i → J9.2 (3V3).
ADR pini GND'deyse adres 0x28, 3.3V'taysa 0x29.

## Çalıştır
```bash
cd ~/nuvoton/projects/11_i2c_bno055
make flash        # derle + karta yükle
make monitor      # seri port çıktısı (çıkış: Ctrl+A, K, Y)
```

## Beklenen sonuç
`CHIP_ID = 0xA0`, ardından sürekli: `Heading=123.4  Roll=-2.1  Pitch=5.0  Kalib(sys,gyr,acc,mag)=3333`
Sensörü döndürünce açılar değişir.

## Kod nasıl çalışıyor?
- BNO055 içinde ivmeölçer, jiroskop ve manyetometre birlikte bulunur. **NDOF** modunda bu üç sensörün verisini kendisi birleştirir (sensör füzyonu) ve doğrudan açı verir.
- Adımlar: ~650 ms açılış beklemesi → `CHIP_ID` (0x00) okunur, 0xA0 olmalı → `OPR_MODE` (0x3D) = CONFIG → NDOF.
- Euler verileri 0x1A adresinden itibaren 6 bayttır: heading, roll, pitch. Her biri 16-bit ve önce LSB gelir.
- **16 LSB = 1 derece.** Float kullanmadan `ham × 10 / 16` ile 0,1 derece hassasiyetinde yazdırılır.
- Kalibrasyon (0x35): her alan 0–3 arasıdır. 3 = tam kalibre. Sensörü 8 çizerek hareket ettirince manyetometre kalibre olur.
- **Dikkat:** STM32 HAL adresi `0x28<<1` olarak ister, Nuvoton ise doğrudan `0x28` ister.

## Kendin dene
- Roll belli bir açıyı geçince buzzer çalsın (eğim alarmı).
- Açıları 12'deki LCD'de göster.

> Pinler Boardoza M032KG şemasına göredir (`~/nuvoton/Boardoza_M032KG_sema.pdf`). Bir pinin hangi fonksiyonlara (MFP) bağlanabildiğini görmek için:
> `grep -o "SYS_GP._MFP._P..MFP_[A-Z0-9_]*" ~/nuvoton/M031BSP/Library/StdDriver/inc/sys.h | grep PA5`
