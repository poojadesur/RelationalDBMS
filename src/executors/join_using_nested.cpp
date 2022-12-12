#include "global.h"
/**
 * @brief 
 * SYNTAX: RS <- JOIN USING NESTED R, S ON <column1> 
<bin_op> <column2> BUFFER <buffer_size> 
 */
bool syntacticParseJOIN_USING_NESTED()
{
    logger.log("syntacticParseJOIN_USING_NESTED");
    if (tokenizedQuery.size() != 13 || tokenizedQuery[7] != "ON")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = JOIN_USING_NESTED;
    parsedQuery.joinResultRelationName = tokenizedQuery[0];
    parsedQuery.joinFirstRelationName = tokenizedQuery[5];
    parsedQuery.joinSecondRelationName = tokenizedQuery[6];
    parsedQuery.joinFirstColumnName = tokenizedQuery[8];
    parsedQuery.joinSecondColumnName = tokenizedQuery[10];
    parsedQuery.joinBufferSize = tokenizedQuery[12];


    string binaryOperator = tokenizedQuery[9];
    if (binaryOperator == "<")
        parsedQuery.joinBinaryOperator = LESS_THAN;
    else if (binaryOperator == ">")
        parsedQuery.joinBinaryOperator = GREATER_THAN;
    else if (binaryOperator == ">=" || binaryOperator == "=>")
        parsedQuery.joinBinaryOperator = GEQ;
    else if (binaryOperator == "<=" || binaryOperator == "=<")
        parsedQuery.joinBinaryOperator = LEQ;
    else if (binaryOperator == "==")
        parsedQuery.joinBinaryOperator = EQUAL;
    else if (binaryOperator == "!=")
        parsedQuery.joinBinaryOperator = NOT_EQUAL;
    else
    {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    return true;
}

bool semanticParseJOIN_USING_NESTED()
{
    logger.log("semanticParseJOIN_USING_NESTED");

    if (tableCatalogue.isTable(parsedQuery.joinResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.joinFirstRelationName) || !tableCatalogue.isTable(parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.joinFirstColumnName, parsedQuery.joinFirstRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.joinSecondColumnName, parsedQuery.joinSecondRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}


void executeJOIN_USING_NESTED()
{

    logger.log("executeJOIN_USING_NESTED");

    Table table1 = *(tableCatalogue.getTable(parsedQuery.joinFirstRelationName));
    Table table2 = *(tableCatalogue.getTable(parsedQuery.joinSecondRelationName));

    vector<string> columns;

    //Creating list of column names
    for (int columnCounter = 0; columnCounter < table1.columnCount; columnCounter++)
    {
        string columnName = table1.columns[columnCounter];
        if (table2.isColumn(columnName))
        {
            columnName = parsedQuery.joinFirstRelationName + "_" + columnName;
        }
        columns.emplace_back(columnName);
    }

    for (int columnCounter = 0; columnCounter < table2.columnCount; columnCounter++)
    {
        string columnName = table2.columns[columnCounter];
        if (table1.isColumn(columnName))
        {
            columnName = parsedQuery.joinSecondRelationName + "_" + columnName;
        }
        columns.emplace_back(columnName);
    }

    // we dont need to create resultTable, if we want to use it we can load it into memory?
    Table *resultantTable = new Table(parsedQuery.joinResultRelationName, columns);

    Cursor cursor1 = table1.getCursor();

    vector<int> row1 = cursor1.getNext();
    vector<int> resultantRow;
    vector< vector <int>> resultantRows;
    
    int pageIdxResultantTable = 0;

    // RS <- JOIN USING NESTED A, DEPARTMENT ON a == Dnumber BUFFER 4

    while (!row1.empty())
    {
        // for(int i=0;i<int(row1.size());i++) cout << row1[i] << " ";
        // cout << endl;

        Cursor cursor2 = table2.getCursor();
        vector<int> row2 = cursor2.getNext();

        vector <vector<int> > table1Rows;

        while((cursor1.pageIndex+1)%(stoi(parsedQuery.joinBufferSize)-2)!=0)
            {
                // for(int i=0;i<int(row1.size());i++) cout << row1[i] << " ";
                // cout << endl;
                // cout << cursor1.pageIndex << endl;
                table1Rows.push_back(row1);
                // cout << "pushed back" << endl;
                row1 = cursor1.getNext();
                // cout << (stoi(parsedQuery.joinBufferSize)) << endl;
                // cout << "got next row" << endl;
                // cout << (cursor1.pageIndex+1)%(stoi(parsedQuery.joinBufferSize)-2) << endl;

                if (row1.empty())
                {
                    // cout << "row1 is empty" << endl;
                    break;
                } 
            }

        cout << "entered out of while loop" << endl;

        while(!row2.empty())
        {
            for(int i=0;i<int(row2.size());i++) cout << row2[i] << " ";
            cout << endl;

            int table1ColIdx = distance(table1.columns.begin(),find(table1.columns.begin(), table1.columns.end(), parsedQuery.joinFirstColumnName));
            int table2ColIdx = distance(table2.columns.begin(),find(table2.columns.begin(), table2.columns.end(), parsedQuery.joinSecondColumnName));
            
            cout << table1ColIdx << " " << table2ColIdx << endl;
            cout << parsedQuery.joinBinaryOperator << endl;

            for(int i=0;i<int(table1Rows.size());i++)
            {
                resultantRow = table1Rows[i];
                cout << table1Rows[i][table1ColIdx] << " " << row2[table2ColIdx] << endl;

                // cout << "befmore checking..." << endl;
                // for(int i=0;i<int(resultantRow.size());i++) cout << resultantRow[i] << " ";
                // cout << endl;

                // int operator = parsedQuery.joinBinaryOperator;

                switch(int(parsedQuery.joinBinaryOperator)){
                    case 0: 
                    {
                        if(table1Rows[i][table1ColIdx] < row2[table2ColIdx]) resultantRow.insert(resultantRow.end(),row2.begin(),row2.end());
                        break;
                    }
                    case 1: 
                    {
                        if(table1Rows[i][table1ColIdx] > row2[table2ColIdx]) resultantRow.insert(resultantRow.end(),row2.begin(),row2.end());
                        break;
                    }
                    case 2: 
                    {
                        if(table1Rows[i][table1ColIdx] <= row2[table2ColIdx]) resultantRow.insert(resultantRow.end(),row2.begin(),row2.end());
                        break;
                    }
                    case 3: 
                    {
                        if(table1Rows[i][table1ColIdx] <= row2[table2ColIdx]) resultantRow.insert(resultantRow.end(),row2.begin(),row2.end());
                        break;
                    }
                    case 4: {
                                if(table1Rows[i][table1ColIdx] == row2[table2ColIdx]) 
                                {
                                    resultantRow.insert(resultantRow.end(),row2.begin(),row2.end());
                                    // cout << "after checking..." << endl;
                                    // for(int i=0;i<int(resultantRow.size());i++) cout << resultantRow[i] << " ";
                                    // cout << endl;
                                }

                                break;
                            }
                    case 5: 
                    {
                        if(table1Rows[i][table1ColIdx] != row2[table2ColIdx]) resultantRow.insert(resultantRow.end(),row2.begin(),row2.end());
                        break;
                    }
                    default: 
                    {
                        cout << "binop doesnt exist" << endl;
                        break;
                    }
                }

                cout << "outside switch case..." << endl;
                for(int i=0;i<int(resultantRow.size());i++) cout << resultantRow[i] << " ";
                cout << endl;

                // in case the block allocated for storing results overflows, write it to the table and empty out this buffer
                if (resultantRow.size() > table1Rows[i].size())
                {
                    for(int i=0;i<int(resultantRow.size());i++) cout << resultantRow[i] << " ";
                    cout << endl;

                    if(int(resultantRows.size()+1)*4*int(columns.size()) > BLOCK_SIZE*1000)
                    {
                        cout << "we are trying to write the page now" << endl;
                        // move to table, empty buffer
                        bufferManager.writePage(parsedQuery.joinResultRelationName,pageIdxResultantTable , resultantRows, int(resultantRows.size()));
                        pageIdxResultantTable += 1;
                        resultantRows.clear();
                    }

                    resultantRows.push_back(resultantRow);

                }
                    
                
            }

            row2 = cursor2.getNext();
            
        }
        
    }

    cout << "size of resultant rows: " << resultantRows.size() << endl;
    cout << "we are trying to write the page now" << endl;
    // move to table, empty buffer
    bufferManager.writePage(parsedQuery.joinResultRelationName,pageIdxResultantTable , resultantRows, int(resultantRows.size()));
    pageIdxResultantTable += 1;
    resultantRows.clear();

    tableCatalogue.insertTable(resultantTable);

    // string newSourceFile = "../data/temp/" + resultantTable->tableName + ".csv";
    // ofstream fout(newSourceFile, ios::out);

    // // TODO: HOW DO I SAVE FROM PAGES TO A TABLE?
    // for(int i=0;i<int(resultantRows.size());i++)
    // {
    //     resultantTable->writeRow(resultantRows[i], fout);

    // }

    return;
    
}