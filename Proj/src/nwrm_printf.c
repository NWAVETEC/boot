#include "nwrm_module.h"
#include "em_usart.h"
#include "nwrm_printf.h"




#define PRINTF_LONG_SUPPORT




typedef void    (*putcf)(void *, char);


typedef void    (*start)(void *);


typedef void    (*stop)(void *);




static void *   stdout_putp;


static putcf    stdout_putf;


static start    stdout_start;


static stop     stdout_stop;



#ifdef PRINTF_LONG_SUPPORT


static void uli2a(unsigned long int num, unsigned int base, int uc, char *bf)
{
    int             n = 0;
    unsigned int    d = 1;

    while (num / d >= base) {
        d *= base;
    }
    while (d != 0) {
        int    dgt = num / d;
        num %= d;
        d   /= base;
        if (n || dgt > 0 || d == 0) {
            *bf++ = dgt + (dgt < 10 ? '0' : (uc ? 'A' : 'a') - 10);
            ++n;
        }
    }
    *bf = 0;
}


static void li2a(long num, char *bf)
{
    if (num < 0) {
        num   = -num;
        *bf++ = '-';
    }
    uli2a(num, 10, 0, bf);
}

#endif


static void ui2a(unsigned int num, unsigned int base, int uc, char *bf)
{
    int             n = 0;
    unsigned int    d = 1;

    while (num / d >= base) {
        d *= base;
    }
    while (d != 0) {
        int    dgt = num / d;
        num %= d;
        d   /= base;
        if (n || dgt > 0 || d == 0) {
            *bf++ = dgt + (dgt < 10 ? '0' : (uc ? 'A' : 'a') - 10);
            ++n;
        }
    }
    *bf = 0;
}


static void i2a(int num, char *bf)
{
    if (num < 0) {
        num   = -num;
        *bf++ = '-';
    }
    ui2a(num, 10, 0, bf);
}


static int a2d(char ch)
{
    if (ch >= '0' && ch <= '9') {
        return(ch - '0');
    } else if (ch >= 'a' && ch <= 'f') {
        return(ch - 'a' + 10);
    } else if (ch >= 'A' && ch <= 'F') {
        return(ch - 'A' + 10);
    } else {
        return(-1);
    }
}


static char a2i(char ch, char **src, int base, int *nump)
{
    char *  p   = *src;
    int     num = 0;
    int     digit;

    while ((digit = a2d(ch)) >= 0) {
        if (digit > base) {
            break;
        }
        num = num * base + digit;
        ch  = *p++;
    }
    *src  = p;
    *nump = num;
    return(ch);
}


static void putchw(void *putp, putcf putf, int n, char z, char *bf)
{
    char    fc = z ? '0' : ' ';
    char    ch;
    char *  p = bf;

    while (*p++ && n > 0) {
        n--;
    }
    while (n-- > 0) {
        putf(putp, fc);
    }
    while ((ch = *bf++)) {
        putf(putp, ch);
    }
}


static void putcp(void *p, char c)
{
    *(*((char **)p))++ = c;
}


static void putcs(void *putp, putcf putf, char *bf)
{
    char ch;
    while ((ch = *bf++)) {
        putf(putp, ch);
    }
}


void tfp_format(void *putp, putcf putf, char *fmt, va_list va)
{
    char    bf[12];

    char    ch;

    while ((ch = *(fmt++))) {
        if (ch != '%') {
            putf(putp, ch);
        } else {
            char    lz = 0;
#ifdef  PRINTF_LONG_SUPPORT
            char    lng = 0;
#endif
            int     w = 0;
            ch = *(fmt++);
            if (ch == '0') {
                ch = *(fmt++);
                lz = 1;
            }
            if (ch >= '0' && ch <= '9') {
                ch = a2i(ch, &fmt, 10, &w);
            }
#ifdef  PRINTF_LONG_SUPPORT
            if (ch == 'l') {
                ch  = *(fmt++);
                lng = 1;
            }
#endif
            switch (ch) {
                case 0:
                    goto abort;
                case 'u': {
#ifdef  PRINTF_LONG_SUPPORT
                    if (lng) {
                        uli2a(va_arg(va, unsigned long int), 10, 0, bf);
                    } else
#endif
                    ui2a(va_arg(va, unsigned int), 10, 0, bf);
                    putchw(putp, putf, w, lz, bf);
                    break;
                }
                case 'd': {
#ifdef  PRINTF_LONG_SUPPORT
                    if (lng) {
                        li2a(va_arg(va, unsigned long int), bf);
                    } else
#endif
                    i2a(va_arg(va, int), bf);
                    putchw(putp, putf, w, lz, bf);
                    break;
                }
                case 'x': case 'X':
#ifdef  PRINTF_LONG_SUPPORT
                    if (lng) {
                        uli2a(va_arg(va, unsigned long int), 16, (ch == 'X'), bf);
                    } else
#endif
                    ui2a(va_arg(va, unsigned int), 16, (ch == 'X'), bf);
                    putchw(putp, putf, w, lz, bf);
                    break;
                case 'c':
                    putf(putp, (char)(va_arg(va, int)));
                    break;
                case 's':
                    putchw(putp, putf, w, 0, va_arg(va, char *));
                    break;
                case '%':
                    putf(putp, ch);
                    break;
                default:
                    break;
            }
        }
    }
abort:;
}


void init_printf(void *putp, void (*putf)(void *, char), void (*start)(void *), void (*stop)(void *))
{
    stdout_putp     = putp;
    stdout_putf     = putf;
    stdout_start    = start;
    stdout_stop     = stop;
}


void tfp_printf(char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    if (stdout_start) {
    	stdout_start(stdout_putp);
    }
    tfp_format(stdout_putp, stdout_putf, fmt, va);
    if (stdout_stop) {
    	stdout_stop(stdout_putp);
    }
    va_end(va);
}


void tfp_vprintf(char *fmt, va_list va)
{
    if (stdout_start) {
    	stdout_start(stdout_putp);
    }
    tfp_format(stdout_putp, stdout_putf, fmt, va);
    if (stdout_stop) {
    	stdout_stop(stdout_putp);
    }
}


void tfp_sprintf(char *s, char *fmt, ...)
{
    va_list va;

    va_start(va, fmt);
    tfp_format(&s, putcp, fmt, va);
    putcp(&s, 0);
    va_end(va);
}


void tfp_dump(char *text, unsigned char *s, unsigned char sz)
{
    static char dump[(3 * 16) + 3];
    unsigned char i, j, k;
    char hex[4];

    if (stdout_start) {
        stdout_start(stdout_putp);
    }

    putcs(stdout_putp, stdout_putf, text);

    for (i = 0, k = 0; i < sz; i++) {
        tfp_sprintf(hex, "%02x ", s[i]);
        for (j = 0; hex[j]; j++) {
            dump[k++] = hex[j];
        }
        if (i && ((i % 16) == 0)) {
            dump[k++] = '\r';
            dump[k++] = '\n';
            dump[k]   = '\0';
            putcs(stdout_putp, stdout_putf, dump);
            k = 0;
        }
    }

    if (k > 0) {
        dump[k++] = '\r';
        dump[k++] = '\n';
        dump[k]   = '\0';
        putcs(stdout_putp, stdout_putf, dump);
    }

    if (stdout_stop) {
        stdout_stop(stdout_putp);
    }
}


