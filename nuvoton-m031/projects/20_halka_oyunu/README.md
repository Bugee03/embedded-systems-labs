# 20: Halka Oyunu "Cyclone"

Phoenix24 halkası: **J15 pin 2 (PE6) → DIN**, **J15 pin 1 (PE7) → CIN**, VCC ve GND (tercihen J6'daki 5V).

## Oyun
- Halkada **kırmızı ışık** döner, bir LED **yeşil** (hedef). Işık hedefin üstündeyken **sarı** yanar.
- Işık hedefteyken **herhangi bir butona** bas (SW2–SW5 ya da encoder'a bas).
- **Tam isabet:** +10 × seviye puan, seviye atlar, ışık **hızlanır ve yön değiştirir**, gökkuşağı + melodi.
- **1 LED yakın:** +3 puan. **Iska:** 1 can gider (canlar LED5–LED7'de ve OLED'de).
- 3 can bitince **OYUN BİTTİ**, rekor saklanır. Bir butonla yeniden başlar.
- POT4 (PB4) = halka parlaklığı.

## Teknik
- Işığın konumunu **TIMER0 kesmesi** ilerletir (`g_poz`).
- Butona basıldığı **an** konum **buton kesmesinin içinde** kaydedilir (`g_durdu_poz = g_poz`). Bu yüzden main o sırada halkayı çiziyor olsa bile ölçüm **adil** ve milisaniye hassasiyetindedir.
- Halka APA102 tipindedir, PE6/PE7 ile yazılımla (bit-bang) sürülür: `[0x00×4] [0xE0|parlaklık, B, G, R]×24 [0xFF×4]`.
