#ifndef __MATRIX_H__
#define __MATRIX_H__

#include "cursor.h"

/**
 * @brief The Matrix class holds all information related to a loaded matrix. It
 * also implements methods that interact with the parsers, executors, cursors
 * and the buffer manager. 
 *
 */

class Matrix {
    // vector<vector<vector<int>>> blocks;

    public:
    string sourceFileName = "";
    string matrixName = "";
    uint columnCount = 0;
    long long int rowCount = 0;
    int minRowsInBlock = INT_MAX;
    uint reqBlockCount = 0;
    uint subMatrixSize = 0;
    vector<int> rowCounts;

    bool load();
    bool blockify();
    Matrix();
    Matrix(string matrixName);
    bool calculateSize();
    void getNextPage(Cursor *cursor);
    void print();
    Cursor getCursor();
    void crossTranspose(Matrix &matrix2);
    void makePermanent();
    bool isPermanent();


        /**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row, ostream &fout, int columnsCount)
{   
    logger.log("Matrix::printRow");
    for (int columnCounter = 0; columnCounter < columnsCount; columnCounter++)
    {   
        logger.log("Matrix::printRow: columnCounter: " + to_string(columnCounter));
        if (columnCounter != 0)
            fout << ", ";
        fout << row[columnCounter];
    }
    fout << ",";
}

/**
 * @brief Static function that takes a vector of valued and prints them out in a
 * comma seperated format.
 *
 * @tparam T current usaages include int and string
 * @param row 
 */
template <typename T>
void writeRow(vector<T> row)
{
    logger.log("Table::printRow");
    ofstream fout(this->sourceFileName, ios::app);
    this->writeRow(row, fout);
    fout.close();
}

};

#endif