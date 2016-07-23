/*
 * =====================================================================================
 *
 *       Filename:  debug.h
 *
 *    Description:  Debug Library header
 *
 *        Version:  1.0
 *        Created:  18/10/2015 12:52:49
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), gduvillie@openmailbox.org
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>

#ifdef DEBUG
#define DEB_PRINT(...) do {\
    fprintf(stderr, "DEBUG -- ");\
    fprintf(stderr,##__VA_ARGS__);\
}while(0);

#define PRINTVAR(x) fprintf(stderr, "DEBUG -- %s:%d " #x " = %d\n", __FILE__, __LINE__, x);
#else
#define DEB_PRINT(...)
#define PRINTVAR(x)
#endif

#endif
