# 15: Refleks Oyunu (reaksiyon süresi ölçümü)

## Çalıştır
```bash
cd ~/nuvoton/projects/15_refleks_oyunu && make flash && make monitor
```

## Nasıl oynanır
1. Herhangi bir butona bas (SW2–SW5): ekranda `BEKLE` yazar, RGB **kırmızı** yanar.
2. 1–4 saniye arasında rastgele bir anda **tüm LED'ler yanar, RGB yeşile döner ve bip çalar**, ekranda `BAS!` yazar.
3. Hemen herhangi bir butona bas.
4. Ekranda **reaksiyon süren (ms)**, en iyi skorun ve deneme sayın görünür. 250 ms altı `SUPER!`, 400 ms altı `IYI` sayılır.
5. Işık yanmadan basarsan: `ERKEN!` ve kalın bir ses.

## Kod nasıl çalışıyor?
- **Durum makinesi (state machine):** `BOSTA → BEKLE → YANDI → SONUC` (ya da `ERKEN`). Her durumda butonun anlamı farklıdır.
- **TIMER0 kesmesi (1 ms)** zamanı sayar. Rastgele bekleme süresi dolunca ışığı **kesmenin içinde** yakar. Böylece ışığın yandığı an milisaniye hassasiyetinde bilinir.
- **Buton kesmesi** basma anını (`g_basma_ani = g_ms`) **kesmenin içinde** kaydeder. Bu yüzden main o sırada ekranla meşgul olsa bile ölçüm bozulmaz.
- Reaksiyon = `g_basma_ani - g_yanma_ani`.
- **Rastgelelik:** butona basılan milisaniye tohuma karıştırılır (`g_tohum ^= g_ms`). İnsan zamanlaması tahmin edilemez olduğu için iyi bir rastgelelik kaynağıdır. Ardından basit bir LCG (`x = x·1103515245 + 12345`) kullanılır.
- Debounce kısa tutulur (~1,7 ms), uzun olsaydı ölçüme eklenirdi.
