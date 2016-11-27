#include <LightedButton.h>

LightedButton button1(0, 7, &lbConfig);
LightedButton button2(1, 8, &lbConfig);

void setup () {
}

void loop() {
    button1.doFSM();
    button2.doFSM();
}

