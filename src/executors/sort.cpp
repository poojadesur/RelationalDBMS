#include"global.h"
/**
 * @brief File contains method to process SORT commands.
 *
 * syntax:
 * R <- SORT relation_name BY column_name IN sorting_order
 * or
 * R <- SORT relation_name BY column_name1 IN sorting_order BUFFER buffer_size
 *
 * sorting_order = ASC | DESC
 */

bool syntacticParseSORT() {
    logger.log("syntacticParseSORT");
    if (tokenizedQuery.size() != 8 || tokenizedQuery[4] != "BY" || tokenizedQuery[6] != "IN") {

        // adding optional buffer parameter
        if (tokenizedQuery.size() != 10 || tokenizedQuery[8] != "BUFFER")
        {
            cout << "SYNTAX ERROR" << endl;
            return false;

        }

    }
    parsedQuery.queryType = SORT;
    parsedQuery.sortResultRelationName = tokenizedQuery[0];
    parsedQuery.sortRelationName = tokenizedQuery[3];
    parsedQuery.sortColumnName = tokenizedQuery[5];
    string sortingStrategy = tokenizedQuery[7];
    if (sortingStrategy == "ASC")
        parsedQuery.sortingStrategy = ASC;
    else if (sortingStrategy == "DESC")
        parsedQuery.sortingStrategy = DESC;
    else {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }
    // buffer parameter
    if (tokenizedQuery.size() == 10)
    {
        // TODO check if this works
        parsedQuery.sortBufferSize = atoi(tokenizedQuery[9].c_str());
    }

    return true;
}

bool semanticParseSORT() {
    logger.log("semanticParseSORT");

    if (tableCatalogue.isTable(parsedQuery.sortResultRelationName)) {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.sortRelationName)) {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.sortColumnName, parsedQuery.sortRelationName)) {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }

    return true;
}


void SortByColumn(std::vector<std::vector<int>>& data, int columnIdx, int sortingStrategy)
{
    std::sort(data.begin(), data.end(),
        [columnIdx, sortingStrategy](std::vector<int> const& v1, std::vector<int> const& v2)
        {
            if (sortingStrategy == ASC) return v1[columnIdx] < v2[columnIdx];
            else return v1[columnIdx] > v2[columnIdx];
        });
}


void writeSubFile(vector<int>row, string fileName)
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

vector <int> stringToVect(string myString)
{
    std::stringstream iss(myString);

    int number;
    std::vector<int> myNumbers;
    while (iss >> number)
        myNumbers.push_back(number);

    return myNumbers;
}

int numRows(string fileName)
{
    // cout << "trying to get number of rows from file " << fileName << endl;

    ifstream in;
    in.open(fileName);
    if (in.fail())
        cout << "File " << fileName << "didn't open" << endl;


    int count = 0;
    string line;
    while (true)
    {
        getline(in, line);
        if (in.fail()) break;
        count++;
    }
    return count;
}

vector <vector <int>> readSubFile(string fileName, int start_line_number, int end_line_number)
{
    // cout << "trying to read from file " << fileName << endl;
    ifstream in;
    // Open the file
    in.open(fileName);
    if (in.fail())
        cout << "File " << fileName << "didn't open" << endl;

    int count = 0;
    string line;
    vector <vector<int>> lines;

    // cout << "reading from file " << fileName << " from line number " << start_line_number << " till " << end_line_number << endl;
    while (true) {

        getline(in, line);
        count += 1;
        if (count > start_line_number)
        {
            lines.push_back(stringToVect(line));
        }

        if (count > end_line_number) break;
        if (in.fail()) break;

    }

    return lines;

}

/*
total number of blocks = 5 = {0,1,2,3,4}
number of rows in one block = 41
number of blocks in a subfile = 2
number of subfiles = 3
sf1 = {0,1}
sf2 = {2,3}
sf3 = {4}
sf1 + sf2 = sf11
sf3 = sf22
sf11 + sf22 = sf111
*/

// e <- SORT EMPLOYEE BY Dno IN ASC BUFFER 4
// e <- SORT emp BY Dno IN ASC BUFFER 2
// implemented using two phase merge sort

void executeSORT() {

    logger.log("executeSORT");
    Table table = *(tableCatalogue.getTable(parsedQuery.sortRelationName));
    Cursor cursor = table.getCursor();

    int i = 0;
    // size of file in blocks
    int j = table.blockCount;
    // size of buffer in blocks
    int k = parsedQuery.sortBufferSize;

    // cout << "number of rows in each block: " << table.maxRowsPerBlock << " size of file in blocks " << j << " size of buffer in blocks " << k << endl;
    // m = number of subfiles - each subfile fits in a buffer
    int m = ceil(float(j) / float(k));
    // cout << "number of subfiles " << m << endl;

    int sortColumnIndex = table.getColumnIndex(parsedQuery.sortColumnName);

    // SORTING PHASE
    // in memory buffer of size k*blockSize
    vector< vector <int>> buffer(k * table.maxRowsPerBlock, vector<int>(table.columnCount));

    while (i <= m)
    {
        buffer.clear();
        // TODO: edge case at the end where j is not a multiple of k
        for (int b = 0;b < k;b++)
        {
            // read block from disk
            for (int r = 0;r < table.maxRowsPerBlock;r++)
            {
                vector<int> row = cursor.getNext();
                // reading a page that has lesser number of rows than maxRowsPerBlock
                if (row.empty()) break;
                buffer.push_back(row);
            }
        }


        // sort run based on particular column
        SortByColumn(buffer, sortColumnIndex, parsedQuery.sortingStrategy);

        // // log buffer
        // for(int r=0;r<buffer.size();r++)
        // {
        //     for(int c=0;c<buffer[r].size();c++)
        //     {
        //         cout<<buffer[r][c]<<" ";
        //     }
        //     cout<<endl;
        // }

        // write sorted run to disk
        string fileName = "../data/temp/phase0_subfile_" + table.tableName + "_" + to_string(i) + ".txt";
        for (int r = 0;r < buffer.size();r++)
        {
            writeSubFile(buffer[r], fileName);
        }

        i++;

    }

    // cout << "Sorting Phase 0 completed..." << endl;
    // cout << "**************" << endl;
    // cout << "Beginning merge..." << endl;

    // MERGING PHASE
    int p = 0;
    // number of passes for the merging phase
    // note: using entire buffer since we never fully use it during merge
    int passes = ceil(log(m) / log(k));
    // number of subfiles
    int num_subfiles = m;

    // print variables
    // cout << "number of passes required: " << passes << " number of subfiles: " << num_subfiles << endl;
    // p = 3

    // while(num_subfiles > 1)
    while (p < passes)
    {
        p += 1;

        // number of subfiles being merged in this pass
        int dm = min(num_subfiles, k);
        // dm = 2

        // number of iterations it will take to merge subfiles dm at a time in this phase
        int iterations = ceil(float(num_subfiles) / float(dm));
        // it = 2

        // idx of first subfile of the set of subfiles we currently merge in this iteration
        // iteration and subfileCounter basically the same thing?
        int subfileCounter = 0;

        // print variables
        // cout << "pass number: " << p << " number of subfiles being merged in this pass " << dm << " number of iterations: " << iterations << " subfileCounter: " << subfileCounter << endl;

        // one iteration writes to one new subfile
        for (int it = 0;it < iterations;it++)
        {
            // cout << "iteration " << it << endl;
            // in memory buffer of size k*blockSize
            vector <vector <int>> sortedBuffer;

            // keep track of which row we are looking at in each subfile
            vector <int> rowTrack(dm, 0);

            int sortedRowCounter = 0;

            int tot_rowCount = 0;

            // one while loop looks at a set of rows, each loop at least one row has never been compared before
            while (true)
            {
                int min = INT_MAX;
                int max = INT_MIN;

                int minSubfileIdx = -1;
                int maxSubfileIdx = -1;

                // keeps track of all the rows being compared in this iteration
                vector <vector <int>> rows(dm, vector<int>(table.columnCount));

                // total number of rows in all the subfiles being merged in this iteration
                tot_rowCount = 0;

                // sf = 0*2, 1*2 ->  0 1
                // sf = 1*2, 2*2 ->  2 3

                for (int sf = it * dm;sf < (it + 1) * dm;sf++)
                {
                    // cout << "subfile index: " << sf << endl;
                    // read subfile sf
                    string fileName = "../data/temp/phase" + to_string(p - 1) + "_subfile_" + table.tableName + "_" + to_string(sf) + ".txt";

                    // check if file exists
                    ifstream file(fileName);
                    if (!file)
                    {
                        continue;
                    }

                    int num_rows = numRows(fileName);
                    tot_rowCount += num_rows;

                    // if subfile has been read completely, skip it
                    // TODO - delete subfile if it has been read completely
                    if (rowTrack[sf % dm] == num_rows) continue;

                    vector <vector <int>> subfile = readSubFile(fileName, rowTrack[sf % dm], rowTrack[sf % dm] + 1);

                    vector<int> row = subfile[0];

                    rows[sf % dm] = (row);

                    if (parsedQuery.sortingStrategy == ASC)
                    {
                        if (row[sortColumnIndex] < min)
                        {
                            min = row[sortColumnIndex];
                            minSubfileIdx = sf;
                        }
                    }
                    else
                    {
                        if (row[sortColumnIndex] > max)
                        {
                            max = row[sortColumnIndex];
                            maxSubfileIdx = sf;
                        }
                    }

                    // if(p == 1)
                    // {
                    //     cout << "sf: " << sf << endl;
                    //     cout << "rowtrack " << rowTrack[sf%dm] << endl;
                    //     cout << "value being compared: " << row[sortColumnIndex] << endl;
                    //     cout << "current min: " << min << endl;
                    //     cout << "current minsubfileidx: " << minSubfileIdx << endl;

                    // }



                }

                // for(int x=0;x<dm;x++)
                // {
                //     if (p==1) cout << "in file " << x << " reading row " << rowTrack[x] << endl;
                // }
                // if(p==1) cout << minSubfileIdx << endl;


                if (sortedBuffer.size() >= (k)*table.maxRowsPerBlock)
                {
                    // cout << "number of rows in sorted buffer: " << sortedBuffer.size() << endl;
                    // // write sortedBuffer to disk
                    // cout << "writing to disk" << endl;
                    // cout << "size of rows: " << rows.size() << endl;
                    string fileName = "../data/temp/phase" + to_string(p) + "_subfile_" + table.tableName + "_" + to_string(subfileCounter) + ".txt";
                    // cout << "size of buffer " << sortedBuffer.size() << endl;
                    for (int r = 0;r < sortedBuffer.size();r++)
                    {
                        writeSubFile(sortedBuffer[r], fileName);
                    }
                    sortedBuffer.clear();

                }

                if (parsedQuery.sortingStrategy == ASC)
                {


                    sortedBuffer.push_back(rows[minSubfileIdx % dm]);
                    rowTrack[minSubfileIdx % dm] += 1;
                    sortedRowCounter += 1;
                }
                if (parsedQuery.sortingStrategy == DESC)
                {
                    sortedBuffer.push_back(rows[maxSubfileIdx % dm]);
                    rowTrack[maxSubfileIdx % dm] += 1;
                    sortedRowCounter += 1;
                }

                // cout << "number of rows sorted " << sortedRowCounter << endl;

                // is this the right place to put it?
                // all the rows in subfiles have been merged
                if (sortedRowCounter == tot_rowCount)
                {
                    // write whatever is left in sortedBuffer to disk
                    string fileName = "../data/temp/phase" + to_string(p) + "_subfile_" + table.tableName + "_" + to_string(subfileCounter) + ".txt";
                    for (int r = 0;r < sortedBuffer.size();r++)
                    {
                        writeSubFile(sortedBuffer[r], fileName);
                    }
                    sortedBuffer.clear();
                    break;
                }

            }

            subfileCounter++;

        }

        num_subfiles = iterations;




    }

    // converting final merged subfile to table
    string fileName = "../data/temp/phase" + to_string(p) + "_subfile_" + table.tableName + "_" + to_string(0) + ".txt";


    Table* sortedTable = new Table(parsedQuery.sortResultRelationName, table.columns);

    int rowCount = 0;
    int it = 0;
    cout << "number of rows in final subfile: " << numRows(fileName) << endl;
    //reading all rows in final subfile
    vector<vector<int>> test = readSubFile(fileName, 0, numRows(fileName));
    cout << "size of test " << test.size() << endl;
    while (rowCount < numRows(fileName))
    {
        vector <vector <int>> rows = readSubFile(fileName, it * table.maxRowsPerBlock, min(int((it + 1) * table.maxRowsPerBlock), numRows(fileName)) - 1);
        cout << "size of rows: " << rows.size() << endl;
        bufferManager.writePage(parsedQuery.sortResultRelationName, it, rows, rows.size());
        sortedTable->rowsPerBlockCount.emplace_back(rows.size());
        sortedTable->blockCount++;
        sortedTable->rowCount += rows.size();
        rowCount += rows.size();
        it += 1;
    }

    tableCatalogue.insertTable(sortedTable);

    return;
}