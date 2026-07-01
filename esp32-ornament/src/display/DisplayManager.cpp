#include "DisplayManager.h"

DisplayManager::DisplayManager()
    : _fontsLoaded(false)
    , _fontSmall(0)
    , _fontLarge(1)
    , _lastStatusPage(-1)
    , _lastWifiStatus(false)
{
    _lastStatusTime[0] = '\0';
}

DisplayManager::~DisplayManager() {}

bool DisplayManager::begin(ScreenOrientation orientation) {
    _tft.begin();
    _tft.setRotation((uint8_t)orientation);
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextWrap(false);  // Don't wrap text, clip instead
    _tft.setViewport(0, 0, _tft.width(), _tft.height());

    // Configure backlight PWM
    ledcSetup(TFT_BL_CHANNEL, 5000, 8);  // 5kHz, 8-bit resolution
    ledcAttachPin(TFT_BL_PIN, TFT_BL_CHANNEL);
    ledcWrite(TFT_BL_CHANNEL, TFT_BRIGHTNESS);

    // Attempt to load fonts (will silently use fallback if not found)
    _loadFonts();

    // Show boot screen
    _tft.fillScreen(TFT_BLACK);
    _tft.setTextColor(TFT_WHITE, TFT_BLACK);
    _tft.drawCentreString("Desk Ornament", _tft.width() / 2, _tft.height() / 2 - 10, 2);
    _tft.drawCentreString("Loading...", _tft.width() / 2, _tft.height() / 2 + 14, 1);
    fadeIn(800);

    return true;
}

bool DisplayManager::_loadFonts() {
    // Attempt to load smooth fonts from LittleFS
    // This will be fully implemented in Phase 5
    _fontsLoaded = false;
    return false;
}

void DisplayManager::clear(uint16_t color) {
    _tft.fillScreen(color);
}

void DisplayManager::fillScreen(uint16_t color) {
    _tft.fillScreen(color);
}

// ============================================================
// Status Bar
// ============================================================
void DisplayManager::drawStatusBar(int pageIndex, int totalPages, const char* timeStr, bool wifiConnected, bool forceRedraw) {
    // Only redraw if something changed
    bool timeChanged = (strcmp(timeStr, _lastStatusTime) != 0);
    bool pageChanged = (pageIndex != _lastStatusPage);
    bool wifiChanged = (wifiConnected != _lastWifiStatus);

    if (!timeChanged && !pageChanged && !wifiChanged && !forceRedraw) return;

    int w = _tft.width();

    if (forceRedraw || pageChanged) {
        _tft.fillRect(0, 0, w, STATUS_BAR_H, TFT_NAVY);
    } else {
        // Only redraw changed areas
        if (timeChanged) {
            _tft.fillRect(w / 3, 0, w / 3, STATUS_BAR_H, TFT_NAVY);
        }
    }

    // WiFi status icon (left side)
    if (wifiChanged || forceRedraw || pageChanged) {
        _tft.fillRect(2, 2, STATUS_BAR_H - 4, STATUS_BAR_H - 4, TFT_NAVY);
        if (wifiConnected) {
            // Draw WiFi icon: 3 arcs
            int cx = STATUS_BAR_H / 2 - 2;
            int cy = STATUS_BAR_H - 6;
            _tft.drawArc(cx, cy, 6, 4, 180, 270, TFT_GREEN, TFT_NAVY);
            _tft.drawArc(cx, cy, 5, 3, 180, 270, TFT_GREEN, TFT_NAVY);
            _tft.drawArc(cx, cy, 4, 2, 180, 270, TFT_GREEN, TFT_NAVY);
            _tft.fillCircle(cx, cy, 1, TFT_GREEN);
        } else {
            // Draw X icon
            _tft.drawLine(2, 2, STATUS_BAR_H - 6, STATUS_BAR_H - 6, TFT_RED);
            _tft.drawLine(2, STATUS_BAR_H - 6, STATUS_BAR_H - 6, 2, TFT_RED);
        }
        _lastWifiStatus = wifiConnected;
    }

    // Time text (center)
    if (timeChanged || forceRedraw) {
        _tft.setTextColor(TFT_WHITE, TFT_NAVY);
        _tft.setTextDatum(MC_DATUM);
        _tft.drawString(timeStr, w / 2, STATUS_BAR_H / 2, 1);
        strncpy(_lastStatusTime, timeStr, sizeof(_lastStatusTime) - 1);
    }

    // Page indicator (right side)
    if (pageChanged || forceRedraw) {
        int dotY = STATUS_BAR_H / 2;
        int dotSpacing = 8;
        int startX = w - totalPages * dotSpacing - 4;
        for (int i = 0; i < totalPages; i++) {
            int dx = startX + i * dotSpacing;
            _tft.fillCircle(dx, dotY, 2, (i == pageIndex) ? TFT_WHITE : TFT_DARKGREY);
        }
        _lastStatusPage = pageIndex;
    }
}

// ============================================================
// Drawing Helpers
// ============================================================
void DisplayManager::drawCenteredText(const char* text, int y, uint8_t fontId, uint16_t color) {
    _tft.setTextColor(color, TFT_BLACK);
    _tft.setTextDatum(TC_DATUM);
    _tft.drawString(text, _tft.width() / 2, y, fontId);
}

void DisplayManager::drawCenteredText(const char* text, int y, const uint8_t* font, uint16_t color) {
    _tft.setTextColor(color, TFT_BLACK);
    _tft.setTextDatum(TC_DATUM);
    _tft.drawString(text, _tft.width() / 2, y, font);
}

void DisplayManager::drawLargeNumber(const char* numStr, int y, uint16_t color) {
    _tft.setTextColor(color, TFT_BLACK);
    _tft.setTextDatum(TC_DATUM);
    // Use font 7 (large numbers) as default, or smooth font if loaded
    if (_fontsLoaded) {
        _tft.drawString(numStr, _tft.width() / 2, y, _fontLarge);
    } else {
        _tft.drawString(numStr, _tft.width() / 2, y, 7);  // Font 7 = 7-seg 48px
    }
}

void DisplayManager::drawInfoLine(const char* label, const char* value, int y, uint16_t labelColor, uint16_t valueColor) {
    int margin = 20;
    _tft.setTextDatum(ML_DATUM);
    _tft.setTextColor(labelColor, TFT_BLACK);
    _tft.drawString(label, margin, y, 1);
    _tft.setTextDatum(MR_DATUM);
    _tft.setTextColor(valueColor, TFT_BLACK);
    _tft.drawString(value, _tft.width() - margin, y, 1);
}

void DisplayManager::drawChinese(const char* text, int x, int y, uint8_t fontIndex, uint16_t color) {
    if (_fontsLoaded) {
        _tft.setTextColor(color, TFT_BLACK);
        _tft.drawString(text, x, y, fontIndex);
    } else {
        // Fallback: just draw ASCII (won't render CJK but won't crash)
        _tft.setTextColor(TFT_ORANGE, TFT_BLACK);
        _tft.drawString("[CN]", x, y, 1);
    }
}

void DisplayManager::drawPageDots(int current, int total, int y) {
    if (total <= 1) return;
    int spacing = 10;
    int startX = (_tft.width() - total * spacing) / 2 + spacing / 2;
    for (int i = 0; i < total; i++) {
        _tft.fillCircle(startX + i * spacing, y, 3, (i == current) ? TFT_WHITE : TFT_DARKGREY);
    }
}

// ============================================================
// Sprite / Framebuffer
// ============================================================
TFT_eSprite& DisplayManager::createSprite(int w, int h) {
    TFT_eSprite* spr = new TFT_eSprite(&_tft);
    spr->createSprite(w, h);
    return *spr;
}

void DisplayManager::pushSprite(TFT_eSprite& sprite, int x, int y) {
    sprite.pushSprite(x, y);
}

void DisplayManager::deleteSprite(TFT_eSprite& sprite) {
    sprite.deleteSprite();
    delete &sprite;
}

// ============================================================
// Backlight Control
// ============================================================
void DisplayManager::setBrightness(uint8_t brightness) {
    ledcWrite(TFT_BL_CHANNEL, brightness);
}

void DisplayManager::fadeIn(int ms) {
    int steps = 20;
    int delayMs = ms / steps;
    for (int i = 0; i <= steps; i++) {
        ledcWrite(TFT_BL_CHANNEL, (uint8_t)((float)i / steps * TFT_BRIGHTNESS));
        delay(delayMs);
    }
}

void DisplayManager::fadeOut(int ms) {
    int steps = 20;
    int delayMs = ms / steps;
    for (int i = steps; i >= 0; i--) {
        ledcWrite(TFT_BL_CHANNEL, (uint8_t)((float)i / steps * TFT_BRIGHTNESS));
        delay(delayMs);
    }
}

void DisplayManager::breatheEffect() {
    for (int i = 0; i < 256; i += 4) {
        ledcWrite(TFT_BL_CHANNEL, i);
        delay(10);
    }
    for (int i = 255; i >= 0; i -= 4) {
        ledcWrite(TFT_BL_CHANNEL, i);
        delay(10);
    }
    ledcWrite(TFT_BL_CHANNEL, TFT_BRIGHTNESS);
}

// ============================================================
// Animated Transitions
// ============================================================
void DisplayManager::transition(TransitionEffect effect, void (*renderPage)(TFT_eSprite&), int spriteW, int spriteH) {
    switch (effect) {
        case FADE_BLACK: {
            fadeOut(150);
            // Render new page
            TFT_eSprite spr(&_tft);
            spr.createSprite(spriteW, spriteH);
            renderPage(spr);
            spr.pushSprite(0, STATUS_BAR_H);
            spr.deleteSprite();
            fadeIn(150);
            break;
        }
        case SLIDE_LEFT: {
            TFT_eSprite spr(&_tft);
            spr.createSprite(spriteW, spriteH);
            renderPage(spr);
            // Slide in from right
            for (int i = 0; i <= spriteW; i += 8) {
                spr.pushSprite(spriteW - i, STATUS_BAR_H);
            }
            spr.pushSprite(0, STATUS_BAR_H);
            spr.deleteSprite();
            break;
        }
        case SLIDE_RIGHT: {
            TFT_eSprite spr(&_tft);
            spr.createSprite(spriteW, spriteH);
            renderPage(spr);
            for (int i = 0; i <= spriteW; i += 8) {
                spr.pushSprite(i - spriteW, STATUS_BAR_H);
            }
            spr.pushSprite(0, STATUS_BAR_H);
            spr.deleteSprite();
            break;
        }
        case NONE:
        default:
            break;
    }
}

// ============================================================
// Notification Overlay
// ============================================================
void DisplayManager::showNotification(const char* msg, int durationMs, uint16_t bgColor) {
    int y = _tft.height() / 2 - 20;
    int x = _tft.width() / 2 - 80;
    _tft.fillRoundRect(x, y, 160, 40, 8, bgColor);
    _tft.setTextColor(TFT_WHITE, bgColor);
    _tft.setTextDatum(MC_DATUM);
    _tft.drawString(msg, _tft.width() / 2, y + 20, 1);
    delay(durationMs);
    // Clear notification — caller must redraw
}

// ============================================================
// Color Helpers
// ============================================================
uint16_t DisplayManager::heatColor(float value, float min, float max) {
    if (value < min) return TFT_BLUE;
    if (value > max) return TFT_RED;

    float ratio = (value - min) / (max - min);
    uint8_t r = (uint8_t)(ratio * 255);
    uint8_t g = (uint8_t)((1 - ratio) * 255);
    uint8_t b = 0;
    return _tft.color565(r, g, b);
}
