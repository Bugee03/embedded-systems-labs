# 02: Butonlar (GPIO giriş + kesme + donanımsal debounce)

## Bağlantı
Harici bağlantı gerekmez. Karttaki butonlar: **SW2 = PC12, SW3 = PC11, SW4 = PC10, SW5 = PB1**. Kartta 4.7k pull-up var, buton basılıyken pin 0 okunur.
LED'ler: SW2 → LED5 (PF8), SW3 → LED6 (PF7), SW4 → LED7 (PF6), SW5 → LED8 (PF14)

## Çalıştır
```bash
cd ~/nuvoton/projects/02_buton
make flash        # derle + karta yükle
make monitor      # seri port çıktısı (çıkış: Ctrl+A, K, Y)
```

## Beklenen sonuç
Her butona basınca kendi LED'i yanar ya da söner. Seri portta `SW3 -> LED6=1` gibi satırlar yazar.

## Kod nasıl çalışıyor?
- Kartta **dış pull-up** olduğu için pinler normal `GPIO_MODE_INPUT` yapılır. (Dış pull-up olmasaydı: M031/M032'de `GPIO_SetPullCtl` **yoktur**, onun yerine `GPIO_MODE_QUASI` kullanılır.)
- `GPIO_SET_DEBOUNCE_TIME` + `GPIO_ENABLE_DEBOUNCE` donanımsal debounce yapar.
- `GPIO_EnableInt(PC, 12, GPIO_INT_FALLING)` 1 → 0 geçişinde kesme üretir.
- **İki ayrı kesme var:** PC pinleri (SW2–SW4) `GPCDEF_IRQHandler` + `GPIO_PCPDPEPF_IRQn` kullanır. PB1 (SW5) ise `GPABGH_IRQHandler` + `GPIO_PAPB_IRQn` kullanır. Fonksiyon adı yanlış yazılırsa derleme hata vermez ama kesme hiç çalışmaz.
- Kesmenin içinde flag mutlaka `GPIO_CLR_INT_FLAG` ile temizlenir.
- ISR hangi butona basıldığını `g_basilan` bit maskesine yazar, main bunu okur ve sıfırlar. Okuma ile sıfırlama arasında kesme girmesin diye `__disable_irq()` / `__enable_irq()` kullanılır.

## Kendin dene
- Debounce'u kapat ve tek basışta birden fazla sayım olduğunu gör.
- Kesmesiz (polling) yöntemle yaz: `if (PC12 == 0) {...}`
- SW2 ile LED'leri hızlandır, SW3 ile yavaşlat (04_timer ile birleştir).

> Pinler Boardoza M032KG şemasına göredir (`~/nuvoton/Boardoza_M032KG_sema.pdf`). Bir pinin hangi fonksiyonlara (MFP) bağlanabildiğini görmek için:
> `grep -o "SYS_GP._MFP._P..MFP_[A-Z0-9_]*" ~/nuvoton/M031BSP/Library/StdDriver/inc/sys.h | grep PA5`
