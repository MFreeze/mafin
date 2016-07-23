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

// Base Tables
#define ACC_TABLE "create table Accounts(AccId int primary key, \
    Label varchar(256) not null unique, \
    InitBal float not null, \
    Iban varchar(34));"

#define PROD_TABLE "create table Products(ProdId int primary key, \
    Label varchar(256) not null);" 

#define OP_TABLE "create table Operations(OpId int primary key, \
    Amount float not null, \
    ProdId int not null, \
    foreign key(ProdId) references Products(ProdId));"

// Joint Tables
#define PR_OP_JOINT "create table OpJointProd(PrOpId varchar(16) primary key, \
    OpId int not null, \
    ProdId int not null, \
    foreign key(OpId) references Operations(OpId), \
    foreign key(ProdId) references Products(ProdId));"

// Tests
#define IS_INITIALIZED do {\
    if (!g_db) {\
        fprintf(stderr, "Unitialized database. Run 'Initialize_Database(path/to/db);' first");\
        return 0;\
    }\
} while (0);

#include <sqlite3.h>
#include <stdlib.h>
#include "debug.h"

extern sqlite3 *g_db;


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
#endif
