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
#define ACC_TABLE "create table Accounts(AccId int primary key, Label varchar(256) not null, InitBal float not null);"
#define PROD_TABLE "create table Products(ProdId int primary key, Label varchar(256) not null);" 
#define OP_TABLE "create table Operations(OpId int primary key, Amount float not null, ProdId int not null, foreign key(ProdId) references Products(ProdId));"

// Joint Tables
#define PR_OP_JOINT "create table OpJointProd(OpId int not null, ProdId int not null, foreign key(OpId) references Operations(OpId), foreign key(ProdId) references Products(ProdId));"

#include <sqlite3.h>
#include <stdlib.h>
#include "debug.h"

    int
init_database(const char *db_path);
#endif
