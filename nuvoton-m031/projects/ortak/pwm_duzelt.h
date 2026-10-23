/*
 * ortak/pwm_duzelt.h - PWM_ConfigOutputChannel() icin duzeltme
 *
 * SORUN: BSP'deki PWM_ConfigOutputChannel() (M031/M032, pwm.c) dalga sekli icin sadece
 *   "asagi sayimda CMR'ye esitlenince -> HIGH" ayarini yapiyor; "sifir noktasinda -> LOW"
 *   ayarini ise SILIYOR. Pin bir kez HIGH olunca bir daha LOW olmaz -> cikis hep %100.
 *   Belirti: RGB LED duty ne olursa olsun BEYAZ yanar, buzzer ses vermez.
 *
 * COZUM: Her PWM_ConfigOutputChannel() cagrisindan sonra cikis seviyelerini acikca ayarla
 *   (Nuvoton'un PWM_DutySwitch ornegi ve hocanin Bluetooth_Kontrol kodu da boyle yapar):
 *     sifir noktasi   -> LOW
 *     asagi esitlenme -> HIGH      => duty = CMR / (CNR + 1)
 *   %0 duty'de CMR=0 olur ve iki olay ayni anda gelir; bu yuzden %0 icin cikis maskelenir (LOW).
 *
 * Kullanim: PWM_ConfigOutputChannel(...) yerine PWM_Kanal_Ayarla(...) yaz.
 */
#ifndef PWM_DUZELT_H
#define PWM_DUZELT_H
#include "NuMicro.h"

static inline uint32_t PWM_Kanal_Ayarla(PWM_T *pwm, uint32_t kanal, uint32_t frekans, uint32_t duty)
{
    uint32_t maske = 1UL << kanal;
    uint32_t gercek_frekans = PWM_ConfigOutputChannel(pwm, kanal, frekans, duty);

    PWM_SET_OUTPUT_LEVEL(pwm, maske, PWM_OUTPUT_LOW, PWM_OUTPUT_NOTHING, PWM_OUTPUT_NOTHING, PWM_OUTPUT_HIGH);

    if (duty == 0) { pwm->MSK &= ~maske; pwm->MSKEN |= maske; }   /* %0: cikisi LOW'a kilitle */
    else           { pwm->MSKEN &= ~maske; }

    return gercek_frekans;
}
#endif
