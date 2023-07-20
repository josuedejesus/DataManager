#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H
#include "FileOperations.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
using std::cerr;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::vector;

class ErrorHandler {
public:
    static bool checkFile(string fileName, string fileType);
    static bool checkFields(string lineData, string fileWrite, vector<fileStructure> fileInformation, int caso);
    static bool CheckKeys(string key, string header);
};

#endif