/*
 * =====================================================================================
 *
 *       Filename:  mafin_db.c
 *
 *    Description:  Database Management Library Code
 *
 *        Version:  1.0
 *        Created:  22/07/2016 18:33:45
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Guillerme Duvillié (mfreeze), guillerme@duvillie.eu
 *   Organization:  
 *
 * =====================================================================================
 */

#include "mafin_db.h"

    int
exec_create_request(sqlite3 *db, const char *request, const char *function_name)
{
    int return_code;
    sqlite3_stmt *req;

    // Prepare the request
    return_code = sqlite3_prepare_v2(db, request, -1, &req, NULL);
    if (return_code != SQLITE_OK)
    {
        fprintf(stderr, "[%s.sqlite3_prepare_v2] Error: %s\n", function_name, sqlite3_errmsg(db));
        return 0;
    }

    // Execute the request 
    // TODO Better handle of the error
    return_code = sqlite3_step(req);
    if (return_code != SQLITE_DONE)
    {
        fprintf(stderr, "[%s.sqlite3_step] Error: %s\n", function_name, sqlite3_errmsg(db));
        return 0;
    }

    // Reset the statement
    return_code = sqlite3_reset(req);
    if (return_code != SQLITE_OK)
    {
        fprintf(stderr, "[%s.sqlite3_step] Error: %s\n", function_name, sqlite3_errmsg(db));
        return 0;
    }

    return 1;
}

    int
init_database(const char *db_path)
{
    sqlite3 *db;
    int return_code = sqlite3_open_v2(db_path, &db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, NULL);

    if (return_code != SQLITE_OK)
    {
        fprintf(stderr, "[init_database.sqlite3_open] Error: %s\n", sqlite3_errmsg(db));
        return 0;
    }

    // (Account table) create the table
    return_code = exec_create_request(db, ACC_TABLE, "(Account) init_database");
    if (!return_code)
    {
        sqlite3_close(db);
        return 0;
    }

    // (Product Table) 
    return_code = exec_create_request(db, PROD_TABLE, "(Product) init_database");
    if (!return_code)
    {
        sqlite3_close(db);
        return 0;
    }

    // (Operations Table) 
    return_code = exec_create_request(db, OP_TABLE, "(Operations) init_database");
    if (!return_code)
    {
        sqlite3_close(db);
        return 0;
    }

    // (Joins Table) 
    return_code = exec_create_request(db, PR_OP_JOINT, "(Joint) init_database");
    if (!return_code)
    {
        sqlite3_close(db);
        return 0;
    }


    sqlite3_close(db);
    return 1;
}

