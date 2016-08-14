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
main (int argc, char **argv) 
{
    // TODO Management of the options
    DEB_PRINT("Database opening\n");
    int return_code = Initialize_Database(DATABASE_FILE);
    DEB_PRINT("Return Code: %d\n", return_code);
    if (return_code != SUCCESS)
    {
        fprintf(stderr, "Error while opening database: %s\n", DATABASE_FILE);
        return EXIT_FAILURE;
    }

    PRINTLINE;
    Populate_DB();
    PRINTLINE;

    DEB_PRINT("Initializing Account.\n");
    Account a;
    RESET(a, Account);
    PRINTACC(a);

    PRINTLINE;
    DEB_PRINT("Get_Account_From_Id(1, &a)\n");
    return_code = Get_Account_From_Id(1, &a);
    DEB_PRINT("Return code: %d\n", return_code);
    PRINTACC(a);
    PRINTLINE;

    DEB_PRINT("Get_Account_From_Id(2, &a)\n");
    return_code = Get_Account_From_Id(2, &a);
    DEB_PRINT("Return code: %d\n", return_code);
    PRINTACC(a);
    PRINTLINE;

    DEB_PRINT("Get_Account_From_Id(4, &a)\n");
    return_code = Get_Account_From_Id(4, &a);
    DEB_PRINT("Return code: %d\n", return_code);
    PRINTACC(a);
    PRINTLINE;

    Product prod;
    RESET(prod, Product);

    DEB_PRINT("Get_Product_From_Id(1, &prod)\n");
    return_code = Get_Product_From_Id(1, &prod);
    DEB_PRINT("Return code: %d\n", return_code);
    PRINTPROD(prod);
    PRINTLINE;
    
    RESET(prod, Product);
    DEB_PRINT("Get_Product_From_Id(2, &prod)\n");
    return_code = Get_Product_From_Id(2, &prod);
    DEB_PRINT("Return code: %d\n", return_code);
    PRINTPROD(prod);
    PRINTLINE;
    
    RESET(prod, Product);
    DEB_PRINT("Get_Product_From_Label(\"Marché\", '.', &prod)\n");
    return_code = Get_Product_From_Label("Marché", '.', &prod);
    DEB_PRINT("Return code: %d\n", return_code);
    PRINTPROD(prod);
    PRINTLINE;
    
    RESET(prod, Product);
    DEB_PRINT("Get_Product_From_Label(\"Supermarché.Marché\", '.', &prod)\n");
    return_code = Get_Product_From_Label("Supermarché.Marché", '.', &prod);
    DEB_PRINT("Return code: %d\n", return_code);
    PRINTPROD(prod);
    PRINTLINE;
    
/********************************************
 *     Useful? 
 ********************************************
 *
 *     RESET(prod, Product);
 *     DEB_PRINT("Fill_Product_From_Label(\"Supermarché.Marché\", '.', &prod)\n");
 *     return_code = Fill_Product_From_Label("Supermarché.Marché", '.', &prod);
 *     DEB_PRINT("Return code: %d\n", return_code);
 *     PRINTPROD(prod);
 *     PRINTLINE;
 * 
 *     RESET(prod, Product);
 *     DEB_PRINT("Fill_Product_From_Label(\"Tomcat\", '.', &prod)\n");
 *     return_code = Fill_Product_From_Label("Tomcat", '.', &prod);
 *     DEB_PRINT("Return code: %d\n", return_code);
 *     PRINTPROD(prod);
 *     PRINTLINE;
 * 
 *     RESET(prod, Product);
 *     DEB_PRINT("Fill_Product_From_Label(\"Tomcat.Marché\", '.', &prod)\n");
 *     return_code = Fill_Product_From_Label("Tomcat.Marché", '.', &prod);
 *     DEB_PRINT("Return code: %d\n", return_code);
 *     PRINTPROD(prod);
 *     PRINTLINE;
 * 
 *     RESET(prod, Product);
 *     DEB_PRINT("Fill_Product_From_Label(\"Marché\", '.', &prod)\n");
 *     return_code = Fill_Product_From_Label("Marché", '.', &prod);
 *     DEB_PRINT("Return code: %d\n", return_code);
 *     PRINTPROD(prod);
 *     PRINTLINE;
 ********************************************
 */


    Close_Database();
    return EXIT_SUCCESS;
}
