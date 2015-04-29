
#ifndef __NWRM_PRINTF
#define __NWRM_PRINTF





/** Flag to include printf() support */

#define USE_PRINTF



/** Flag to include UART support */

#define USE_UART





#ifndef USE_PRINTF

/** sprintf() conditional macro */
#define SPRINTF(i)

/** printf() conditional macro */
#define PRINTF(i)

/** dump() conditional macro */
#define DUMP(i)

#else // USE_PRINTF

#include <stdarg.h>


 #define printf         tfp_printf
 #define sprintf        tfp_sprintf

 #define SPRINTF(i)     tfp_sprintf i
 #define PRINTF(i)      tfp_printf i
 #define DUMP(i)        tfp_dump i



void init_printf(void *putp, void (*putf)(void *, char), void (*start)(void *), void (*stop)(void *));
void tfp_printf(char *fmt, ...);
void tfp_vprintf(char *fmt, va_list va);
void tfp_sprintf(char *s, char *fmt, ...);
void tfp_dump(char *text, unsigned char *s, unsigned char sz);
void tfp_format(void *putp, void (*putf)(void *, char), char *fmt, va_list va);




#endif // USE_PRINTF

#endif // __NWRM_PRINTF
