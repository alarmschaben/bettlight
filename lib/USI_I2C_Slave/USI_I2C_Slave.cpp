// USI I2C Slave implementation after http://www.mikrocontroller.net/topic/38917
// Portions Copyright Axel Gartner

#include <USI_I2C_Slave.h>

volatile uint8_t USI_COMM_STATUS;
uint8_t usi_address;
uint8_t *usi_rxBuffer;
uint8_t usi_rxBufferPos;
uint8_t *usi_txBuffer;
uint8_t usi_txBufferPos;
bool usi_rxFlag;
bool usi_txFlag;

void USI_init(uint8_t address, int rxBufferSize, int txBufferSize) {
    usi_address = address;
    usi_rxBuffer = new uint8_t[rxBufferSize];
    usi_txBuffer = new uint8_t[txBufferSize];

    USI_COMM_STATUS = NONE;
    // 2-wire mode; Hold SCL on start and overflow; ext. clock
    USI_CONTROL |= (1<<USIWM1) | (1<<USICS1);
    USI_STATUS = 0xf0;  // write 1 to clear flags, clear counter
    DDR_USI  &= ~(1<<PORT_USI_SDA);
    PORT_USI &= ~(1<<PORT_USI_SDA);
    DDR_USI  |=  (1<<PORT_USI_SCL);
    PORT_USI |=  (1<<PORT_USI_SCL);
    // startcondition interrupt enable
    USI_CONTROL |= (1<<USISIE);
}

SIGNAL(SIG_USI_START) {
    uint8_t tmpUSI_STATUS;
    tmpUSI_STATUS = USI_STATUS;
    USI_COMM_STATUS = NONE;
    // Wait for SCL to go low to ensure the "Start Condition" has completed.
    // otherwise the counter will count the transition
    while ( (PIN_USI & (1<<PORT_USI_SCL)) );
    USI_STATUS = 0xf0; // write 1 to clear flags; clear counter
    // enable USI interrupt on overflow; SCL goes low on overflow
    USI_CONTROL |= (1<<USIOIE) | (1<<USIWM0);
}

SIGNAL(SIG_USI_OVERFLOW) {
    uint8_t BUF_USI_DATA = USI_DATA;
    switch(USI_COMM_STATUS) {
        case NONE:
            if (((BUF_USI_DATA & 0xfe) >> 1) != usi_address) {  // if not receiving my address
                // disable USI interrupt on overflow; disable SCL low on overflow
                USI_CONTROL &= ~((1<<USIOIE) | (1<<USIWM0));
            }
            else { // else address is mine
                DDR_USI  |=  (1<<PORT_USI_SDA);
                USI_STATUS = 0x0e;  // reload counter for ACK, (SCL) high and back low
                if (BUF_USI_DATA & 0x01) {
                    USI_COMM_STATUS = ACK_PR_TX;
                    usi_txBufferPos = 0;
                    usi_txFlag = false;
                } else {
                    USI_COMM_STATUS = ACK_PR_RX;
                    usi_rxBufferPos = 0;
                    usi_rxFlag = false;
                }
            }
            break;
        case ACK_PR_RX:
            DDR_USI  &= ~(1<<PORT_USI_SDA);
            USI_COMM_STATUS = BYTE_RX;
            // if (usi_rxBufferPos >= sizeof(usi_rxBuffer))
            if (usi_rxBufferPos >= sizeof(usi_rxBuffer))
                usi_rxFlag = true;
            break;
        case BYTE_RX:
            usi_rxBuffer[usi_rxBufferPos++] = USI_DATA;
            /* Save received byte here! ... = USI_DATA*/
            DDR_USI  |=  (1<<PORT_USI_SDA);
            USI_STATUS = 0x0e;  // reload counter for ACK, (SCL) high and back low
            USI_COMM_STATUS = ACK_PR_RX;
            break;
        case ACK_PR_TX:
            USI_DATA = usi_txBuffer[usi_txBufferPos++];
            /* Put first byte to transmit in buffer here! USI_DATA = ... */
            PORT_USI |=  (1<<PORT_USI_SDA); // transparent for shifting data out
            USI_COMM_STATUS = BYTE_TX;
            break;
        case PR_ACK_TX:
            if(BUF_USI_DATA & 0x01) {
                USI_COMM_STATUS = NONE; // no ACK from master --> no more bytes to send
                usi_txFlag = true;
            }
            else {
                /* Put next byte to transmit in buffer here! USI_DATA = ... */
                PORT_USI |=  (1<<PORT_USI_SDA); // transparent for shifting data out
                DDR_USI  |=  (1<<PORT_USI_SDA);
                USI_COMM_STATUS = BYTE_TX;
            }
            break;
        case BYTE_TX:
            DDR_USI  &= ~(1<<PORT_USI_SDA);
            PORT_USI &= ~(1<<PORT_USI_SDA);
            USI_STATUS = 0x0e;  // reload counter for ACK, (SCL) high and back low
            USI_COMM_STATUS = PR_ACK_TX;
            break;
    }
    USI_STATUS |= (1<<USIOIF); // clear overflowinterruptflag, this also releases SCL
}
