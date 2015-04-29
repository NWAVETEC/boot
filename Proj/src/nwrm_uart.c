#include "nwrm_module.h"
#include <stdint.h>
#include <stdbool.h>
#include <em_cmu.h>
#include <em_gpio.h>
#include <em_leuart.h>
#include "nwrm_uart.h"
#include "nwrm_rtc.h"




#define NWRM_UART_RXBUFSIZE              512+128//140 // 61 increase buffer



volatile NWRM_UART_CALLBACK NWRM_UART_RxCallback = 0;


static bool PortShared = false;


static LEUART_Enable_TypeDef PortEnable = leuartDisable;


static int RxReadIndex = 0;


static int RxWriteIndex = 0;


static char RxBuffer[NWRM_UART_RXBUFSIZE];

void *NWRM_UART_Init(uint32_t speed, bool rxEnable, bool shared)
{
    LEUART_Init_TypeDef init = LEUART_INIT_DEFAULT;                                         // defining the LEUART0 initialization data

    if (speed > 9600) {
        CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_CORELEDIV2);
        CMU_ClockDivSet(cmuClock_LEUART0, cmuClkDiv_4);
        init.refFreq = 1750000;                                                             // 14MHz/2 prescaled by 4
    } else {
        CMU_ClockSelectSet(cmuClock_LFB, cmuSelect_LFXO);
        CMU_ClockDivSet(cmuClock_LEUART0, cmuClkDiv_1);
        init.refFreq = 0;
    }

    CMU_ClockEnable(cmuClock_LEUART0, true);                                                // enabling the LEUART0 clock

    init.enable     = leuartDisable;
    init.baudrate   = speed;

    LEUART_Reset(LEUART0);                                                                  // reseting and initializing LEUART0
    LEUART_Init(LEUART0, &init);

    GPIO_PinModeSet(TX_PORT, TX_BIT, gpioModePushPull, 1);                                  // TX PORT
    PortEnable = leuartEnableTx;                                                            // always enable tx

    if (rxEnable) {

        GPIO_PinModeSet(RX_PORT, RX_BIT, gpioModeInput, 1);                             // RX PORT

        LEUART_IntClear(LEUART0, LEUART_IF_RXDATAV);                                        // clear previous RX interrupts
        NVIC_ClearPendingIRQ(LEUART0_IRQn);

        LEUART_IntEnable(LEUART0, LEUART_IF_RXDATAV);                                       // enable RX interrupts
        NVIC_EnableIRQ(LEUART0_IRQn);

        PortEnable |= leuartEnableRx;                                                       // enable rx
    }

    if (!shared) {                                                                          // port will not be shared
        PortShared = true;                                                                  // temporary set this flag to set initial route
        NWRM_UART_Start(LEUART0);                                                             // we can start uart now
    }

    PortShared = shared;                                                                    // save port shared flag

    LEUART_Enable(LEUART0, PortEnable);                                                     // finally enable uart
    return(LEUART0);
}


void NWRM_UART_Start(void *p)
{
    if (PortShared) {                                                                       // enable LEUART0 TX and RX route
        if (PortEnable & leuartEnableRx) {
            LEUART0->ROUTE = LEUART_ROUTE_TXPEN | LEUART_ROUTE_RXPEN | LEUART_ROUTE_LOCATION_LOC0;
            LEUART0->CMD = LEUART_CMD_TXDIS | LEUART_CMD_RXDIS | LEUART_CMD_CLEARTX | LEUART_CMD_CLEARRX;
            LEUART0->CMD = LEUART_CMD_TXEN | LEUART_CMD_RXEN;
        } else {
            LEUART0->ROUTE = LEUART_ROUTE_TXPEN | LEUART_ROUTE_LOCATION_LOC0;
            LEUART0->CMD = LEUART_CMD_TXDIS | LEUART_CMD_CLEARTX;
            LEUART0->CMD = LEUART_CMD_TXEN;
        }
        NWRM_RTC_CalibratedDelay(1000);
    }
}


void NWRM_UART_Stop(void *p)
{
    while (!(LEUART0->STATUS & LEUART_STATUS_TXC));                                         // wait end of transmission

    if (PortShared) {                                                                       // disable LEUART0 TX and RX route
        LEUART0->CMD = LEUART_CMD_TXDIS | LEUART_CMD_RXDIS | LEUART_CMD_CLEARTX | LEUART_CMD_CLEARRX;
        LEUART0->ROUTE = 0;
    }
}


__ramfunc void NWRM_UART_Putc(void *p, char c)
{
    LEUART_Tx(LEUART0, c);
}


__ramfunc int NWRM_UART_GetChar(void)
{
    int c;

    if (RxReadIndex == RxWriteIndex) {
        return -1;
    }
    c = RxBuffer[RxReadIndex++];
    if (RxReadIndex == NWRM_UART_RXBUFSIZE) {
        RxReadIndex = 0;                                                                    // wrap Rx read Index
    }
    if (RxReadIndex == RxWriteIndex) {
        RxReadIndex = RxWriteIndex = 0;
    }
    return c;
}


int NWRM_UART_AvailableChars(void)
{
	int count = RxWriteIndex - RxReadIndex;

	return count < 0 ? NWRM_UART_RXBUFSIZE + count : count;
}


void NWRM_UART_SendString(char *string)
{
    char c;

    while ((c = *string++) != '\0') {
        LEUART_Tx(LEUART0, c);
    }
}


__ramfunc void NWRM_UART_Send(char *buffer, char length)
{
    int i;

    for (i = 0; i < length; i++) {                                                          // check that transmit buffer is empty
        LEUART_Tx(LEUART0, buffer[i]);
    }
}

#if LOADER_TRANSMITTER == 0

unsigned int irq_cnt = 0;

__ramfunc void LEUART0_IRQHandler(void)
{

    char data = LEUART0->RXDATA;                                                            // get the received byte

    irq_cnt++;
      
    if (NWRM_UART_RxCallback != 0) {                                                          // rx callback supplied
        (*NWRM_UART_RxCallback)(data);                                                        // call it
    }

    if (RxWriteIndex < NWRM_UART_RXBUFSIZE) {                                                         // enough space in receive buffer
        RxBuffer[RxWriteIndex++] = data;                                                    // save the received byte
    } else {
        if (RxWriteIndex == RxReadIndex) {                                                  // buffer overflow
            return;
        }
        RxWriteIndex = 0;
    }
}
#endif

