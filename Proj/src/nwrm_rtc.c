#include "nwrm_module.h"
#include <time.h>
#include <em_cmu.h>
#include <em_rtc.h>
#include <em_emu.h>
#include <stdbool.h>
#include "nwrm_rtc.h"


#define NWRM_RTC_TICKS_PER_SECOND 32768


#define NWRM_RTC_OVERFLOW_PERIOD ((0x00FFFFFF + 1) / NWRM_RTC_TICKS_PER_SECOND)


#define NWRM_RTC_OVERFLOWED ((0x00FFFFFF + 1) % NWRM_RTC_TICKS_PER_SECOND)


#define MODULE_KEEPALIVE


volatile bool NWRM_RTC_AbortDelay = false;

#ifdef MODULE_KEEPALIVE

volatile bool NWRM_RTC_KeepAlive = false;
#endif




static RTC_Init_TypeDef RTCInit = {
	.debugRun = false,
	.comp0Top = false,
	.enable   = false,
};


static volatile bool NWRM_RTC_EndDelay = false;


static NWRM_RTC_handler_t NWRM_RTC_SystemHandler  = 0;


static NWRM_RTC_handler_t NWRM_RTC_UserHandler  = 0;

static uint32_t rtcOverflowCounter = 0;
static int   rtcOffsetTime = 0;

#ifdef MODULE_KEEPALIVE

NWRM_RTC_handler_t NWRM_RTC_KeepAliveHandler = 0;


static uint32_t NWRM_RTC_KeepAliveCounter = 0;


static uint32_t NWRM_RTC_KeepAliveLimit = 0;
#endif



#ifdef __ICCARM__
// #pragma optimize = speed
#else
#pragma GCC optimize ("O3")
#endif


static void NWRM_RTC_DelayHandler(void)
{
    NWRM_RTC_EndDelay = true;
}

#ifdef MODULE_KEEPALIVE

static void NWRM_RTC_DefaultKeepAliveHandler(void)
{
    NWRM_RTC_KeepAlive = true;
}
#endif




#if (MODULE_REVISION == REVISION_VBC4)
#else
void RTC_IRQHandler(void)
{
    if (RTC->IF & RTC_IF_COMP0)
      {

    	// System RTC interrupt
        if ((RTC->IEN & RTC_IF_COMP0) && (NWRM_RTC_SystemHandler != 0)) {
            NWRM_RTC_SystemHandler();
        }

        // Clear interrupt source
        RTC_IntClear(RTC_IFC_COMP0);
    }

    if (RTC->IF & RTC_IF_COMP1) {

    	// User RTC interrupt
        if ((RTC->IEN & RTC_IF_COMP1) && (NWRM_RTC_UserHandler != 0)) {
            NWRM_RTC_UserHandler();
        }

    	// Clear interrupt source
    	RTC_IntClear(RTC_IFC_COMP1);
    }

    if (RTC->IF & RTC_IF_OF) {
    	rtcOverflowCounter++;

    	// Keep-alive interrupt
    	RTC_IntClear(RTC_IF_OF);

#ifdef MODULE_KEEPALIVE
    	if ((RTC->IEN & RTC_IF_OF) && (NWRM_RTC_KeepAliveLimit != 0)) {
    		if (++NWRM_RTC_KeepAliveCounter >= (NWRM_RTC_KeepAliveLimit >> 9)) {
    			NWRM_RTC_KeepAliveCounter -= NWRM_RTC_KeepAliveLimit >> 9;
    			NWRM_RTC_DefaultKeepAliveHandler();
    		}
    	}
    }
#endif

}
#endif


void NWRM_RTC_Init(NWRM_RTC_handler_t function)
{

	CMU_ClockDivSet(cmuClock_RTC, cmuClkDiv_1);


	CMU_ClockEnable(cmuClock_RTC, true);


        NWRM_RTC_SetSystemHandler(function);


        RTC_Reset();
	RTC_Init(&RTCInit);


	RTC_CompareSet(0, 0xFFFFFF);



	RTC_CompareSet(1, 32768); 


	NVIC_EnableIRQ(RTC_IRQn);


	RTC_Enable(true);

        NWRM_RTC_EndDelay = false;


#ifdef MODULE_KEEPALIVE
        NWRM_RTC_KeepAlive = false;
        NWRM_RTC_KeepAliveLimit = 0;
        NWRM_RTC_KeepAliveCounter = 0;
        RTC_IntClear(RTC_IF_OF);
        RTC_IntEnable(RTC_IF_OF);
#endif
}


#if (MODULE_REVISION == REVISION_VBC4)
bool NWRM_RTC_Delay(uint32_t duration)
{
	volatile unsigned int counter;

	for (counter = 0; counter < 1500 * duration / 32; counter++) {
		;
	}
	return true;
}
#else
bool NWRM_RTC_Delay(uint32_t duration)
{
	NWRM_RTC_handler_t old = NWRM_RTC_SystemHandler;
    uint32_t top = 0xFFFFFF; 

	if (duration > 0) {
		RTC_FreezeEnable(true);
        if (RTC_CounterGet() + duration > top) {

        	RTC_CompareSet(0, duration - (top - RTC_CounterGet()));
        } else {
        	RTC_CompareSet(0, RTC_CounterGet() + duration);
        }
        RTC_FreezeEnable(false);

        NWRM_RTC_AbortDelay = false;
        NWRM_RTC_EndDelay = false;

        NWRM_RTC_SystemHandler = NWRM_RTC_DelayHandler;
        RTC_IntClear(RTC_IFC_COMP0);
        RTC_IntEnable(RTC_IF_COMP0);

        while (!NWRM_RTC_EndDelay && !NWRM_RTC_AbortDelay) {
            EMU_EnterEM2(false);
        }

        NWRM_RTC_EnableSystemInterrupts(false);
        NWRM_RTC_SystemHandler = old;

        return NWRM_RTC_EndDelay;
    }
    return true;
}
#endif


void NWRM_RTC_Sleep(void)
{
	RTC_IntClear(RTC_IFC_COMP0);
	RTC_IntClear(RTC_IFC_COMP1);        
	RTC_IntEnable(RTC_IF_COMP0);
	RTC_IntEnable(RTC_IF_COMP1);        
	EMU_EnterEM2(false);
	RTC_IntDisable(RTC_IF_COMP0);
	RTC_IntDisable(RTC_IF_COMP1);        
}


NWRM_RTC_handler_t NWRM_RTC_SetSystemHandler(NWRM_RTC_handler_t function)
{
	NWRM_RTC_handler_t old = NWRM_RTC_SystemHandler;

    NWRM_RTC_SystemHandler = function;
    return old;
}


NWRM_RTC_handler_t NWRM_RTC_SetUserHandler(NWRM_RTC_handler_t function)
{
	NWRM_RTC_handler_t old = NWRM_RTC_UserHandler;

    NWRM_RTC_UserHandler = function;
    return old;
}

#ifdef MODULE_KEEPALIVE

void NWRM_RTC_SetKeepAliveHandler(NWRM_RTC_handler_t function, uint32_t period)
{
	NWRM_RTC_KeepAliveLimit = period;
    NWRM_RTC_KeepAliveHandler = (function == 0) ? NWRM_RTC_DefaultKeepAliveHandler : function;
}
#endif


void NWRM_RTC_CalibratedDelay(uint32_t udelay)
{
    static uint32_t clock = 0;
    long long delay;

    if (clock == 0) {
    	clock  = CMU_ClockFreqGet(cmuClock_HFPER);
    	clock /= CMU_ClockDivGet(cmuClock_HFPER);
    }
    delay  = clock;
    delay *= udelay;
    delay /= 1000000;
    CMU_Calibrate(delay, cmuOsc_HFRCO);
}


uint32_t NWRM_RTC_TimeDiff(uint32_t reference)
{
    uint32_t difference, now = RTC_CounterGet();

    if (now < reference) {
    	difference = 0xFFFFFF - reference + now;
    } else {
    	difference = now - reference;
    }
    return difference;
}


void NWRM_RTC_SetOffsetTime(int  delta)
{
  rtcOffsetTime = delta;
}


uint32_t NWRM_RTC_GetOverflowCounter(void)
{
  return rtcOverflowCounter;
}


time_t __time32(time_t *timer)
{
	time_t t;


	t = rtcOverflowCounter * NWRM_RTC_OVERFLOW_PERIOD;


	if (NWRM_RTC_OVERFLOWED != 0) {
		t += rtcOverflowCounter * NWRM_RTC_OVERFLOWED / NWRM_RTC_TICKS_PER_SECOND;
	}


	t += RTC->CNT / NWRM_RTC_TICKS_PER_SECOND;
	t += rtcOffsetTime;


	if (!timer) {
		timer = &t;
	}
	return t;
}


