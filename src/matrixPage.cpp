#include "global.h"
#include "matrixPage.h"
#include "matrixBufferManager.h"
#include "matrixCatalogue.h"
#include "matrix.h"

/**
 * 
 * @brief Construct a new MatrixPage object. Never used as part of the code
 *
 */
MatrixPage::MatrixPage()
{
    this->pageName = "";
    this->matrixName = "";
    this->pageIndex = -1;
    this->pageRowCount = 0;
    this->pageColumnCount = 0;
    this->pageRowIndexInMatrix = 0;
    this->pageColIndexInMatrix = 0;
    this->rows.clear();
}

/**
 * @brief Construct a new MatrixPage:: MatrixPage object given the table name and page
 * index. When tables are loaded they are broken up into blocks of BLOCK_SIZE
 * and each block is stored in a different file named
 * "<tablename>_Page<pageindex>". For example, If the MatrixPage being loaded is of
 * table "R" and the pageIndex is 2 then the file name is "R_Page2". The page
 * loads the rows (or tuples) into a vector of rows (where each row is a vector
 * of integers).
 *
 * @param matrixName 
 * @param pageIndex 
 */
MatrixPage::MatrixPage(string matrixName, int pageIndex)
{
    logger.log("MatrixPage::MatrixPage");
    


    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
    this->pageName = "../data/temp/" + this->matrixName + "_Page" + to_string(pageIndex);
    Matrix matrix = *matrixCatalogue.getMatrix(matrixName);

    // logger.log("MatrixPage::MatrixPage submatrixSize: " + to_string(matrix.subMatrixSize));
    // logger.log("MatrixPage::MatrixPage reqNumofBlocks: " + to_string(matrix.reqBlockCount));

    this->pageRowIndexInMatrix = pageIndex / (uint)(sqrt(matrix.reqBlockCount));
    this->pageColIndexInMatrix =  pageIndex % (uint)(sqrt(matrix.reqBlockCount));

    if (this->pageColIndexInMatrix == (sqrt(matrix.reqBlockCount) - 1)) this->pageColumnCount = matrix.columnCount % matrix.subMatrixSize;
    else this->pageColumnCount = matrix.subMatrixSize;
    if (this->pageRowIndexInMatrix == (sqrt(matrix.reqBlockCount) - 1)) this->pageRowCount = matrix.rowCount % matrix.subMatrixSize;
    else this->pageRowCount = matrix.subMatrixSize;
    
    // uint maxRowCount = matrix.rowCount;
    
    // TODO THE SIZE OF THIS ROW DOES NOT LOG CORRECTLY !!!
    vector<int> row(this->pageColumnCount, 0);
    // logger.log("MatrixPage::MatrixPage when creating page size of the row: " + to_string(row.size()));

    this->rows.assign(this->pageRowCount, row);

    // logger.log("MatrixPage::MatrixPage when creating page column count " + to_string(this->pageColumnCount) + " in page index " + to_string(pageIndex));
    // logger.log("MatrixPage::MatrixPage when creating page row count " + to_string(this->pageRowCount) + " in page index " + to_string(pageIndex));
    // logger.log("MatrixPage::MatrixPage when creating page size of one row: " + to_string(sizeof(rows[0])));


    ifstream fin(pageName, ios::in);
    string line, word;

    this->rowCount = matrix.rowCounts[pageIndex];
    int number;
    for (uint rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        getline(fin, line);
        stringstream s(line);

        for (int columnCounter = 0; columnCounter < this->pageColumnCount; columnCounter++)
        {
            
            if (!getline(s, word, ' '))  logger.log("MatrixPage::MatrixPage getline returned 0 characters");
            int num = stoi(word);

            // fin >> number;
            this->rows[rowCounter][columnCounter] = num;
        }
    }
    // logger.log("MatrixPage::MatrixPage when creating page column count " + to_string(this->pageColumnCount) + " in page index " + to_string(pageIndex));
    // logger.log("MatrixPage::MatrixPage when creating page size of one row: " + to_string(rows[0].size()));
    // logger.log("MatrixPage::MatrixPage when creating page row count " + to_string(this->rowCount) + " in page index " + to_string(pageIndex));


    fin.close();
}


/**
 * @brief Get row from page indexed by rowIndex
 * 
 * @param rowIndex 
 * @return vector<int> 
 */
vector<int> MatrixPage::getRow(int rowIndex)
{
    logger.log("MatrixPage::getRow");
    // logger.log("MatrixPage::getRow size of one row " + to_string(rows[0].size()));

    vector<int> result;
    result.clear();
    // logger.log("MatrixPage::this->rowCount: " + to_string(this->rowCount));
    if (rowIndex >= this->rowCount)
        return result;
    return this->rows[rowIndex];
}

MatrixPage::MatrixPage(string matrixName, int pageIndex, vector<vector<int>> rows, int rowCount)
{
    logger.log("MatrixPage::MatrixPage");
    this->matrixName = matrixName;
    this->pageIndex = pageIndex;
    this->rows = rows;
    this->rowCount = rowCount;
    // IS THIS CORRECT - CHECK
    this->columnCount = rows[0].size();
    // logger.log("MatrixPage::this->columnCount: " + to_string(this->columnCount));
    // logger.log("MatrixPage::this->rowCount: " + to_string(this->rowCount));
    this->pageName = "../data/temp/"+this->matrixName + "_Page" + to_string(pageIndex);
}

/**
 * @brief writes current page contents to file.
 * 
 */
void MatrixPage::writePage()
{
    logger.log("MatrixPage::writePage");
    ofstream fout(this->pageName, ios::trunc);
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (columnCounter != 0)
                fout << " ";
            fout << this->rows[rowCounter][columnCounter];
        }
        fout << endl;
    }

    logger.log("successfully wrote page");
    fout.close();
}
