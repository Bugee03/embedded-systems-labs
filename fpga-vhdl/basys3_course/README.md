# FPGA → Radar Kursu (Basys 3 · Verilog · Offline-first)

> Hedef: Sıfırdan başlayıp, **internet ve yapay zekâ olmadan**, sadece resmî dokümanlar
> (AMD/Xilinx UG/PG/DS) ve kendi el notlarınla bir **radar sinyal işleme zincirini**
> FPGA (Field-Programmable Gate Array — программируемая логическая матрица) üzerinde
> tasarlayabilecek seviyeye gelmek.

## Donanım
| Özellik | Basys 3 (Digilent) |
|---|---|
| FPGA | **XC7A35T-1CPG236C** (Artix-7 ailesi, speed grade −1) |
| Logic | 5 200 slice → 20 800 LUT (6-input), 41 600 FF |
| Block RAM | 50 × 36 Kb = 1 800 Kb |
| DSP48E1 | 90 adet (25×18 çarpıcı + 48-bit akümülatör) |
| Saat | 100 MHz osilatör, pin **W5** |
| Clock management | 5 × CMT (MMCM + PLL) |
| Analog | **XADC** — 12-bit, 1 MSPS, JXADC Pmod'dan dış giriş (radar deneyleri için altın!) |
| I/O | 16 switch, 16 LED, 5 buton, 4 hane 7-segment, VGA, USB-UART, 3 Pmod + 1 XADC Pmod |

## Klasör yapısı
```
fpga/
├── README.md              ← bu dosya (kurallar + yol haritası özeti)
├── SYLLABUS.md            ← 30 derslik müfredat (her ders → radar bağlantısı)
├── GLOSSARY.md            ← kısaltmalar: EN tam adı + RU anlamı + TR
├── 00_setup/              ← araç kurulumu + offline hazırlık + smoke test
├── docs/
│   ├── DOCS_CHECKLIST.md  ← indirmen gereken resmî PDF'ler (ID + neden + hangi bölüm)
│   ├── BOOKS.md           ← kitaplar (öncelik sırasıyla)
│   ├── READING_MAP.md     ← hangi derste hangi kitabın hangi sayfası
│   ├── basys3/            ← Basys-3-Master.xdc + pin tablosu
│   └── pdf/               ← indirdiğin UG/PG PDF'leri buraya
├── cheatsheet/            ← terminal + Verilog + Vivado Tcl kopya kâğıtları
├── lessons/               ← L01, L02 ... (teori → matematik → örnek → alıştırma)
├── labs/                  ← senin elle yazdığın kodlar (her lab ayrı klasör)
├── my_library/            ← KENDİ yazdığın, test ettiğin modüller (işte yanında taşıyacağın kütüphane)
└── notebook/              ← dijital not; asıl defter KAĞIT olmalı
```

## Çalışma kuralları ("şirket modu")
1. **Copy-paste yok.** Ders dosyasındaki örnek kodu *okursun*, sonra kapatıp **kendin yazarsın**.
2. **Önce doküman.** Her soruda önce `docs/pdf/` içindeki ilgili UG'ye bakarsın. Her derste
   "Doc Hunt" (doküman avı) soruları olacak: cevabı PDF'te bulup sayfa numarasıyla yazacaksın.
3. **Önce simülasyon, sonra kart.** Testbench'te geçmeyen tasarım karta yüklenmez.
4. **Kendi kütüphanen.** Çalışan ve test edilmiş her modül `my_library/`'ye girer
   (ör. `debounce.v`, `uart_tx.v`, `fir.v`). Gerçek işte seni kurtaracak şey bu.
5. **Kağıt defter.** Blok diyagramı, timing diyagramı, bit genişliği hesabı — hep elle.
6. **Çözümler ayrı.** Alıştırma çözümlerini ben ancak sen denedikten sonra veririm.

## Ders formatı
Her ders dosyası (`lessons/Lxx_*.md`):
1. **Teori** — kavram, donanımda neye dönüşüyor
2. **Matematik** — formüller (sembol açıklaması + sayısal örnek)
3. **Örnek** — okunacak kod + beklenen sentez sonucu
4. **Terminal** — hangi komutları hangi sırayla yazacağın
5. **Alıştırmalar** — elle yazılacak (kolay → zor)
6. **Doc Hunt** — PDF'te bulunacak cevaplar
7. **Radar bağlantısı** — bu bilgi radar zincirinin neresinde kullanılır
8. **Kontrol listesi** — geçmeden önce bilmen gerekenler

## Başlangıç sırası
1. `00_setup/TOOLS_INSTALL.md` → araçları kur
2. `00_setup/smoke_test/` → `./run.sh` ile simülatörün çalıştığını doğrula
3. `docs/DOCS_CHECKLIST.md` → **Tier 1** PDF'leri indir (internet varken!)
4. Hazır olunca bana "L01 başlayalım" yaz.
