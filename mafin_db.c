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

    /* Creation of an empty Product that will be the father of main products categories.
     * This will allow us to let sqlite manage the uniqueness of the tuple (FatherProdId, Label)
     * Indeed, if FatherProdId is let NULL, then two products with the same label with no father
     * are considered to be different by the database.
     */
    mafin_db_return_code = sqlite3_exec(g_db, "insert into Products(ProdId, Label) values(1, \"Dummy\");", NULL, 0, &errmsg);
    if (mafin_db_return_code != SQLITE_OK)
    {
        DEB_PRINT("[Create_Database.Dummy] %s\n", errmsg);
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
    {
        return(Create_Database());
    }
    
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
    char *errmsg;

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

    mafin_db_return_code = sqlite3_exec(g_db, request, NULL, NULL, &errmsg);
    DEB_PRINT("Return code : %d\n", mafin_db_return_code);
    switch(mafin_db_return_code)
    {
        case SQLITE_OK:
            return SUCCESS;
        case SQLITE_CONSTRAINT:
            // Print Error message
            fprintf(stderr, "[Add_Account.exec] %s\n", errmsg);
            sqlite3_free(errmsg);

            // Get the extended code
            mafin_db_extended_return_code = sqlite3_extended_errcode(g_db);
            if (mafin_db_extended_return_code == SQLITE_CONSTRAINT_UNIQUE || mafin_db_extended_return_code == SQLITE_CONSTRAINT_PRIMARYKEY)
                return ALREADY_IN_TABLE;
            if (mafin_db_extended_return_code == SQLITE_CONSTRAINT_NOTNULL)
                return NULL_FIELD;
            return UNDEFINED_ERROR;
        default:
            fprintf(stderr, "[Add_Account.exec] %s\n", errmsg);
            sqlite3_free(errmsg);
            return UNDEFINED_ERROR;
    }
}		/* -----  end of function Add_Account  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Add_Product_From_Label
 *  Description:  Add a product based on its label definition
 *   Parameters:  const char *label: the complete label of the product. The label 
 *                  have to contain every parent category, separated by the given 
 *                  separator
 *                const char separator: the character that delimits each category in the
 *                  label
 *                int create_missing: the boolean that determines if missing parent
 *                  category in the label have to be created.
 *       Return:  SUCCESS if succeed, the appropriate error code otherwise.
 * =====================================================================================
 */
    int
Add_Product_From_Label (const char *label, const char separator, int create_missing)
{
    IS_INITIALIZED;

    Product tmp;

    int return_code = Get_Product_From_Label(label, separator, &tmp);
    if (return_code == SUCCESS)
    {
        DEB_PRINT("[Add_Product_From_Label] the product \"%s\" already exists in the table.\n", label);
        return ALREADY_IN_TABLE;
    }


    return_code = Fill_Product_From_Label(label, separator, &tmp);
    if (return_code == PARENT_CAT_NON_EXIST)
    {
        if (!create_missing)
        {
            // TODO Implement a way to identify the missing parent category...
            DEB_PRINT("One parent category does not exists\n");
            return PARENT_CAT_NON_EXIST;
        }

        char *lab = strdup(label);
        if (!lab)
        {
            perror("[Add_Product_From_Label.strdup]: ");
            return ALLOC_FAIL;
        }

        char *marker = strrchr(lab, separator);
        *marker = '\0';
        // TODO Check return value...
        Add_Product_From_Label(lab, separator, create_missing);
        Fill_Product_From_Label(label, separator, &tmp);

        free(lab);
    }

	return Add_Product(&tmp);
}		/* -----  end of function Add_Product_From_Label  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Add_Product
 *  Description:  Add one product in the database
 *   Parameters:  const Product *prod: the product to insert into the database
 *       Return:  SUCCESS if succeeds in adding the product, the appropriate error code 
 *                  otherwise.
 * =====================================================================================
 */
    int
Add_Product (const Product *prod)
{
    IS_INITIALIZED;

    char request[MAX_REQ_SIZE];
    char *errmsg;

    SNPRINTF(request, MAX_REQ_SIZE, "insert into Products(FatherProdId, Label) values (%d, \"%s\");", prod->father_id, prod->label);    
    PRINTSTR(request);

    mafin_db_return_code = sqlite3_exec(g_db, request, NULL, NULL, &errmsg);
    DEB_PRINT("Return code : %d\n", mafin_db_return_code);
    switch(mafin_db_return_code)
    {
        case SQLITE_OK:
            return SUCCESS;
        case SQLITE_CONSTRAINT:
            // Print Error message
            fprintf(stderr, "[Add_Account.exec] %s\n", errmsg);
            sqlite3_free(errmsg);

            // Get the extended code
            mafin_db_extended_return_code = sqlite3_extended_errcode(g_db);
            if (mafin_db_extended_return_code == SQLITE_CONSTRAINT_UNIQUE || mafin_db_extended_return_code == SQLITE_CONSTRAINT_PRIMARYKEY)
                return ALREADY_IN_TABLE;
            if (mafin_db_extended_return_code == SQLITE_CONSTRAINT_NOTNULL)
                return NULL_FIELD;
            return UNDEFINED_ERROR;
        default:
            fprintf(stderr, "[Add_Product.exec] %s\n", errmsg);
            sqlite3_free(errmsg);
            return UNDEFINED_ERROR;
    }
}		/* -----  end of function Add_Product  ----- */


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
    char *errmsg;

    Account tmp;
    memset(&tmp, 0, sizeof(tmp));

    SNPRINTF(request, MAX_REQ_SIZE, "select * from Accounts where AccId = %d;", account_id);

    mafin_db_return_code = sqlite3_exec(g_db, request, Fill_Account_Struct, (void *) &tmp, &errmsg);
    if (mafin_db_return_code != SQLITE_OK)
    {
        fprintf(stderr, "[Get_Account_From_Id.exec] %s\n", errmsg);
        sqlite3_free(errmsg);
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
    char *errmsg;
    SNPRINTF(request, MAX_REQ_SIZE, "select * from Products where ProdId = %d;", product_id);

    mafin_db_return_code = sqlite3_exec(g_db, request, Fill_Product_Struct, (void *) &tmp, &errmsg);
    if (mafin_db_return_code != SQLITE_OK)
    {
        DEB_PRINT("[Get_Product_From_Id.exec] %s\n", errmsg);
        sqlite3_free(errmsg);
        return UNDEFINED_ERROR;
    }

    if (tmp.product_id)
    {
        memcpy(result, &tmp, sizeof(Product));
        return SUCCESS;
    }

	return NO_ENTRY_FOUND;
}		/* -----  end of function Get_Product_From_Id  ----- */


/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Get_Product_From_Label
 *  Description:  Get a product based on its label
 *   Parameters:  const char *product_label: the complete label of the product
 *                const char separator: the separator between categories in the label product
 *                Product *result: the structure in which the result is stored
 *       Return:  SUCCESS if succeed, the appropriate error code otherwise
 * =====================================================================================
 */
    int
Get_Product_From_Label (const char *product_label, const char separator, Product *result)
{
    IS_INITIALIZED;

    Product tmp;
    memset(&tmp, 0, sizeof(Product));

    char *lab = strdup(product_label);
    if (!lab)
    {
        perror("[Get_Product_From_Label.strdup] ");
        return ALLOC_FAIL;
    }

    char *marker = strrchr(lab, separator);
    char request[MAX_REQ_SIZE];
    char *errmsg;

    // If no separator then it is a main category
    if (!marker)
    {
        SNPRINTF(request, MAX_REQ_SIZE, "select * from Products where FatherProdId = 1 and Label = \"%s\";", product_label);

        mafin_db_return_code = sqlite3_exec(g_db, request, Fill_Product_Struct, (void *) &tmp, &errmsg);
        if (mafin_db_return_code != SQLITE_OK)
        {
            DEB_PRINT("[Get_Product_From_Label.exec] %s\n", errmsg);
            sqlite3_free(errmsg);
            free(lab);
            return UNDEFINED_ERROR;
        }

        if (tmp.product_id)
        {
            memcpy(result, &tmp, sizeof(Product));
            free(lab);
            return SUCCESS;
        }

        free(lab);
        return NO_ENTRY_FOUND;
    }

    // Otherwise we need to identify the Id of the Father.

    // Get the label of the father...
    *marker = '\0';

    // ... and look for the latter in the database
    int ret = Get_Product_From_Label(lab, separator, &tmp);
    if (ret != SUCCESS)
    {
        free(lab);
        return ret;
    }

    *marker = separator;
    marker++;

    SNPRINTF(request, MAX_REQ_SIZE, "select * from Products where FatherProdId = %d and Label = \"%s\";", tmp.product_id, marker);

    RESET(tmp, Product);

    mafin_db_return_code = sqlite3_exec(g_db, request, Fill_Product_Struct, (void *) &tmp, &errmsg);
    if (mafin_db_return_code != SQLITE_OK)
    {
        DEB_PRINT("[Get_Product_From_Label.exec] %s\n", errmsg);
        sqlite3_free(errmsg);
        free(lab);
        return UNDEFINED_ERROR;
    }

    if (tmp.product_id)
    {
        memcpy(result, &tmp, sizeof(Product));
        free(lab);
        return SUCCESS;
    }

    free(lab);
    return NO_ENTRY_FOUND;
}		/* -----  end of function Get_Product_From_Id  ----- */



/*-----------------------------------------------------------------------------
 *  Other functions
 *-----------------------------------------------------------------------------*/

// XXX Not sure this function is useful...
// XXX Be careful if the name does not contain the separator, no verification is done
// about the existence of the category.
/* 
 * ===  FUNCTION  ======================================================================
 *         Name:  Fill_Product_From_Label
 *  Description:  Fill a product structure from its complete description.
 *   Parameters:  const char *label: the complete definition of the product to add in
 *                  the database. It must be under the following form:
 *                      root[sep]parent1[sep]parent2[sep]...[sep]produit
 *                const char separator: the character used to separate the products from 
 *                  its parents.
 *                Product *prod: the structure to fill
 *       Return:  SUCCESS if succeeds in adding the product, the appropriate error code 
 *                  otherwise.
 * =====================================================================================
 */
    int
Fill_Product_From_Label (const char *label, const char separator, Product *prod)
{
    IS_INITIALIZED;

    char *lab = strdup(label);
    if (!lab)
    {
        perror("[Fill_Product_From_Label.strdup]: ");
        return ALLOC_FAIL;
    }
    
    char *marker = strrchr(lab, separator);
    if (!marker)
    {
        prod->product_id = 0;
        prod->father_id = 1;
        strncpy(prod->label, lab, 256);
        prod->label[256] = '\0';
        free(lab);
        return SUCCESS;
    }

    *marker = '\0';
    marker++;

    Product tmp;
    RESET(tmp, Product);

    int return_code = Get_Product_From_Label(lab, separator, &tmp);
    if (return_code == SUCCESS)
    {
        prod->product_id = 0;
        prod->father_id = tmp.product_id;
        strncpy(prod->label, marker, 256);
        prod->label[256] = '\0';
        free(lab);
        return SUCCESS;
    }
    else if (return_code == NO_ENTRY_FOUND)
    {
        DEB_PRINT("[Fill_Product_From_Label.Get_Product_From_Label]: category %s does not exists.\n", lab);
        free(lab);
        return PARENT_CAT_NON_EXIST;
    }

    DEB_PRINT("[Fill_Product_From_Label.Get_Product_From_Label]: Unknown error.\n");
    free(lab);
    
	return return_code;
}		/* -----  end of function Construct_Product_From_Label  ----- */

/*-----------------------------------------------------------------------------
 *  Test Functions
 *-----------------------------------------------------------------------------*/
// No documentation for these functions
    int
Populate_DB()
{
    IS_INITIALIZED;

    char *errmsg;

    // Accounts
    Account account;
    RESET(account, Account);

    FILL_ACCOUNT(account, "Test", 12.8);
    Add_Account(&account);

    RESET(account, Account);
    FILL_IBAN_ACCOUNT(account, "Pouet", 1400, "1349BM24");
    Add_Account(&account);

    RESET(account, Account);
    FILL_IBAN_ACCOUNT(account, "Pouet", 1400, "1349BM24");
    Add_Account(&account);

    RESET(account, Account);
    FILL_IBAN_ACCOUNT(account, "Poueta", 1400, "1349BM24");
    Add_Account(&account);

    // Products
    // TODO Check return codes
    Add_Product_From_Label("Supermarché", '.', 1);
    Add_Product_From_Label("Supermarché.Marché", '.', 1);
    Add_Product_From_Label("Marché", '.', 1);
    Add_Product_From_Label("Tomcat.Marché", '.', 1);
    Add_Product_From_Label("Tomcat.Marché", '.', 1);
    Add_Product_From_Label("Pouet.Marché", '.', 0);
    Add_Product_From_Label("Pouet..Marché", '.', 1);
/*     DEB_PRINT("insert into Products(FatherProdId, Label) values(1,\"Supermarché\");\n");
 *     mafin_db_return_code = sqlite3_exec(g_db, "insert into Products(FatherProdId, Label) values(1,\"Supermarché\");", NULL, 0, &errmsg);
 *     if (mafin_db_return_code != SQLITE_OK)
 *     {
 *         DEB_PRINT("[Populate_DB.Supermarché] %s\n", errmsg);
 *         sqlite3_free(errmsg);
 *         return UNDEFINED_ERROR;
 *     }
 * 
 *     DEB_PRINT("insert into Products(Label, FatherProdId) values(\"Marché\", 2);\n");
 *     mafin_db_return_code = sqlite3_exec(g_db, "insert into Products(Label, FatherProdId) values(\"Marché\", 2);", NULL, 0, &errmsg);
 *     if (mafin_db_return_code != SQLITE_OK)
 *     {
 *         DEB_PRINT("[Populate_DB.Supermarché.Marché] %s\n", errmsg);
 *         sqlite3_free(errmsg);
 *         return UNDEFINED_ERROR;
 *     }
 * 
 *     DEB_PRINT("insert into Products(Label, FatherProdId) values(\"Marché\", 1);\n");
 *     mafin_db_return_code = sqlite3_exec(g_db, "insert into Products(Label, FatherProdId) values(\"Marché\", 1);", NULL, 0, &errmsg);
 *     if (mafin_db_return_code != SQLITE_OK)
 *     {
 *         DEB_PRINT("[Populate_DB.Marché] %s\n", errmsg);
 *         sqlite3_free(errmsg);
 *         return UNDEFINED_ERROR;
 *     }
 */

    return SUCCESS;
}

