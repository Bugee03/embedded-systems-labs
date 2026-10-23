# Offline doküman listesi (internet VARKEN indir!)

Nereden: **docs.amd.com** → arama kutusuna doküman ID'sini yaz (ör. `UG901`).
Dikkat: **kullandığın Vivado sürümüyle aynı sürümün** UG'sini indir (UG901 v2023.2 vs v2024.2 farklı olabilir).
Dosya adı standardı: `docs/pdf/UG901_vivado_synthesis_v2024.2.pdf` → `ls docs/pdf | grep UG9` ile hızlı bulursun.

> İpucu: Vivado içinde **Tools → Language Templates** tamamen offline çalışır —
> Verilog/VHDL kodlama şablonları, primitive instantiation, XDC örnekleri. İşte en çok kullanacağın şey.

## Tier 1 — Hemen indir (Faz I–II için şart)
| ☐ | ID | Başlık | Neden / hangi bölüm |
|---|----|--------|---------------------|
| ☐ | Basys 3 Reference Manual | Digilent (digilent.com/reference → Basys 3) | Kart şeması, pinler, XADC header, programlama modları |
| ☐ | Basys 3 Schematic | Digilent (aynı sayfa) | Butonların pull-down'ı, LED sürücü, 7-seg ortak anot |
| ☐ | **UG901** | Vivado Synthesis | **"HDL Coding Techniques"** bölümü = offline kod şablonu kütüphanesi (RAM, ROM, FSM, DSP inference) |
| ☐ | **UG903** | Using Constraints | XDC: create_clock, set_input_delay, false_path |
| ☐ | UG893 | Using the Vivado IDE | GUI temel |
| ☐ | UG892 | Design Flows Overview | Project vs non-project akış |
| ☐ | **UG894** | Using Tcl Scripting | Terminalden Vivado (bizim ana akışımız) |
| ☐ | UG835 | Tcl Command Reference | `synth_design`, `place_design` vs. tüm parametreler |
| ☐ | UG900 | Logic Simulation | xsim (Vivado simülatörü) |
| ☐ | UG908 | Programming and Debugging | Karta yükleme, ILA |
| ☐ | **UG474** | 7 Series CLB User Guide | LUT, FF, slice, carry chain — L02'nin kaynağı |
| ☐ | DS180 | 7 Series FPGAs Overview | Aile karşılaştırma, XC7A35T kaynak sayıları |
| ☐ | UG973 | Vivado Release Notes, Installation & Licensing | Kurulum, desteklenen OS |

## Tier 2 — Faz II–III başlamadan
| ☐ | ID | Başlık | Neden |
|---|----|--------|-------|
| ☐ | **UG473** | 7 Series Memory Resources | BRAM, FIFO — corner turn belleği |
| ☐ | **UG479** | 7 Series DSP48E1 Slice | Çarpıcı/MAC — FIR, FFT, korelasyon |
| ☐ | UG472 | 7 Series Clocking Resources | MMCM/PLL, BUFG |
| ☐ | UG471 | 7 Series SelectIO Resources | I/O standartları, IOB register |
| ☐ | **UG480** | 7 Series XADC | Analog giriş (L22) |
| ☐ | DS181 | Artix-7 Data Sheet: DC & AC Switching | Gerçek hız limitleri (Fmax, XADC spec) |
| ☐ | UG906 | Design Analysis and Closure Techniques | Timing raporu okuma |
| ☐ | **UG949** | UltraFast Design Methodology Guide | Profesyonel kodlama/CDC/reset kuralları |
| ☐ | UG953 | 7 Series Libraries Guide | Primitive'ler (BUFG, IDDR, DSP48E1...) |
| ☐ | UG470 | 7 Series Configuration | Flash'a yazma, boot |

## Tier 3 — IP çekirdekleri (Faz III–IV)
| ☐ | ID | IP | Radar kullanımı |
|---|----|----|-----------------|
| ☐ | PG065 | Clocking Wizard | Saat üretimi |
| ☐ | PG057 | FIFO Generator | CDC, akış tamponu |
| ☐ | PG058 | Block Memory Generator | Corner turn |
| ☐ | PG141 | DDS Compiler | Chirp / NCO |
| ☐ | PG149 | FIR Compiler | Matched filter |
| ☐ | PG105 | CORDIC | Genlik/faz |
| ☐ | PG109 | Fast Fourier Transform | Doppler/Range FFT |
| ☐ | PG172 | ILA | Sahada debug |
| ☐ | PG022 | AXI4-Stream Infrastructure | IP'ler arası veri akışı standardı |

## Ücretsiz makaleler (çok önemli, kısa)
Clifford E. Cummings — sunburst-design.com → Papers:
| ☐ | Makale | Konu |
|---|--------|------|
| ☐ | *Nonblocking Assignments in Verilog Synthesis, Coding Styles That Kill!* | `=` vs `<=` — L06 |
| ☐ | *Clock Domain Crossing (CDC) Design & Verification Techniques Using SystemVerilog* | L12 |
| ☐ | *Simulation and Synthesis Techniques for Asynchronous FIFO Design* | L12 |
| ☐ | *Synthesis and Scripting Techniques for Designing Multi-Asynchronous Clock Designs* | L12 |
| ☐ | *The Fundamentals of Efficient Synthesizable FSM Design* | L08 |

## Standartlar (opsiyonel referans)
- IEEE 1364-2005 (Verilog) — dil tanımı; IEEE 1800 (SystemVerilog) IEEE GET programıyla ücretsiz indirilebiliyor olabilir, kontrol et.
