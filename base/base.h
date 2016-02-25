#ifndef _BASE_H_
#define _BASE_H_

#include <stdio.h>


#ifdef DEBUG
#define verify(expr, fmt, ...) do { if (!(expr)) fprintf(stderr, "%s verify failed at %s, %d:", fmt, #expr, __FILE__, __LINE__, __VA_ARGS__); } while(0)
#else
#define verify(expr, fmt, ...) do { if (!(expr)) fprintf(stderr, fmt, __VA_ARGS__); } while(0)
#endif


#endif
