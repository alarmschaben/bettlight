#include <LightedButton.h>

LightedButton button1(0, 7);
LightedButton button2(1, 8);

void setup () {
}

void loop() {
    button1.doFSM();
    button2.doFSM();
}

