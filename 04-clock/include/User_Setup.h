// User_Setup.h — TFT_eSPI display configuration
// Uncomment ONE display type below. Comment out the other.
//
// NOTE: This file is loaded by TFT_eSPI when -DUSER_SETUP_LOADED=1 is set
//       in platformio.ini build_flags.

// ============================================================
// Option A: ST7789 (1.3" 240x240 / 2" 240x320)
// ============================================================
#define ST7789_DRIVER
#define TFT_WIDTH  240
// For 240x240 square displays (1.3"):
// #define TFT_HEIGHT 240
// For 240x320 rectangular displays (2"):
#define TFT_HEIGHT 320

// ============================================================
// Option B: ST7735 (1.8" 128x160)
// ============================================================
// #define ST7735_DRIVER
// #define TFT_WIDTH  128
// #define TFT_HEIGHT 160
// #define ST7735_GREENTAB3  // Adjust based on your display variant
// #define ST7735_INITB       // Some red boards need this

// ============================================================
// SPI Pin Configuration (ESP32)
// ============================================================
#define TFT_CS   5   // Chip select
#define TFT_DC   17  // Data/Command
#define TFT_RST  16  // Reset
#define TFT_MOSI 23  // Master Out Slave In
#define TFT_SCLK 18  // SPI Clock
#define TFT_MISO 19  // Master In Slave Out (optional, read not needed)

// Touch screen (not used, but define to avoid compile warnings)
#define TOUCH_CS -1

// ============================================================
// Backlight control (optional)
// ============================================================
#define TFT_BL   4   // Backlight (PWM-capable)

// ============================================================
// SPI frequency
// ============================================================
#define SPI_FREQUENCY   40000000  // 40 MHz
#define SPI_READ_FREQUENCY  20000000

// ============================================================
// Font & rendering options
// ============================================================
#define SPI_READ_FREQUENCY  20000000
#define SMOOTH_FONT         1       // Enable smooth font support

// ============================================================
// Rotation (0-3)
// ============================================================
// We'll set this at runtime in DisplayManager
