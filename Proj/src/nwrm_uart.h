#ifndef __NWRM_UART_H
#define __NWRM_UART_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif



/** UARt receive interrupt callback function pointer */
typedef void    (*NWRM_UART_CALLBACK)(char);


static __INLINE void NWRM_UART_SetRxCallback(NWRM_UART_CALLBACK cb)
{
    extern volatile NWRM_UART_CALLBACK NWRM_UART_RxCallback;
    NWRM_UART_RxCallback = cb;
}



void *NWRM_UART_Init(uint32_t speed, bool rxEnable, bool shared);
void NWRM_UART_Putc(void *p, char c);
void NWRM_UART_Start(void *p);
void NWRM_UART_Stop(void *p);
int NWRM_UART_GetChar(void);
int NWRM_UART_AvailableChars(void);
void NWRM_UART_SendString(char *string);
void NWRM_UART_Send(char *buffer, char length);


#ifdef __cplusplus
}
#endif

#endif // __NWRM_UART_H
