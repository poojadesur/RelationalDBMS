#ifndef __MATRIX_CATALOGUE_H__
#define __MATRIX_CATALOGUE_H__

#include "matrix.h"

/**
 * @brief The TableCatalogue acts like an index of tables existing in the
 * system. Everytime a table is added(removed) to(from) the system, it needs to
 * be added(removed) to(from) the tableCatalogue. 
 *
 */
class MatrixCatalogue
{

    unordered_map<string, Matrix*> matrices;

public:
    MatrixCatalogue() {}
    void insertMatrix(Matrix* matrix);
    void deleteMatrix(string matrixName);
    Matrix* getMatrix(string matrixName);
    bool isMatrix(string matrixName);
    bool isColumnFromTable(string columnName, string matrixName);
    void print();
    ~MatrixCatalogue();
};

#endif