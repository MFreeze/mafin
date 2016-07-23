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
exist_file(const char *path)
{
    FILE *pfile = fopen(path, "r");
    if (!pfile)
        return 0;
    fclose(pfile);
    return 1;
}

    int
main (int argc, char **argv) 
{
    // TODO Management of the options
    sqlite3 *database;

    // Test if the database exists
    if (!exist_file(DATABASE_FILE))
        init_database(DATABASE_FILE);

    int return_code = sqlite3_open(DATABASE_FILE, &database);
    if (return_code != SQLITE_OK)
    {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(database));
        exit(EXIT_FAILURE);
    }

    printf("Open database ok\n");
    
    sqlite3_close(database);
    return EXIT_SUCCESS;
}
