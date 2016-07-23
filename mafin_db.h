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

// TODO Use macros for table names?
// TODO Use macros for table columns?

/*-----------------------------------------------------------------------------
 *  SQL Macros
 *-----------------------------------------------------------------------------*/

// Base Tables
#define ACC_TABLE "create table Accounts(AccId integer primary key, \
    Label varchar(256) not null unique, \
    InitBal float not null, \
    Iban varchar(34));"

#define PROD_TABLE "create table Products(ProdId integer primary key, \
    FatherProdId integer, \
    Label varchar(256) not null,\
    foreign key(FatherProdId) references Products(ProdId));" 

#define OP_TABLE "create table Operations(OpId integer primary key, \
    Amount float not null, \
    ProdId integer not null, \
    foreign key(ProdId) references Products(ProdId));"

// Joint Tables
#define PR_OP_JOINT "create table OpJointProd(\
    OpId integer not null, \
    ProdId integer not null, \
    primary key(OpId, ProdId), \
    foreign key(OpId) references Operations(OpId), \
    foreign key(ProdId) references Products(ProdId));"

// Tests
#define IS_INITIALIZED do {\
    if (!g_db) {\
        fprintf(stderr, "Unitialized database. Run 'Initialize_Database(path/to/db);' first");\
        return 0;\
    }\
} while (0);



/*-----------------------------------------------------------------------------
 *  Error Code
 *-----------------------------------------------------------------------------*/

#define ALREADY_IN_TABLE -1
#define NO_ENTRY_FOUND -2


/*-----------------------------------------------------------------------------
 *  Other Macros
 *-----------------------------------------------------------------------------*/
#define MAX_REQ_SIZE 2048
#define SNPRINTF(str, ...) do {\
    mafin_db_return_code = snprintf(str, ##__VA_ARGS__);\
    if (mafin_db_return_code < 0) {\
        perror("Something wen wrong");\
        return 0;\
    }\
} while (0);



/*-----------------------------------------------------------------------------
 *  Includes
 *-----------------------------------------------------------------------------*/

#include <sqlite3.h>
#include <stdlib.h>
//#include <glib.h>
#include <errno.h>
#include "debug.h"



/*-----------------------------------------------------------------------------
 *  Types definitions
 *-----------------------------------------------------------------------------*/

typedef struct account_t 
{
    int account_id;
    char label[257];
    double initial_balance;
    char iban[35];
} Account;

typedef struct product_t
{
    int product_id;
    int father_id;
    char label[257];
} Product;

/*-----------------------------------------------------------------------------
 *  Global variables
 *-----------------------------------------------------------------------------*/

extern sqlite3 *g_db;
extern int mafin_db_return_code;


/*-----------------------------------------------------------------------------
 *  Database connection management
 *-----------------------------------------------------------------------------*/
    int
Initialize_Database(const char *db_path); // Initialise the database

    static inline void
Close_Database()
{
    if (g_db)
        sqlite3_close_v2(g_db);
}


/*-----------------------------------------------------------------------------
 *  Add entries
 *-----------------------------------------------------------------------------*/
    int
Add_Account(const char *label, double init_balance, const char iban[35]);
#endif


/*-----------------------------------------------------------------------------
 *  Get informations
 *-----------------------------------------------------------------------------*/
    int
Get_Account_From_Id(int account_id, Account *result);
