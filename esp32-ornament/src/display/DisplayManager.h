#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include <TFT_eSPI.h>
#include <TFT_eWidget.h>  // Button, Graph, etc.
#include "config.h"

// Screen orientation
enum ScreenOrientation {
    PORTRAIT = 0,   // USB port on left/right
    LANDSCAPE = 1,  // USB port on bottom
    PORTRAIT_FLIP = 2,
    LANDSCAPE_FLIP = 3
};

class DisplayManager {
public:
    DisplayManager();
    ~DisplayManager();

    // Initialize TFT, backlight, fonts
    bool begin(ScreenOrientation orientation = LANDSCAPE);

    // Clear screen
    void clear(uint16_t color = TFT_BLACK);
    void fillScreen(uint16_t color);

    // -- Drawing helpers --
    void drawStatusBar(int pageIndex, int totalPages, const char* timeStr, bool wifiConnected, bool forceRedraw);

    // Draw centered text with a specific font
    void drawCenteredText(const char* text, int y, uint8_t fontId = 1, uint16_t color = TFT_WHITE);
    void drawCenteredText(const char* text, int y, const uint8_t* font, uint16_t color = TFT_WHITE);

    // Draw large number (48px) centered
    void drawLargeNumber(const char* numStr, int y, uint16_t color = TFT_WHITE);

    // Draw label + value line
    void drawInfoLine(const char* label, const char* value, int y, uint16_t labelColor = TFT_CYAN, uint16_t valueColor = TFT_WHITE);

    // Draw Chinese text (via smooth font)
    void drawChinese(const char* text, int x, int y, uint8_t fontIndex = 0, uint16_t color = TFT_WHITE);

    // Draw page indicator dots at bottom
    void drawPageDots(int current, int total, int y);

    // -- Sprite / framebuffer helpers (for animation) --
    TFT_eSprite& createSprite(int w, int h);
    void pushSprite(TFT_eSprite& sprite, int x, int y);
    void deleteSprite(TFT_eSprite& sprite);

    // -- Backlight control --
    void setBrightness(uint8_t brightness);  // 0-255
    void fadeIn(int ms = 500);
    void fadeOut(int ms = 500);
    void breatheEffect();  // One breathe cycle

    // -- Animated transitions --
    enum TransitionEffect {
        FADE_BLACK = 0,
        SLIDE_LEFT,
        SLIDE_RIGHT,
        NONE
    };
    void transition(TransitionEffect effect, void (*renderPage)(TFT_eSprite&), int spriteW, int spriteH);

    // -- Error / notification overlay --
    void showNotification(const char* msg, int durationMs = 1500, uint16_t bgColor = TFT_DARKGREY);

    // Access to the underlying TFT (for advanced use)
    TFT_eSPI& getTFT() { return _tft; }

    // Color helpers
    static uint16_t heatColor(float value, float min, float max);  // Blue → Red gradient

    // Dimensions
    int width() const { return _tft.width(); }
    int height() const { return _tft.height(); }

private:
    TFT_eSPI _tft;
    bool _fontsLoaded;

    // Smooth font indices
    uint8_t _fontSmall;   // 24px for body Chinese
    uint8_t _fontLarge;   // 48px for big numbers

    // Status bar internal state (to avoid redrawing unchanged parts)
    char _lastStatusTime[16];
    int _lastStatusPage;
    bool _lastWifiStatus;

    // Load smooth fonts from LittleFS
    bool _loadFonts();
};

#endif // DISPLAY_MANAGER_H
