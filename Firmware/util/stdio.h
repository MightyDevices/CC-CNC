/*
 * sstdio.h
 *
 *  Created on: 22-12-2012
 *      Author: Tomek
 */

#ifndef UTIL_STDIO_H_
#define UTIL_STDIO_H_


#include <stdarg.h>

/* sends formatted output to a string using an argument list passed to it. */
int vsprintf(char *out, char *fmt, va_list args);
/* simple sprintf */
int sprintf(char *out, char *fmt, ...);
/* simple sscanf */
int sscanf(char *in, char *fmt, ...);

/* check if character is digit */
int is_digit(char c);
/* check if character is hex digit */
int is_xdigit(char c);
/* is whitespace */
int is_space(char c);
/* convert string to integer */
int atoi(char *str);

#endif /* SSTDIO_H_ */
