/*
 * =====================================================================================
 *
 *       Filename:  mafin.c
 *
 *    Description:  Personal finance manage with command line interface
 *
 *        Version:  1.0
 *        Created:  22/07/2016 17:48:26
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include "mafin_db.h"

#define DATABASE_FILE "mafin.db"

    int
main (int argc, char **argv) 
{
    // TODO Management of the options
    int return_code = Initialize_Database(DATABASE_FILE);
    if (!return_code)
    {
        fprintf(stderr, "Error while opening database: %s\n", DATABASE_FILE);
        return EXIT_FAILURE;
    }

    printf("Open database ok\n");
    
    Close_Database();
    return EXIT_SUCCESS;
}
