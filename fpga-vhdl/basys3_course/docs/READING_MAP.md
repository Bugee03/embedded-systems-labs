# Okuma haritası — hangi derste hangi kitabın neresi

Elindeki kitaplar:
- **[CHU]** Pong P. Chu — *FPGA Prototyping by Verilog Examples* (Spartan-3 sürümü, 2008) → `docs/pdf/FPGA Prototyping with Verilog examples.pdf`
- **[MER]** Russell Merrick — *Getting Started with FPGAs* (2023) → `docs/pdf/...getting-started-with-fpgas....epub` (Apple Books ile aç)

## ⚠️ PDF sayfa numarası kuralı (CHU)
Kitaptaki basılı sayfa numarası ≠ PDF'teki sayfa numarası.
$$\text{PDF sayfası} = \text{kitap sayfası} + 33$$
Örnek: Bölüm 3.2.8 kitapta s. 46 → Preview'da **Cmd+Option+G** (Go to Page) → **79**.

## Bu iki kitap bizim kurs için ne kadar güncel?
| | Güncel ve değerli ✅ | Eski / atla ❌ |
|---|---|---|
| **CHU** | Verilog dili, kodlama kuralları, combinational/sequential/FSM/FSMD, UART, FIFO, 7-seg, VGA | Bölüm 2.3–2.7 (ISE + ModelSim tutorial — biz Vivado + iverilog kullanıyoruz), Bölüm 9–11 (PS2, SRAM), 15–18 (PicoBlaze) |
| **MER** | Kavramlar (LUT, FF, latch, CDC, timing, fixed-point), Verilog ↔ VHDL yan yana, kariyer ekleri | Bölüm 2'deki iCEcube2 / Diamond Programmer (Lattice araçları) → Basys3'te Vivado kullanacağız |

**Kural:** MER = *"neden ve sezgi"*, CHU = *"nasıl kodlanır ve detay"*. Her derste önce MER, sonra CHU.

## Ders ↔ okuma tablosu
| Ders | Önce oku (MER) | Sonra oku (CHU) | Not |
|---|---|---|---|
| **L01** Dijital mantık | Ch 3 *Boolean Algebra and the Look-Up Table* (tamamı); Ch 10 → *Numerical Data Types* … *Sizing Signals Appropriately* + *Addition*, *Subtraction* | Ch 1 → 1.1–1.5 (kitap s. 1–9, **PDF 34–42**) | VHDL kısımlarını şimdilik atla |
| L02 FPGA'nın içi | Ch 1 (tamamı); Ch 4 → *Look-Up Tables and Flip-Flops on a Real FPGA* | Ch 2 → 2.2 (s. 15–17, **PDF 48–50**) | Spartan-3 anlatımı; Artix-7 farkını ben vereceğim (6-input LUT) |
| L03 Verilog kombinasyonel | Ch 3 → *Project #2* kodu | Ch 1 → 1.5–1.6 (**PDF 38–45**); Ch 3 → 3.1–3.7 (s. 39–64, **PDF 72–97**) | 3.7 *coding guidelines* = altın |
| L04 Testbench | Ch 5 → *The Testbench* … *Self-Checking Testbenches* | Ch 1 → 1.7 (s. 12, **PDF 45**); Ch 4 → 4.4 | |
| L05 Vivado akışı | Ch 7 → *Synthesis*, *Place and Route*, *Constraints* | — (CHU ISE anlatıyor, atla) | Kaynak: UG892/UG894 + benim ders notu |
| L06 Sıralı mantık | Ch 4 (tamamı; *The Dangers of Latches*, *Resetting a Flip-Flop* önemli) | Ch 4 → 4.1–4.3 (s. 81–?, **PDF 114+**) | |
| L07 Sayaç, debounce | Ch 5 → *Project #4: Debouncing a Switch* | Ch 4 → 4.3.2, 4.5.1; Ch 6 → debounce örneği | |
| L08 FSM | Ch 8 (tamamı) | Ch 5 (s. 119, **PDF 152**) | |
| L09 Parametrik tasarım | Ch 6 → *Multiplexers*, *Shift Register* | Ch 3 → 3.8 parameter; Ch 4 → 4.5.1 LED multiplexing | |
| L10 UART | — | Ch 8 UART (s. 215, **PDF 248**) | |
| L11 Timing | Ch 7 → *Timing Errors*, *Addressing Timing Errors* | — | UG906 |
| L12 CDC | Ch 7 → *Crossing Clock Domains* | — | Cummings makaleleri |
| L13 Bellek/FIFO | Ch 6 → *RAM*, *FIFO*; Ch 9 → *Block RAM* | Ch 4 → 4.5.3 FIFO; Ch 12 (s. 297, **PDF 330**) | |
| L14 Clocking | Ch 9 → *The Phase-Locked Loop* | — | UG472 |
| L15 Fixed-point | Ch 10 → *Working with Decimals* … sonu | — | Meyer-Baese (ileride) |
| L16 DSP48 | Ch 9 → *The Digital Signal Processing Block* | — | UG479 |
| L17+ DSP & Radar | — | — | Meyer-Baese, Richards, Lyons + AMD PG'ler |
| Kariyer | Appendix B *Tips for a Career in FPGA Engineering* | — | Mülakattan önce mutlaka oku |

## Okuma yöntemi (offline çalışma alışkanlığı)
1. Önce **benim ders dosyamı** oku → ne arayacağını bil.
2. MER bölümünü oku → sezgi.
3. CHU bölümünü oku → kod detayı. Kod örneklerini **yazma**, sadece oku.
4. Ders dosyasını kapat → alıştırmaları kendin yaz.
5. Takılırsan önce kitapta ara (Cmd+F), sonra bana sor.
