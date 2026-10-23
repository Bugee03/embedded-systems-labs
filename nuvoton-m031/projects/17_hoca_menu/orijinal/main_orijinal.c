#include "NuMicro.h"
#include <stdio.h>
#include <string.h>

// --- PIN TANIMLAMALARI ---
#define BTN_UP      (PB1)
#define BTN_DOWN    (PC10)
#define BTN_START   (PC11)
#define BTN_STOP    (PC12)

// LED Pin Tanimlamalari (PF14, PF6, PF7, PF8)
#define LED1        (PF14)
#define LED2        (PF6)
#define LED3        (PF7)
#define LED4        (PF8)
#define LED_PINS    (BIT14 | BIT6 | BIT7 | BIT8)

#define OLED_I2C_ADDR   0x3C // 0.91 inc SSD1306 standart I2C adresi (7-bit)

// --- NOTA FREKANS TANIMLARI (Hz) ---
#define NOTE_REST 0
#define NOTE_A3   220
#define NOTE_AS3  233
#define NOTE_B3   247
#define NOTE_C4   262
#define NOTE_CS4  277
#define NOTE_D4   294
#define NOTE_DS4  311
#define NOTE_Eb4  NOTE_DS4
#define NOTE_E4   330
#define NOTE_F4   349
#define NOTE_FS4  370
#define NOTE_G4   392
#define NOTE_GS4  415
#define NOTE_A4   440
#define NOTE_AS4  466
#define NOTE_Bb4  NOTE_AS4
#define NOTE_B4   494
#define NOTE_C5   523
#define NOTE_CS5  554
#define NOTE_D5   587
#define NOTE_DS5  622
#define NOTE_Eb5  NOTE_DS5
#define NOTE_E5   659
#define NOTE_F5   698
#define NOTE_FS5  740
#define NOTE_G5   784
#define NOTE_GS5  831
#define NOTE_A5   880
#define NOTE_AS5  932
#define NOTE_Bb5  NOTE_AS5
#define NOTE_B5   988

// --- DIL SEÇENEKLERI ---
typedef enum {
    LANG_TR = 0,
    LANG_EN = 1
} Language_t;

// --- MENÜ VE SISTEM DURUMLARI ---
typedef enum {
    PAGE_LANG_SELECT = 0,
    PAGE_MAIN_MENU,
    PAGE_MUSIC_MENU,
    PAGE_LED_MENU
} MenuPage_t;

typedef enum {
    MAIN_MENU_MUZIK = 0,
    MAIN_MENU_LED,
    MAIN_MENU_SENSORLER,
    MAIN_MENU_ITEM_COUNT
} MainMenuItem_t;

typedef enum {
    MUSIC_MARIO = 0,
    MUSIC_PIRATES,
    MUSIC_GODFATHER,
    MUSIC_IMPERIAL,
    MUSIC_ITEM_COUNT
} MusicMenuItem_t;

typedef enum {
    LED_MENU_ALL_ON = 0,
    LED_MENU_ALL_OFF,
    LED_MENU_CHASER,
    LED_MENU_TOGGLE_1, // PF14
    LED_MENU_TOGGLE_2, // PF6
    LED_MENU_TOGGLE_3, // PF7
    LED_MENU_TOGGLE_4, // PF8
    LED_MENU_ITEM_COUNT
} LedMenuItem_t;

volatile Language_t current_lang = LANG_TR;
volatile MenuPage_t current_page = PAGE_LANG_SELECT;

volatile uint8_t lang_menu_index = 0;
volatile uint8_t main_menu_index = MAIN_MENU_MUZIK;
volatile uint8_t music_menu_index = MUSIC_MARIO;
volatile uint8_t led_menu_index = LED_MENU_ALL_ON;

volatile uint8_t is_playing = 0;   // 0: Durdu, 1: Çaliyor
volatile uint8_t active_song = 0;  // 0: Mario, 1: Pirates, 2: Godfather, 3: Imperial March
volatile uint8_t led_chaser_active = 0;
uint8_t chaser_step = 0;

typedef struct {
    uint32_t frequency; // Hz
    uint32_t duration;  // ms
} Note_t;

// --- DÜZENLENMIS VE DÜZELTILMIS MELODI DIZILERI ---

// 1. MARIO MAIN THEME
const Note_t mario_melody[] = {
    {NOTE_E5, 120}, {NOTE_E5, 120}, {NOTE_REST, 120}, {NOTE_E5, 120},
    {NOTE_REST, 120}, {NOTE_C5, 120}, {NOTE_E5, 240}, {NOTE_G5, 240},
    {NOTE_REST, 240}, {NOTE_G4, 240}, {NOTE_REST, 240},

    {NOTE_C5, 360}, {NOTE_REST, 120}, {NOTE_G4, 360}, {NOTE_REST, 120},
    {NOTE_E4, 360}, {NOTE_REST, 120}, {NOTE_A4, 240}, {NOTE_B4, 240},
    {NOTE_AS4, 120}, {NOTE_A4, 240}, {NOTE_G4, 160}, {NOTE_E5, 160}, {NOTE_G5, 160},
    {NOTE_A5, 240}, {NOTE_F5, 120}, {NOTE_G5, 120}, {NOTE_REST, 120},
    {NOTE_E5, 240}, {NOTE_C5, 120}, {NOTE_D5, 120}, {NOTE_B4, 240}
};
#define MARIO_LENGTH (sizeof(mario_melody) / sizeof(Note_t))

// 2. PIRATES OF THE CARIBBEAN (He's a Pirate)
const Note_t pirates_melody[] = {
    {NOTE_D4, 125}, {NOTE_F4, 125}, {NOTE_G4, 250}, {NOTE_G4, 250}, {NOTE_G4, 125}, {NOTE_A4, 125},
    {NOTE_AS4, 250}, {NOTE_AS4, 250}, {NOTE_AS4, 125}, {NOTE_C5, 125}, {NOTE_A4, 250}, {NOTE_A4, 250},
    {NOTE_G4, 125}, {NOTE_F4, 125}, {NOTE_G4, 375}, {NOTE_REST, 125},

    {NOTE_D4, 125}, {NOTE_F4, 125}, {NOTE_G4, 250}, {NOTE_G4, 250}, {NOTE_G4, 125}, {NOTE_A4, 125},
    {NOTE_AS4, 250}, {NOTE_AS4, 250}, {NOTE_AS4, 125}, {NOTE_C5, 125}, {NOTE_A4, 250}, {NOTE_A4, 250},
    {NOTE_G4, 125}, {NOTE_F4, 125}, {NOTE_D4, 375}, {NOTE_REST, 125},

    {NOTE_D4, 125}, {NOTE_F4, 125}, {NOTE_G4, 250}, {NOTE_G4, 250}, {NOTE_G4, 125}, {NOTE_AS4, 125},
    {NOTE_C5, 250}, {NOTE_C5, 250}, {NOTE_C5, 125}, {NOTE_D5, 125}, {NOTE_D5, 250}, {NOTE_D5, 250},
    {NOTE_C5, 125}, {NOTE_AS4, 125}, {NOTE_C5, 250}, {NOTE_G4, 250}, {NOTE_REST, 125}
};
#define PIRATES_LENGTH (sizeof(pirates_melody) / sizeof(Note_t))

// 3. THE GODFATHER (Speak Softly, Love)
const Note_t godfather_melody[] = {
    {NOTE_C4, 200}, {NOTE_F4, 200}, {NOTE_GS4, 200}, {NOTE_G4, 200}, 
    {NOTE_F4, 200}, {NOTE_GS4, 200}, {NOTE_F4, 200}, {NOTE_G4, 200}, 
    {NOTE_F4, 200}, {NOTE_CS4, 200}, {NOTE_DS4, 200}, {NOTE_C4, 600},

    {NOTE_C4, 200}, {NOTE_F4, 200}, {NOTE_GS4, 200}, {NOTE_G4, 200}, 
    {NOTE_F4, 200}, {NOTE_GS4, 200}, {NOTE_F4, 200}, {NOTE_C5, 200}, 
    {NOTE_AS4, 200}, {NOTE_A4, 200}, {NOTE_AS4, 200}, {NOTE_G4, 600}, {NOTE_REST, 300}
};
#define GODFATHER_LENGTH (sizeof(godfather_melody) / sizeof(Note_t))

// 4. STAR WARS - IMPERIAL MARCH
const Note_t imperial_march_melody[] = {
    // Ana Intro Motif
    {NOTE_G4, 350}, {NOTE_G4, 350}, {NOTE_G4, 350}, 
    {NOTE_Eb4, 250}, {NOTE_Bb4, 100}, {NOTE_G4, 350}, 
    {NOTE_Eb4, 250}, {NOTE_Bb4, 100}, {NOTE_G4, 700},

    // Tiz Yükselis
    {NOTE_D5, 350}, {NOTE_D5, 350}, {NOTE_D5, 350}, 
    {NOTE_Eb5, 250}, {NOTE_Bb4, 100}, {NOTE_FS4, 350}, 
    {NOTE_Eb4, 250}, {NOTE_Bb4, 100}, {NOTE_G4, 700},

    // Koro Köprü Bölümü
    {NOTE_G5, 350}, {NOTE_G4, 220}, {NOTE_G4, 100}, 
    {NOTE_G5, 350}, {NOTE_FS5, 250}, {NOTE_F5, 120}, 
    {NOTE_E5, 120}, {NOTE_DS5, 120}, {NOTE_E5, 250}, {NOTE_REST, 150},

    {NOTE_GS4, 200}, {NOTE_CS5, 350}, {NOTE_C5, 250}, 
    {NOTE_B4, 120}, {NOTE_Bb4, 120}, {NOTE_A4, 120}, 
    {NOTE_Bb4, 250}, {NOTE_REST, 150},

    {NOTE_Eb4, 200}, {NOTE_FS4, 350}, {NOTE_Eb4, 250}, 
    {NOTE_Bb4, 100}, {NOTE_G4, 350}, {NOTE_Eb4, 250}, 
    {NOTE_Bb4, 100}, {NOTE_G4, 700}, {NOTE_REST, 400}
};
#define IMPERIAL_MARCH_LENGTH (sizeof(imperial_march_melody) / sizeof(Note_t))

uint8_t note_index = 0;

// --- DAHILI OLED SÜRÜCÜSÜ VE FONT MATRISI ---
static const uint8_t font8x8[][8] = {
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['!'] = {0x00, 0x00, 0x5F, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['-'] = {0x10, 0x10, 0x10, 0x10, 0x10, 0x10, 0x00, 0x00},
    ['.'] = {0x00, 0x00, 0x60, 0x60, 0x00, 0x00, 0x00, 0x00},
    ['/'] = {0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00},
    [':'] = {0x00, 0x00, 0x36, 0x36, 0x00, 0x00, 0x00, 0x00},
    ['>'] = {0x42, 0x24, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['['] = {0x7F, 0x41, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00},
    [']'] = {0x00, 0x00, 0x41, 0x41, 0x7F, 0x00, 0x00, 0x00},
    
    // Rakamlar
    ['0'] = {0x3E, 0x51, 0x49, 0x45, 0x3E, 0x00, 0x00, 0x00},
    ['1'] = {0x00, 0x42, 0x7F, 0x40, 0x00, 0x00, 0x00, 0x00},
    ['2'] = {0x42, 0x61, 0x51, 0x49, 0x46, 0x00, 0x00, 0x00},
    ['3'] = {0x21, 0x41, 0x45, 0x4B, 0x31, 0x00, 0x00, 0x00},
    ['4'] = {0x18, 0x14, 0x12, 0x7F, 0x10, 0x00, 0x00, 0x00},
    ['5'] = {0x27, 0x45, 0x45, 0x45, 0x39, 0x00, 0x00, 0x00},
    ['6'] = {0x3C, 0x4A, 0x49, 0x49, 0x30, 0x00, 0x00, 0x00},
    ['7'] = {0x01, 0x71, 0x09, 0x05, 0x03, 0x00, 0x00, 0x00},
    ['8'] = {0x36, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00},
    ['9'] = {0x06, 0x49, 0x49, 0x29, 0x1E, 0x00, 0x00, 0x00},

    // Harfler
    ['A'] = {0x7C, 0x12, 0x11, 0x12, 0x7C, 0x00, 0x00, 0x00},
    ['B'] = {0x7F, 0x49, 0x49, 0x49, 0x36, 0x00, 0x00, 0x00},
    ['C'] = {0x3E, 0x41, 0x41, 0x41, 0x22, 0x00, 0x00, 0x00},
    ['D'] = {0x7F, 0x41, 0x41, 0x22, 0x1C, 0x00, 0x00, 0x00},
    ['E'] = {0x7F, 0x49, 0x49, 0x49, 0x41, 0x00, 0x00, 0x00},
    ['F'] = {0x7F, 0x09, 0x09, 0x09, 0x01, 0x00, 0x00, 0x00},
    ['G'] = {0x3E, 0x41, 0x49, 0x49, 0x7A, 0x00, 0x00, 0x00},
    ['H'] = {0x7F, 0x08, 0x08, 0x08, 0x7F, 0x00, 0x00, 0x00},
    ['I'] = {0x00, 0x41, 0x7F, 0x41, 0x00, 0x00, 0x00, 0x00},
    ['J'] = {0x20, 0x40, 0x41, 0x3F, 0x01, 0x00, 0x00, 0x00},
    ['K'] = {0x7F, 0x08, 0x14, 0x22, 0x41, 0x00, 0x00, 0x00},
    ['L'] = {0x7F, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00},
    ['M'] = {0x7F, 0x02, 0x0C, 0x02, 0x7F, 0x00, 0x00, 0x00},
    ['N'] = {0x7F, 0x04, 0x08, 0x10, 0x7F, 0x00, 0x00, 0x00},
    ['O'] = {0x3E, 0x41, 0x41, 0x41, 0x3E, 0x00, 0x00, 0x00},
    ['P'] = {0x7F, 0x09, 0x09, 0x09, 0x06, 0x00, 0x00, 0x00},
    ['R'] = {0x7F, 0x09, 0x19, 0x29, 0x46, 0x00, 0x00, 0x00},
    ['S'] = {0x26, 0x49, 0x49, 0x49, 0x32, 0x00, 0x00, 0x00},
    ['T'] = {0x01, 0x01, 0x7F, 0x01, 0x01, 0x00, 0x00, 0x00},
    ['U'] = {0x3F, 0x40, 0x40, 0x40, 0x3F, 0x00, 0x00, 0x00},
    ['V'] = {0x1F, 0x20, 0x40, 0x20, 0x1F, 0x00, 0x00, 0x00},
    ['W'] = {0x7F, 0x20, 0x18, 0x20, 0x7F, 0x00, 0x00, 0x00},
    ['Y'] = {0x07, 0x08, 0x70, 0x08, 0x07, 0x00, 0x00, 0x00},
    ['Z'] = {0x61, 0x51, 0x49, 0x45, 0x43, 0x00, 0x00, 0x00},
    ['a'] = {0x20, 0x54, 0x54, 0x54, 0x78, 0x00, 0x00, 0x00},
    ['b'] = {0x7F, 0x48, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00},
    ['c'] = {0x38, 0x44, 0x44, 0x44, 0x20, 0x00, 0x00, 0x00},
    ['d'] = {0x38, 0x44, 0x44, 0x48, 0x7F, 0x00, 0x00, 0x00},
    ['e'] = {0x38, 0x54, 0x54, 0x54, 0x18, 0x00, 0x00, 0x00},
    ['f'] = {0x08, 0x7E, 0x09, 0x01, 0x02, 0x00, 0x00, 0x00},
    ['g'] = {0x08, 0x54, 0x54, 0x54, 0x3C, 0x00, 0x00, 0x00},
    ['h'] = {0x7F, 0x08, 0x04, 0x04, 0x78, 0x00, 0x00, 0x00},
    ['i'] = {0x00, 0x44, 0x7D, 0x40, 0x00, 0x00, 0x00, 0x00},
    ['k'] = {0x7F, 0x10, 0x28, 0x44, 0x00, 0x00, 0x00, 0x00},
    ['l'] = {0x00, 0x41, 0x7F, 0x40, 0x00, 0x00, 0x00, 0x00},
    ['m'] = {0x7C, 0x04, 0x18, 0x04, 0x78, 0x00, 0x00, 0x00},
    ['n'] = {0x7C, 0x08, 0x04, 0x04, 0x78, 0x00, 0x00, 0x00},
    ['o'] = {0x38, 0x44, 0x44, 0x44, 0x38, 0x00, 0x00, 0x00},
    ['p'] = {0x7C, 0x14, 0x14, 0x14, 0x08, 0x00, 0x00, 0x00},
    ['r'] = {0x7C, 0x08, 0x04, 0x04, 0x08, 0x00, 0x00, 0x00},
    ['s'] = {0x48, 0x54, 0x54, 0x54, 0x24, 0x00, 0x00, 0x00},
    ['t'] = {0x04, 0x3E, 0x44, 0x24, 0x00, 0x00, 0x00, 0x00},
    ['u'] = {0x3C, 0x40, 0x40, 0x20, 0x7C, 0x00, 0x00, 0x00},
    ['w'] = {0x3C, 0x40, 0x30, 0x40, 0x3C, 0x00, 0x00, 0x00},
    ['y'] = {0x0C, 0x50, 0x50, 0x50, 0x3C, 0x00, 0x00, 0x00},
    ['z'] = {0x44, 0x64, 0x54, 0x4C, 0x44, 0x00, 0x00, 0x00}
};

void OLED_WriteCmd(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    I2C_WriteMultiBytes(I2C0, OLED_I2C_ADDR, buf, 2);
}

void OLED_WriteData(uint8_t *data, uint16_t len) {
    uint8_t buf[129];
    buf[0] = 0x40;
    memcpy(&buf[1], data, len);
    I2C_WriteMultiBytes(I2C0, OLED_I2C_ADDR, buf, len + 1);
}

void OLED_SetPos(uint8_t x, uint8_t page) {
    OLED_WriteCmd(0xB0 + page);
    OLED_WriteCmd(((x & 0xF0) >> 4) | 0x10);
    OLED_WriteCmd(x & 0x0F);
}

void OLED_Clear(void) {
    uint8_t zero_buf[128] = {0};
    uint8_t page;
    for (page = 0; page < 4; page++) {
        OLED_SetPos(0, page);
        OLED_WriteData(zero_buf, 128);
    }
}

void OLED_PutChar(uint8_t x, uint8_t page, char ch) {
    OLED_SetPos(x, page);
    OLED_WriteData((uint8_t*)font8x8[(uint8_t)ch], 8);
}

void OLED_PrintStr(uint8_t x, uint8_t page, const char *str) {
    while (*str) {
        OLED_PutChar(x, page, *str);
        x += 8;
        if (x > 120) break;
        str++;
    }
}

void OLED_Init(void) {
    CLK_SysTickDelay(100000);
    OLED_WriteCmd(0xAE);
    OLED_WriteCmd(0xD5); OLED_WriteCmd(0x80);
    OLED_WriteCmd(0xA8); OLED_WriteCmd(0x1F);
    OLED_WriteCmd(0xD3); OLED_WriteCmd(0x00);
    OLED_WriteCmd(0x40);
    OLED_WriteCmd(0x8D); OLED_WriteCmd(0x14);
    OLED_WriteCmd(0x20); OLED_WriteCmd(0x02);
    OLED_WriteCmd(0xA1);
    OLED_WriteCmd(0xC8);
    OLED_WriteCmd(0xDA); OLED_WriteCmd(0x02);
    OLED_WriteCmd(0x81); OLED_WriteCmd(0x8F);
    OLED_WriteCmd(0xD9); OLED_WriteCmd(0xF1);
    OLED_WriteCmd(0xDB); OLED_WriteCmd(0x40);
    OLED_WriteCmd(0xA4);
    OLED_WriteCmd(0xA6);
    OLED_WriteCmd(0xAF);
    OLED_Clear();
}

void SYS_Init(void) {
    SYS_UnlockReg();
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    CLK_EnableModuleClock(I2C0_MODULE);
    CLK_EnableModuleClock(PWM1_MODULE);
    CLK_SetModuleClock(PWM1_MODULE, CLK_CLKSEL2_PWM1SEL_PCLK1, 0);

    SYS->GPB_MFPH = SYS_GPB_MFPH_PB13MFP_UART0_TXD | SYS_GPB_MFPH_PB12MFP_UART0_RXD;
    SYS->GPC_MFPH = SYS_GPC_MFPH_PC9MFP_PWM1_CH3 | SYS_GPC_MFPH_PC8MFP_I2C0_SDA;
    SYS->GPE_MFPH = SYS_GPE_MFPH_PE13MFP_I2C0_SCL;
    SYS->GPF_MFPL = SYS_GPF_MFPL_PF1MFP_ICE_CLK | SYS_GPF_MFPL_PF0MFP_ICE_DAT;

    SYS_LockReg();
}

void GPIO_Init(void) {
    GPIO_SetMode(PB, BIT1, GPIO_MODE_INPUT);
    GPIO_SetMode(PC, BIT10 | BIT11 | BIT12, GPIO_MODE_INPUT);

    GPIO_SetMode(PF, LED_PINS, GPIO_MODE_OUTPUT);
    LED1 = 0; LED2 = 0; LED3 = 0; LED4 = 0;
}

void OLED_I2C_Init(void) {
    I2C_Open(I2C0, 400000);
    OLED_Init();
}

void Buzzer_PWM_Init(void) {
    PWM_ConfigOutputChannel(PWM1, 3, 1000, 0);
    PWM_EnableOutput(PWM1, PWM_CH_3_MASK);
    PWM_Start(PWM1, PWM_CH_3_MASK);
}

void Play_Note(uint32_t freq) {
    if (freq == 0) {
        PWM_ConfigOutputChannel(PWM1, 3, 1000, 0);
    } else {
        PWM_ConfigOutputChannel(PWM1, 3, freq, 50);
    }
}

void Stop_Music(void) {
    is_playing = 0;
    note_index = 0;
    PWM_ConfigOutputChannel(PWM1, 3, 1000, 0);
    
    // Müzik durdugunda LED'leri kapat
    LED1 = 0; LED2 = 0; LED3 = 0; LED4 = 0;
}

void Update_OLED_Menu(void) {
    OLED_Clear();

    if (current_page == PAGE_LANG_SELECT) {
        OLED_PrintStr(0, 0, "Select Language:");
        OLED_PrintStr(0, 1, (lang_menu_index == 0) ? "> 1. Turkce"  : "  1. Turkce");
        OLED_PrintStr(0, 2, (lang_menu_index == 1) ? "> 2. English" : "  2. English");
        OLED_PrintStr(0, 3, "[START: Select]");
    }
    else if (current_page == PAGE_MAIN_MENU) {
        if (current_lang == LANG_TR) {
            OLED_PrintStr(0, 0, (main_menu_index == MAIN_MENU_MUZIK)    ? "> 1. Muzik Menusu" : "  1. Muzik Menusu");
            OLED_PrintStr(0, 1, (main_menu_index == MAIN_MENU_LED)      ? "> 2. LED Kontrol"  : "  2. LED Kontrol");
            OLED_PrintStr(0, 2, (main_menu_index == MAIN_MENU_SENSORLER)? "> 3. Sensorler"    : "  3. Sensorler");
            OLED_PrintStr(0, 3, is_playing ? "[Muzik Caliyor]" : "[Durdu]");
        } else {
            OLED_PrintStr(0, 0, (main_menu_index == MAIN_MENU_MUZIK)    ? "> 1. Music Menu"   : "  1. Music Menu");
            OLED_PrintStr(0, 1, (main_menu_index == MAIN_MENU_LED)      ? "> 2. LED Control"  : "  2. LED Control");
            OLED_PrintStr(0, 2, (main_menu_index == MAIN_MENU_SENSORLER)? "> 3. Sensors"      : "  3. Sensors");
            OLED_PrintStr(0, 3, is_playing ? "[Playing Music]" : "[Stopped]");
        }
    } 
    else if (current_page == PAGE_MUSIC_MENU) {
        if (music_menu_index < 3) {
            OLED_PrintStr(0, 0, (music_menu_index == MUSIC_MARIO)    ? "> 1. Mario Theme"  : "  1. Mario Theme");
            OLED_PrintStr(0, 1, (music_menu_index == MUSIC_PIRATES)  ? (current_lang == LANG_TR ? "> 2. Karayip Kors." : "> 2. Pirates Theme") : (current_lang == LANG_TR ? "  2. Karayip Kors." : "  2. Pirates Theme"));
            OLED_PrintStr(0, 2, (music_menu_index == MUSIC_GODFATHER)? "> 3. Godfather"    : "  3. Godfather");
        } else {
            OLED_PrintStr(0, 0, (music_menu_index == MUSIC_PIRATES)  ? (current_lang == LANG_TR ? "> 2. Karayip Kors." : "> 2. Pirates Theme") : (current_lang == LANG_TR ? "  2. Karayip Kors." : "  2. Pirates Theme"));
            OLED_PrintStr(0, 1, (music_menu_index == MUSIC_GODFATHER)? "> 3. Godfather"    : "  3. Godfather");
            OLED_PrintStr(0, 2, (music_menu_index == MUSIC_IMPERIAL) ? "> 4. Imperial March": "  4. Imperial March");
        }

        if (is_playing) {
            if (active_song == MUSIC_MARIO) OLED_PrintStr(0, 3, (current_lang == LANG_TR) ? "[Mario Caliyor!]" : "[Mario Playing]");
            else if (active_song == MUSIC_PIRATES) OLED_PrintStr(0, 3, (current_lang == LANG_TR) ? "[Korsan Caliyor]" : "[Pirates Play]");
            else if (active_song == MUSIC_GODFATHER) OLED_PrintStr(0, 3, "[Godfather]");
            else if (active_song == MUSIC_IMPERIAL) OLED_PrintStr(0, 3, "[Imperial March]");
        } else {
            OLED_PrintStr(0, 3, (current_lang == LANG_TR) ? "[Durdu]" : "[Stopped]");
        }
    }
    else if (current_page == PAGE_LED_MENU) {
        char buff[20];
        if (led_menu_index < 3) {
            if (current_lang == LANG_TR) {
                OLED_PrintStr(0, 0, (led_menu_index == LED_MENU_ALL_ON)  ? "> Hepsi ACIK" : "  Hepsi ACIK");
                OLED_PrintStr(0, 1, (led_menu_index == LED_MENU_ALL_OFF) ? "> Hepsi KAPALI" : "  Hepsi KAPALI");
                OLED_PrintStr(0, 2, (led_menu_index == LED_MENU_CHASER)  ? "> Yuruyen Isik" : "  Yuruyen Isik");
                sprintf(buff, "LED1(PF14): %s", LED1 ? "ACIK " : "KAPALI");
            } else {
                OLED_PrintStr(0, 0, (led_menu_index == LED_MENU_ALL_ON)  ? "> All ON" : "  All ON");
                OLED_PrintStr(0, 1, (led_menu_index == LED_MENU_ALL_OFF) ? "> All OFF" : "  All OFF");
                OLED_PrintStr(0, 2, (led_menu_index == LED_MENU_CHASER)  ? "> Chaser Mode" : "  Chaser Mode");
                sprintf(buff, "LED1(PF14): %s", LED1 ? "ON  " : "OFF ");
            }
            OLED_PrintStr(0, 3, buff);
        } else {
            if (current_lang == LANG_TR) {
                sprintf(buff, "%sLED1(PF14): %s", (led_menu_index == LED_MENU_TOGGLE_1) ? ">" : " ", LED1 ? "ACIK " : "KAPALI");
                OLED_PrintStr(0, 0, buff);
                sprintf(buff, "%sLED2(PF6) : %s", (led_menu_index == LED_MENU_TOGGLE_2) ? ">" : " ", LED2 ? "ACIK " : "KAPALI");
                OLED_PrintStr(0, 1, buff);
                sprintf(buff, "%sLED3(PF7) : %s", (led_menu_index == LED_MENU_TOGGLE_3) ? ">" : " ", LED3 ? "ACIK " : "KAPALI");
                OLED_PrintStr(0, 2, buff);
                sprintf(buff, "%sLED4(PF8) : %s", (led_menu_index == LED_MENU_TOGGLE_4) ? ">" : " ", LED4 ? "ACIK " : "KAPALI");
                OLED_PrintStr(0, 3, buff);
            } else {
                sprintf(buff, "%sLED1(PF14): %s", (led_menu_index == LED_MENU_TOGGLE_1) ? ">" : " ", LED1 ? "ON  " : "OFF ");
                OLED_PrintStr(0, 0, buff);
                sprintf(buff, "%sLED2(PF6) : %s", (led_menu_index == LED_MENU_TOGGLE_2) ? ">" : " ", LED2 ? "ON  " : "OFF ");
                OLED_PrintStr(0, 1, buff);
                sprintf(buff, "%sLED3(PF7) : %s", (led_menu_index == LED_MENU_TOGGLE_3) ? ">" : " ", LED3 ? "ON  " : "OFF ");
                OLED_PrintStr(0, 2, buff);
                sprintf(buff, "%sLED4(PF8) : %s", (led_menu_index == LED_MENU_TOGGLE_4) ? ">" : " ", LED4 ? "ON  " : "OFF ");
                OLED_PrintStr(0, 3, buff);
            }
        }
    }
}

void Process_Buttons(void) {
    if (BTN_UP == 0) {
        CLK_SysTickDelay(50000);
        if (BTN_UP == 0) {
            if (current_page == PAGE_LANG_SELECT) {
                if (lang_menu_index > 0) lang_menu_index--;
            } else if (current_page == PAGE_MAIN_MENU) {
                if (main_menu_index > 0) main_menu_index--;
            } else if (current_page == PAGE_MUSIC_MENU) {
                if (music_menu_index > 0) music_menu_index--;
            } else if (current_page == PAGE_LED_MENU) {
                if (led_menu_index > 0) led_menu_index--;
            }
            Update_OLED_Menu();
            while(BTN_UP == 0);
        }
    }

    if (BTN_DOWN == 0) {
        CLK_SysTickDelay(50000);
        if (BTN_DOWN == 0) {
            if (current_page == PAGE_LANG_SELECT) {
                if (lang_menu_index < 1) lang_menu_index++;
            } else if (current_page == PAGE_MAIN_MENU) {
                if (main_menu_index < MAIN_MENU_ITEM_COUNT - 1) main_menu_index++;
            } else if (current_page == PAGE_MUSIC_MENU) {
                if (music_menu_index < MUSIC_ITEM_COUNT - 1) music_menu_index++;
            } else if (current_page == PAGE_LED_MENU) {
                if (led_menu_index < LED_MENU_ITEM_COUNT - 1) led_menu_index++;
            }
            Update_OLED_Menu();
            while(BTN_DOWN == 0);
        }
    }

    if (BTN_START == 0) {
        CLK_SysTickDelay(50000);
        if (BTN_START == 0) {
            if (current_page == PAGE_LANG_SELECT) {
                current_lang = (lang_menu_index == 0) ? LANG_TR : LANG_EN;
                current_page = PAGE_MAIN_MENU;
            }
            else if (current_page == PAGE_MAIN_MENU) {
                if (main_menu_index == MAIN_MENU_MUZIK) {
                    current_page = PAGE_MUSIC_MENU;
                } else if (main_menu_index == MAIN_MENU_LED) {
                    current_page = PAGE_LED_MENU;
                }
            } 
            else if (current_page == PAGE_MUSIC_MENU) {
                active_song = music_menu_index;
                is_playing = 1;
                note_index = 0;
            }
            else if (current_page == PAGE_LED_MENU) {
                led_chaser_active = 0;
                switch (led_menu_index) {
                    case LED_MENU_ALL_ON:   LED1 = 1; LED2 = 1; LED3 = 1; LED4 = 1; break;
                    case LED_MENU_ALL_OFF:  LED1 = 0; LED2 = 0; LED3 = 0; LED4 = 0; break;
                    case LED_MENU_CHASER:   led_chaser_active = 1; chaser_step = 0; break;
                    case LED_MENU_TOGGLE_1: LED1 ^= 1; break;
                    case LED_MENU_TOGGLE_2: LED2 ^= 1; break;
                    case LED_MENU_TOGGLE_3: LED3 ^= 1; break;
                    case LED_MENU_TOGGLE_4: LED4 ^= 1; break;
                }
            }
            Update_OLED_Menu();
            while(BTN_START == 0);
        }
    }

    if (BTN_STOP == 0) {
        CLK_SysTickDelay(50000);
        if (BTN_STOP == 0) {
            if (is_playing) {
                Stop_Music();
            } else if (led_chaser_active) {
                led_chaser_active = 0;
                LED1 = 0; LED2 = 0; LED3 = 0; LED4 = 0;
            } else if (current_page == PAGE_MAIN_MENU) {
                current_page = PAGE_LANG_SELECT;
            } else if (current_page != PAGE_LANG_SELECT) {
                current_page = PAGE_MAIN_MENU;
            }
            Update_OLED_Menu();
            while(BTN_STOP == 0);
        }
    }
}

// --- MELODIYE SENKRON LED FLASÖR VE DINAMIK RITIM GÖREVI ---
void Process_Music_Task(void) {
    if (!is_playing) return;

    const Note_t *current_melody_ptr = NULL;
    uint32_t current_length = 0;

    if (active_song == MUSIC_MARIO) {
        current_melody_ptr = mario_melody;
        current_length = MARIO_LENGTH;
    } else if (active_song == MUSIC_PIRATES) {
        current_melody_ptr = pirates_melody;
        current_length = PIRATES_LENGTH;
    } else if (active_song == MUSIC_GODFATHER) {
        current_melody_ptr = godfather_melody;
        current_length = GODFATHER_LENGTH;
    } else if (active_song == MUSIC_IMPERIAL) {
        current_melody_ptr = imperial_march_melody;
        current_length = IMPERIAL_MARCH_LENGTH;
    }

    if (current_melody_ptr != NULL) {
        uint32_t freq = current_melody_ptr[note_index].frequency;
        uint32_t duration = current_melody_ptr[note_index].duration;

        // --- FREKANSA GÖRE LED FLASÖR MANTIGI ---
        if (freq == NOTE_REST) {
            LED1 = 0; LED2 = 0; LED3 = 0; LED4 = 0;
        } else if (freq < 350) { 
            LED1 = 1; LED2 = 0; LED3 = 0; LED4 = 0;
        } else if (freq < 500) { 
            LED1 = 1; LED2 = 1; LED3 = 0; LED4 = 0;
        } else if (freq < 700) { 
            LED1 = 1; LED2 = 1; LED3 = 1; LED4 = 0;
        } else {                 
            LED1 = 1; LED2 = 1; LED3 = 1; LED4 = 1;
        }

        // Dynamic Staccato: %85 Ses, %15 Notalar Arasi Es
        uint32_t play_duration = (duration * 85) / 100;
        uint32_t rest_duration = duration - play_duration;

        // Notayi çal
        Play_Note(freq);
        CLK_SysTickDelay(play_duration * 1000);

        // Staccato kesintisi (Ritim vurgularini netlestirmek için)
        Play_Note(0);
        LED1 = 0; LED2 = 0; LED3 = 0; LED4 = 0;
        CLK_SysTickDelay(rest_duration * 1000);

        note_index++;
        if (note_index >= current_length) {
            note_index = 0;
        }
    }
}

void Process_LED_Task(void) {
    if (!led_chaser_active) return;

    LED1 = (chaser_step == 0);
    LED2 = (chaser_step == 1);
    LED3 = (chaser_step == 2);
    LED4 = (chaser_step == 3);

    CLK_SysTickDelay(150000);

    chaser_step++;
    if (chaser_step >= 4) {
        chaser_step = 0;
    }
}

// --- MAIN ---
int main(void) {
    SYS_Init();

    GPIO_Init();
    OLED_I2C_Init();
    Buzzer_PWM_Init();

    Update_OLED_Menu();

    while (1) {
        Process_Buttons();
        Process_Music_Task();
        Process_LED_Task();
    }
}