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

// TODO Change the return code to handle the different kinds of error:
// * Already in the table
// * ...
#include "mafin_db.h"

sqlite3 *g_db = NULL;
int mafin_db_return_code;

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
// XXX This function should not be used as it does the same as sqlite3_exec in worst...
    int
Exec_Request(const char *request, const char *function_name)
{
    IS_INITIALIZED;

    sqlite3_stmt *req;

    // Prepare the request
    mafin_db_return_code = sqlite3_prepare_v2(g_db, request, -1, &req, NULL);
    if (mafin_db_return_code != SQLITE_OK)
    {
        fprintf(stderr, "[%s.sqlite3_prepare_v2] Error: %s\n", function_name, sqlite3_errmsg(g_db));
        return 0;
    }

    // Execute the request 
    // TODO Better handle of the error
    mafin_db_return_code = sqlite3_step(req);
    if (mafin_db_return_code != SQLITE_DONE)
    {
        fprintf(stderr, "[%s.sqlite3_step] Error: %s\n", function_name, sqlite3_errmsg(g_db));
        // Delete the statement and store the ERROR code in mafin_db_return_code
        mafin_db_return_code = sqlite3_finalize(req);
        return 0;
    }

    // Delete the statement
    mafin_db_return_code = sqlite3_finalize(req);
    if (mafin_db_return_code != SQLITE_OK)
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


/*-----------------------------------------------------------------------------
 *  Callbacks
 *-----------------------------------------------------------------------------*/

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Fill_Account_Struct
 *  Description:  Call back function that fills an account struct with the last returned
 *                  result of the select
 *   Parameters:  void *data: the structure that is filled
 *                int nbcol: the number of columns in the row
 *                char **col_vals: the value in the column
 *                char **col_names: the name of the columns
 *       Return:  1 to stop the processing, 0 otherwise
 * =====================================================================================
 */
    static int
Fill_Account_Struct (void *data, int nbcol, char **col_vals, char **col_names)
{
    DEB_PRINT("[Fill_Account_Struct] Got %d columns.\n", nbcol);
    int i = 0;
    Account *account = (Account *) data;
    for (i=0; i<nbcol; i++)
    {
        if (!strcmp(col_names[i], "AccId"))
            account->account_id = atoi(col_vals[i]);
        else if (!strcmp(col_names[i], "Label"))
            strncpy(account->label, col_vals[i], 257);
        else if (!strcmp(col_names[i], "Iban"))
        {
            if (col_vals[i])
                strncpy(account->iban, col_vals[i], 35);
        }
        else if (!strcmp(col_names[i], "InitBal"))
            account->initial_balance = atof(col_vals[i]);
    }
	return 0;
}		/* -----  end of function Fill_Account_Struct  ----- */


/*-----------------------------------------------------------------------------
 *  Global functions
 *-----------------------------------------------------------------------------*/

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



/*-----------------------------------------------------------------------------
 *  Insert management
 *-----------------------------------------------------------------------------*/

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Add_Account
 *  Description:  Add a new account to the database
 *   Parameters:  const char label[257]: Label of the account
 *                double init_balance: Initial balance of the account
 *                const char iban[35]: IBAN code of the account
 *       Return:  0 if an error occurs, 1 otherwise
 * =====================================================================================
 */
    int
Add_Account (const char label[257], double init_balance, const char iban[35])
{
    char request[MAX_REQ_SIZE], buf[MAX_REQ_SIZE];

    // No need to test if the string has been truncated until the user values are used
    SNPRINTF(request, MAX_REQ_SIZE, "insert into Accounts(Label, InitBal");
    if (iban)
    {
        SNPRINTF(buf, MAX_REQ_SIZE, "%s, Iban", request);
        strncpy(request, buf, MAX_REQ_SIZE);
    }


    SNPRINTF(buf, MAX_REQ_SIZE, "%s) values ('%s', %.2lf", request, label, init_balance);
    strncpy(request, buf, MAX_REQ_SIZE);
    if (iban)
    {
        SNPRINTF(buf, MAX_REQ_SIZE, "%s, '%s'", request, iban);
        strncpy(request, buf, MAX_REQ_SIZE);
    }


    SNPRINTF(buf, MAX_REQ_SIZE, "%s);", request);
    strncpy(request, buf, MAX_REQ_SIZE);

    PRINTSTR(request);
    return Exec_Request(request, "Add_Account");
}		/* -----  end of function Add_Account  ----- */


/*-----------------------------------------------------------------------------
 *  Select Management
 *-----------------------------------------------------------------------------*/

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Get_Account_From_Id
 *  Description:  Fill an account structure from an Account ID
 *   Parameters:  int account_id: the id of the account to select
 *                Account *result: the structure filled by the function. If no account
 *                  is selected then the structure is left untouched
 *       Return:  0 if an error occurs, 1 otherwise
 * =====================================================================================
 */
    int
Get_Account_From_Id (int account_id, Account *result)
{
    char request[MAX_REQ_SIZE];
    char *err_msg;

    Account tmp;
    memset(&tmp, 0, sizeof(tmp));

    SNPRINTF(request, MAX_REQ_SIZE, "select * from Accounts where AccId = %d;", account_id);

    mafin_db_return_code = sqlite3_exec(g_db, request, Fill_Account_Struct, (void *) &tmp, &err_msg);
    if (mafin_db_return_code != SQLITE_OK)
    {
        fprintf(stderr, "[Get_Account_From_Id.exec] %s\n", err_msg);
        sqlite3_free(err_msg);
        return 0;
    }

    if (tmp.account_id)
    {
        memcpy(result, &tmp, sizeof(Account));
        return 1;
    }

	return NO_ENTRY_FOUND;
}		/* -----  end of function Get_Account_From_Id  ----- */
