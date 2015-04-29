#ifndef __NWRM_RTC_H
#define __NWRM_RTC_H

#include "em_rtc.h"

#ifdef __cplusplus
extern "C" {
#endif



/** Duration values in ticks based on 32768 Hz clock */
#define  T0S               0							/**< 0 s */
#define  T20MICROS         1							/**< 20 탎 */
#define  T50MICROS         2							/**< 50 탎 */
#define  T100MICROS        2							/**< 100 탎 */
#define  T125MICROS        3							/**< 125 탎 */
#define  T150MICROS        4							/**< 150 탎 */
#define  T200MICROS        ((32768 * 2 ) / 10000)		/**< 200 탎 */
#define  T300MICROS        ((32768 * 3 ) / 10000)		/**< 300 탎 */
#define  T400MICROS        ((32768 * 4 ) / 10000)		/**< 400 탎 */
#define  T480MICROS        ((32768 * 48 )/ 100000)		/**< 480 탎 */
#define  T500MICROS        ((32768 * 5 ) / 10000)		/**< 500 탎 */
#define  T600MICROS        ((32768 * 6 ) / 10000)		/**< 600 탎 */
#define  T700MICROS        ((32768 * 7 ) / 10000)		/**< 700 탎 */
#define  T800MICROS        ((32768 * 8 ) / 10000)		/**< 800 탎 */
#define  T900MICROS        ((32768 * 9 ) / 10000)		/**< 900 탎 */
#define  T1MS              ((32768 * 10) / 10000)		/**< 1 ms */
#define  T1_25MS           ((32768 * 125) / 100000)		/**< 1.25 ms */
#define  T1_6MS            ((32768 * 160) / 100000)		/**< 1.6 ms */
#define  T2MS              ((32768 * 20) / 10000)		/**< 2 ms */
#define  T2_5MS            ((32768 * 25) / 10000)		/**< 2.5 ms */
#define  T3_32MS           ((32768 * 33) / 10000)		/**< 3.32 ms */
#define  T3_8MS            ((32768 * 38) / 10000)		/**< 3.8 ms */
#define  T5MS              ((32768 * 50) / 10000)		/**< 5 ms */
#define  T7MS              ((32768 * 70) / 10000)		/**< 6 ms */
#define  T7_5MS            ((32768 * 75) / 10000)		/**< 7.5 ms */
#define  T8_5MS            ((32768 * 85) / 10000)		/**< 8.5 ms */
#define  T10MS             ((32768 * 100) / 10000)		/**< 10 ms */
#define  T11MS             ((32768 * 110) / 10000)		/**< 11 ms */
#define  T12MS             ((32768 * 120) / 10000)		/**< 12 ms */
#define  T15MS             ((32768 * 150) / 10000)		/**< 15 ms */
#define  T16MS             ((32768 * 160) / 10000)		/**< 16 ms */
#define  T17MS             ((32768 * 171) / 10000)		/**< 17 ms */
#define  T19_2MS           ((32768 * 192) / 10000)		/**< 19.2 ms */
#define  T20MS             ((32768 * 200) / 10000)		/**< 20 ms */
#define  T20_2MS           ((32768 * 202) / 10000)		/**< 20 ms */
#define  T24MS             ((32768 * 240) / 10000)		/**< 24 ms */
#define  T24_5MS           ((32768 * 245) / 10000)		/**< 24 ms */
#define  T27_6MS           ((32768 * 276) / 10000)		/**< 27 ms */   
#define  T38MS             ((32768 * 380) / 10000)		/**< 38 ms */
#define  T44MS             ((32768 * 440) / 10000)		/**< 44 ms */
#define  T50MS             ((32768 * 500) / 10000)		/**< 50 ms */
#define  T100MS            ((32768 * 1000) / 10000)		/**< 100 ms */
#define  T200MS            ((32768 * 2000) / 10000)		/**< 200 ms */
#define  T250MS            ((32768 * 2500) / 10000)		/**< 250 ms */
#define  T300MS            ((32768 * 3000) / 10000)		/**< 300 ms */
#define  T350MS            ((32768 * 3500) / 10000)		/**< 350 ms */
#define  T400MS            ((32768 * 4000) / 10000)		/**< 400 ms */
#define  T500MS            ((32768 * 5000) / 10000)		/**< 500 ms */
#define  T800MS            ((32768 * 8000) / 10000)		/**< 800 ms */
#define  T900MS            ((32768 * 9000) / 10000)		/**< 900 ms */
#define  T950MS            ((32768 * 9500) / 10000)		/**< 950 ms */
#define  T1S               ((32768 * 10000) / 10000)	/**< 1 s */
#define  T1060MS           ((32768 * 10600) / 10000)	/**< 1.06 s */
#define  T1100MS           ((32768 * 11000) / 10000)	/**< 1.1 s */
#define  T1200MS           ((32768 * 12000) / 10000)	/**< 1.2 s */
#define  T2S               (T1S * 2)					/**< 2 s */
#define  T3S               (T1S * 3)					/**< 3 s */
#define  T4S               (T1S * 4)					/**< 4 s */
#define  T5S               (T1S * 5)					/**< 5 s */
#define  T8S               (T1S * 8)					/**< 8 s */
#define  T10S              (T1S * 10)					/**< 10 s */
#define  T20S              (T1S * 20)					/**< 20 s */
#define  T30S              (T1S * 30)					/**< 30 s */
#define  T1MN              (T1S * 60)					/**< 1 min */
#define  T2MN              (T1S * 60 * 2)				/**< 2 min */
#define  T8MN              (T1S * 60 * 8)				/**< 8 min */
#define  T8H               (T1S * 3600 * 8)				/**< 8 h */
#define  T24H              0xA8C00000					/**< 24 h */
#define  TICK8MMBYDAY      (60 * 3)						/**< Number of 8 min ticks /day */
#define  TICK32768_BYDAY   (T24H)						/**< Number of 32768 hz ticks /day */
#define  TICK1MMBYDAY      (24 * 60)					/**< Number of 1 min ticks /day */


#define NWRM_RTC_SYSTEM		0


#define NWRM_RTC_USER			1


#define NWRM_RTC_Now()        RTC_CounterGet()




typedef void (*NWRM_RTC_handler_t)(void);



 static __INLINE void NWRM_RTC_AlarmAt(uint32_t count)
 {
	 RTC_CompareSet(NWRM_RTC_SYSTEM, count);
 }


static __INLINE void NWRM_RTC_AlarmAfter(uint32_t delay)
{
	RTC_CompareSet(NWRM_RTC_SYSTEM, delay + RTC_CounterGet());
}


static __INLINE void NWRM_RTC_Abort(void)
{
    extern volatile bool NWRM_RTC_AbortDelay;
    NWRM_RTC_AbortDelay = true;
}


static __INLINE void NWRM_RTC_AckKeepAlive(void)
{
	extern volatile bool NWRM_RTC_KeepAlive;
	NWRM_RTC_KeepAlive = false;
}


static __INLINE void NWRM_RTC_EnableSystemInterrupts(bool enable)
{
	if (enable) {
		RTC_IntEnable(RTC_IF_COMP0);
	} else {
		RTC_IntDisable(RTC_IF_COMP0);
	}
}


static __INLINE void NWRM_RTC_EnableUserInterrupts(bool enable)
{
	if (enable) {
		RTC_IntEnable(RTC_IF_COMP1);
	} else {
		RTC_IntDisable(RTC_IF_COMP1);
	}
}


static __INLINE void NWRM_RTC_ClearSystemInterrupts(void)
{
	RTC_IntClear(RTC_IFC_COMP0);
}


static __INLINE void NWRM_RTC_ClearUserInterrupts(void)
{
	RTC_IntClear(RTC_IFC_COMP1);
}



void NWRM_RTC_Init(NWRM_RTC_handler_t function);
bool NWRM_RTC_Delay(uint32_t duration);
void NWRM_RTC_Sleep(void);
NWRM_RTC_handler_t NWRM_RTC_SetSystemHandler(NWRM_RTC_handler_t function);
NWRM_RTC_handler_t NWRM_RTC_SetUserHandler(NWRM_RTC_handler_t function);
void NWRM_RTC_SetKeepAliveHandler(NWRM_RTC_handler_t function, uint32_t period);
void NWRM_RTC_CalibratedDelay(uint32_t udelay);
uint32_t NWRM_RTC_TimeDiff(uint32_t reference);
uint32_t NWRM_RTC_GetOverflowCounter(void);
void NWRM_RTC_SetOffsetTime(int delta);


extern volatile bool NWRM_RTC_KeepAlive;
extern NWRM_RTC_handler_t NWRM_RTC_KeepAliveHandler;



#ifdef __cplusplus
}
#endif

#endif // __NWRM_RTC_H
