class LightedButton {
    public:
        enum ButtonPressState {
            NONE,
            SHORT,
            LONG};

    private:
        // method variables
        int buttonPin;
        int ledPin;

        // parameters, TODO: get those from EEPROM
        uint8_t idleBrightness = 1;
        uint8_t maxBrightness = 254;
        uint16_t fullyLitDuration = 2000;

        uint16_t shortPressMinDuration = 100;
        uint16_t longPressMinDuration = 800;

        // FSM declarations
        enum ButtonFSMState {
            BTN_IDLE,
            BTN_SHORT_PRESS,
            BTN_LONG_PRESS};

        enum ButtonLEDFSMstate {
            LED_IDLE,
            LED_FADE_UP,
            LED_FULLY_LIT,
            LED_FADE_DOWN};

        // runtime variables
        uint8_t currentBrightness;

        ButtonFSMState bs;
        ButtonPressState bps;
        ButtonLEDFSMstate bls;

        uint32_t lastLEDStateChangeMillis;
        uint32_t lastButtonStateChangeMillis;

    public:
        LightedButton(int buttonPin, int ledPin) {
            this->buttonPin = buttonPin;
            this->ledPin = ledPin;

            pinMode(buttonPin, INPUT);
            digitalWrite(buttonPin, HIGH);

            pinMode(ledPin, OUTPUT);
            analogWrite(ledPin, idleBrightness);

            bs = BTN_IDLE;
            bps = NONE;
            bls = LED_IDLE;
        }

        // forward declarations
        void doFSM();
        void doButtonFSM();
        void doLEDFSM();
        ButtonPressState getButtonPress();
        void clearButtonPress();
};

void LightedButton::doFSM() {
    doButtonFSM();
    doLEDFSM();
}

void LightedButton::doButtonFSM() {
    switch(bs) {
        case BTN_IDLE:
            if (!digitalRead(buttonPin) && (millis() - lastButtonStateChangeMillis >= shortPressMinDuration)) {
                bs = BTN_SHORT_PRESS;
                lastButtonStateChangeMillis = millis();
            }
            else if (digitalRead(buttonPin)) {
                lastButtonStateChangeMillis = millis();
            }
            break;
        case BTN_SHORT_PRESS:
            if (!digitalRead(buttonPin)) {
                if (millis() - lastButtonStateChangeMillis >= longPressMinDuration) {
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
            currentBrightness = idleBrightness;
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
            if (currentBrightness >= maxBrightness) {
                bls = LED_FULLY_LIT;
                lastLEDStateChangeMillis = millis();
            }
            break;
        case LED_FULLY_LIT:
            currentBrightness=maxBrightness;
            if (!digitalRead(buttonPin))
                lastLEDStateChangeMillis = millis();
            if (millis() - lastLEDStateChangeMillis >= fullyLitDuration) {
                bls = LED_FADE_DOWN;
                lastLEDStateChangeMillis = millis();
            }
            break;
        case LED_FADE_DOWN:
            if (millis() - lastLEDStateChangeMillis >= 1) {
                currentBrightness -= 2;
                lastLEDStateChangeMillis = millis();
            }
            if (currentBrightness <= idleBrightness) {
                bls = LED_IDLE;
                lastLEDStateChangeMillis = millis();
            }
            lastLEDStateChangeMillis = millis();
            break;
    }
    analogWrite(ledPin, currentBrightness);
}

LightedButton button1(2, 5);
LightedButton button2(0, 7);

void setup () {
}

void loop() {
    button1.doFSM();
    button2.doFSM();
}

