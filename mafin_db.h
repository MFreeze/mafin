/*
 * =====================================================================================
 *
 *       Filename:  mafin_db.h
 *
 *    Description:  Database Management Library
 *
 *        Version:  1.0
 *        Created:  22/07/2016 18:21:58
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef MAFIN_DB_H

#include <sqlite3.h>

    int
init_database(const char *db_path)
{
    sqlite3 *db;
    int return_code = sqlite3_open(DATABASE_FILE, &db);

    if (return_code != SQLITE_OK)
    {
        fprintf(stderr, "Error: %s\n", sqlite3_errmsg(database));
        return 0;
    }

    sqlite3_close(db);
}

#endif
