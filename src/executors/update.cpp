#include "global.h"

// UPDATE EMPLOYEE COLUMN Ssn ADD 1

bool syntacticParseUPDATE()
{
    logger.log("syntacticParseUPDATE");
    if (tokenizedQuery.size() != 6 || tokenizedQuery[2] != "COLUMN")
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = UPDATE;
    parsedQuery.updateTableName = tokenizedQuery[1];
    parsedQuery.updateColumnName = tokenizedQuery[3];
    string operation = tokenizedQuery[4];
    parsedQuery.updateValue = stoi(tokenizedQuery[5]);
    // cout << operation << endl;

    if (!operation.compare("MULTIPLY"))
        parsedQuery.updateOperation = MULTIPLY;
    else if (!operation.compare("ADD")) {
        parsedQuery.updateOperation = ADD;
    }
    else if (!operation.compare("SUBTRACT"))
        parsedQuery.updateOperation = SUBTRACT;
    else {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    //log all update attributes
    logger.log(parsedQuery.updateTableName);
    logger.log(parsedQuery.updateColumnName);
    logger.log(to_string(parsedQuery.updateValue));

    return true;
}


bool semanticParseUPDATE()
{
    logger.log("semanticParseUPDATE");

    if (!tableCatalogue.isTable(parsedQuery.updateTableName))
    {
        cout << "SEMANTIC ERROR: Table does not exist" << endl;
        return false;
    }


    if (!tableCatalogue.isColumnFromTable(parsedQuery.updateColumnName, parsedQuery.updateTableName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void executeUPDATE()
{
    logger.log("executeUPDATE");

    Table* table = tableCatalogue.getTable(parsedQuery.updateTableName);
    Cursor cursor(table->tableName, 0);

    int columnIndex = table->getColumnIndex(parsedQuery.updateColumnName);

    vector<int> row;
    vector<vector<int>> rows;
    // Iterate through all the rows
    for (int i = 0; i < table->rowCount;i++) {
        row = cursor.getNext();
        int value = row[columnIndex];
        switch (parsedQuery.updateOperation) {
        case MULTIPLY:
            value *= parsedQuery.updateValue;
            break;
        case ADD:
            value += parsedQuery.updateValue;
            break;
        case SUBTRACT:
            value -= parsedQuery.updateValue;
            break;
        }
        row[columnIndex] = value;
        rows.push_back(row);
    }


    int pageCounter = 0;
    vector<vector<int>> rowsInPage(table->maxRowsPerBlock, row);

    table->blockCount = 0;
    // write the new rows to the table
    for (int i = 0; i < table->rowCount;i++) {
        rowsInPage[pageCounter] = rows[i];
        pageCounter++;

        if (pageCounter == table->maxRowsPerBlock) {
            // log rowsInPage
            logger.log("rowsInPage");
            for (int j = 0; j < rowsInPage.size();j++) {
                for (int k = 0; k < rowsInPage[j].size();k++) {
                    logger.log(to_string(rowsInPage[j][k]) + " ");
                }
                logger.log("\n");
            }
            string pageName = "../data/temp/" + table->tableName + "_Page" + to_string(table->blockCount);
            int fd = open(pageName.c_str(), O_RDWR);
            if (fd == -1)
            {
                cout << "ERROR: Unable to open file" << endl;
                return;
            }

            if (flock(fd, LOCK_EX) == -1)
            {
                cout << "ERROR: Unable to lock file" << endl;
                return;
            }
            cout << "Lock Acquired" << endl;
            bufferManager.removeFromPool(pageName);
            bufferManager.writePage(table->tableName, table->blockCount, rowsInPage, pageCounter);
            auto newPage = bufferManager.insertIntoPool(table->tableName, table->blockCount);
            table->blockCount++;
            pageCounter = 0;

            if (flock(fd, LOCK_UN) == -1)
            {
                cout << "ERROR: Unable to unlock file" << endl;
                return;
            }

            close(fd);
        }
    }

    if (pageCounter) {
        string pageName = "../data/temp/" + table->tableName + "_Page" + to_string(table->blockCount);
        int fd = open(pageName.c_str(), O_RDWR);
        if (fd == -1)
        {
            cout << "ERROR: Unable to open file" << endl;
            return;
        }

        if (flock(fd, LOCK_EX) == -1)
        {
            cout << "ERROR: Unable to lock file" << endl;
            return;
        }
        cout << "Lock Acquired" << endl;
        bufferManager.removeFromPool(pageName);
        bufferManager.writePage(table->tableName, table->blockCount, rowsInPage, pageCounter);
        auto newPage = bufferManager.insertIntoPool(table->tableName, table->blockCount);
        table->blockCount++;
        pageCounter = 0;
        sleep(10);
        if (flock(fd, LOCK_UN) == -1)
        {
            cout << "ERROR: Unable to unlock file" << endl;
            return;
        }

        close(fd);
    }


    return;
}