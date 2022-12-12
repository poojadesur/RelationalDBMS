#include"global.h"

void executeCommand(){

    switch(parsedQuery.queryType){
        case CLEAR: executeCLEAR(); break;
        case CROSS: executeCROSS(); break;
        case DISTINCT: executeDISTINCT(); break;
        case EXPORT: executeEXPORT(); break;
        case INDEX: executeINDEX(); break;
        case JOIN: executeJOIN(); break;
        case JOIN_USING_NESTED: executeJOIN_USING_NESTED(); break;
        case JOIN_USING_PARTHASH: executeJOIN_USING_PARTHASH(); break;
        case LIST: executeLIST(); break;
        case LOAD: executeLOAD(); break;
        case LOAD_MATRIX: executeLOAD_MATRIX(); break;
        case PRINT: executePRINT(); break;
        case PRINT_MATRIX: executePRINT_MATRIX(); break;
        case PROJECTION: executePROJECTION(); break;
        case RENAME: executeRENAME(); break;
        case SELECTION: executeSELECTION(); break;
        case SORT: executeSORT(); break;
        case SOURCE: executeSOURCE(); break;
        case CROSS_TRANSPOSE: executeCROSS_TRANSPOSE(); break;
        case EXPORT_MATRIX: executeEXPORT_MATRIX(); break;
        case GROUP_BY: executeGROUP_BY(); break;
        case UPDATE: executeUPDATE(); break;
        default: cout<<"PARSING ERROR"<<endl;
    }

    return;
}

void printRowCount(int rowCount){
    cout<<"\n\nRow Count: "<<rowCount<<endl;
    return;
}