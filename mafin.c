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

    Add_Account("Test", 12.8, NULL);
    Add_Account("Pouet", 1400, "1349BM24");
    Add_Account("Poueta", 1400, "1349BM24");

    Account a;
    memset(&a, 0, sizeof(Account));
    PRINTACC(a);

    return_code = Get_Account_From_Id(1, &a);
    if (return_code == NO_ENTRY_FOUND)
        DEB_PRINT("Id 1 not found.\n");
    PRINTACC(a);

    return_code = Get_Account_From_Id(4, &a);
    if (return_code == NO_ENTRY_FOUND)
        DEB_PRINT("Id 4 not found.\n");
    PRINTACC(a);
    
    Close_Database();
    return EXIT_SUCCESS;
}
