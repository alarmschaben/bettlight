#include <LightedButton.h>

void LightedButton::doFSM() {
    doButtonFSM();
    doLEDFSM();
}

void LightedButton::doButtonFSM() {
    switch(bs) {
        case BTN_IDLE:
            if (!digitalRead(buttonPin) && (millis() - lastButtonStateChangeMillis >= config->shortPressMinDuration)) {
                bs = BTN_SHORT_PRESS;
                lastButtonStateChangeMillis = millis();
            }
            else if (digitalRead(buttonPin)) {
                lastButtonStateChangeMillis = millis();
            }
            break;
        case BTN_SHORT_PRESS:
            if (!digitalRead(buttonPin)) {
                if (millis() - lastButtonStateChangeMillis >= config->longPressMinDuration) {
                    bs = BTN_LONG_PRESS;
                }
            } else {
                bs = BTN_IDLE;
                bps = SHORT;
            }
            break;
        case BTN_LONG_PRESS:
            if (digitalRead(buttonPin)) {
                bs = BTN_IDLE;
                bps = LONG;
            }
            break;
    }
}

LightedButton::ButtonPressState LightedButton::getButtonPress() {
    return bps;
}

void LightedButton::clearButtonPress() {
    bps = NONE;
}

void LightedButton::doLEDFSM() {
    switch(bls) {
        case LED_IDLE:
            currentBrightness = config->idleBrightness;
            if (!digitalRead(buttonPin)) {
                bls = LED_FADE_UP;
                lastLEDStateChangeMillis = millis();
            }
            break;
        case LED_FADE_UP:
            if (millis() - lastLEDStateChangeMillis >= 1) {
                currentBrightness += 2;
                lastLEDStateChangeMillis = millis();
            }
            if (currentBrightness >= config->maxBrightness) {
                bls = LED_FULLY_LIT;
                lastLEDStateChangeMillis = millis();
            }
            break;
        case LED_FULLY_LIT:
            currentBrightness=config->maxBrightness;
            if (!digitalRead(buttonPin))
                lastLEDStateChangeMillis = millis();
            if (millis() - lastLEDStateChangeMillis >= config->fullyLitDuration) {
                bls = LED_FADE_DOWN;
                lastLEDStateChangeMillis = millis();
            }
            break;
        case LED_FADE_DOWN:
            if (millis() - lastLEDStateChangeMillis >= 1) {
                currentBrightness -= 2;
                lastLEDStateChangeMillis = millis();
            }
            if (currentBrightness <= config->idleBrightness) {
                bls = LED_IDLE;
                lastLEDStateChangeMillis = millis();
            }
            lastLEDStateChangeMillis = millis();
            break;
    }
    analogWrite(ledPin, currentBrightness);
}

