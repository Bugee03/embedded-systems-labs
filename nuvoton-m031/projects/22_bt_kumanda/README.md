# 22: Bluetooth Kumanda: telefonla tüm kartı yönet

## Bağlantı
| Parça | Bağlantı |
|---|---|
| MDBT42TV-AT | J11: modül **RX → PC7 (TXD)**, modül **TX → PC6 (RXD)**, **PD → GND**, 3V3, GND (9600 baud) |
| Phoenix24 halka | J15: PE6 → DIN, PE7 → CIN (+ besleme) |
| OLED 0.91" | J9 |

## Telefon
Android: **Serial Bluetooth Terminal** (Devices → Bluetooth LE) · iOS: **Bluefruit Connect** (UART)
Cihaz: **M032KG-01**

## Komutlar (Enter ile; büyük/küçük harf fark etmez)
| Komut | Ne yapar |
|---|---|
| `Merhaba hocam!` (herhangi bir yazı) | OLED'de **büyük harflerle kayar** + bip |
| `EFEKT 1` … `EFEKT 7` | Işık efekti: 1 gökkuşağı, 2 yıldız, 3 nefes, 4 polis, 5 yıldızlar, 6 doldur, 7 ateş. **Karttaki RGB LED ve LED5–8'de** görünür (halka takılıysa onda da). `HALKA` da aynı komut |
| `EFEKT 0` / `EFEKT OTO` | Kapat / 8 sn'de bir otomatik değiştir |
| `RENK 255 0 128` | Halka **ve** karttaki RGB LED aynı renk |
| `PARLAK 20` | Halka parlaklığı (1–31) |
| `MUZIK 1` … `MUZIK 4` / `MUZIK DUR` | Melodi çal / durdur |
| `BIP 1000` | Kısa ses |
| `LED5 ON` … `LED8 OFF` | Karttaki LED'ler (önce `EFEKT 0`, yoksa efekt LED'leri ezer) |
| `SICAKLIK?` `ISIK?` `POT?` `DURUM?` | Sensör değerleri telefona gelir |
| `CANLI ON` / `CANLI OFF` | Her saniye sıcaklık/ışık/potlar telefona akar |
| `YARDIM` | Komut listesi |

## Karttan telefona olaylar
SW3 = sonraki efekt · SW4 = önceki efekt · SW5 = müzik aç/kapa · SW2 = halka kapat · encoder çevir = parlaklık · encoder bas = OTO.
Her olay telefona `OLAY: ...` olarak gelir.

## Teknik
- UART4 alma **kesmesi** → ring buffer → main satırı çözer (Enter ya da 200 ms sessizlik ile satır biter).
- TIMER0 kesmesi: 1 ms zaman + müzik motoru. GPIO kesmeleri: butonlar ve encoder.
- Kırmızı RGB (PWM1 ch2) ve buzzer (ch3) aynı periyodu paylaşır, nota değişince kırmızı yeniden hesaplanır.
- Sıcaklık: NTC + Beta denklemi.
