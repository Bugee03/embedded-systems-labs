# Nuvoton M031: Mac'te Keil'siz geliştirme

Derleyici: `arm-none-eabi-gcc` (STM32CubeCLT ile gelen)
Karta yükleme: `pyOCD` (`.venv` içinde) + Nuvoton.NuMicro_DFP paketi
BSP: `M031BSP/` (github.com/OpenNuvoton/M031BSP)

## Klasör yapısı
```
M031BSP/          Nuvoton BSP (dokunma)
mk/m031.mk        ortak Makefile
projects/         senin projelerin (her biri ayrı klasör)
  template/       boş şablon (Hello World)
  01_led … 12_i2c_lcd  konu projeleri (DERSLER.md)
yeni.sh           yeni proje oluşturur
```

## Günlük kullanım
```bash
cd ~/nuvoton/projects/01_led
make            # derle
make flash      # derle + karta yükle
make monitor    # seri port (printf çıktısı). Çıkış: Ctrl+A, K, Y
```

Yeni proje:
```bash
cd ~/nuvoton
./yeni.sh odev1                       # boş şablondan
./yeni.sh adc1 ADC_SingleMode         # BSP örneğinden (ls M031BSP/SampleCode/StdDriver)
cd projects/adc1 && make flash
```
Projeye `.c` dosyası eklemek için dosyayı klasöre koyman yeterli. Tüm BSP sürücüleri (adc, pwm, i2c, spi...) otomatik olarak derlemeye dahil edilir.

## Kartı ilk bağladığında
1. `make probe`: Nu-Link görünmeli.
2. Çipin adını kartın üstünden oku (ör. M031SD2AE) ve gerekirse `TARGET` ayarla:
   `make flash TARGET=m031sd2ae` (liste için: `.venv/bin/pyocd list --targets | grep m031`)
   Kalıcı olarak değiştirmek için `mk/m031.mk` içindeki `TARGET ?=` satırını düzenle.
3. `make flash` çalıştır, ardından `make monitor`.

## Sorun giderme
- **"No available debug probes"**: USB kablosu sadece şarj kablosu olabilir; kartın Nu-Link tarafındaki USB'ye tak.
  Hâlâ görünmüyorsa Nu-Link2 firmware'i eski olabilir (CMSIS-DAP desteği için v3.16 veya üstü gerekir).
  Güncellemek için Windows'lu bir bilgisayarda Nu-Link Keil Driver ile bir kez bağlanmak yeterli.
- **Seri port yok**: Nu-Link2-Me üzerindeki VCOM switch'leri (1 ve 2) ON olmalı. `ls /dev/cu.usbmodem*`
- **Flash/RAM taştı**: varsayılan linker 16K/2K (her M031'de çalışır). Büyük çipler için
  `make LDSCRIPT=$HOME/nuvoton/M031BSP/Library/Device/Nuvoton/M031/Source/GCC/gcc_arm_128k.ld`
- **Çip kilitli / yükleme hata veriyor**: `make erase` ve ardından `make flash`.
