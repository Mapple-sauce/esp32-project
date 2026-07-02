#ifndef INDOOR_PAGE_H
#define INDOOR_PAGE_H

#include <Arduino.h>
#include "config.h"
#include "../display/DisplayManager.h"

class IndoorPage {
public:
    IndoorPage();
    ~IndoorPage();

    void render(DisplayManager& display, const IndoorData& data);

private:
    bool _forceRedraw;

    // Draw a large value with label and unit
    void _drawValueCard(DisplayManager& display, const char* label, const char* value,
                        const char* unit, int y, uint16_t valueColor);
};

#endif // INDOOR_PAGE_H
