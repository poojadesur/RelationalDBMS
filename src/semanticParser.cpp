#include"global.h"

bool semanticParse(){
    logger.log("semanticParse");
    switch(parsedQuery.queryType){
        case CLEAR: return semanticParseCLEAR();
        case CROSS: return semanticParseCROSS();
        case DISTINCT: return semanticParseDISTINCT();
        case EXPORT: return semanticParseEXPORT();
        case INDEX: return semanticParseINDEX();
        case JOIN: return semanticParseJOIN();
        case LIST: return semanticParseLIST();
        case LOAD: return semanticParseLOAD();
        case LOAD_MATRIX: return semanticParseLOAD_MATRIX();
        case PRINT: return semanticParsePRINT();
        case PRINT_MATRIX: return semanticParsePRINT_MATRIX();
        case PROJECTION: return semanticParsePROJECTION();
        case RENAME: return semanticParseRENAME();
        case SELECTION: return semanticParseSELECTION();
        case SORT: return semanticParseSORT();
        case SOURCE: return semanticParseSOURCE();
        case CROSS_TRANSPOSE: return semanticParseCROSS_TRANSPOSE();
        case EXPORT_MATRIX: return semanticParseEXPORT_MATRIX();
        case GROUP_BY: return semanticParseGROUP_BY();
        case JOIN_USING_NESTED: return semanticParseJOIN_USING_NESTED();
        case JOIN_USING_PARTHASH: return semanticParseJOIN_USING_PARTHASH();
        case UPDATE: return semanticParseUPDATE();
        default: cout<<"SEMANTIC ERROR"<<endl;
    }

    return false;
}