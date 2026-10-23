# 14: Kesmeler (Interrupt): her buton bir iş yapar

## Çalıştır
```bash
cd ~/nuvoton/projects/14_kesme && make flash && make monitor
```
Kablo gerekmez, karttaki parçalar kullanılır. OLED (J9) takılıysa son olay ekranda görünür.

| Kaynak | Kesme fonksiyonu | Ne yapar |
|---|---|---|
| TIMER0 (1 kHz) | `TMR0_IRQHandler` | zaman sayacı + kayan ışık adımı + bip süresini bitirir |
| SW2 (PC12) | `GPCDEF_IRQHandler` | kayan ışığı başlat / durdur |
| SW3 (PC11) | `GPCDEF_IRQHandler` | bip |
| SW4 (PC10) | `GPCDEF_IRQHandler` | RGB rengini değiştir (8 renk) |
| SW5 (PB1) | `GPABGH_IRQHandler` | sayaçları sıfırla |
| Encoder A (PB7) | `GPABGH_IRQHandler` | kayan ışığın hızı (20–1000 ms) |
| Encoder düğme (PA7) | `GPABGH_IRQHandler` | kayan ışığın yönü |

OLED'de son olay büyük yazıyla görünür. Altta her kesme kaynağının kaç kez çalıştığı kayan yazıyla akar.

## Kesme nedir? (kısaca)
- Normalde işlemci `main` içindeki kodu sırayla çalıştırır. Bir **kesme** geldiğinde (buton, timer, UART...) işlemci bulunduğu yerde durur, ilgili `..._IRQHandler` fonksiyonunu çalıştırır, sonra kaldığı yerden devam eder.
- Bu projede `main` butonları **hiç okumaz**, sadece ekranı günceller. Butonlar yine de anında tepki verir.

## Kurallar (sınavda sorulur)
1. **ISR kısa olmalı.** Bekleme (`delay`) ve `printf` ISR'ye konmaz. Uzun işler için ISR bir bayrak (flag) koyar, işi main yapar (örnek: `g_bip_istek`).
2. **Bayrak temizlenmeli:** `GPIO_CLR_INT_FLAG`, `TIMER_ClearIntFlag`. Temizlenmezse ISR durmadan tekrar çalışır.
3. **ISR ile main arasında paylaşılan değişkenler `volatile` olmalı.** Yoksa derleyici değişkeni bir kez okuyup değişmediğini varsayabilir.
4. **Fonksiyon adı startup dosyasındakiyle birebir aynı olmalı** (`GPCDEF_IRQHandler`, `GPABGH_IRQHandler`, `TMR0_IRQHandler`). Yanlış yazarsan derleme hata vermez ama kesme çalışmaz.
5. **Aynı hattı paylaşan pinler:** PA/PB tek kesmeye, PC–PF başka bir kesmeye düşer. ISR içinde `GPIO_GET_INT_FLAG` ile hangi pinin tetiklediğine bakılır.
6. **Öncelik:** `NVIC_SetPriority(TMR0_IRQn, 0)`. M0'da öncelik 0–3 arasıdır, 0 en yüksektir. Zaman tabanı en yüksek öncelikte olmalı ki zaman kaymasın.

## Kurulum sırası (her GPIO kesmesi için)
```c
GPIO_SetMode(PC, BIT12, GPIO_MODE_INPUT);      // 1) giriş
GPIO_ENABLE_DEBOUNCE(PC, BIT12);               // 2) debounce (isteğe bağlı)
GPIO_EnableInt(PC, 12, GPIO_INT_FALLING);      // 3) hangi kenarda
NVIC_EnableIRQ(GPIO_PCPDPEPF_IRQn);            // 4) NVIC'te aç
void GPCDEF_IRQHandler(void) { ... }           // 5) ISR
```
