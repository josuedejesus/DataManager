#ifndef FILEOPERATIONS_H
#define FILEOPERATIONS_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#include "Stack.h"
#include <set>
using std::cerr;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::cout;
using std::to_string;
using std::vector;
using std::istream;
using std::stoi;
using std::set;

struct fileStructure {
    string name;
    string type;
    int length;
};

struct fileMetaData {
    int fieldsNumber;
    int recordLength;
    string primaryKey;
    string secondaryKey;
    string secondaryKey2;
    int recordsNumber;
};

struct indexStruct {
    string key;
    int offset;
};

enum KeyOperations {EXISTS, REPEATED};

class FileOperations {
    public:
        static void formatField(std::string& field, int length);
        static void setIndex(string dataFile, string indexFile, int keyPosition, int initialOffset, int recordLength);
        static fileMetaData getMetaData(string headerData);
        static string GetFileName(string fileName, string secondary, string type, int charToRemove);
        static vector<indexStruct> setIndexToMemory(string dataFile, string indexFile);
        static bool insertRecord(string fileName, string record, vector<fileStructure> fileInformation, vector<indexStruct> indexList);
        static bool insert(string header, string record, string mainFile);
        static int setRecords(string dataFile, string mainFile, string header, vector<fileStructure> fileStructureList);
        static string parseJson(string jsonInput, vector<fileStructure> fileStructureList); 
        static int reindex(int recordLength, vector<indexStruct> indexList);
        static string getKey(string jsonInput, vector<fileStructure> fileStructureList, int keyPosition);
        static void updateIndex(string key, int offset, string indexFileName, bool append, int lineNumber);
        static int getFieldPosition(string keyName, vector<fileStructure> fileStructureList);
        static vector<fileStructure> setFileStructure(string header);
        static void convertoJson(string dataFile);
        static int getNumberOfRecords(string fileName);
        static string trimField(const string& field);
        static void jsonHelper(string name, string data, int type, bool isLast);
        static string getHeader(string fileName);
        static int getOffset(string key, vector<indexStruct> indexList);
        static void deleteRecord(int offset, string fileName);
        static Stack getAvailList(string fileName);
        static void getData(string fileName, vector<fileStructure> fileInformation, vector<indexStruct> indexList);
        static void getDataByKey(string key, string fileName, string indexFile, vector<fileStructure> fileInformation, vector<indexStruct> indexList);
        static void CompactFile(vector<indexStruct> index, string mainFile);
        static int ReindexFile(string header, string mainFile, string indexFile, int cases);
};

#endif