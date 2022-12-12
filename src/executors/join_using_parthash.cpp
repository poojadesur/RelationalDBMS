#include "global.h"
/**
 * @brief 
 * SYNTAX: RS <- JOIN USING PARTHASH R, S ON <column1> 
<bin_op> <column2> BUFFER <buffer_size> 
 */

#include <fstream>
#include <iterator>
#include <string>
#include <vector>
#include <iostream>


bool syntacticParseJOIN_USING_PARTHASH()
{
    logger.log("syntacticParseJOIN_USING_PARTHASH");
    if (tokenizedQuery.size() != 13 || tokenizedQuery[7] != "ON")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = JOIN_USING_PARTHASH;
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

bool semanticParseJOIN_USING_PARTHASH()
{
    logger.log("semanticParseJOIN_USING_PARTHASH");

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


// void writeFile(vector<int> row, string fileName)
// {
//     std::ofstream output_file(fileName);
//     std::ostream_iterator<std::string> output_iterator(output_file, "\n");
//     std::copy(row.begin(), row.end(), output_iterator);
// }

void writeFile(vector<int>row, string fileName)
{
    ofstream file;
    file.open(fileName, std::ios::app);
    for (int i = 0; i < row.size(); i++)
    {
        file << row[i] << " ";
    }
    file << endl;
    file.close();

}

vector <int> stringToVector(string myString)
{
    std::stringstream iss( myString );

    int number;
    std::vector<int> myNumbers;
    while ( iss >> number )
        myNumbers.push_back( number );
    
    return myNumbers;
}

vector <vector <int>> readFile(string fileName, int start_line_number, int end_line_number)
{
    ifstream in;
    // Open the file
    in.open(fileName);
    if(in.fail())
        cout << "File didn't open"<<endl;

    int count = 0;
    string line;
    vector <vector<int>> lines;

    cout << "reading from file " << fileName << " from line number " << start_line_number << " till " << end_line_number << endl;
    while (true) {
        
        getline(in, line);  
        count += 1;
        if (count > start_line_number) 
        {
            lines.push_back(stringToVector(line));
        }

        if (count > end_line_number) break;
        if (in.fail()) break;
        
    }

    return lines;

}


// AD <- JOIN USING PARTHASH A, DEPARTMENT ON a == Dnumber BUFFER 5
// create partitions using mod function
void executeJOIN_USING_PARTHASH()
{

    logger.log("executeJOIN_USING_PARTHASH");

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
    Cursor cursor2 = table2.getCursor();

    vector<int> resultantRow;
    vector< vector <int>> resultantRows;

    int pageIdxResultantTable = 0;

    // splitting into partitions
    // partitions of length M
    // QUESTION: How are partitions stored on disk subfile?? does that mean a page??

    vector<int> row1 = cursor1.getNext();
    // vector<int> partition1Count = (stoi(parsedQuery.joinBufferSize) - 1, 0);
    // vector < vector <int> > partitions1 [stoi(parsedQuery.joinBufferSize) - 1];

    int tableColIdx1 = distance(table1.columns.begin(),find(table1.columns.begin(), table1.columns.end(), parsedQuery.joinFirstColumnName));
    
    // creating partitions by writing them to temp files
    // block size = 1 KB
    // number of buffers for partition = nb-1
    // size of one row = 4*number of columns*size of partition vector

    vector<int> partitionCount1(stoi(parsedQuery.joinBufferSize) - 1, 0);

    while(!row1.empty())
    {
        int partitionIdx = row1[tableColIdx1] % (stoi(parsedQuery.joinBufferSize) - 1);
        // cout << partitionIdx << endl;

        writeFile(row1, "../data/temp/" +parsedQuery.joinFirstRelationName+ to_string(partitionIdx) + ".txt");
        partitionCount1[partitionIdx] += 1;

        row1 = cursor1.getNext();

    }   

    vector<int> row2 = cursor2.getNext();

    // vector < vector <int> > partitions2 [stoi(parsedQuery.joinBufferSize) - 1];

    int tableColIdx2 = distance(table2.columns.begin(),find(table2.columns.begin(), table2.columns.end(), parsedQuery.joinSecondColumnName));

    vector<int> partitionCount2(stoi(parsedQuery.joinBufferSize) - 1, 0);
    
    while(!row2.empty())
    {
        int partitionIdx = row2[tableColIdx2] % (stoi(parsedQuery.joinBufferSize) - 1);
        // cout << partitionIdx << endl;
        writeFile(row2, "../data/temp/" +parsedQuery.joinSecondRelationName+ to_string(partitionIdx) + ".txt");
        partitionCount2[partitionIdx] += 1;

        row2 = cursor2.getNext();

    }   

    cout << "complete partitioning " << endl;

    // getting partitions in chunks from temp files and joining when required
    // first smaller partition chunk is loaded and checked with all chunks of partition 2,

    // iterate through all partitions
    //    iterate through all chunks of partition 1
    //        iterate through all chunks of partition 2
    //           check join condition and write to new table (temp txt file)

    // total size = 4*rowCount*colCount
    // size of buffer = BLOCK SIZE * 1000
    // number of buffers needed = total size / size of buffer
    // number of rows each buffer can hold = size of buffer / 4*colCount
    cout << table1.rowCount << " " << table1.columnCount << endl;
    cout << "number of rows that can be read at a time in table1 " << int((BLOCK_SIZE*1000)/(4*table1.columnCount)) << endl;
    cout << "number of rows that can be read at a time in table2 " << int((BLOCK_SIZE*1000)/(4*table2.columnCount)) << endl;
    

    for (int partitionIdx=0;partitionIdx < stoi(parsedQuery.joinBufferSize) - 1; partitionIdx++)
    {
        cout << "join for partitionIdx " << partitionIdx << endl;
        // loading partition 1
        int end_line_number1 = partitionCount1[partitionIdx];
        int end_line_number2 = partitionCount2[partitionIdx];
        end_line_number1 = min(end_line_number1, int((BLOCK_SIZE*1000)/(4*table1.columnCount)));
        end_line_number2 = min(end_line_number2, int((BLOCK_SIZE*1000)/(4*table2.columnCount)));
        int start_line_number1 = 0;
        int start_line_number2 = 0; 

        cout << "start_line_number1 " << start_line_number1 << endl;
        cout << "end_line_number1 " << end_line_number1 << endl;
        cout << "start_line_number2 " << start_line_number2 << endl;
        cout << "end_line_number2 " << end_line_number2 << endl;

        int p1Chunks;
        int p2Chunks;

        if (end_line_number1 == 0) p1Chunks = 0;
        else p1Chunks = ceil(partitionCount1[partitionIdx] / end_line_number1);

        if (end_line_number2 == 0) p2Chunks = 0;
        else p2Chunks = ceil(partitionCount2[partitionIdx] / end_line_number2);

        cout << "p1Chunks " << p1Chunks << endl;
        cout << "p2Chunks " << p2Chunks << endl;

        for(int i=0;i<p1Chunks;i++)
        {
            vector <vector<int>> partition1 = readFile("../data/temp/" +parsedQuery.joinFirstRelationName+ to_string(partitionIdx) + ".txt", start_line_number1, end_line_number1);
            
            // printing partition1
            cout << "partition1" << endl;
            for(int x=0;x<partition1.size();x++)
            {
                for(int y=0;y<partition1[x].size();y++)
                {
                    cout << partition1[x][y] << " ";
                }
                cout << endl;
            }

            for(int j=0;j<p2Chunks;j++)
            {
                vector <vector<int>> partition2 = readFile("../data/temp/" +parsedQuery.joinSecondRelationName+ to_string(partitionIdx) + ".txt", start_line_number2, end_line_number2);
                start_line_number2 = end_line_number2;
                end_line_number2 = min(end_line_number2 + end_line_number2, partitionCount2[partitionIdx]);

                // printing partition2
                cout << "partition 2" << endl;
                for(int x=0;x<partition2.size();x++)
                {
                    for(int y=0;y<partition2[x].size();y++)
                    {
                        cout << partition2[x][y] << " ";
                    }
                    cout << endl;
                }

                // join partition1 and partition2
                for(int x=0;x<partition1.size();x++)
                {
                    for(int y=0;y<partition2.size();y++)
                    {
                        if(partition1[x][tableColIdx1] == partition2[y][tableColIdx2])
                        {
                            // write to new table
                            vector<int> row;
                            row.insert(row.end(), partition1[x].begin(), partition1[x].end());
                            row.insert(row.end(), partition2[y].begin(), partition2[y].end());
                            writeFile(row, "../data/temp/" +parsedQuery.joinResultRelationName+ ".txt");
                            cout << "wrote to new table" << endl;

                        }
                    }
                }
                
            }

            start_line_number1 = end_line_number1;
            end_line_number1 = min(end_line_number1 + end_line_number1, partitionCount1[partitionIdx]);
        }
        
    }
    
    // // bufferManager.writePage(parsedQuery.joinResultRelationName,pageIdxResultantTable , resultantRows, int(resultantRows.size()));
    // pageIdxResultantTable += 1;
    // resultantRows.clear();

    // tableCatalogue.insertTable(resultantTable);
    
    return;
}