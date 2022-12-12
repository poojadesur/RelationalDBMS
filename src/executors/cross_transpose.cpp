#include "global.h"

/**
 * @brief 
 * SYNTAX: R <- CROSS relation_name relation_name
 */
bool syntacticParseCROSS_TRANSPOSE()
{
    logger.log("syntacticParseCROSS_TRANSPOSE");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = CROSS_TRANSPOSE;
    parsedQuery.crossFirstRelationName = tokenizedQuery[1];
    parsedQuery.crossSecondRelationName = tokenizedQuery[2];
    return true;
}

bool semanticParseCROSS_TRANSPOSE()
{
    logger.log("semanticParseCROSS_TRANSPOSE");

    if (!matrixCatalogue.isMatrix(parsedQuery.crossFirstRelationName) || !matrixCatalogue.isMatrix(parsedQuery.crossSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Cross relations don't exist" << endl;
        return false;
    }
    return true;
}

void executeCROSS_TRANSPOSE()
{
    logger.log("executeCROSS_TRANSPOSE");

    Matrix matrix1 = *(matrixCatalogue.getMatrix(parsedQuery.crossFirstRelationName));
    Matrix matrix2 = *(matrixCatalogue.getMatrix(parsedQuery.crossSecondRelationName));

    if(matrix1.rowCount != matrix2.rowCount)
    {
        cout << "SEMANTIC ERROR: Cross relations don't have same row count" << endl;
        return;
    }

    if(matrix1.columnCount != matrix2.columnCount)
    {
        cout << "SEMANTIC ERROR: Cross relations don't have same column count" << endl;
        return;
    }

    matrix1.crossTranspose(matrix2);
    return;
}