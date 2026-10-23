# Araç kurulumu + offline hazırlık

## Büyük resim: hangi araç nerede?
| Araç | Ne işe yarar | Mac (Apple Silicon) | Linux (Fedora/Ubuntu x86_64) | Lab PC |
|---|---|---|---|---|
| **Icarus Verilog** (`iverilog`, `vvp`) | Hızlı RTL simülasyonu | ✅ brew | ✅ dnf/apt | — |
| **GTKWave** / **Surfer** | Dalga formu (waveform) görüntüleme | ✅ | ✅ | — |
| **Verilator** | Lint (kod denetimi) + çok hızlı sim | ✅ brew | ✅ | — |
| **Yosys** | Açık kaynak sentez → RTL'in hangi kapılara dönüştüğünü GÖRMEK | ✅ brew | ✅ | — |
| **Python** (numpy/scipy/matplotlib) | Golden model, test vektörü, sonuç çizimi | ✅ | ✅ | — |
| **Vivado ML Standard** (ücretsiz) | Gerçek sentez + P&R + bitstream + karta yükleme | ❌ **macOS'ta çalışmaz** | ✅ | ✅ |

**Strateji:** Mac'te günlük öğrenme (yaz → simüle et → dalga formuna bak), Linux/lab PC'de Vivado ile karta yükleme.
Bu, şirketlerdeki gerçek akışa da benzer: RTL + simülasyon çoğu zaman, sentez/P&R daha az.

---
## 1) Mac — simülasyon ortamı (terminal)
```bash
# Homebrew kurulu değilse: https://brew.sh
brew update
brew install icarus-verilog verilator yosys graphviz

# Dalga formu görüntüleyici — biri çalışması yeterli:
brew install gtkwave          # olmazsa:
brew install --cask gtkwave   # o da olmazsa Surfer kullan (surfer-project.org, macOS binary indir)

# Kontrol
iverilog -V | head -1
verilator --version
yosys -V

# Python sanal ortamı (kurs klasörünün içinde)
cd ~/Desktop/fpga
python3 -m venv .venv
source .venv/bin/activate
pip install numpy scipy matplotlib pyserial
```
`pyserial` → L10'da Basys3 UART'ından veri okumak için.

## 2) Vivado — Linux makinede
- Sürüm: **Lab PC'de hangi Vivado sürümü var, ÖNCE onu öğren.** Yeni sürümde açılan proje eski sürümde açılmaz.
  Biz Tcl script (non-project) akışı kullanacağımız için bu sorun büyük ölçüde ortadan kalkıyor:
  kaynaklar = `.v` + `.xdc` + `build.tcl` → her sürümde çalışır.
- Edition: **Vivado ML Standard** (ücretsiz, lisans gerekmez, XC7A35T destekli).
- Kurulumda sadece: *Vivado* + *Devices → 7 Series → Artix-7* seç (Zynq, UltraScale vs. KAPAT → disk ~ onlarca GB tasarruf).
- **Offline kurulum için:** installer'da *"Download Image (Install Separately)"* seçeneği → tüm kurulum dosyalarını
  bir klasöre/USB'ye indirir; internetsiz makinede oradan kurarsın. Şirket ortamını simüle etmek için bu imajı sakla.
- Fedora resmî destekli OS değil (UG973'te listeye bak), ama genelde çalışır; eksik kütüphane hatası olursa:
  `sudo dnf install ncurses-compat-libs libxcrypt-compat` (hata mesajındaki `.so` dosyasına göre).
- JTAG kablo sürücüsü (Basys3 USB'yi tanısın diye), kurulumdan sonra bir kez:
```bash
cd <Vivado_kurulum>/data/xicom/cable_drivers/lin64/install_script/install_drivers/
sudo ./install_drivers
# sonra Basys3'ü çıkar-tak
```
- Ortam değişkeni (her terminalde veya ~/.bashrc):
```bash
source <Vivado_kurulum>/settings64.sh
vivado -version
```

## 3) Offline hazırlık kontrol listesi (internet varken!)
- ☐ Vivado offline kurulum imajı (USB/harici disk)
- ☐ `docs/DOCS_CHECKLIST.md` Tier 1 PDF'leri → `docs/pdf/`
- ☐ Basys 3 Reference Manual + Schematic
- ☐ Kitaplar (en az Chu veya Merrick)
- ☐ Python paketleri: `pip download numpy scipy matplotlib pyserial -d ~/Desktop/fpga/00_setup/wheels`
  (internetsiz kurulum: `pip install --no-index --find-links wheels numpy ...`)
- ☐ Test: Wi-Fi'yi KAPAT → smoke test + bir Vivado build çalışıyor mu?

## 4) Smoke test
```bash
cd ~/Desktop/fpga/00_setup/smoke_test
chmod +x run.sh
./run.sh
```
Beklenen: `ALL TESTS PASSED` ve `half_adder.vcd` dosyası. Sonra `gtkwave half_adder.vcd` ile aç.
