#include "tableCatalogue.h"
#include "matrixCatalogue.h"

using namespace std;

enum QueryType
{
    CLEAR,
    CROSS,
    DISTINCT,
    EXPORT,
    INDEX,
    JOIN,
    JOIN_USING_NESTED,
    JOIN_USING_PARTHASH,
    LIST,
    LOAD,
    LOAD_MATRIX,
    PRINT,
    PRINT_MATRIX,
    PROJECTION,
    RENAME,
    SELECTION,
    SORT,
    SOURCE,
    CROSS_TRANSPOSE,
    EXPORT_MATRIX,
    GROUP_BY,
    UPDATE,
    UNDETERMINED
};

enum BinaryOperator
{
    LESS_THAN,
    GREATER_THAN,
    LEQ,
    GEQ,
    EQUAL,
    NOT_EQUAL,
    NO_BINOP_CLAUSE
};

enum SortingStrategy
{
    ASC,
    DESC,
    NO_SORT_CLAUSE
};

enum SelectType
{
    COLUMN,
    INT_LITERAL,
    NO_SELECT_CLAUSE
};

enum OperationType
{
    MAX,
    MIN,
    SUM,
    AVG,
    NO_OPERATION_CLAUSE
};

enum UpdateType
{
    MULTIPLY,
    ADD,
    SUBTRACT,
    NO_UPDATE_CLAUSE
};

class ParsedQuery
{

public:
    QueryType queryType = UNDETERMINED;

    string clearRelationName = "";

    string crossResultRelationName = "";
    string crossFirstRelationName = "";
    string crossSecondRelationName = "";

    string distinctResultRelationName = "";
    string distinctRelationName = "";

    string exportRelationName = "";

    IndexingStrategy indexingStrategy = NOTHING;
    string indexColumnName = "";
    string indexRelationName = "";

    BinaryOperator joinBinaryOperator = NO_BINOP_CLAUSE;
    string joinResultRelationName = "";
    string joinFirstRelationName = "";
    string joinSecondRelationName = "";
    string joinFirstColumnName = "";
    string joinSecondColumnName = "";
    string joinBufferSize = "";


    string loadRelationName = "";

    string printRelationName = "";

    string projectionResultRelationName = "";
    vector<string> projectionColumnList;
    string projectionRelationName = "";

    string renameFromColumnName = "";
    string renameToColumnName = "";
    string renameRelationName = "";

    SelectType selectType = NO_SELECT_CLAUSE;
    BinaryOperator selectionBinaryOperator = NO_BINOP_CLAUSE;
    string selectionResultRelationName = "";
    string selectionRelationName = "";
    string selectionFirstColumnName = "";
    string selectionSecondColumnName = "";
    int selectionIntLiteral = 0;

    SortingStrategy sortingStrategy = NO_SORT_CLAUSE;
    string sortResultRelationName = "";
    string sortColumnName = "";
    string sortRelationName = "";
    int sortColumnIndex = 0;
    int sortBufferSize = 10;

    string groupByResultRelationName = "";
    string groupByAttributeName = "";
    string groupByRelationName = "";
    OperationType groupByOperation = NO_OPERATION_CLAUSE;
    string groupByOperationOnAttributeName = "";

    string updateTableName = "";
    string updateColumnName = "";
    UpdateType updateOperation = NO_UPDATE_CLAUSE;
    int updateValue = 0;

    string sourceFileName = "";

    ParsedQuery();
    void clear();
};

bool syntacticParse();
bool syntacticParseCLEAR();
bool syntacticParseCROSS();
bool syntacticParseDISTINCT();
bool syntacticParseEXPORT();
bool syntacticParseINDEX();
bool syntacticParseJOIN();
bool syntacticParseLIST();
bool syntacticParseLOAD();
bool syntacticParseLOAD_MATRIX();
bool syntacticParsePRINT();
bool syntacticParsePRINT_MATRIX();
bool syntacticParsePROJECTION();
bool syntacticParseRENAME();
bool syntacticParseSELECTION();
bool syntacticParseSORT();
bool syntacticParseSOURCE();
bool syntacticParseCROSS_TRANSPOSE();
bool syntacticParseEXPORT_MATRIX();
bool syntacticParseJOIN_USING_NESTED();
bool syntacticParseJOIN_USING_PARTHASH();
bool syntacticParseGROUP_BY();
bool syntacticParseUPDATE();

bool isFileExists(string tableName);
bool isQueryFile(string fileName);
