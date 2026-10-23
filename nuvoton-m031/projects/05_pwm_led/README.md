# 05: PWM: RGB LED rengini 3 pot ile ayarla

## Bağlantı
Harici bağlantı gerekmez.
- RGB LED (transistörle sürülür, 1 = yanar): **Kırmızı = PC3 (PWM1_CH2), Yeşil = PC5 (PWM1_CH0), Mavi = PC4 (PWM1_CH1)**
- Potlar: **PB3 → kırmızı, PB4 → yeşil, PB5 → mavi**

## Çalıştır
```bash
cd ~/nuvoton/projects/05_pwm_led
make flash        # derle + karta yükle
make monitor      # seri port çıktısı (çıkış: Ctrl+A, K, Y)
```

## Beklenen sonuç
Her pot bir rengin parlaklığını ayarlar, potları çevirerek renk karıştırırsın. Seri portta `R=% 40  G=% 75  B=%  0` yazar.

## Kod nasıl çalışıyor?
- PWM sinyali çok hızlı açıp kapatır. **Duty** (açık kalma oranı) arttıkça LED daha parlak görünür.
- `PWM_ConfigOutputChannel(PWM1, kanal, 1000, duty)`: 1 kHz, %duty. Bu frekansta göz titremeyi fark etmez.
- Register mantığı: PERIOD (CNR) frekansı belirler, CMR duty'yi belirler, **D = CMR / (CNR+1)**.
- Sıralama yine aynı: `PWM1_MODULE` clock → PC3/PC4/PC5 MFP = `PWM1_CHx` → `EnableOutput` + `Start`.
- 3 renk × %0–100 karışımıyla binlerce renk elde edilir (additive renk karışımı).

## Kendin dene
- Potsuz, otomatik gökkuşağı geçişi yap.
- 03'teki LDR ile karanlıkta RGB'yi beyaz yak (gece lambası).

> Pinler Boardoza M032KG şemasına göredir (`~/nuvoton/Boardoza_M032KG_sema.pdf`). Bir pinin hangi fonksiyonlara (MFP) bağlanabildiğini görmek için:
> `grep -o "SYS_GP._MFP._P..MFP_[A-Z0-9_]*" ~/nuvoton/M031BSP/Library/StdDriver/inc/sys.h | grep PA5`
