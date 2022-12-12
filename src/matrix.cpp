#include "global.h"

/**
 * @brief Construct a new Matrix:: Matrix object
 *
 */

Matrix::Matrix()
{
    logger.log("Matrix::Matrix");
}

/**
 * @brief Construct a new Matrix:: Matrix object used in the case where file is available and LOAD MATRIX command has been called.
 * This command should be followed by calling the load function;
 *
 */

Matrix::Matrix(string matrixName)
{
    logger.log("Matrix::Matrix");
    this->sourceFileName = "../data/" + matrixName + ".csv";
    this->matrixName = matrixName;

}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks.
 *
 * @return true if the matrix has been successfully loaded
 * @return false if an error occurred
 */

bool Matrix::load()
{
    logger.log("Matrix::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line)) {
        fin.close();
        if (this->blockify())
            return true;
    }
    fin.close();
    return false;
}

/**
 * @brief The blockify function is used to split the data into blocks and update the table statistics.
 *
 * @return true if the matrix has been successfully loaded
 * @return false if an error occurred
 */

bool Matrix::calculateSize() {
    logger.log("Matrix::calculateSize");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    while (getline(fin, line)) {
        this->rowCount++;
    }
    //given that all matrices are square matrices 
    // TODO - error when matrice is not a square matrice
    this->columnCount = this->rowCount;

    logger.log("Matrix::calculateSize: rowCount: " + to_string(this->rowCount) + " columnCount: " + to_string(this->columnCount));

    // subMatrixSize = n where n is maximum size of a submatrix n*n
    logger.log("HERE");
    this->subMatrixSize = (uint)(sqrt(BLOCK_SIZE * 1000 / sizeof(int)));
    logger.log("Matrix::calculateSize: subMatrixSize: " + to_string(this->subMatrixSize));

    this->reqBlockCount = (uint)pow(ceil((double)this->rowCount / (double)this->subMatrixSize), 2);
    logger.log("Matrix::calculateSize: reqBlockCount: " + to_string(this->reqBlockCount));

    if (rowCount == columnCount) {
        return true;
    }
    return false;
}

bool Matrix::blockify() {

    logger.log("Matrix::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;

    int pageIdxTrack = 0;
    int pageIdx = -1;

    if (this->calculateSize()) {

        vector<int> row(this->subMatrixSize, 0);
        vector<vector<int>> submatrix(this->subMatrixSize, row);
        vector<vector<vector<int>>> blocks(this->reqBlockCount, submatrix);

        this->rowCounts.resize(this->reqBlockCount, 0);
        this->rowCounts[0] = 0;

        for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
        {
            getline(fin, line);
            stringstream s(line);

            if (rowCounter % this->subMatrixSize == 0)
            {
                pageIdxTrack = pageIdx;
                // this->rowCounts[pageIdx+1] = 1;
            }
            else {
                pageIdx = pageIdxTrack;
                // this->rowCounts[pageIdx]++;
            }

            int pageRowIdx = rowCounter % this->subMatrixSize;
            // this->rowCounts[pageIdx]++;

            for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
            {
                if (!getline(s, word, ',')) return false;

                int num = stoi(word);
                logger.log("Matrix::blockify: num read from getline " + to_string(num));


                if (columnCounter % this->subMatrixSize == 0) {
                    pageIdx++;
                    this->rowCounts[pageIdx]++;

                }

                int pageColIdx = columnCounter % this->subMatrixSize;
                blocks[pageIdx][pageRowIdx][pageColIdx] = num;

            }
            // rowCounts[pageIdx]++; 
        }

        logger.log("Matrix::blockify: rowCounts.size(): " + to_string(rowCounts.size()));
        logger.log("Matrix::blockify: blocks.size(): " + to_string(blocks.size()));


        for (int i = 0; i < this->reqBlockCount; i++) {
            logger.log("Matrix::blocify: rowCounts[" + to_string(i) + "]: " + to_string(this->rowCounts[i]));
            matrixBufferManager.writePage(this->matrixName, i, blocks[i], this->rowCounts[i]);
        }

        return true;

    }

    return false;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Matrix::print()
{
    logger.log("Matrix::print");
    int count = min((long long)PRINT_COUNT, this->rowCount);

    int size = sqrt(this->reqBlockCount);
    vector<Cursor> cursors;

    for(int blockIdx = 0; blockIdx < this->reqBlockCount; blockIdx++) {
        cursors.push_back(Cursor(blockIdx, this->matrixName));
    }

    int offset = -size;

    for(int rowCounter = 0; rowCounter < count; rowCounter++) {
        if(rowCounter % this->subMatrixSize == 0) {
            offset += size;
        }
        for(int i=0;i<size;i++){
            vector<int> row = cursors[i+offset].getNextMatrix();
            this->writeRow(row, cout, row.size());
        }
        cout<<endl;
    }

    printRowCount(count);
}

/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor
 * @return vector<int>
 */
void Matrix::getNextPage(Cursor* cursor)
{
    logger.log("Matrix::getNextPage");
    logger.log("Matrix::getNextPage current pageIndex " + to_string(cursor->pageIndex));

    // logger.log("Matrix::getNextPage: cursor->pageIdx: " + to_string(cursor->pageIndex));
    // logger.log("Matrix::getNextPage: cursor->pageIdx: " + this->matrixName);
    // logger.log("Matrix::getNextPage: this-reqBlockCount" + to_string(this->reqBlockCount));
    if (cursor->pageIndex < this->reqBlockCount - 1)
    {
        cursor->nextMatrixPage(cursor->pageIndex + 1);
    }
}

/**
 * @brief Function that returns a cursor that reads submatrices from this matrix
 *
 * @return Cursor
 */

Cursor Matrix::getCursor()
{
    logger.log("Matrix::getCursor");
    Cursor cursor(0, this->matrixName);
    return cursor;
}

/**
 * @brief Function that performs the transpose operation.
 *
 * Explanation:
 * 1. Copy the submatrix of the 1st matrix to ogMatrix
 * 2. Transpose the submatrix of the 1st matrix into transposedMatrix
 * 3. Copy the submatrix of the 2nd matrix to ogMatrix
 * 4. Copy the transposed submatrix into the 2nd matrix
 * 5. Transpose submatrix of the 2nd matrix into transposedMatrix
 * 6. Copy the transposed submatrix into the 1st matrix
 *
 * @param matrix2
 */

void Matrix::crossTranspose(Matrix& matrix2)
{
    logger.log("Matrix::crossTranspose");
    uint size = (uint)(sqrt(this->reqBlockCount));

    for (int i = 0;i < size;i++) {
        for (int j = 0;j < size;j++) {

            logger.log("Matrix::crossTranspose:" + to_string(size * i + j));
            Cursor cursor1(size * i + j, this->matrixName);
            Cursor cursor2(size * j + i, matrix2.matrixName);

            logger.log("Matrix::crossTranspose: step1");
            vector<int> row = cursor1.getNextMatrix();
            for (int idx = row.size(); idx < this->subMatrixSize; idx++) {
                row.push_back(0);
            }
            vector<vector<int>> ogMatrix;
            ogMatrix.push_back(row);

            //1
            for (int k = 1;k < row.size();k++) {
                row = cursor1.getNextMatrix();
                for (int idx = row.size(); idx < this->subMatrixSize; idx++) {
                    row.push_back(0);
                }
                ogMatrix.push_back(row);
            }

            for(int idx = ogMatrix.size(); idx < this->subMatrixSize; idx++) {
                ogMatrix.push_back(vector<int>(this->subMatrixSize, 0));
            }

            logger.log("Matrix::crossTranspose: step2");
            vector<vector<int>> transposedMatrix(row.size(), vector<int>(row.size(), 0));

            //2
            for (int k = 0;k < row.size();k++) {
                for (int l = 0;l < row.size();l++) {
                    transposedMatrix[k][l] = ogMatrix[l][k];
                }
            }

            //3
            logger.log("Matrix::crossTranspose: stepsize");
            row = cursor2.getNextMatrix();
            for (int idx = row.size(); idx < this->subMatrixSize; idx++) {
                row.push_back(0);
            }
            ogMatrix.clear();
            ogMatrix.push_back(row);
            for (int k = 1;k < row.size();k++) {
                row = cursor2.getNextMatrix();
                for (int idx = row.size(); idx < this->subMatrixSize; idx++) {
                    row.push_back(0);
                }
                ogMatrix.push_back(row);
            }

            for(int idx = ogMatrix.size(); idx < this->subMatrixSize; idx++) {
                ogMatrix.push_back(vector<int>(this->subMatrixSize, 0));
            }

            //4
            logger.log("Matrix::crossTranspose: step4");
            matrixBufferManager.writePage(matrix2.matrixName, size * j + i, transposedMatrix, row.size());

            //5
            logger.log("Matrix::crossTranspose: step5");
            for (int k = 0;k < row.size();k++) {
                for (int l = 0;l < row.size();l++) {
                    transposedMatrix[k][l] = ogMatrix[l][k];
                }
            }

            //6
            logger.log("Matrix::crossTranspose: step6");
            matrixBufferManager.writePage(this->matrixName, size * i + j, transposedMatrix, row.size());
        }
    }
    return;
}

void Matrix::makePermanent()
{
    logger.log("Matrix::makePermanent");
    if(!this->isPermanent())
        matrixBufferManager.deleteFile(this->matrixName);

    string newSourceFile = "../data/" + this->matrixName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    int size = sqrt(this->reqBlockCount);
    vector<Cursor> cursors;

    for(int blockIdx = 0; blockIdx < this->reqBlockCount; blockIdx++) {
        cursors.push_back(Cursor(blockIdx, this->matrixName));
    }

    int offset = -size;

    for(int rowCounter = 0; rowCounter < this->rowCount; rowCounter++) {
        if(rowCounter % this->subMatrixSize == 0) {
            offset += size;
        }
        for(int i=0;i<size;i++){
            vector<int> row = cursors[i+offset].getNextMatrix();
            this->writeRow(row, fout, row.size());
        }
        fout<<"\n";
    }

}


/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Matrix::isPermanent()
{
    logger.log("Matrix::isPermanent");
    if (this->sourceFileName == "../data/" + this->matrixName + ".csv")
    return true;
    return false;
}