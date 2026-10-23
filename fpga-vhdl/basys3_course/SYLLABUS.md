# Müfredat — FPGA'dan Radar Sinyal İşlemeye (30 ders)

Her ders ≈ 1 hafta içinde 2–3 oturum. Toplam ≈ 6–7 ay (sınav dönemlerinde yavaşlar, sorun değil).
Dil: **Verilog-2001** (tasarım) → ilerde **SystemVerilog** (testbench). Türk savunma sanayisinde
**VHDL** de çok yaygın → Faz V'te VHDL okuma/yazma modülü var.

---

## Faz I — Temeller (L01–L08): "Donanım düşünmek"
| # | Ders | İçerik | Kart çıktısı | Radar bağlantısı |
|---|------|--------|--------------|------------------|
| L01 | Dijital mantık hızlı tekrar | Boolean, 2's complement, mux, decoder, adder, gecikme | — (kağıt + sim) | Tüm sinyal örnekleri signed 2's complement |
| L02 | FPGA'nın içi | LUT, FF, CLB/slice, routing, BRAM, DSP48, IOB, saat ağı | — | Kaynak bütçesi: FFT/FIR kaç DSP/BRAM yer? |
| L03 | Verilog: kombinasyonel | module, port, wire, assign, always @(*), vektörler, latch tuzağı | switch → LED mantık | Karşılaştırıcı (CFAR eşik) |
| L04 | Simülasyon & testbench | iverilog/vvp, $display, $dumpvars, self-checking TB, GTKWave | — | Her DSP bloğu Python "golden model"e karşı test edilir |
| L05 | Vivado akışı | Proje vs Tcl non-project akış, XDC, synth → impl → bitstream → JTAG | ilk kart tasarımı | Aynı akış tüm radar tasarımlarında |
| L06 | Sıralı mantık | FF, reset (sync/async), blocking vs non-blocking, register, shift register | LED shift | Gecikme hattı (delay line), MTI |
| L07 | Sayaçlar & zamanlama | clock enable (clock bölme DEĞİL), prescaler, PWM, debounce | 1 Hz blink, PWM LED | **PRF** (Pulse Repetition Frequency) zamanlayıcı |
| L08 | FSM | Moore/Mealy, state encoding, 3-always stili | trafik lambası / buton FSM | Radar kontrol durumları: TX → listen → process |

## Faz II — Profesyonel tasarım pratiği (L09–L14)
| # | Ders | İçerik | Kart çıktısı | Radar bağlantısı |
|---|------|--------|--------------|------------------|
| L09 | Parametrik & hiyerarşik tasarım | parameter, localparam, generate, $clog2, instantiation | 7-seg multiplex sürücü | Genişliği ayarlanabilir FIR/CFAR |
| L10 | UART | baud hesap, TX/RX FSM, oversampling | PC ↔ Basys3 terminal | Sonuçları Python'a gönderme (A-scope çizimi) |
| L11 | Timing (STA) | setup/hold, slack, WNS/TNS, critical path, pipelining | timing raporu okuma | 100 MHz'de FIR'ın timing'i kapatması |
| L12 | CDC & metastabilite | 2-FF sync, pulse sync, handshake, async FIFO, MTBF | buton → farklı clock domain | ADC clock ↔ işlem clock'u |
| L13 | Bellek | BRAM çıkarımı (inference), ROM ($readmemh), sync FIFO | ROM'dan pattern oynatma | Range-Doppler için **corner turn** belleği |
| L14 | Clocking & debug | MMCM/Clocking Wizard, ILA (Integrated Logic Analyzer) | 25/200 MHz üretme, ILA ile iç sinyal | Örnekleme saati üretimi, sahada debug |

## Faz III — FPGA üzerinde DSP (L15–L22)
| # | Ders | İçerik | Kart çıktısı | Radar bağlantısı |
|---|------|--------|--------------|------------------|
| L15 | Fixed-point aritmetik | Qm.n, bit büyümesi, yuvarlama, saturation, Python ile nicemleme hatası | — | Tüm zincirin bit bütçesi |
| L16 | DSP48E1 | MAC (multiply-accumulate), pipeline register'ları, inference kuralları | MAC ünitesi | FIR, korelasyon, FFT kelebeği |
| L17 | NCO / DDS | faz akümülatörü, sin LUT, SFDR, frekans çözünürlüğü | XADC/PWM ile ton | **LFM chirp** üretimi |
| L18 | FIR filtre | direct / transposed / simetrik, katsayı nicemleme | UART'tan veri → filtre → UART | Pulse compression = FIR (matched filter) |
| L19 | CIC & decimation | integrator-comb, bit büyümesi, droop | — | Yüksek hızlı ADC → düşük hıza indirme |
| L20 | CORDIC | vectoring/rotation mod, magnitude & phase | — | \|I+jQ\| genlik, faz → Doppler |
| L21 | FFT | radix-2 DIT, butterfly, twiddle ROM, bit-reversal; FFT IP (PG109) | — | **Doppler FFT**, range FFT (FMCW) |
| L22 | XADC | DRP, sürekli mod, örnekleme, Python'da spektrum | gerçek analog sinyal örnekleme | Basys3'te gerçek "alıcı" girişi |

## Faz IV — Radar blokları (L23–L29)
| # | Ders | İçerik | Radar bağlantısı |
|---|------|--------|------------------|
| L23 | Radar sistem zamanlaması | PRI/PRF, pulse width, range gate, blind range | Zamanlama kontrolcüsü |
| L24 | Sentetik hedef üretici | gecikme + Doppler + gürültü (LFSR) | FPGA içinde test sinyali (HackRF IQ kayıtlarıyla doğrulama) |
| L25 | Pulse compression | LFM matched filter, zaman-domeni korelasyon | Menzil çözünürlüğü $\Delta R = c/(2B)$ |
| L26 | MTI | 2-/3-pulse canceller, blind speeds | Sabit yankı (clutter) bastırma |
| L27 | Range-Doppler | corner turn (BRAM), slow-time FFT | Hız ölçümü |
| L28 | CFAR | CA-CFAR kayan pencere, guard/train hücreleri, $P_{FA}$ | Otomatik hedef tespiti |
| L29 | Veri çıkışı | paketleme, UART/(ileride Ethernet), Python PPI / A-scope | Operatör ekranı |

## Faz V — Final & profesyonellik (L30+)
- **L30 Capstone:** Basys3 üzerinde mini radar işlemcisi: sentetik yankı → pulse compression → MTI → Doppler FFT → CFAR → UART → Python range-Doppler haritası.
- **Ek modüller:** VHDL okuma/yazma (Verilog ↔ VHDL eşdeğerleri), SystemVerilog testbench, kodlama standartları (review checklist), versiyon kontrolü (git), timing kapatma teknikleri.

---
## Faz başına "çıkış sınavı"
Her fazın sonunda: internetsiz, sadece `docs/pdf/` + `my_library/` ile, zaman sınırlı bir tasarım görevi.
