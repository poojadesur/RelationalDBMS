#include "global.h"

Cursor::Cursor(string tableName, int pageIndex)
{
    logger.log("Cursor::Cursor");
    this->page = bufferManager.getPage(tableName, pageIndex);
    this->pagePointer = 0;
    this->tableName = tableName;
    this->pageIndex = pageIndex;
}

Cursor::Cursor(int pageIndex, string matrixName)
{
    logger.log("Cursor::Cursor (matrix)");
    this->matrixPage = matrixBufferManager.getPage(matrixName, pageIndex);
    this->pagePointer = 0;
    this->tableName = matrixName;
    this->pageIndex = pageIndex;
}

/**
 * @brief This function reads the next row from the page. The index of the
 * current row read from the page is indicated by the pagePointer(points to row
 * in page the cursor is pointing to).
 *
 * @return vector<int> 
 */
vector<int> Cursor::getNext()
{
    logger.log("Cursor::getNext");
    vector<int> result = this->page.getRow(this->pagePointer);
    this->pagePointer++;
    if(result.empty()){
        tableCatalogue.getTable(this->tableName)->getNextPage(this);
        if(!this->pagePointer){
            result = this->page.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}

vector<int> Cursor::getNextMatrix()
{
    logger.log("Cursor::getNextMatrix");
    // logger.log("Cursor::matrixName: " + this->tableName);
    // logger.log("Cursor::pageIndex: " + to_string(this->pageIndex));
    logger.log("Cursor::pagePointer: " + to_string(this->pagePointer));
    // logger.log("Cursor::matrixPage->matrixName " + this->matrixPage.matrixName);
    vector<int> result = this->matrixPage.getRow(this->pagePointer);
    this->pagePointer++;
    // logger.log("Cursor::getNextMatrix: pagePointer: " + to_string(this->pagePointer));
    // for(int idx=0;idx<result.size();idx++) {
        // logger.log("Cursor::getNextMatrix: result: " + to_string(result[idx]));
    // }
    if(result.empty()){
        logger.log("Cursor::getNextMatrix reading new page");
        matrixCatalogue.getMatrix(this->tableName)->getNextPage(this);
        if(!this->pagePointer){
            // logger.log("Cursor::getNextMatrix: pagePointer: " + to_string(this->pagePointer));
            // logger.log("Cursor::getNextMatrix: pageIndex: " + to_string(this->pageIndex));
            result = this->matrixPage.getRow(this->pagePointer);
            this->pagePointer++;
        }
    }
    return result;
}
/**
 * @brief Function that loads Page indicated by pageIndex. Now the cursor starts
 * reading from the new page.
 *
 * @param pageIndex 
 */
void Cursor::nextPage(int pageIndex)
{
    logger.log("Cursor::nextPage");
    this->page = bufferManager.getPage(this->tableName, pageIndex);
    this->pageIndex = pageIndex;
    this->pagePointer = 0;
}

void Cursor::nextMatrixPage(int pageIndex)
{
    logger.log("Cursor::nextMatrixPage");
    this->matrixPage = matrixBufferManager.getPage(this->tableName, pageIndex);
    this->pageIndex = pageIndex;
    // logger.log("Cursor::nextMatrixPage new pageindex: " + to_string(this->pageIndex));
    this->pagePointer = 0;
}