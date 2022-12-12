#include "global.h"

/**
 * @brief 
 * SYNTAX: LOAD MATRIX matrix_name
 */

bool syntacticParseLOAD_MATRIX()
{
    logger.log("syntacticParseLOAD_MATRIX");
    if (tokenizedQuery.size() != 3)
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    parsedQuery.queryType = LOAD_MATRIX;
    logger.log("Ended here 1");
    parsedQuery.loadRelationName = tokenizedQuery[2];
    logger.log("Ended here 2");
    return true;
}

bool semanticParseLOAD_MATRIX()
{
    logger.log("semanticParseLOAD_MATRIX");
    if (matrixCatalogue.isMatrix(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Matrix already exists" << endl;
        return false;
    }

    if (!isFileExists(parsedQuery.loadRelationName))
    {
        cout << "SEMANTIC ERROR: Data file doesn't exist" << endl;
        return false;
    }
    return true;
}

void executeLOAD_MATRIX()
{

    Matrix *matrix = new Matrix(parsedQuery.loadRelationName);
    if (matrix->load())
    {
        matrixCatalogue.insertMatrix(matrix);
        logger.log(to_string(matrixCatalogue.isMatrix(matrix->matrixName)));
        logger.log("Matrix loaded successfully");
    }
    return;
}