# Terminal kopya kâğıdı (her derste büyüyecek)

## Simülasyon — Icarus Verilog
| Komut | Ne yapar |
|---|---|
| `iverilog -g2005 -Wall -o sim.out dut.v tb_dut.v` | Derle (Verilog-2005, tüm uyarılar) |
| `iverilog -g2012 ...` | SystemVerilog özellikleri gerekince |
| `iverilog -I rtl/ -s tb_top ...` | include klasörü / top modülü seç |
| `iverilog -D SIM ...` | `` `ifdef SIM `` makrosu tanımla |
| `vvp sim.out` | Simülasyonu çalıştır |
| `vvp sim.out +N=100` | Plusarg ver (`$value$plusargs`) |
| `gtkwave dump.vcd &` | Dalga formunu aç |
| `gtkwave dump.vcd view.gtkw &` | Kaydedilmiş sinyal düzeniyle aç (File → Write Save File) |

## Lint — Verilator
| `verilator --lint-only -Wall dut.v` | Sentezlenebilirlik + stil hataları (latch, genişlik uyuşmazlığı) |
|---|---|

## Yosys — RTL'in donanım karşılığını görmek
```bash
yosys -p "read_verilog dut.v; proc; opt; show"            # RTL şeması (graphviz)
yosys -p "read_verilog dut.v; synth_xilinx -top dut; stat" # Xilinx LUT/FF/DSP tahmini
```

## Vivado — terminal (non-project / Tcl) akışı
```bash
source <Vivado>/settings64.sh
vivado -mode batch -source build.tcl -nojournal -nolog   # tam build
vivado -mode tcl                                          # interaktif Tcl kabuğu
vivado &                                                  # GUI (gerekirse)
```

`build.tcl` iskeleti (ANLA, sonra kendin yaz):
```tcl
set top  top
set part xc7a35tcpg236-1
read_verilog [glob rtl/*.v]
read_xdc     constr/basys3.xdc
synth_design -top $top -part $part
opt_design
place_design
route_design
report_utilization     -file reports/util.rpt
report_timing_summary  -file reports/timing.rpt
write_bitstream -force build/$top.bit
```

`program.tcl` (karta yükleme):
```tcl
open_hw_manager
connect_hw_server
open_hw_target
set dev [lindex [get_hw_devices xc7a35t*] 0]
current_hw_device $dev
set_property PROGRAM.FILE build/top.bit $dev
program_hw_devices $dev
close_hw_manager
```

## Rapor okuma (grep ile hızlı)
```bash
grep -A3 "Design Timing Summary" reports/timing.rpt   # WNS/TNS
grep -E "Slice LUTs|Slice Registers|DSPs|Block RAM" reports/util.rpt
```

## Python
```bash
source .venv/bin/activate
python3 gen_vectors.py      # test vektörü üret
python3 plot_result.py      # sim çıktısını çiz
```

## Seri port (UART, L10+)
```bash
ls /dev/tty.usbserial*        # macOS'ta Basys3 portu
screen /dev/tty.usbserial-XXXX 115200   # çıkış: Ctrl-A, sonra K
```
