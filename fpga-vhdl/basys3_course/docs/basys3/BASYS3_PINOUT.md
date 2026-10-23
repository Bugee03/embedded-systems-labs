# Basys 3 pin tablosu (kaynak: Digilent Basys-3-Master.xdc, rev B)

Hepsi `IOSTANDARD LVCMOS33`. Kullanım: `Basys-3-Master.xdc`'yi projeye kopyala, kullandığın satırların `#`'ini kaldır.

## Saat
| Sinyal | Pin | Not |
|---|---|---|
| clk | **W5** | 100 MHz → `create_clock -period 10.00` |

## Switch (sw[15:0]) — yukarı = 1
| 0 V17 | 1 V16 | 2 W16 | 3 W17 | 4 W15 | 5 V15 | 6 W14 | 7 W13 |
|---|---|---|---|---|---|---|---|
| **8 V2** | **9 T3** | **10 T2** | **11 R3** | **12 W2** | **13 U1** | **14 T1** | **15 R2** |

## LED (led[15:0]) — 1 = yanar
| 0 U16 | 1 E19 | 2 U19 | 3 V19 | 4 W18 | 5 U15 | 6 U14 | 7 V14 |
|---|---|---|---|---|---|---|---|
| **8 V13** | **9 V3** | **10 W3** | **11 U3** | **12 P3** | **13 N3** | **14 P1** | **15 L1** |

## Butonlar — basılı = 1 (dış pull-down)
| btnC U18 | btnU T18 | btnL W19 | btnR T17 | btnD U17 |
|---|---|---|---|---|

## 7-segment — **aktif-LOW** (0 = yanar), ortak anot
| seg[0]=a W7 | seg[1]=b W6 | seg[2]=c U8 | seg[3]=d V8 | seg[4]=e U5 | seg[5]=f V5 | seg[6]=g U7 | dp V7 |
|---|---|---|---|---|---|---|---|

| an[0] U2 (sağ hane) | an[1] U4 | an[2] V4 | an[3] W4 (sol hane) |
|---|---|---|---|

## USB-UART
| RsRx (PC→FPGA) B18 | RsTx (FPGA→PC) A18 |
|---|---|

## Pmod
| Header | [0] | [1] | [2] | [3] | [4] | [5] | [6] | [7] |
|---|---|---|---|---|---|---|---|---|
| JA | J1 | L2 | J2 | G2 | H1 | K2 | H2 | G3 |
| JB | A14 | A16 | B15 | B16 | A15 | A17 | C15 | C16 |
| JC | K17 | M18 | N17 | P18 | L17 | M19 | P17 | R18 |
| **JXADC** | J3 (XA1_P) | L3 (XA2_P) | M2 (XA3_P) | N2 (XA4_P) | K3 (XA1_N) | M3 (XA2_N) | M1 (XA3_N) | N1 (XA4_N) |

> JXADC = XADC'nin 4 diferansiyel analog girişi. **0–1 V aralığı!** 3.3 V vermek XADC girişini bozabilir — L22'de Reference Manual'dan doğrulayacağız.

## VGA (4-bit/renk)
Red G19 H19 J19 N19 · Green J17 H17 G17 D17 · Blue N18 L18 K18 J18 · Hsync P19 · Vsync R19

## Her tasarıma eklenecek konfigürasyon satırları
```tcl
set_property CONFIG_VOLTAGE 3.3 [current_design]
set_property CFGBVS VCCO [current_design]
set_property BITSTREAM.GENERAL.COMPRESS TRUE [current_design]
set_property BITSTREAM.CONFIG.CONFIGRATE 33 [current_design]
set_property CONFIG_MODE SPIx4 [current_design]
```
