#include"semanticParser.h"

void executeCommand();

void executeCLEAR();
void executeCROSS();
void executeDISTINCT();
void executeEXPORT();
void executeINDEX();
void executeJOIN();
void executeJOIN_USING_NESTED();
void executeJOIN_USING_PARTHASH();
void executeLIST();
void executeLOAD();
void executeLOAD_MATRIX();
void executePRINT();
void executePRINT_MATRIX();
void executePROJECTION();
void executeRENAME();
void executeSELECTION();
void executeSORT();
void executeSOURCE();
void executeCROSS_TRANSPOSE();
void executeEXPORT_MATRIX();
void executeGROUP_BY();
void executeUPDATE();

bool evaluateBinOp(int value1, int value2, BinaryOperator binaryOperator);
void printRowCount(int rowCount);