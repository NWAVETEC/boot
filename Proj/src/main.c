#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "string.h"
#include "time.h"
#include "em_device.h"
#include "em_cmu.h"
#include "em_gpio.h"
#include "em_chip.h"
#include "unb.h"
#include "nwrm_uart.h"
#include "nwrm_cmu.h"
#include "nwrm_rtc.h"
#include "nwrm_printf.h"
#include "nwrm_flash.h"
#include "em_usart.h"

#define AT_PARSER  0
#define UART_2_RM    1

#define EXAMPLE_CODE  AT_PARSER


#ifndef EXAMPLE_CODE 
 #error "User example code is not defined"
#endif




/* Counts 1ms timeTicks */
volatile uint32_t msTicks;
void Delay(uint32_t dlyTicks);
void delay(uint32_t dlyTicks);

void init_printf(void *putp, void (*putf)(void *, char), void (*start)(void *), void (*stop)(void *));
void tfp_printf(char *fmt, ...);

void SysTick_Handler(void)
{
	msTicks++;       /* increment counter necessary in Delay()*/
}


void delay(uint32_t dlyTicks)
{
	uint32_t curTicks;
	
	curTicks = msTicks;
	while ((msTicks - curTicks) < dlyTicks) ;
}


uint8_t RSSI = 0;
uint8_t curRSSI = 0;
uint8_t SNR=0;



#define AT_MAX_LEN   256
#define RF_RANGE_LOW_END   858500000 
#define RF_RANGE_HIGH_END   925500000
char at_cmd_buffer[AT_MAX_LEN];

int i = 0;
unsigned int freq = 0;
unsigned int bandw = 0;
struct t_send_buf_len
{
 unsigned char send_buffer[20];
 unsigned char len ;
} send_buf_len;

#if EXAMPLE_CODE==UART_2_RM
void uart_2_rm (void)
{
    int watch_c;        
    while ((watch_c = NWRM_UART_GetChar()) >= 0) {
    send_buf_len.send_buffer[i++] = watch_c;
     if (i >= 4) {      
         unsigned char packetRec[256];       
#warning "Set a proper frequency value for your environment"
	 /* NWAVE_Set_Frequency(866500000, 50000, 100); */
         NWAVE_Set_Frequency(868800000, 50000, 100);          
         NWAVE_send(send_buf_len.send_buffer, 4 /*send_buf_len.len*/ , packetRec, PROTOCOL_B);
         tfp_printf("Data sent.\n");
         i = 0;
     }
    }
}
#elif EXAMPLE_CODE==AT_PARSER
void at_cmd_parser(void)
{
    int watch_c;
    unsigned char temp1 = 0 ;   

    while ((watch_c = NWRM_UART_GetChar()) >= 0) {
      
        at_cmd_buffer[i++] = watch_c;
        
        if (i >= AT_MAX_LEN) {
            tfp_printf("Error\n");
            i = 0;
            watch_c = -1;
            break;
        }
       if (watch_c == 0xA) { 
        if ((at_cmd_buffer[0] == 'a') || (at_cmd_buffer[0] == 'A')) 
           {
             if (i > 1) {
                if ((at_cmd_buffer[1] == 't') || (at_cmd_buffer[1] == 'T')) {
                  if (i > 2) {
                    switch (at_cmd_buffer[2]) {
                        case '+':
                            switch (at_cmd_buffer[3]) {
                                case 'f':
                                case 'F':
                                  switch (at_cmd_buffer[4]) {
                                      case 'r':
                                      case 'R':
                                        switch (at_cmd_buffer[5]) {
                                            case 'e':
                                            case 'E':
                                              switch (at_cmd_buffer[6]) {
                                                  case 'q':
                                                  case 'Q':
                                                    switch (at_cmd_buffer[7]) {
                                                        case '=':

                                                          {
                                                            if (at_cmd_buffer[8] == '\n')   {
                                                              tfp_printf("Error\n");
                                                              break;
                                                            }  
                                                            unsigned char temp1 = 0 ;
                                                            unsigned char temp2 = 0 ;
                                                            while (at_cmd_buffer[temp1] != ',' ) {
                                                              if (at_cmd_buffer[temp1] == '\n') {
                                                               break;                                                              
                                                              }  
                                                              temp1++;
                                                            }
                                                              if (at_cmd_buffer[temp1] == '\n') {
                                                                tfp_printf("Error\n");
                                                                break;
                                                              }
                                                              freq = atoi((char *)(at_cmd_buffer + sizeof("AT+FREQ=") -1));

                                                              if ((freq < RF_RANGE_LOW_END) || (freq > RF_RANGE_HIGH_END)) {
                                                                tfp_printf("Error\n");
                                                                break;
                                                              }
                                                              
                                                              while (at_cmd_buffer[temp1 + temp2] != '\n' ) temp2++;
                                                              {
                                                                bandw = atoi((char *)(at_cmd_buffer + temp1 + 1));
                                                                if (bandw == 0) {  // Zero is a typical error code after attempt to convert letters
                                                                 tfp_printf("Error\n");
                                                                 break;
                                                                }
                                                                else if (((freq - bandw) < RF_RANGE_LOW_END) || ((freq + bandw) > RF_RANGE_HIGH_END) ) {
                                                                 tfp_printf("Error\n");
                                                                 break;
                                                                }  
                                                                else {                                                                         
                                                                     NWAVE_Set_Frequency(freq, bandw, 100); 
                                                                     tfp_printf("Ok\n");                                                                     
                                                                }
                                                              }  
                                                          }
                                                        break;
                                                        default: tfp_printf("Error\n");                                                        
                                                    }    
                                                  break;
                                                  default: tfp_printf("Error\n");                                                  
                                               }
                                            break;
                                            default: tfp_printf("Error\n");                                            
                                        }
                                      break;
                                      default: tfp_printf("Error\n");                                      
                                  }
                                break;

                                case 's':
                                case 'S':
                                  switch (at_cmd_buffer[4]) {
                                    case 'e':
                                    case 'E':
                                      switch (at_cmd_buffer[5]) {
                                        case 'n':
                                        case 'N':
                                          switch (at_cmd_buffer[6]) {
                                            case 'd':
                                            case 'D':
                                              switch (at_cmd_buffer[7]) {
                                                case '=':   
                                                  {
                                                    temp1 = sizeof("AT+SEND=") - 1;
                                                    unsigned char j = 0;
                                                    while (at_cmd_buffer[temp1] != '\n' ) 
                                                    {  
                                                      if (at_cmd_buffer[temp1 + 1] == '\n') break;
                                                      if (at_cmd_buffer[temp1] == '$')
                                                      {
                                                        unsigned char arr[3] = { 0,0,'\n'};
                                                        temp1++;
                                                        arr[0] = at_cmd_buffer[temp1++];
                                                        arr[1] = at_cmd_buffer[temp1++];
                                                        int dat;
                                                        sscanf((char *)arr, "%x", &dat);                                                         
                                                        send_buf_len.send_buffer[j++] = dat; 
                                                        send_buf_len.len = j;
                                                        if ( j > 20 ) {
                                                          tfp_printf("Error\n");
                                                        }
                                                      }
                                                      else {
                                                        tfp_printf("Error\n");
                                                      }
                                                    }
                                                    if (send_buf_len.len == 0) {
                                                      tfp_printf("Error\n");
                                                    }
                                                    else {
                                                      unsigned char packetRec[256]; 
                                                      NWAVE_send(send_buf_len.send_buffer, send_buf_len.len, packetRec, PROTOCOL_B);
                                                      init_printf(NWRM_UART_Init(9600, true, false),
                                                                NWRM_UART_Putc,
                                                                NWRM_UART_Start,
                                                                NWRM_UART_Stop);                                                      
                                                      tfp_printf("Ok\n");
                                                    }
                                                }
                                                break;
                                                default: tfp_printf("Error\n");                                                  
                                              }
                                              break;
                                              default: tfp_printf("Error\n");                                              
                                          }                                      
                                          break;
                                        case 'r':
                                        case 'R':  
                                          switch (at_cmd_buffer[6]) {
                                            case 'i':
                                            case 'I':
                                              switch (at_cmd_buffer[7]) {                                              
                                                case 'a':
                                                case 'A':
                                                      switch (at_cmd_buffer[8]) {                                              
                                                        case 'l':
                                                        case 'L':
                                                          switch (at_cmd_buffer[9]) {                                              
                                                            case '=':
                                                              switch (at_cmd_buffer[10]) {                                              
                                                                case '?':
                                                                  {
                                                                    NWRM_DEVICE device;
                                                                    NWRM_FLASH_DeviceRead(&device); 
                                                                    unsigned char buffer[8];
                                                                    sprintf((char *)buffer, "%08x", device.Serial);
                                                                    tfp_printf((char *)buffer);
                                                                    tfp_printf("\n");
                                                                  }
                                                                break;                                                          
                                                                default: tfp_printf("Error\n"); 
                                                              }
                                                            break;                                                          
                                                            default: tfp_printf("Error\n");                                                            
                                                          }
                                                        break;
                                                        default: tfp_printf("Error\n");
                                                      }
                                                break;
                                                default: tfp_printf("Error\n");
                                              }
                                             break;
                                             default: tfp_printf("Error\n");
                                          }
                                        break;
                                        default: tfp_printf("Error\n");
                                      }                                      
                                      break;
                                      default: tfp_printf("Error\n");                                      
                                  }
                                break;
                                default: tfp_printf("Error\n");                                
                            }
                        break;
                        case 'z':
                        case 'Z':
                            tfp_printf("ATZ is not supported, use AT+FREQ=\n");
                        break;
                        default: tfp_printf("Error\n");                        
                    }
                  }
                }
                else {
                  tfp_printf("Error\n");
                }
            }
           }
        else {
          tfp_printf("Error\n");
        }
        i = 0;      
        break;
      }
    }
}
#endif

void all_init(void)
{
  
	/* Chip errata */
	CHIP_Init();
	
	CMU_ClockDivSet(cmuClock_CORE,cmuClkDiv_32);	
	
	/* Setup SysTick Timer for 1 msec interrupts  */
	if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;
	
	CMU_ClockDivSet(cmuClock_CORE,cmuClkDiv_1);
	if (SysTick_Config(CMU_ClockFreqGet(cmuClock_CORE) / 1000)) while (1) ;
	
	NWAVE_Set_Frequency(866500000, 50000, 100);
        /* NWAVE_Set_Frequency(868800000, 50000, 100); */
	
	CMU_ClockEnable(cmuClock_GPIO, true);

	Delay(200);
	
	GPIO_PinModeSet(gpioPortF,2, gpioModePushPull, 0);
	GPIO_PinOutSet(gpioPortF,2); 
	
	GPIO_PinModeSet(gpioPortB,14, gpioModePushPull, 0);
	
	//RF reset 
	GPIO_PinModeSet(gpioPortB,14, gpioModePushPull, 0);
	GPIO_PinOutClear(gpioPortB,14);
	Delay(1);
	GPIO_PinModeSet(gpioPortB,14, gpioModeInput,0);
	
	/////////////////////////////  
	GPIO_PinModeSet(gpioPortC,1, gpioModeInput, 0);
	GPIO_PinModeSet(gpioPortC,0, gpioModePushPull, 0);
	
	//UART
	
	CMU_ClockEnable(cmuClock_USART1, true);          // Enable USART1 peripheral clock

	// Initialize the clock Management Unit
	NWRM_CMU_Init(true);

	// Initialize the RTC clock
	NWRM_RTC_Init(0);        
        
        init_printf(NWRM_UART_Init(9600, true, false),
    		NWRM_UART_Putc,
    		NWRM_UART_Start,
    		NWRM_UART_Stop);        
        
    

       
}      

void user_setup (void)
{
    all_init();  
    
#if EXAMPLE_CODE==UART_2_RM        
    tfp_printf("This is UART TO NWAVE example, each 4 bytes coming to UART are sent to the radio module transmitter.\n" );
#elif EXAMPLE_CODE==AT_PARSER    
    tfp_printf("Welcome to NWAVETEC Terminal! Input an AT command:\n" );
#endif    
}

void user_loop (void)
{
#if EXAMPLE_CODE==UART_2_RM
    uart_2_rm();
#elif EXAMPLE_CODE==AT_PARSER
    at_cmd_parser();
#endif     
}

int main(void)
{
    user_setup();
    while (1) {
        NWRM_RTC_Sleep();
        user_loop();
    }
}


