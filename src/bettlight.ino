#include <LightedButton.h>
#include <USI_I2C_Slave.h>

LightedButton::LightedButtonConfig lbConfig;

unsigned char configBuffer[sizeof(lbConfig)], readPos;
unsigned char buttonPressBuffer;

LightedButton button1(0, 7, &lbConfig);
LightedButton button2(1, 8, &lbConfig);

void setup () {
    cli();
    USI_init(0x44, sizeof(lbConfig), 1);
    sei();
}

void loop() {
    button1.doFSM();
    button2.doFSM();

    if (usi_rxFlag) {
        memcpy(&lbConfig, usi_rxBuffer, sizeof(lbConfig));
        usi_rxFlag = false;
    }

    doButtons();
}

void doButtons() {
    unsigned char tmp1 = button1.getButtonPress();
    unsigned char tmp2 = button2.getButtonPress();

    usi_txBuffer[0] = (tmp1 << 4) | tmp2;

    if (usi_txFlag) {
        button1.clearButtonPress();
        button2.clearButtonPress();
        usi_txBuffer[0] = 0;
        usi_txFlag = false;
    }
}

