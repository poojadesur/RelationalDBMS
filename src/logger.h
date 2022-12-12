#ifndef __LOGGER_H_
#define __LOGGER_H_

#include<iostream>
#include<bits/stdc++.h>
#include<sys/stat.h> 
#include<fstream>
#include<sys/file.h>
#include<unistd.h>


using namespace std;

class Logger{

    string logFile = "log";
    ofstream fout;
    
    public:

    Logger();
    void log(string logString);
};

extern Logger logger;

#endif
