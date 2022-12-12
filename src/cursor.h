#ifndef __CURSOR_H__
#define __CURSOR_H__

#include"bufferManager.h"
#include"matrixBufferManager.h"
/**
 * @brief The cursor is an important component of the system. To read from a
 * table, you need to initialize a cursor. The cursor reads rows from a page one
 * at a time.
 *
 */
class Cursor{
    public:
    Page page;
    MatrixPage matrixPage;
    int pageIndex;
    string tableName;
    int pagePointer;

    public:
    Cursor(int pageIndex, string matrixName);
    Cursor(string tableName, int pageIndex);
    vector<int> getNext();
    vector<int> getNextMatrix();
    void nextPage(int pageIndex);
    void nextMatrixPage(int pageIndex);
};

#endif
