# Boardoza M032KG: Pin ve Register Bit Haritası

Kaynak: `Boardoza_M032KG_sema.pdf` + BSP başlık dosyaları (`sys.h`). MFP değerleri doğrudan `sys.h`'den otomatik okundu.

## Önce kural: bir pinin bitleri nerede?

Pin **Pxn** için (x = port harfi, n = 0..15):

| Ne | Register | Bitler | Formül |
|---|---|---|---|
| Mod (giriş/çıkış) | `Px->MODE` | `[2n+1 : 2n]` | 00 giriş, 01 çıkış (push-pull), 10 open-drain, 11 quasi |
| Çıkış değeri | `Px->DOUT` | bit `n` | 1 → pin HIGH |
| Okunan değer | `Px->PIN` | bit `n` | pinin anlık seviyesi |
| Kesme bayrağı | `Px->INTSRC` | bit `n` | 1 yazınca temizlenir |
| Debounce aç | `Px->DBEN` | bit `n` | |
| Dijital girişi kapat (analog) | `Px->DINOFF` | bit `n+16` | ADC pinlerinde |
| Pin fonksiyonu (MFP) | `SYS->GPx_MFPL` (n = 0..7) / `SYS->GPx_MFPH` (n = 8..15) | `[4m+3 : 4m]`, m = n mod 8 | her pin için 4 bit |
| Tek pin erişimi | `Pxn` makrosu (ör. `PF8`) | | `PF8 = 1;` `x = PC12;` |
| Maske | `BITn` (ör. `BIT8` = `0x0100`) | | `GPIO_SetMode(PF, BIT8, ...)` |

Kesme hattı: **PA, PB** → `GPIO_PAPB_IRQn` / `GPABGH_IRQHandler`; **PC, PD, PE, PF** → `GPIO_PCPDPEPF_IRQn` / `GPCDEF_IRQHandler`.

## Tablo 1: Parça → pin

| Parça | Pin | Port / bit | Maske | Fonksiyon | Açıklama | Proje |
|---|---|---|---|---|---|---|
| LED5 | **PF8** | PF / bit 8 | `BIT8` = `0x0100` | GPIO | Çıkış, 1 = yanar | 01,02,04,07,08 |
| LED6 | **PF7** | PF / bit 7 | `BIT7` = `0x0080` | GPIO | Çıkış, 1 = yanar | 01,02,07 |
| LED7 | **PF6** | PF / bit 6 | `BIT6` = `0x0040` | GPIO | Çıkış, 1 = yanar | 01,02,07 |
| LED8 | **PF14** | PF / bit 14 | `BIT14` = `0x4000` | GPIO | Çıkış, 1 = yanar | 01,02,07 |
| SW2 | **PC12** | PC / bit 12 | `BIT12` = `0x1000` | GPIO | Giriş, basınca 0 (dış pull-up) | 02 |
| SW3 | **PC11** | PC / bit 11 | `BIT11` = `0x0800` | GPIO | Giriş, basınca 0 | 02 |
| SW4 | **PC10** | PC / bit 10 | `BIT10` = `0x0400` | GPIO | Giriş, basınca 0 | 02 |
| SW5 | **PB1** | PB / bit 1 | `BIT1` = `0x0002` | GPIO | Giriş, basınca 0 | 02 |
| Pot (PB3) | **PB3** | PB / bit 3 | `BIT3` = `0x0008` | ADC0 CH3 | Analog, ADC kanal 3 | 03,05,13 |
| Pot (PB4) | **PB4** | PB / bit 4 | `BIT4` = `0x0010` | ADC0 CH4 | Analog, ADC kanal 4 | 03,05,13 |
| Pot (PB5) | **PB5** | PB / bit 5 | `BIT5` = `0x0020` | ADC0 CH5 | Analog, ADC kanal 5 | 03,05,13 |
| NTC | **PB2** | PB / bit 2 | `BIT2` = `0x0004` | ADC0 CH2 | Analog, ADC kanal 2 | 03,13 |
| LDR | **PB15** | PB / bit 15 | `BIT15` = `0x8000` | ADC0 CH15 | Analog, ADC kanal 15 | 03,13 |
| RGB kırmızı | **PC3** | PC / bit 3 | `BIT3` = `0x0008` | PWM1 CH2 | PWM1 kanal 2 | 05 |
| RGB yeşil | **PC5** | PC / bit 5 | `BIT5` = `0x0020` | PWM1 CH0 | PWM1 kanal 0 | 05 |
| RGB mavi | **PC4** | PC / bit 4 | `BIT4` = `0x0010` | PWM1 CH1 | PWM1 kanal 1 | 05 |
| Buzzer | **PC9** | PC / bit 9 | `BIT9` = `0x0200` | PWM1 CH3 | PWM1 kanal 3 | 06 |
| Encoder A | **PB7** | PB / bit 7 | `BIT7` = `0x0080` | GPIO | Giriş + kesme | 07 |
| Encoder B | **PB6** | PB / bit 6 | `BIT6` = `0x0040` | GPIO | Giriş | 07 |
| Encoder düğme | **PA7** | PA / bit 7 | `BIT7` = `0x0080` | GPIO | Giriş + kesme | 07 |
| UART4 RX (J11.1) | **PC6** | PC / bit 6 | `BIT6` = `0x0040` | UART4 RXD | Bluetooth modülünün TX'i buraya | 08 |
| UART4 TX (J11.2) | **PC7** | PC / bit 7 | `BIT7` = `0x0080` | UART4 TXD | Bluetooth modülünün RX'ine | 08 |
| I2C SDA (J9.4) | **PC8** | PC / bit 8 | `BIT8` = `0x0100` | I2C0 SDA | OLED / sensör | 10-13,08 |
| I2C SCL (J9.3) | **PE13** | PE / bit 13 | `BIT13` = `0x2000` | I2C0 SCL | OLED / sensör | 10-13,08 |
| SPI MOSI (J10.4) | **PA0** | PA / bit 0 | `BIT0` = `0x0001` | SPI0 MOSI |  | 09 |
| SPI MISO (J10.3) | **PA1** | PA / bit 1 | `BIT1` = `0x0002` | SPI0 MISO |  | – |
| SPI CLK (J10.2) | **PA2** | PA / bit 2 | `BIT2` = `0x0004` | SPI0 CLK |  | 09 |
| SPI SS (J10.1) | **PA3** | PA / bit 3 | `BIT3` = `0x0008` | SPI0 SS |  | – |
| Motor IN1 | **PA5** | PA / bit 5 | `BIT5` = `0x0020` | PWM0 CH0 | DRV8837 (PWM ile hız) | – |
| Motor IN2 | **PA4** | PA / bit 4 | `BIT4` = `0x0010` | PWM0 CH1 | DRV8837 | – |
| Motor nSLEEP | **PD15** | PD / bit 15 | `BIT15` = `0x8000` | GPIO | 1 = sürücü aktif | – |
| ESP-01M RX | **PD0** | PD / bit 0 | `BIT0` = `0x0001` | UART3 RXD | Wi-Fi modülü (JP1 ile güç) | – |
| ESP-01M TX | **PD1** | PD / bit 1 | `BIT1` = `0x0002` | UART3 TXD |  | – |
| VCOM RX | **PB12** | PB / bit 12 | `BIT12` = `0x1000` | UART0 RXD | Mac → kart (seri port) | hepsi |
| VCOM TX | **PB13** | PB / bit 13 | `BIT13` = `0x2000` | UART0 TXD | Kart → Mac (printf) | hepsi |
| SWD DAT | **PF0** | PF / bit 0 | `BIT0` = `0x0001` | ICE DAT | Programlayıcı, DOKUNMA | – |
| SWD CLK | **PF1** | PF / bit 1 | `BIT1` = `0x0002` | ICE CLK | Programlayıcı, DOKUNMA | – |

## Tablo 2: Pin → register bitleri

| Pin | MODE bitleri | DOUT / PIN biti | MFP register | MFP bitleri | MFP değeri | MFP makrosu | Kesme |
|---|---|---|---|---|---|---|---|
| PF8 | `PF->MODE[17:16]` | bit 8 | `SYS->GPF_MFPH` | `[3:0]` | `0x0` | `SYS_GPF_MFPH_PF8MFP_GPIO` | GPCDEF |
| PF7 | `PF->MODE[15:14]` | bit 7 | `SYS->GPF_MFPL` | `[31:28]` | `0x0` | `SYS_GPF_MFPL_PF7MFP_GPIO` | GPCDEF |
| PF6 | `PF->MODE[13:12]` | bit 6 | `SYS->GPF_MFPL` | `[27:24]` | `0x0` | `SYS_GPF_MFPL_PF6MFP_GPIO` | GPCDEF |
| PF14 | `PF->MODE[29:28]` | bit 14 | `SYS->GPF_MFPH` | `[27:24]` | `0x0` | `SYS_GPF_MFPH_PF14MFP_GPIO` | GPCDEF |
| PC12 | `PC->MODE[25:24]` | bit 12 | `SYS->GPC_MFPH` | `[19:16]` | `0x0` | `SYS_GPC_MFPH_PC12MFP_GPIO` | GPCDEF |
| PC11 | `PC->MODE[23:22]` | bit 11 | `SYS->GPC_MFPH` | `[15:12]` | `0x0` | `SYS_GPC_MFPH_PC11MFP_GPIO` | GPCDEF |
| PC10 | `PC->MODE[21:20]` | bit 10 | `SYS->GPC_MFPH` | `[11:8]` | `0x0` | `SYS_GPC_MFPH_PC10MFP_GPIO` | GPCDEF |
| PB1 | `PB->MODE[3:2]` | bit 1 | `SYS->GPB_MFPL` | `[7:4]` | `0x0` | `SYS_GPB_MFPL_PB1MFP_GPIO` | GPABGH |
| PB3 | `PB->MODE[7:6]` | bit 3 | `SYS->GPB_MFPL` | `[15:12]` | `0x1` | `SYS_GPB_MFPL_PB3MFP_ADC0_CH3` | GPABGH |
| PB4 | `PB->MODE[9:8]` | bit 4 | `SYS->GPB_MFPL` | `[19:16]` | `0x1` | `SYS_GPB_MFPL_PB4MFP_ADC0_CH4` | GPABGH |
| PB5 | `PB->MODE[11:10]` | bit 5 | `SYS->GPB_MFPL` | `[23:20]` | `0x1` | `SYS_GPB_MFPL_PB5MFP_ADC0_CH5` | GPABGH |
| PB2 | `PB->MODE[5:4]` | bit 2 | `SYS->GPB_MFPL` | `[11:8]` | `0x1` | `SYS_GPB_MFPL_PB2MFP_ADC0_CH2` | GPABGH |
| PB15 | `PB->MODE[31:30]` | bit 15 | `SYS->GPB_MFPH` | `[31:28]` | `0x1` | `SYS_GPB_MFPH_PB15MFP_ADC0_CH15` | GPABGH |
| PC3 | `PC->MODE[7:6]` | bit 3 | `SYS->GPC_MFPL` | `[15:12]` | `0xC` | `SYS_GPC_MFPL_PC3MFP_PWM1_CH2` | GPCDEF |
| PC5 | `PC->MODE[11:10]` | bit 5 | `SYS->GPC_MFPL` | `[23:20]` | `0xC` | `SYS_GPC_MFPL_PC5MFP_PWM1_CH0` | GPCDEF |
| PC4 | `PC->MODE[9:8]` | bit 4 | `SYS->GPC_MFPL` | `[19:16]` | `0xC` | `SYS_GPC_MFPL_PC4MFP_PWM1_CH1` | GPCDEF |
| PC9 | `PC->MODE[19:18]` | bit 9 | `SYS->GPC_MFPH` | `[7:4]` | `0xC` | `SYS_GPC_MFPH_PC9MFP_PWM1_CH3` | GPCDEF |
| PB7 | `PB->MODE[15:14]` | bit 7 | `SYS->GPB_MFPL` | `[31:28]` | `0x0` | `SYS_GPB_MFPL_PB7MFP_GPIO` | GPABGH |
| PB6 | `PB->MODE[13:12]` | bit 6 | `SYS->GPB_MFPL` | `[27:24]` | `0x0` | `SYS_GPB_MFPL_PB6MFP_GPIO` | GPABGH |
| PA7 | `PA->MODE[15:14]` | bit 7 | `SYS->GPA_MFPL` | `[31:28]` | `0x0` | `SYS_GPA_MFPL_PA7MFP_GPIO` | GPABGH |
| PC6 | `PC->MODE[13:12]` | bit 6 | `SYS->GPC_MFPL` | `[27:24]` | `0x5` | `SYS_GPC_MFPL_PC6MFP_UART4_RXD` | GPCDEF |
| PC7 | `PC->MODE[15:14]` | bit 7 | `SYS->GPC_MFPL` | `[31:28]` | `0x5` | `SYS_GPC_MFPL_PC7MFP_UART4_TXD` | GPCDEF |
| PC8 | `PC->MODE[17:16]` | bit 8 | `SYS->GPC_MFPH` | `[3:0]` | `0x4` | `SYS_GPC_MFPH_PC8MFP_I2C0_SDA` | GPCDEF |
| PE13 | `PE->MODE[27:26]` | bit 13 | `SYS->GPE_MFPH` | `[23:20]` | `0x4` | `SYS_GPE_MFPH_PE13MFP_I2C0_SCL` | GPCDEF |
| PA0 | `PA->MODE[1:0]` | bit 0 | `SYS->GPA_MFPL` | `[3:0]` | `0x4` | `SYS_GPA_MFPL_PA0MFP_SPI0_MOSI` | GPABGH |
| PA1 | `PA->MODE[3:2]` | bit 1 | `SYS->GPA_MFPL` | `[7:4]` | `0x4` | `SYS_GPA_MFPL_PA1MFP_SPI0_MISO` | GPABGH |
| PA2 | `PA->MODE[5:4]` | bit 2 | `SYS->GPA_MFPL` | `[11:8]` | `0x4` | `SYS_GPA_MFPL_PA2MFP_SPI0_CLK` | GPABGH |
| PA3 | `PA->MODE[7:6]` | bit 3 | `SYS->GPA_MFPL` | `[15:12]` | `0x4` | `SYS_GPA_MFPL_PA3MFP_SPI0_SS` | GPABGH |
| PA5 | `PA->MODE[11:10]` | bit 5 | `SYS->GPA_MFPL` | `[23:20]` | `0xD` | `SYS_GPA_MFPL_PA5MFP_PWM0_CH0` | GPABGH |
| PA4 | `PA->MODE[9:8]` | bit 4 | `SYS->GPA_MFPL` | `[19:16]` | `0xD` | `SYS_GPA_MFPL_PA4MFP_PWM0_CH1` | GPABGH |
| PD15 | `PD->MODE[31:30]` | bit 15 | `SYS->GPD_MFPH` | `[31:28]` | `0x0` | `SYS_GPD_MFPH_PD15MFP_GPIO` | GPCDEF |
| PD0 | `PD->MODE[1:0]` | bit 0 | `SYS->GPD_MFPL` | `[3:0]` | `0x5` | `SYS_GPD_MFPL_PD0MFP_UART3_RXD` | GPCDEF |
| PD1 | `PD->MODE[3:2]` | bit 1 | `SYS->GPD_MFPL` | `[7:4]` | `0x5` | `SYS_GPD_MFPL_PD1MFP_UART3_TXD` | GPCDEF |
| PB12 | `PB->MODE[25:24]` | bit 12 | `SYS->GPB_MFPH` | `[19:16]` | `0x6` | `SYS_GPB_MFPH_PB12MFP_UART0_RXD` | GPABGH |
| PB13 | `PB->MODE[27:26]` | bit 13 | `SYS->GPB_MFPH` | `[23:20]` | `0x6` | `SYS_GPB_MFPH_PB13MFP_UART0_TXD` | GPABGH |
| PF0 | `PF->MODE[1:0]` | bit 0 | `SYS->GPF_MFPL` | `[3:0]` | `0xE` | `SYS_GPF_MFPL_PF0MFP_ICE_DAT` | GPCDEF |
| PF1 | `PF->MODE[3:2]` | bit 1 | `SYS->GPF_MFPL` | `[7:4]` | `0xE` | `SYS_GPF_MFPL_PF1MFP_ICE_CLK` | GPCDEF |

## Örnek: LED5 (PF8) adım adım

```c
// 1) MFP: PF8 -> GPIO.  PF8, MFPH'de (8..15), m = 8 mod 8 = 0 -> bitler [3:0], değer 0x0
SYS->GPF_MFPH = (SYS->GPF_MFPH & ~SYS_GPF_MFPH_PF8MFP_Msk) | SYS_GPF_MFPH_PF8MFP_GPIO;  // açılışta zaten böyle

// 2) MODE: PF->MODE[17:16] = 01 (çıkış)
GPIO_SetMode(PF, BIT8, GPIO_MODE_OUTPUT);       // aynısı: PF->MODE = (PF->MODE & ~(3u<<16)) | (1u<<16);

// 3) Değer: PF->DOUT bit 8
PF8 = 1;                                        // aynısı: PF->DOUT |= BIT8;
```

## Örnek: Buzzer (PC9 → PWM1_CH3)

```c
// PC9, MFPH'de, m = 9 mod 8 = 1 -> bitler [7:4], değer 0xC (PWM1_CH3)
SYS->GPC_MFPH = (SYS->GPC_MFPH & ~SYS_GPC_MFPH_PC9MFP_Msk) | SYS_GPC_MFPH_PC9MFP_PWM1_CH3;
```

## Debugger ile register okuma (çalışırken)

Port taban adresleri: PA `0x40004000`, PB `0x40004040`, PC `0x40004080`, PD `0x400040C0`, PE `0x40004100`, PF `0x40004140` (her port +0x40).
Ofsetler: MODE `+0x00`, DOUT `+0x08`, PIN `+0x10`. `SYS->GPx_MFPL` = `0x40000030 + 8·port`, `MFPH` = `+4`.

```bash
~/nuvoton/.venv/bin/pyocd commander -t m032kg8ae -c "read32 0x40004150 4"   # PF->PIN (LED'ler bit 6,7,8,14)
~/nuvoton/.venv/bin/pyocd commander -t m032kg8ae -c "read32 0x40004090 4"   # PC->PIN (butonlar bit 10,11,12)
```

