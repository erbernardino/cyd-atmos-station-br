#pragma once

#include <Arduino.h>
#include <SPI.h>
#include <XPT2046_Touchscreen.h>
#include "Config.h"

class TouchHandler {
public:
    TouchHandler();
    void init();
    bool isTouched();
    bool getTouchCoordinates(int& x, int& y);

private:
    SPIClass touchSPI;
    XPT2046_Touchscreen touch;
    unsigned long lastTouchTime;
};
