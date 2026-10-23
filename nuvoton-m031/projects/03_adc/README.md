# 03: ADC: 3 Potansiyometre + NTC + LDR

## Bağlantı
Harici bağlantı gerekmez. Karttaki analog kaynaklar:
- Potansiyometreler: **PB3 (CH3), PB4 (CH4), PB5 (CH5)**
- NTC (sıcaklık): **PB2 (CH2)**. VDD → 10.2k → PB2 → NTC 10k → GND
- LDR (ışık): **PB15 (CH15)**. VDD → 10.2k → PB15 → LDR → GND

## Çalıştır
```bash
cd ~/nuvoton/projects/03_adc
make flash        # derle + karta yükle
make monitor      # seri port çıktısı (çıkış: Ctrl+A, K, Y)
```

## Beklenen sonuç
`POT3=2069 POT4=2112 POT5=1965 | NTC=1929 (1554 mV) | LDR=1344 (1083 mV)` gibi satırlar akar (bu değerler kartta ölçüldü).
Potu çevir → POT değişir. LDR'yi elinle kapat → LDR **yükselir**. NTC'ye parmağını bas → NTC **düşer**.

## Kod nasıl çalışıyor?
- Çevre birimi kuralı: **clock enable → pin MFP → Open**.
  1. `CLK_EnableModuleClock(ADC_MODULE)` + `CLK_SetModuleClock(...)`
  2. Pin ayarı: `GPIO_MODE_INPUT` → MFP = `ADC0_CHx` → `GPIO_DISABLE_DIGITAL_PATH` (dijital girişi kapatır, kaçak akımı önler)
  3. `ADC_POWER_ON` + `ADC_Open`
- `ADC_ADMD_SINGLE_CYCLE`: tek bir START ile seçili 5 kanalın hepsi sırayla okunur. Sonuç `ADC_GET_CONVERSION_DATA(ADC, kanal_no)` ile alınır (ör. LDR için 15).
- 12-bit ADC: 0 → 0 V, 4095 → 3.3 V. **mV = ham × 3300 / 4095**
- Cortex-M0'da FPU yoktur. Float yerine tam sayıyla hesaplanır.
- NTC ve LDR **gerilim bölücü** içindedir ve alt taraftadırlar: dirençleri düşünce (ısınınca ya da ışık artınca) pindeki gerilim **düşer**.
- NTC oda sıcaklığında ~10k olduğu için gerilim ~VDD/2 (~1,55 V) çıkar.

## Kendin dene
- LDR değeri belli bir eşiği geçince (karanlık) LED5'i yak.
- NTC değerinden yaklaşık sıcaklık hesapla (Beta formülü ya da tablo).

> Pinler Boardoza M032KG şemasına göredir (`~/nuvoton/Boardoza_M032KG_sema.pdf`). Bir pinin hangi fonksiyonlara (MFP) bağlanabildiğini görmek için:
> `grep -o "SYS_GP._MFP._P..MFP_[A-Z0-9_]*" ~/nuvoton/M031BSP/Library/StdDriver/inc/sys.h | grep PA5`
