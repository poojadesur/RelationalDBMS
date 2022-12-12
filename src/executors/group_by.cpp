#include "global.h"

bool syntacticParseGROUP_BY()
{
    logger.log("syntacticParseGROUP_BY");
    if (tokenizedQuery.size() != 9 || tokenizedQuery[3] != "BY")
    {
        cout << "SYNTAC ERROR" << endl;
        return false;
    }

    parsedQuery.queryType = GROUP_BY;
    parsedQuery.groupByResultRelationName = tokenizedQuery[0];
    parsedQuery.groupByAttributeName = tokenizedQuery[4];
    parsedQuery.groupByRelationName = tokenizedQuery[6];

    string operationAndAttribute = tokenizedQuery[8];


    auto leftItr = find(operationAndAttribute.begin(), operationAndAttribute.end(), '(');
    auto rightItr = find(operationAndAttribute.begin(), operationAndAttribute.end(), ')');
    int leftIdx = distance(operationAndAttribute.begin(), leftItr);
    parsedQuery.groupByOperationOnAttributeName = operationAndAttribute.substr(leftIdx+1, distance(leftItr, rightItr)-1);

    string operation = operationAndAttribute.substr(0, 3);

    if (!operation.compare("MAX"))
        parsedQuery.groupByOperation = MAX;
    else if (!operation.compare("MIN"))
        parsedQuery.groupByOperation = MIN;
    else if (!operation.compare("SUM"))
        parsedQuery.groupByOperation = SUM;
    else if (!operation.compare("AVG"))
        parsedQuery.groupByOperation = AVG;
    else {
        cout << "SYNTAX ERROR" << endl;
        return false;
    }

    //log all groupBy attributes
    logger.log(parsedQuery.groupByResultRelationName);
    logger.log(parsedQuery.groupByAttributeName);
    logger.log(parsedQuery.groupByRelationName);
    logger.log(parsedQuery.groupByOperationOnAttributeName);
    

    return true;
}


bool semanticParseGROUP_BY()
{
    logger.log("semanticParseGROUP_BY");

    if (tableCatalogue.isTable(parsedQuery.groupByResultRelationName))
    {
        cout << "SEMANTIC ERROR: Resultant relation already exists" << endl;
        return false;
    }

    if (!tableCatalogue.isTable(parsedQuery.groupByRelationName))
    {
        cout << "SEMANTIC ERROR: Relation doesn't exist" << endl;
        return false;
    }

    if (!tableCatalogue.isColumnFromTable(parsedQuery.groupByAttributeName, parsedQuery.groupByRelationName) || !tableCatalogue.isColumnFromTable(parsedQuery.groupByOperationOnAttributeName, parsedQuery.groupByRelationName))
    {
        cout << "SEMANTIC ERROR: Column doesn't exist in relation" << endl;
        return false;
    }
    return true;
}

void executeGROUP_BY()
{
    logger.log("executeGROUP_BY");

    Table* table = tableCatalogue.getTable(parsedQuery.groupByRelationName);
    Cursor cursor(table->tableName, 0);

    unordered_map<int, int> groups;
    unordered_map<int, int> groupFreq;

    auto groupItr = find(table->columns.begin(), table->columns.end(), parsedQuery.groupByAttributeName);
    int groupIdx = distance(table->columns.begin(), groupItr);

    auto operandItr = find(table->columns.begin(), table->columns.end(), parsedQuery.groupByOperationOnAttributeName);
    int operandIdx = distance(table->columns.begin(), operandItr);

    vector<string> newColumns;

    newColumns.push_back(parsedQuery.groupByAttributeName);

    switch (parsedQuery.groupByOperation) {
    case MAX: newColumns.push_back("MAX(" + parsedQuery.groupByOperationOnAttributeName + ")"); break;
    case MIN: newColumns.push_back("MIN(" + parsedQuery.groupByOperationOnAttributeName + ")"); break;
    case SUM: newColumns.push_back("SUM(" + parsedQuery.groupByOperationOnAttributeName + ")"); break;
    case AVG: newColumns.push_back("AVG(" + parsedQuery.groupByOperationOnAttributeName + ")"); break;
    }

    Table* newTable = new Table(parsedQuery.groupByResultRelationName, newColumns);
    vector<int> row(2);

    for (int i = 0;i < table->rowCount;i++) {
        row = cursor.getNext();
        int groupEle = row[groupIdx];
        int operand = row[operandIdx];

        if (groups.find(groupEle) == groups.end()) {
            groups.insert({ groupEle, operand });
            groupFreq.insert({ groupEle, 1 });
        }
        else {
            groupFreq[groupEle]++;
            switch (parsedQuery.groupByOperation) {
            case MAX: groups[groupEle] = max(groups[groupEle], operand); break;
            case MIN: groups[groupEle] = min(groups[groupEle], operand); break;
            case SUM: groups[groupEle] += operand; break;
            case AVG: groups[groupEle] = ((groups[groupEle] * (groupFreq[groupEle] - 1)) + operand) / groupFreq[groupEle];
            }
        }
    }

    int pageCounter = 0;
    vector<vector<int>> rowsInPage(newTable->maxRowsPerBlock, row);

    for (auto i : groups) {
        row = { i.first, i.second };
        //log row
        logger.log(to_string(row[0]) + " " + to_string(row[1]));
        rowsInPage[pageCounter] = row;
        pageCounter++;
        newTable->rowCount++;

        if (pageCounter == table->maxRowsPerBlock) {
            bufferManager.writePage(newTable->tableName, newTable->blockCount, rowsInPage, pageCounter);
            newTable->blockCount++;
            newTable->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }

    if (pageCounter) {
        bufferManager.writePage(newTable->tableName, newTable->blockCount, rowsInPage, pageCounter);
        newTable->blockCount++;
        newTable->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    tableCatalogue.insertTable(newTable);

    return;
}