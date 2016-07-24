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

#define OL_DEB_PRINT(...) do {\
    fprintf(stderr,##__VA_ARGS__);\
}while(0);

#define PRINTVAR(x) fprintf(stderr, "DEBUG -- %s:%d " #x " = %d\n", __FILE__, __LINE__, x);
#define OL_PRINTVAR(x) fprintf(stderr, "%s:%d " #x " = %d\n", __FILE__, __LINE__, x);

#define PRINTSTR(x) fprintf(stderr, "DEBUG -- %s:%d " #x " = %s\n", __FILE__, __LINE__, x);
#define OL_PRINTSTR(x) fprintf(stderr, "%s:%d " #x " = %s\n", __FILE__, __LINE__, x);

#define PRINTACC(x) fprintf(stderr, "DEBUG -- %s:%d " #x " : {\n"\
        "\tId: %d,\n"\
        "\tLabel: %s,\n"\
        "\tInitial Balance: %lf,\n"\
        "\tIban: %s\n"\
        "}\n", __FILE__, __LINE__, x.account_id, x.label, x.initial_balance, x.iban);
#define OL_PRINTACC(x) fprintf(stderr, "DEBUG -- %s:%d " #x " : { "\
        "Id: %d, "\
        "Label: %s, "\
        "Initial Balance: %lf, "\
        "Iban: %s "\
        "}", __FILE__, __LINE__, x.account_id, x.label, x.initial_balance, x.iban);

#define PRINTPROD(x) fprintf(stderr, "DEBUG -- %s:%d " #x " : {\n"\
        "\tId: %d,\n"\
        "\tLabel: %s,\n"\
        "\tFather: %d\n"\
        "}\n", __FILE__, __LINE__, x.product_id, x.label, x.father_id);
#define OL_PRINTPROD(x) fprintf(stderr, "DEBUG -- %s:%d " #x " : {"\
        "Id: %d, "\
        "Label: %s, "\
        "Father: %d "\
        "}", __FILE__, __LINE__, x.product_id, x.label, x.father_id);

#else
#define DEB_PRINT(...)
#define PRINTVAR(x)
#define PRINTSTR(x)
#define PRINTACC(x)
#define PRINTPROD(x)
#define OL_DEB_PRINT(...)
#define OL_PRINTVAR(x)
#define OL_PRINTSTR(x)
#define OL_PRINTACC(x)
#define OL_PRINTPROD(x)
#endif

#endif
