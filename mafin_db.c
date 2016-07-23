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

sqlite3 *g_db = NULL;

/*-----------------------------------------------------------------------------
 *  Internal Functions
 *-----------------------------------------------------------------------------*/

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Open_File
 *  Description:  Test if a file can be opened
 *   Parameters:  const char *path: the path of the file
 *       Return:  0 if the file cannot be opened, 1 otherwise
 * =====================================================================================
 */
    int
Open_File (const char *path)
{
    FILE *pfile = fopen(path, "r");
    if (!pfile)
        return 0;
    fclose(pfile);
    return 1;
}		/* -----  end of function File_Exists  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Exec_Request
 *  Description:  Execute a simple request with no returns
 *                  1. Compile the SQL statement
 *                  2. Execute the statement
 *                  3. Reset
 *   Parameters:  * const char *request: the request to compile
 *                * const char *function_name: function name in which this procedure is
 *                      called
 *       Return:  0 if an error occurs, 1 otherwise
 * =====================================================================================
 */
    int
Exec_Request(const char *request, const char *function_name)
{
    IS_INITIALIZED;

    int return_code;
    sqlite3_stmt *req;

    // Prepare the request
    return_code = sqlite3_prepare_v2(g_db, request, -1, &req, NULL);
    if (return_code != SQLITE_OK)
    {
        fprintf(stderr, "[%s.sqlite3_prepare_v2] Error: %s\n", function_name, sqlite3_errmsg(g_db));
        return 0;
    }

    // Execute the request 
    // TODO Better handle of the error
    return_code = sqlite3_step(req);
    if (return_code != SQLITE_DONE)
    {
        fprintf(stderr, "[%s.sqlite3_step] Error: %s\n", function_name, sqlite3_errmsg(g_db));
        return 0;
    }

    // Reset the statement
    return_code = sqlite3_reset(req);
    if (return_code != SQLITE_OK)
    {
        fprintf(stderr, "[%s.sqlite3_step] Error: %s\n", function_name, sqlite3_errmsg(g_db));
        return 0;
    }

    return 1;
}		/* -----  end of function Exec_Request  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Create_Database
 *  Description:  Creates the database and populates it with tables
 *   Parameters:  
 *       Return:  0 if an error occurs, 1 otherwise
 * =====================================================================================
 */
    int
Create_Database()
{
    IS_INITIALIZED;

    int return_code;

    // (Account table) create the table
    return_code = Exec_Request(ACC_TABLE, "(Account) init_database");
    DEB_PRINT("%s\n", ACC_TABLE);
    if (!return_code)
    {
        sqlite3_close(g_db);
        return 0;
    }

    // (Product Table) 
    return_code = Exec_Request(PROD_TABLE, "(Product) init_database");
    DEB_PRINT("%s\n", PROD_TABLE);
    if (!return_code)
    {
        sqlite3_close(g_db);
        return 0;
    }

    // (Operations Table) 
    return_code = Exec_Request(OP_TABLE, "(Operations) init_database");
    DEB_PRINT("%s\n", OP_TABLE);
    if (!return_code)
    {
        sqlite3_close(g_db);
        return 0;
    }

    // (Joint Table) 
    return_code = Exec_Request(PR_OP_JOINT, "(Joint) init_database");
    DEB_PRINT("%s\n", PR_OP_JOINT);
    if (!return_code)
    {
        sqlite3_close(g_db);
        return 0;
    }

    return 1;
}		/* -----  end of function Create_Database  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Initialize_Database
 *  Description:  Open the database if it exists or create a new one
 *   Parameters:  const char *g_db_path: the path of the database file
 *       Return:  0 is an error occurs, 1 otherwise
 * =====================================================================================
 */
    int
Initialize_Database (const char *g_db_path)
{
    int return_code;
    int exists = Open_File(g_db_path);

    return_code = sqlite3_open_v2(g_db_path, &g_db, SQLITE_OPEN_READWRITE|SQLITE_OPEN_CREATE, NULL);

    if (return_code != SQLITE_OK)
    {
        fprintf(stderr, "[init_database.sqlite3_open] Error: %s\n", sqlite3_errmsg(g_db));
        return 0;
    }

    if (!exists)
        return(Create_Database());

	return 1;
}		/* -----  end of function Initialize_Database  ----- */


