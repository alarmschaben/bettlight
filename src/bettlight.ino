#include <LightedButton.h>
#include <USI_I2C_Slave.h>

#include <EEPROM.h>

LightedButton::LightedButtonConfig lbConfig;

unsigned char configBuffer[sizeof(lbConfig)], readPos;
unsigned char buttonPressBuffer;

LightedButton button1(0, 7, &lbConfig);
LightedButton button2(1, 8, &lbConfig);

void setup () {
    cli();
    uint8_t address = EEPROM.read(0);
    for (int i=0; i < sizeof(lbConfig); i++)
        configBuffer[i] = EEPROM.read(i+1);
    memcpy(&lbConfig, configBuffer, sizeof(lbConfig));
    USI_init(address, sizeof(lbConfig), 1);
    sei();
}

void loop() {
    button1.doFSM();
    button2.doFSM();

    if (usi_rxFlag) {
        memcpy(&lbConfig, usi_rxBuffer, sizeof(lbConfig));
        for (int i=0; i < sizeof(lbConfig); i++)
            EEPROM.write(i+1, usi_rxBuffer[i]);

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

