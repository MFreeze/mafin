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
int mafin_db_extended_return_code;

/*-----------------------------------------------------------------------------
 *  Internal Functions
 *-----------------------------------------------------------------------------*/

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Open_File
 *  Description:  Test if a file can be opened
 *   Parameters:  const char *path: the path of the file
 *       Return:  SUCCESS if file can be opened, UNDEFINED_ERROR otherwise
 * =====================================================================================
 */
    int
Open_File (const char *path)
{
    FILE *pfile = fopen(path, "r");
    if (!pfile)
        return UNDEFINED_ERROR;
    fclose(pfile);
    return SUCCESS;
}		/* -----  end of function File_Exists  ----- */



/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Create_Database
 *  Description:  Creates the database and populates it with tables
 *   Parameters:  
 *       Return:  The appropriate code
 * =====================================================================================
 */
    int
Create_Database()
{
    IS_INITIALIZED;

    char *errmsg;

    // (Account table) create the table
    DEB_PRINT("%s\n", ACC_TABLE);
    mafin_db_return_code = sqlite3_exec(g_db, ACC_TABLE, NULL, NULL, &errmsg);
    if (mafin_db_return_code != SQLITE_OK)
    {
        fprintf(stderr, "[Create_Database.exec] Cannot create Accounts table: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(g_db);
        return UNDEFINED_ERROR;
    }

    // (Product Table) 
    DEB_PRINT("%s\n", PROD_TABLE);
    mafin_db_return_code = sqlite3_exec(g_db, PROD_TABLE, NULL, NULL, &errmsg);
    if (mafin_db_return_code != SQLITE_OK)
    {
        fprintf(stderr, "[Create_Database.exec] Cannot create Products table: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(g_db);
        return UNDEFINED_ERROR;
    }

    // (Operations Table) 
    DEB_PRINT("%s\n", OP_TABLE);
    mafin_db_return_code = sqlite3_exec(g_db, OP_TABLE, NULL, NULL, &errmsg);
    if (mafin_db_return_code != SQLITE_OK)
    {
        fprintf(stderr, "[Create_Database.exec] Cannot create Operations table: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(g_db);
        return UNDEFINED_ERROR;
    }

    // (Joint Table) 
    DEB_PRINT("%s\n", PR_OP_JOINT);
    mafin_db_return_code = sqlite3_exec(g_db, PR_OP_JOINT, NULL, NULL, &errmsg);
    if (mafin_db_return_code != SQLITE_OK)
    {
        fprintf(stderr, "[Create_Database.exec] Cannot create Joint table: %s\n", errmsg);
        sqlite3_free(errmsg);
        sqlite3_close(g_db);
        return UNDEFINED_ERROR;
    }

    return SUCCESS;
}		/* -----  end of function Create_Database  ----- */


/*-----------------------------------------------------------------------------
 *  Callbacks
 *-----------------------------------------------------------------------------*/

// TODO Use dichotomy to reduce the number of tests
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
	return SUCCESS;
}		/* -----  end of function Fill_Account_Struct  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Fill_Product_Struct
 *  Description:  Fill a Product Structure with the last returned result of the select
 *                  request
 *   Parameters:  void *data: a pointer on the structure to fill
 *                int nbcol: the number of columns in the result
 *                char **col_vals: the value in the column
 *                char **col_names: the name of the columns
 *       Return:  SUCCESS
 * =====================================================================================
 */
    static int
Fill_Product_Struct (void *data, int nbcol, char **col_vals, char **col_names)
{
    DEB_PRINT("[Fill_Product_Struct.callback] Got %d columns in the entry\n", nbcol);

    int i;
    Product *prod = (Product *) data;

    for (i=0; i<nbcol; i++)
    {
        if (!strcmp(col_names[i], "ProdId"))
            prod->product_id = atoi(col_vals[i]);
        else if (!strcmp(col_names[i], "Label"))
            strncpy(prod->label, col_vals[i], 257);
        else if (!strcmp(col_names[i], "FatherProdId"))
        {
            if (col_vals[i])
                prod->father_id = atoi(col_vals[i]);
            else
                prod->father_id = 0;
        }
    }
	return SUCCESS;
}		/* -----  end of function Fill_Product_Struct  ----- */


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
        return UNDEFINED_ERROR;
    }

    if (exists == UNDEFINED_ERROR)
        return(Create_Database());

	return SUCCESS;
}		/* -----  end of function Initialize_Database  ----- */



/*-----------------------------------------------------------------------------
 *  Insert management
 *-----------------------------------------------------------------------------*/

/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Add_Account
 *  Description:  Add a new account to the database
 *   Parameters:  const Account *account: the structure to add to the database
 *       Return:  0 if an error occurs, 1 otherwise
 * =====================================================================================
 */
// TODO Use the dedicated structure
    int
Add_Account (const Account *account)
{
    char request[MAX_REQ_SIZE], buf[MAX_REQ_SIZE];
    char *err_msg;

    // No need to test if the string has been truncated until the user values are used
    SNPRINTF(request, MAX_REQ_SIZE, "insert into Accounts(Label, InitBal");
    if (strcmp(account->iban, ""))
    {
        SNPRINTF(buf, MAX_REQ_SIZE, "%s, Iban", request);
        strncpy(request, buf, MAX_REQ_SIZE);
    }


    SNPRINTF(buf, MAX_REQ_SIZE, "%s) values ('%s', %.2lf", request, account->label, account->initial_balance);
    strncpy(request, buf, MAX_REQ_SIZE);
    if (strcmp(account->iban, ""))
    {
        SNPRINTF(buf, MAX_REQ_SIZE, "%s, '%s'", request, account->iban);
        strncpy(request, buf, MAX_REQ_SIZE);
    }

    SNPRINTF(buf, MAX_REQ_SIZE, "%s);", request);
    strncpy(request, buf, MAX_REQ_SIZE);

    PRINTSTR(request);

    mafin_db_return_code = sqlite3_exec(g_db, request, NULL, NULL, &err_msg);
    DEB_PRINT("Return code : %d\n", mafin_db_return_code);
    switch(mafin_db_return_code)
    {
        case SQLITE_OK:
            return 1;
        case SQLITE_CONSTRAINT:
            // Print Error message
            fprintf(stderr, "[Add_Account.exec] %s\n", err_msg);
            sqlite3_free(err_msg);

            // Get the extended code
            mafin_db_extended_return_code = sqlite3_extended_errcode(g_db);
            if (mafin_db_extended_return_code == SQLITE_CONSTRAINT_UNIQUE || mafin_db_extended_return_code == SQLITE_CONSTRAINT_PRIMARYKEY)
                return ALREADY_IN_TABLE;
            if (mafin_db_extended_return_code == SQLITE_CONSTRAINT_NOTNULL)
                return NULL_FIELD;
            return 0;
        default:
            fprintf(stderr, "[Add_Account.exec] %s\n", err_msg);
            sqlite3_free(err_msg);
            return 0;
    }
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
    IS_INITIALIZED;

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
        return UNDEFINED_ERROR;
    }

    if (tmp.account_id)
    {
        memcpy(result, &tmp, sizeof(Account));
        return SUCCESS;
    }

	return NO_ENTRY_FOUND;
}		/* -----  end of function Get_Account_From_Id  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Get_Product_From_Id
 *  Description:  Fill a product structure from an id
 *   Parameters:  int product_id: the id of the product to fetch
 *                Product *result: a pointer on the product structure to fill
 *       Return:  SUCCESS if succeed, the appropriate error code otherwise
 * =====================================================================================
 */
    int
Get_Product_From_Id (int product_id, Product *result)
{
    IS_INITIALIZED;

    Product tmp;
    memset(&tmp, 0, sizeof(Product));

    char request[MAX_REQ_SIZE];
    char *err_msg;
    SNPRINTF(request, MAX_REQ_SIZE, "select * from Products where ProdId = %d;", product_id);

    mafin_db_return_code = sqlite3_exec(g_db, request, Fill_Product_Struct, (void *) &tmp, &err_msg);
    if (mafin_db_return_code != SQLITE_OK)
    {
        DEB_PRINT("[Get_Product_From_Id.exec] %s\n", err_msg);
        sqlite3_free(err_msg);
        return UNDEFINED_ERROR;
    }

    if (tmp.product_id)
    {
        memcpy(result, &tmp, sizeof(Product));
        return SUCCESS;
    }

	return NO_ENTRY_FOUND;
}		/* -----  end of function Get_Product_From_Id  ----- */



/*-----------------------------------------------------------------------------
 *  Test Functions
 *-----------------------------------------------------------------------------*/
// No documentation for these functions
    int
Populate_DB()
{
    IS_INITIALIZED;

    char *err_msg;

    // Accounts
    Account account;
    RESET(account, Account);

    FILL_ACCOUNT(account, "Test", 12.8);
    Add_Account(&account);

    FILL_IBAN_ACCOUNT(account, "Pouet", 1400, "1349BM24");
    Add_Account(&account);

    FILL_IBAN_ACCOUNT(account, "Pouet", 1400, "1349BM24");
    Add_Account(&account);

    FILL_IBAN_ACCOUNT(account, "Poueta", 1400, "1349BM24");
    Add_Account(&account);

    // Products
    DEB_PRINT("insert into Products(Label) values(\"Supermarché\");\n");
    mafin_db_return_code = sqlite3_exec(g_db, "insert into Products(Label) values(\"Supermarché\");", NULL, 0, &err_msg);
    if (mafin_db_return_code != SQLITE_OK)
    {
        DEB_PRINT("[Populate_DB.Supermarché] %s\n", err_msg);
        sqlite3_free(err_msg);
        return UNDEFINED_ERROR;
    }

    DEB_PRINT("insert into Products(Label, FatherProdId) values(\"Marché\", 1);\n");
    mafin_db_return_code = sqlite3_exec(g_db, "insert into Products(Label, FatherProdId) values(\"Marché\", 1);", NULL, 0, &err_msg);
    if (mafin_db_return_code != SQLITE_OK)
    {
        DEB_PRINT("[Populate_DB.Marché] %s\n", err_msg);
        sqlite3_free(err_msg);
        return UNDEFINED_ERROR;
    }

    return SUCCESS;
}

