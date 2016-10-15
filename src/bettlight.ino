class LightedButton {
    private:
        // method variables
        int buttonPin;
        int ledPin;

        // parameters, TODO: get those from EEPROM
        uint8_t idleBrightness = 1;
        uint8_t maxBrightness = 254;
        uint16_t fullyLitDuration = 2000;

        // runtime variables
        uint8_t currentBrightness;

        enum ButtonLEDstate {
            IDLE,
            FADE_UP,
            FULLY_LIT,
            FADE_DOWN};

        ButtonLEDstate bls;
        uint32_t lastStateChangeMillis;

    public:
        LightedButton(int buttonPin, int ledPin) {
            this->buttonPin = buttonPin;
            this->ledPin = ledPin;

            pinMode(buttonPin, INPUT);
            digitalWrite(buttonPin, HIGH);

            pinMode(ledPin, OUTPUT);
            analogWrite(ledPin, idleBrightness);
        }

        void doFSM();
};

void LightedButton::doFSM() {
    switch(bls) {
        case IDLE:
            currentBrightness = idleBrightness;
            if (!digitalRead(buttonPin)) {
                bls = FADE_UP;
                lastStateChangeMillis = millis();
            }
            break;
        case FADE_UP:
            if (millis() - lastStateChangeMillis >= 1) {
                currentBrightness += 2;
                lastStateChangeMillis = millis();
            }
            if (currentBrightness >= maxBrightness) {
                bls = FULLY_LIT;
                lastStateChangeMillis = millis();
            }
            break;
        case FULLY_LIT:
            currentBrightness=maxBrightness;
            if (!digitalRead(buttonPin))
                lastStateChangeMillis = millis();
            if (millis() - lastStateChangeMillis >= fullyLitDuration) {
                bls = FADE_DOWN;
                lastStateChangeMillis = millis();
            }
            break;
        case FADE_DOWN:
            if (millis() - lastStateChangeMillis >= 1) {
                currentBrightness -= 2;
                lastStateChangeMillis = millis();
            }
            if (currentBrightness <= idleBrightness) {
                bls = IDLE;
                lastStateChangeMillis = millis();
            }
            lastStateChangeMillis = millis();
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

