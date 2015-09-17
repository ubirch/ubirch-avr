#ifndef _UBIRCH_SUART_H_
#define _UBIRCH_SUART_H_

#include <stdio.h>

#define SUART_TXD
#define SUART_RXD

extern void suart_init();

#ifdef SUART_TXD

extern void suart_putchar(const char, FILE *stream);

#endif // SUART_RXD

#ifdef SUART_RXD

extern int suart_getchar();

#define suart_getchar(s) suart_getchar_wait()

#endif // SUART_RXD

#endif /* _UBIRCH_SUART_H_ */