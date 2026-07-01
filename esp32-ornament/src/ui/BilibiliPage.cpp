#include "BilibiliPage.h"

BilibiliPage::BilibiliPage() : _forceRedraw(true) {}
BilibiliPage::~BilibiliPage() {}

void BilibiliPage::render(DisplayManager& display, const BilibiliData& data) {
    auto& tft = display.getTFT();
    int w = display.width();
    int contentY = STATUS_BAR_H;

    tft.fillRect(0, contentY, w, display.height() - contentY, TFT_BLACK);
    _forceRedraw = false;

    if (!data.valid) {
        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        tft.setTextDatum(MC_DATUM);
        tft.drawString("Bilibili data pending...", w / 2, display.height() / 2, 1);
        return;
    }

    // Bilibili icon (simple "B" logo area)
    tft.fillRoundRect(w / 2 - 20, contentY + 5, 40, 20, 4, TFT_PINK);
    tft.setTextColor(TFT_WHITE, TFT_PINK);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("B站", w / 2, contentY + 15, 1);

    // User name
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString(data.userName, w / 2, contentY + 30, 2);

    // Follower count (large)
    char followerStr[16];
    _formatFollowerCount(followerStr, sizeof(followerStr), data.followerCount);

    tft.setTextColor(TFT_YELLOW, TFT_BLACK);
    tft.setTextDatum(TC_DATUM);
    tft.drawString("Fans", w / 2, contentY + 60, 1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.drawString(followerStr, w / 2, contentY + 80, 7);

    // Stats row
    int rowY = contentY + 140;

    // Videos
    char str[32];
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Videos", 20, rowY, 1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TR_DATUM);
    snprintf(str, sizeof(str), "%ld", data.videoCount);
    tft.drawString(str, w / 2 - 10, rowY, 1);

    // Following
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Following", w / 2 + 10, rowY, 1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TR_DATUM);
    snprintf(str, sizeof(str), "%ld", data.followingCount);
    tft.drawString(str, w - 20, rowY, 1);

    // Views (second row)
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Views", 20, rowY + 25, 1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TR_DATUM);
    if (data.viewCount > 10000) {
        snprintf(str, sizeof(str), "%.1f万", data.viewCount / 10000.0f);
    } else {
        snprintf(str, sizeof(str), "%ld", data.viewCount);
    }
    tft.drawString(str, w / 2 - 10, rowY + 25, 1);

    // Likes
    tft.setTextColor(TFT_CYAN, TFT_BLACK);
    tft.setTextDatum(TL_DATUM);
    tft.drawString("Likes", w / 2 + 10, rowY + 25, 1);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextDatum(TR_DATUM);
    if (data.likeCount > 10000) {
        snprintf(str, sizeof(str), "%.1f万", data.likeCount / 10000.0f);
    } else {
        snprintf(str, sizeof(str), "%ld", data.likeCount);
    }
    tft.drawString(str, w - 20, rowY + 25, 1);

    // Update timestamp
    if (data.lastUpdate > 0) {
        snprintf(str, sizeof(str), "Updated %lu min ago",
                 (millis() - data.lastUpdate) / 60000);
        tft.setTextColor(TFT_DARKGREY, TFT_BLACK);
        tft.setTextDatum(TC_DATUM);
        tft.drawString(str, w / 2, display.height() - 15, 1);
    }
}

void BilibiliPage::_formatFollowerCount(char* buf, size_t len, long count) {
    if (count >= 100000000) {
        snprintf(buf, len, "%.1f亿", count / 100000000.0f);
    } else if (count >= 10000) {
        snprintf(buf, len, "%.1f万", count / 10000.0f);
    } else {
        snprintf(buf, len, "%ld", count);
    }
}
