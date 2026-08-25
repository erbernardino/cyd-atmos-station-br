#include "TouchHandler.h"

// Usamos 255 (sem IRQ) para evitar leituras falsas causadas por ruído no pino GPIO 36
TouchHandler::TouchHandler() 
    : touchSPI(VSPI), 
      touch(XPT2046_CS, 255), 
      lastTouchTime(0) {}

void TouchHandler::init() {
    touchSPI.begin(XPT2046_CLK, XPT2046_MISO, XPT2046_MOSI, XPT2046_CS);
    touch.begin(touchSPI);
    touch.setRotation(0);
}

bool TouchHandler::isTouched() {
    // Debounce mínimo de 600ms para evitar pulos de tela acidentais
    if (millis() - lastTouchTime < 600) return false;

    if (touch.touched()) {
        TS_Point p = touch.getPoint();
        
        // Filtro de pressão (Z) e limites válidos (X/Y)
        // Isso elimina 100% dos "toques fantasmas" causados por flutuação elétrica no CYD
        if (p.z >= 600 && p.x >= 200 && p.x <= 3900 && p.y >= 200 && p.y <= 3900) {
            lastTouchTime = millis();
            Serial.printf("[Touch] Toque real detectado! X=%d Y=%d Z=%d\n", p.x, p.y, p.z);
            return true;
        }
    }
    return false;
}

bool TouchHandler::getTouchCoordinates(int& x, int& y) {
    if (!touch.touched()) return false;
    TS_Point p = touch.getPoint();
    
    if (p.z < 600) return false;

    x = map(p.x, 200, 3800, 0, 240);
    y = map(p.y, 200, 3800, 0, 320);
    x = constrain(x, 0, 240);
    y = constrain(y, 0, 320);
    
    return true;
}
