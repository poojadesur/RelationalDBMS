#include "global.h"

void MatrixCatalogue::insertMatrix(Matrix* matrix)
{
    logger.log("MatrixCatalogue::~insertMatrix"); 
    this->matrices[matrix->matrixName] = matrix;
}
void MatrixCatalogue::deleteMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::deleteMatrix"); 
    // this->matrices[matrixName]->unload();
    // delete this->tab[tableName];
    // this->tables.erase(tableName);
}
Matrix* MatrixCatalogue::getMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::getTable"); 
    Matrix *matrix = this->matrices[matrixName];
    return matrix;
}

bool MatrixCatalogue::isMatrix(string matrixName)
{
    logger.log("MatrixCatalogue::isTable"); 
    if (this->matrices.count(matrixName))
    {
        logger.log("MatrixCatalogue::isTable: true");
        return true;
    }
    return false;
}

void MatrixCatalogue::print()
{
    logger.log("MatrixCatalogue::print"); 
    // cout << "\nRELATIONS" << endl;

    // int rowCount = 0;
    // for (auto rel : this->tables)
    // {
    //     cout << rel.first << endl;
    //     rowCount++;
    // }
    // printRowCount(rowCount);
}

MatrixCatalogue::~MatrixCatalogue(){
    logger.log("MatrixCatalogue::~TableCatalogue"); 
    for(auto table: this->matrices){
        // table.second->unload();
        delete table.second;
    }
}
