#ifndef BILIBILI_PAGE_H
#define BILIBILI_PAGE_H

#include <Arduino.h>
#include "config.h"
#include "../display/DisplayManager.h"

class BilibiliPage {
public:
    BilibiliPage();
    ~BilibiliPage();

    void render(DisplayManager& display, const BilibiliData& data);

private:
    bool _forceRedraw;

    // Format large number (e.g. 12846 → "12.8K" or "1.3万")
    void _formatFollowerCount(char* buf, size_t len, long count);
};

#endif // BILIBILI_PAGE_H
