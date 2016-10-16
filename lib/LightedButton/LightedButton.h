#include <Arduino.h>

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

