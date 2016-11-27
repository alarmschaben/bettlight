// USI I2C Slave implementation after http://www.mikrocontroller.net/topic/38917
// Portions Copyright Axel Gartner

#include <Arduino.h>

#define DDR_USI             DDRA
#define PORT_USI            PORTA
#define PIN_USI             PINA
#define PORT_USI_SDA        PORTA6
#define PORT_USI_SCL        PORTA4
#define PIN_USI_SDA         PINA6
#define PIN_USI_SCL         PINA4
#define USI_START_COND_INT  USISIF
#define USI_START_VECTOR    USI_START_vect
#define USI_OVERFLOW_VECTOR USI_OVF_vect

#define USI_DATA        USIDR
#define USI_STATUS      USISR
#define USI_CONTROL     USICR

#define NONE           0
#define ACK_PR_RX      1
#define BYTE_RX        2
#define ACK_PR_TX      3
#define PR_ACK_TX      4
#define BYTE_TX        5

extern volatile uint8_t USI_COMM_STATUS;
extern uint8_t usi_address;
extern uint8_t *usi_rxBuffer;
extern uint8_t usi_rxBufferPos;
extern bool usi_rxFlag;

extern uint8_t *usi_txBuffer;
extern uint8_t usi_txBufferPos;
extern bool usi_txFlag;

void USI_init(uint8_t address, int rxBufferSize, int txBufferSize);
