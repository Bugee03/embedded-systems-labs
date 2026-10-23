# 06: Buzzer ile Nota / Melodi

## Bağlantı
Harici bağlantı gerekmez. Karttaki buzzer BZ1 = **PC9 (PWM1_CH3)**, NPN transistörle sürülür.

## Çalıştır
```bash
cd ~/nuvoton/projects/06_buzzer
make flash        # derle + karta yükle
make monitor      # seri port çıktısı (çıkış: Ctrl+A, K, Y)
```

## Beklenen sonuç
Do-Re-Mi gamı ve kısa bir melodi çalar, ardından tekrar başlar.

## Kod nasıl çalışıyor?
- LED'in tersine burada **duty sabittir (%50), FREKANS değişir**. Sesin perdesi frekansla belirlenir.
- `ton(f, ms)`: f Hz'de ms süre boyunca çalar. f = 0 ise susar.
- Notalar: Do 262, Re 294, Mi 330, Fa 349, Sol 392, La 440, Si 494, Do5 523 Hz.
- Ses kapatılırken `PWM_DisableOutput` çağrılır.
- M031/M032'de PWM kanalları çiftler hâlinde (0-1, 2-3, 4-5) aynı periyodu paylaşır. Buzzer (PWM1_CH3) ile RGB'nin kırmızısı (PWM1_CH2) aynı çifttedir, bu yüzden ikisini **aynı anda farklı frekansta** kullanamazsın.

## Kendin dene
- `melodi[]` dizisine kendi şarkını yaz.
- Potansiyometre ile frekansı değiştirerek bir "theremin" yap.

> Pinler Boardoza M032KG şemasına göredir (`~/nuvoton/Boardoza_M032KG_sema.pdf`). Bir pinin hangi fonksiyonlara (MFP) bağlanabildiğini görmek için:
> `grep -o "SYS_GP._MFP._P..MFP_[A-Z0-9_]*" ~/nuvoton/M031BSP/Library/StdDriver/inc/sys.h | grep PA5`
