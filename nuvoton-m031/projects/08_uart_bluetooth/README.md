# 08: UART: Bluetooth Modül + OLED

## Ne yapar?
Kart iki farklı yerden **komut** alır:
- **BT**: Bluetooth modülü (telefon), kartın **J11** konnektörüne bağlıdır ve **UART4** kullanır.
- **PC**: Mac'teki terminal (`make monitor`), **UART0** kullanır.

| Yazdığın satır (Enter ile) | Sonuç |
|---|---|
| `1` veya `ac` | LED5 yanar, cevap: `LED ACIK` |
| `0` veya `kapat` | LED5 söner, cevap: `LED KAPALI` |
| `?` | LED'in durumunu söyler |
| başka bir yazı | OLED'de mesaj olarak görünür |

OLED'de şunlar görünür: başlık, LED durumu (dolu kare = açık) ve son 3 mesaj. Her mesajın başında kaynağı yazar: `BT>` telefondan, `PC>` Mac'ten.
Mac'te yazdığın her satır telefona da gider, telefondan gelen her satır Mac'te de görünür.

## Modül olmadan dene (sadece Mac)
```bash
cd ~/nuvoton/projects/08_uart_bluetooth
make flash
make monitor
```
Terminalde `1` yazıp Enter'a bas: LED5 yanar, OLED'de `PC>1` ve `LED5: ACIK` görünür. Sonra `merhaba` yaz ve Enter'a bas: yazı OLED'e gelir. `0` ile LED söner.

## Bluetooth modülüyle (telefon)
Bağlantı (**TX → RX çapraz!**):
- J11.2 **TXD (PC7)** → modül **RX**
- J11.1 **RXD (PC6)** ← modül **TX**
- J11.3 **GND** → modül GND, modülün **3.3V**'u → J14 (3V3)

Telefonda BLE terminal uygulamasıyla (nRF Toolbox → UART, Serial Bluetooth Terminal vb.) modüle bağlan ve `1`, `0`, `?` gönder.
- Modülün baud hızı 9600 değilse `main.c` içindeki `BT_BAUD`'u değiştir.
- **Not:** Karttaki antenli modül **ESP-01M (Wi-Fi)**, Bluetooth değil. Bluetooth için harici bir modül (MDBT42 vb.) J11'e takılır.

## Kod nasıl çalışıyor?
- **UART4 (BT)** veri alımı **kesme** ile yapılır: `UART_EnableInt(UART4, RDAIEN)` + `NVIC_EnableIRQ(UART46_IRQn)`. `UART_EnableInt` NVIC'i kendisi açmaz. UART4 ve UART6 aynı kesmeyi paylaşır, fonksiyonun adı `UART46_IRQHandler`'dır.
- ISR'de iş yapılmaz: gelen bayt **ring buffer**'a yazılır, işlem main'de yapılır. ISR'ler kısa tutulmalıdır.
- **UART0 (PC)** main döngüsünde **polling** ile okunur. Karşılaştırma için iki yöntem de aynı projede var.
- Karakterler bir **satır tamponunda** toplanır (`karakter_al`). Enter (`\r` veya `\n`) gelince satır `komut_isle`'ye gider. Backspace de desteklenir.
- Komutlar büyük harfe çevrilerek karşılaştırılır (`toupper`), böylece `ac`, `AC` ve `Ac` aynı komuttur.
- OLED kodu **`../ortak/oled.c`** dosyasındadır. Makefile'daki `EXTRA_SRCS` satırı bu dosyayı derlemeye ekler. Aynı sürücüyü 13_i2c_oled de kullanır.
- UART4'ün clock ayarı: `CLK_CLKSEL3_UART4SEL_HIRC` + `CLK_CLKDIV4_UART4(1)`.

## Kendin dene
- `LED 1 3` gibi bir komut ekle (LED numarası + durum), LED5–LED8'i ayrı ayrı yönet.
- 03'teki ADC değerlerini `?` komutuna cevap olarak gönder.
- Gelen her mesajda buzzer kısa bir "bip" yapsın (06).
