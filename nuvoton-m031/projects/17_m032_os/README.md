# 17: M032 OS: Mini Oyun Konsolu

Kartın üzerindeki **her şeyi** tek bir projede kullanan, menülü bir mini işletim sistemi.
Ekran: **0.91" OLED 128×32** (J9). Ek kablo gerekmez.

```bash
cd ~/nuvoton/projects/17_m032_os && make flash
```

## Kontroller
| | |
|---|---|
| **Encoder çevir** | menüde gezin / uygulamada seç |
| **Encoder bas** | gir / tamam / dondur |
| **SW2** | her yerden **menüye dön** |
| **SW3 / SW4 / SW5** | DINO'da zıpla |

## Uygulamalar
| | Ne yapar | Göster |
|---|---|---|
| **Açılış** | "M032" sağdan kayarak gelir, RGB gökkuşağı, LED'ler dolar, jingle çalar | Kartı resetle |
| **DINO** | Chrome dinozor oyunu: kaktüslerden zıpla, hız artar, rekor tutulur | 400 puanda bir hız artar, LED'ler yanar |
| **OSILOSKOP** | POT3/POT4/POT5/LDR/NTC'nin **canlı grafiği** + voltaj, LED çubuğu | Potu hızlı çevir, LDR'yi elinle kapat. Encoder: kaynak, bas: dondur |
| **TERMOMETRE** | NTC ile **gerçek sıcaklık (°C)**, RGB mavi→yeşil→kırmızı | **NTC'ye parmağını bas**: sıcaklık yükselir, LED'ler dolar, renk ısınır |
| **MUZIK** | 10 melodi (Ode to Joy, **Tetris**, **Melankoli (Am)**: özgün indie-pop havası, **Nokia**, **Happy Birthday**, **Türk Marşı**, Für Elise, Twinkle, Jingle Bells, Benim Şarkım), ekranda **ekolayzer**, notaya göre RGB ve LED | Çalarken encoder'ı çevir: şarkı anında değişir |
| **ISIK SOVU** | RGB gökkuşağı, knight rider LED, ekranda iki sinüs dalgası | POT3 ile hızı değiştir |

## Hocaya anlatırken (teknik noktalar)
- **Her şey kesmelerle:** TIMER0 (1 ms) zaman tabanı + müzik motoru + bip süresi; GPIO kesmeleri encoder ve 4 buton için. `main` hiçbir butonu beklemez.
- **ISR → main iletişimi:** kesme tuşları bir bit maskesinde biriktirir (`g_tus`), main bunu `__disable_irq()` ile **atomik** okuyup sıfırlar. Hiçbir basış kaybolmaz.
- **Öncelik:** `NVIC_SetPriority(TMR0_IRQn, 0)`: zaman ve müzik en yüksek öncelikte, butonlar 1.
- **PWM paylaşımı:** kırmızı LED (PWM1 ch2) ve buzzer (ch3) **aynı periyodu paylaşır**. Buzzer frekansı değişince kırmızının duty'si yeni periyoda göre yeniden hesaplanır, bu yüzden renk bozulmaz.
- **BSP hatasının düzeltmesi:** `PWM_ConfigOutputChannel()` sıfır noktasındaki LOW ayarını siliyor. `ortak/pwm_duzelt.h` bunu düzeltiyor (yoksa RGB hep beyaz yanar).
- **NTC → °C:** gerilim bölücüden direnç, sonra **Beta denklemi**: `1/T = 1/T0 + ln(R/R0)/B` (B = 3950, R0 = 10k @ 25 °C).
- **Oyun fiziği:** sabit noktalı (×16) yerçekimi ve zıplama hızı. M0'da FPU olmadığı için oyunda float kullanılmıyor.
- **OLED:** 1 KB frame buffer, I2C 400 kHz, ~14 ms'de tam ekran → oyun 40 kare/sn.

## Kendi şarkını ekle
`main.c` içinde `sarkilar[]` listesine bir satır ekle (en altta `BENIM SARKIM` örneği var):
```c
{ "SARKI ADI", "E4 E4 F4 G4/4 R/2 D#5/1 C5/1" },
```
- Nota: `C D E F G A B` (Do Re Mi Fa Sol La Si), `R` = sus, `#` = diyez (`F#4`)
- Oktav: `4` orta oktav (`A4` = 440 Hz), `5` bir üst, `3` bir alt
- Süre: `/1` sekizlik, `/2` dörtlük (yazmazsan bu), `/4` ikilik, `/8` birlik
- Sonra `make flash`. Menü ve müzik uygulaması yeni şarkıyı kendiliğinden gösterir.
