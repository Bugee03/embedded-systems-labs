# Nuvoton M031: Komut Kılavuzu (Mac, Keil'siz)

Her şey `~/nuvoton` klasöründe. Terminalde çalışılır, Keil gerekmez.

> Konu projeleri (01_led … 12_i2c_lcd) ve açıklamaları için: **`DERSLER.md`**

```
~/nuvoton/
├── M031BSP/          Nuvoton BSP (sürücüler, örnekler). DOKUNMA
├── mk/m031.mk        ortak Makefile (derleme + yükleme ayarları)
├── projects/         senin projelerin, her biri ayrı klasör
│   ├── template/     boş şablon (Hello World)
│   └── 00_pin_bulucu … 12_i2c_lcd  konu projeleri (DERSLER.md)
├── yeni.sh           yeni proje oluşturur
└── .venv/            pyOCD (karta yükleme aracı)
```

---

## 1. Hızlı başlangıç (en sık kullanılan 3 komut)

```bash
cd ~/nuvoton/projects/01_led
make flash        # derle + karta yükle
make monitor      # printf çıktısını izle (çıkış: Ctrl+A, sonra K, sonra Y)
```

---

## 2. Tüm komutlar

Hepsi bir proje klasörünün içinde çalıştırılır (ör. `cd ~/nuvoton/projects/01_led`).

| Komut | Ne yapar |
|---|---|
| `make` | Derler. Çıktı: `build/<proje>.elf`, `.hex`, `.bin` |
| `make flash` | Derler ve karta yükler |
| `make monitor` | Seri portu 115200 baud'da açar (printf çıktısı burada görünür) |
| `make probe` | Bağlı Nu-Link'i listeler (kart görünüyor mu?) |
| `make info` | Çipe bağlanıp bilgi gösterir |
| `make reset` | Kartı resetler (program baştan başlar) |
| `make erase` | Çipin flash'ını tamamen siler |
| `make clean` | `build/` klasörünü siler (temiz derleme için) |

Komutlar birleştirilebilir:
```bash
make clean flash          # sıfırdan derle + yükle
make flash monitor        # yükle ve hemen seri portu aç
```

---

## 3. Yeni proje oluşturma

```bash
cd ~/nuvoton

./yeni.sh odev1                      # boş şablondan (Template)
./yeni.sh adc1 ADC_SingleMode        # BSP'deki hazır örnekten kopyala
./yeni.sh pwm1 PWM_OutputWaveform

cd projects/adc1
make flash
```

- Projeye yeni `.c` / `.h` dosyası eklemek için dosyayı klasöre koyman yeterli. Makefile'a bir şey yazman gerekmez.
- BSP sürücülerinin hepsi (adc, pwm, i2c, spi, uart, timer, gpio…) otomatik olarak derlenir. Kullanılmayanlar son dosyadan atılır.
- Bir projeyi silmek için: `rm -rf ~/nuvoton/projects/odev1`

### İşine yarayacak BSP örnekleri
```bash
ls ~/nuvoton/M031BSP/SampleCode/StdDriver          # hepsini listele
ls ~/nuvoton/M031BSP/SampleCode/StdDriver | grep -i pwm   # konuya göre ara
```

| Konu | Örnek adı |
|---|---|
| GPIO / LED / buton | `GPIO_OutputInput`, `GPIO_INT`, `GPIO_EINTAndDebounce` |
| ADC (LDR, pot) | `ADC_SingleMode`, `ADC_ContinuousScanMode` |
| Timer | `TIMER_PeriodicINT`, `TIMER_Delay`, `TIMER_CaptureCounter` |
| PWM (LED, buzzer) | `PWM_OutputWaveform`, `PWM_DutySwitch`, `BPWM_OutputWaveform` |
| UART (Bluetooth) | `UART_TxRxFunction`, `UART_AutoBaudRate` |
| SPI | `SPI_MasterFIFOMode`, `SPI_Loopback` |
| I2C (sensör, LCD) | `I2C_Master`, `I2C_MultiBytes_Master`, `I2C_EEPROM` |
| RTC | `RTC_Time_Display` |
| Watchdog | `WDT_TimeoutWakeupAndReset` |

---

## 4. Çipi değiştirme (TARGET)

Varsayılan çip **`m032kg8ae`** (Boardoza M032KG kiti: 256K flash / 64K RAM). Kartındaki çip farklıysa (çipin üstünde yazar, ör. `M031LG8AE`) adı **küçük harfle** ver:

**Tek seferlik:**
```bash
make flash TARGET=m031lg8ae
```

**Sadece bu proje için kalıcı:** projenin `Makefile`'ını şöyle yap:
```make
TARGET = m032kg8ae
include ../../mk/m031.mk
```

**Tüm projeler için kalıcı:** `~/nuvoton/mk/m031.mk` içindeki satırı değiştir:
```make
TARGET  ?= m031lg8ae
```

**Desteklenen çipleri listele:**
```bash
~/nuvoton/.venv/bin/pyocd list --targets | grep -i m03
```
Desteklenenler: m031 (btyd2an, btye3an, eb0ae, ec1ae, fb0ae, fc1ae, ggc1ae, ggc2ae, ggd2ae, gtc1ae, gtc2ae, gtd2ae, kg6ae, kg8ae, kiaae, lc2ae, ld2ae, le3ae, lg6ae, lg8ae, sc2ae, sd2ae, se3ae, sg6ae, sg8ae, siaae, tb0ae, tc1ae, td2ae, te3ae) ve m032 serisi.

---

## 5. Bellek ayarı (Linker script)

Varsayılan linker script artık **`gcc_arm_256k.ld` (256K / 64K)**; Boardoza M032KG (M032KG8AE) ile birebir aynı. Derleme sonunda şöyle bir satır görürsün:
```
   text    data     bss     dec     hex  filename
   6376     112     368    6856    1ac8  build/01_led.elf
```
`text + data` = flash kullanımı, `data + bss` = RAM kullanımı.

**"region FLASH overflowed"** veya **"region RAM overflowed"** hatası alırsan çipine uygun dosyayı seç:

| Dosya | Flash / RAM |
|---|---|
| `gcc_arm.ld` | 16K / 2K (her çipte güvenli) |
| `gcc_arm_32K.ld` | 32K / 4K |
| `gcc_arm_128k.ld` | 128K / 16K |
| `gcc_arm_256k.ld` (varsayılan) | 256K / 64K |

Çipinin flash/RAM değerinden **küçük veya eşit** olan en büyük dosyayı seç (ör. 64K flash / 8K RAM bir çipte `gcc_arm_32K.ld`).

```bash
make clean
make flash LDSCRIPT=~/nuvoton/M031BSP/Library/Device/Nuvoton/M031/Source/GCC/gcc_arm_128k.ld
```
Kalıcı yapmak için projenin Makefile'ına `include` satırının **üstüne** ekle:
```make
LDSCRIPT = $(HOME)/nuvoton/M031BSP/Library/Device/Nuvoton/M031/Source/GCC/gcc_arm_128k.ld
```

> Çipin tam flash/RAM değerinden emin değilsen datasheet'e bak. Yanlış (fazla büyük) linker seçersen program derlenir ama kartta çöker.

---

## 6. Seri port (printf)

```bash
make monitor                          # otomatik bulur ve açar
ls /dev/cu.usbmodem*                  # portu elle bulmak için
screen /dev/cu.usbmodem1234 115200    # elle açmak için
```
- **Çıkış:** `Ctrl+A`, sonra `K`, sonra `Y`
- `printf` → UART0 (PB12 = RX, PB13 = TX) → Nu-Link VCOM → Mac
- Hiçbir şey görünmüyorsa: Nu-Link2-Me üzerindeki **VCOM switch'lerini (1 ve 2) ON** yap, sonra `make reset` ile kartı resetle.
- Takılı kalan bir `screen` oturumu portu kilitliyorsa: `pkill screen`

---

## 7. Derleyici ayarları (gerekirse)

Projenin Makefile'ına, `include` satırının **üstüne** yazılır:

```make
CFLAGS += -DDEBUG_MODE=1           # kendi #define'ın
CFLAGS += -O0                      # optimizasyonu kapat (debug için)
EXTRA_SRCS = ../ortak/lcd.c        # başka klasördeki bir .c dosyasını ekle
include ../../mk/m031.mk
```

---

## 8. Sorun giderme

| Hata / belirti | Çözüm |
|---|---|
| `No available debug probes are connected` | USB kablosunu değiştir (bazı kablolar sadece şarj eder). Kartın **Nu-Link** tarafındaki USB'ye tak. `make probe` ile kontrol et. |
| Probe hâlâ görünmüyor | Nu-Link2 firmware'i eski olabilir (v3.16 veya üstü gerekir). Windows'lu bir bilgisayarda Nuvoton aracıyla bir kez bağlanıp güncellenmesi gerekir. |
| `Target type ... not recognized` | TARGET adı yanlış. Bölüm 4'e bak, adı küçük harfle yaz. |
| Yükleme hata veriyor / çip kilitli | `make erase` çalıştır, sonra `make flash`. |
| `region FLASH/RAM overflowed` | Bölüm 5: daha büyük linker script seç. |
| `undefined reference to ...` | Fonksiyon adını yanlış yazmış olabilirsin. `grep -rn "FonksiyonAdi" ~/nuvoton/M031BSP/Library/StdDriver/inc` ile kontrol et. |
| Değişiklik karta yansımıyor | `make clean flash` |
| Program çalışıyor ama çıktı yok | VCOM switch'leri, baud (115200), `make reset` |
| `make: command not found` / `arm-none-eabi-gcc` bulunamadı | Yeni terminal aç. Olmazsa: `export PATH=/opt/ST/STM32CubeCLT_1.22.0/GNU-tools-for-STM32/bin:/opt/ST/STM32CubeCLT_1.22.0/Make/bin:$PATH` |

### Fonksiyon / makro arama
```bash
cd ~/nuvoton/M031BSP
grep -rn "PWM_ConfigOutputChannel" Library/StdDriver/inc     # API'yi bul
grep -rn "PB0MFP_ADC" Library/Device/Nuvoton/M031/Include    # pin fonksiyonu (MFP) makrosu bul
grep -rln "I2C_Open" SampleCode/StdDriver                    # bu fonksiyonu kullanan örnekler
```

---

## 9. Her M031 programının iskeleti

```c
#include <stdio.h>
#include "NuMicro.h"

void SYS_Init(void)
{
    SYS_UnlockReg();                                       // korumalı register'ları aç

    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);               // dahili 48 MHz
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    // 1) Çevre birimi clock'u
    CLK_EnableModuleClock(UART0_MODULE);
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    SystemCoreClockUpdate();

    // 2) Pin fonksiyonu (MFP): PB12 = RX, PB13 = TX
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    SYS_LockReg();
}

int main(void)
{
    SYS_Init();
    UART_Open(UART0, 115200);                              // 3) çevre birimini aç
    printf("Merhaba M031\n");
    while (1) { }
}
```
**Kural:** önce **clock enable**, sonra **pin MFP**, en son **Open/Config**. Bir şey çalışmıyorsa büyük ihtimalle bu üç adımdan biri eksiktir.

---

## 10. İlk gün kontrol listesi (kart bağlanınca)

```bash
cd ~/nuvoton/projects/01_led
make probe                    # 1. Nu-Link görünüyor mu?
make info                     # 2. Çipe bağlanabiliyor mu?
make flash                    # 3. Yükleme çalışıyor mu? (TARGET doğru mu?)
make monitor                  # 4. "tick 0, tick 1..." akıyor mu?
```
Karttaki LED5–LED8 = PF8, PF7, PF6, PF14. Tüm pinler için: `DERSLER.md` ve `Boardoza_M032KG_sema.pdf`.
