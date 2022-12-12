#ifndef __MATRIX_PAGE_H_
#define __MATRIX_PAGE_H_

#include"logger.h"
/**
 * @brief The Page object is the main memory representation of a physical page
 * (equivalent to a block). The page class and the page.h header file are at the
 * bottom of the dependency tree when compiling files. 
 *<p>
 * Do NOT modify the Page class. If you find that modifications
 * are necessary, you may do so by posting the change you want to make on Moodle
 * or Teams with justification and gaining approval from the TAs. 
 *</p>
 */

class MatrixPage{

    string pageIndex;
    int columnCount;
    int rowCount;
    vector<vector<int>> rows;

    // how many rows and columns of data a given page has
    int pageRowCount;
    int pageColumnCount;

    // in the whole matrix that is made up of submatrices of pages, what row and col index is this page
    int pageRowIndexInMatrix;
    int pageColIndexInMatrix;

    public:

    string matrixName;
    string pageName = "";
    MatrixPage();
    MatrixPage(string matrixName, int pageIndex);
    MatrixPage(string matrixName, int pageIndex, vector<vector<int>> rows, int rowCount);
    vector<int> getRow(int rowIndex);
    void writePage();
};

#endif