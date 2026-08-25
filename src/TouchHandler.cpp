#include "TouchHandler.h"

TouchHandler::TouchHandler() 
    : touchSPI(VSPI), 
      touch(XPT2046_CS, XPT2046_IRQ), 
      lastTouchTime(0) {}

void TouchHandler::init() {
    touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touch.begin(touchSPI);
    touch.setRotation(0);
}

bool TouchHandler::isTouched() {
    if (millis() - lastTouchTime < 300) return false; // Debounce de 300ms
    if (touch.touched()) {
        lastTouchTime = millis();
        return true;
    }
    return false;
}

bool TouchHandler::getTouchCoordinates(int& x, int& y) {
    if (!touch.touched()) return false;
    TS_Point p = touch.getPoint();
    
    // Mapeamento simples de calibração para 240x320
    x = map(p.x, 200, 3800, 0, 240);
    y = map(p.y, 200, 3800, 0, 320);
    x = constrain(x, 0, 240);
    y = constrain(y, 0, 320);
    
    return true;
}
