/*
    Title:      Data Structures 2 Proyect – data-manager.cpp
    Purpose:    implement fixed length records, primary and secondary indexes, avail-list and headers 
    Author:     Josue De Jesus
    Date:       July 7, 2023
*/
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "nlohmann/json.hpp"
#include "ErrorHandler.h"
#include "FileOperations.h"
#include "Stack.h"
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::ifstream;
using std::ofstream;
using std::stringstream;
using std::vector;
using nlohmann::json;
using std::to_string;

int main(int argc, char* argv[]) {
    bool fileOK = true;
    int counter = 0;
    string flag1 = argv[1];
    if (flag1 == "-create") {
        string fileName = argv[2];
        fileOK = ErrorHandler::checkFile(fileName, "JSON");
        if (!fileOK) return 0;
        ifstream inputFile(fileName);
        int recordSize = 0, incidents = 0;
        string secondaryKey = "", secondaryKey2 = "";
        string mainFile = FileOperations::GetFileName(fileName, "", "BIN", 4);
        string indexFile = FileOperations::GetFileName(fileName, "", "IDX", 4);
        string availListName = FileOperations::GetFileName(fileName, "", "AVAIL", 4);
        ofstream outputFile(mainFile);
        string fileData((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
        json fileDescription = json::parse(fileData);
        int fieldsNumber = fileDescription["fields"].size();
        string primaryKey = fileDescription["primary-key"].get<string>().substr(0, fileDescription["primary-key"].get<string>().length());
        for (auto& element : fileDescription["secondary-key"]) {
            if (counter == 0)
                secondaryKey = element.get<std::string>().substr(0, element.get<std::string>().length());
            else if (counter == 1)
                secondaryKey2 = element.get<std::string>().substr(0, element.get<std::string>().length());
            counter++;
        }
        for(auto& element : fileDescription["fields"]) {
            string name = element["name"].get<std::string>().substr(0, element["name"].get<std::string>().length());
            if (primaryKey == name || secondaryKey == name || secondaryKey2 == name) {
                incidents++;
            }
            recordSize += element["length"].template get<int>();
        }
        string secIndexFile = "";
        string secIndexFile2 = "";
        if (incidents == 3) {
            secIndexFile = FileOperations::GetFileName(fileName, secondaryKey, "SDX", 5);
            secIndexFile2 = FileOperations::GetFileName(fileName, secondaryKey2, "SDX", 5);
            string header = to_string(fieldsNumber) + " " + to_string(recordSize) + " " + primaryKey + " " + secondaryKey + " " + secondaryKey2 + " " + "0";
            outputFile.write(header.c_str(), header.length());
            for(auto& element : fileDescription["fields"]) {
                string name = element["name"].get<std::string>().substr(0, element["name"].get<std::string>().length());
                string type = element["type"].get<std::string>().substr(0, element["type"].get<std::string>().length());
                int length = element["length"];
                outputFile << "/" << name << " ";
                outputFile << type << " ";
                outputFile << length;
            }
            outputFile << endl;
            ofstream outputIndex(indexFile);
            ofstream outputSecIndex(secIndexFile);
            ofstream outputSecIndex2(secIndexFile2);
            ofstream outputAvail(availListName);
            inputFile.close();
            outputIndex.close();
            outputSecIndex.close();
            outputSecIndex2.close();
            outputAvail.close();
        } else {
            cerr << "{\"result\": \"ERROR\", \"error\": \"primary index field does not exist\"}" << endl;
            return 0;
        }
        cout << "{" << "\"result\": " << "\"OK\", " << "\"fields-count\": " << fieldsNumber << ", \"file\": " << "\"" << mainFile.substr(7, mainFile.length()) << "\", " << "\"index\": " << "\"" << indexFile.substr(7, indexFile.length()) << "\", " << "\"secondary\": " << "[\"" << secIndexFile.substr(7, secIndexFile.length()) << "\", " << "\"" << secIndexFile2.substr(7, secIndexFile2.length()) << "\"" << "]}" << endl;
    } else if (flag1 == "-file") {
        if (argc == 4) {
            string dataFile = argv[2];
            string flag2 = argv[3];
            fileOK = ErrorHandler::checkFile(dataFile, "BIN");
            if (!fileOK) return 0;
            string header = FileOperations::getHeader(dataFile);
            fileMetaData metadata = FileOperations::getMetaData(header);
            string indexFile = FileOperations::GetFileName(dataFile, "", "IDX", 3);
            string secIndexFile = FileOperations::GetFileName(dataFile, metadata.secondaryKey, "SDX", 3);
            string secIndexFile2 = FileOperations::GetFileName(dataFile, metadata.secondaryKey2, "SDX", 3);
            vector<indexStruct> index = FileOperations::setIndexToMemory(dataFile, indexFile);
            string availListFile = FileOperations::GetFileName(dataFile, "", "AVAIL", 4 );
            int linesReclaimed = 0;
            if (flag2 == "-compact") {
                string availListFile = FileOperations::GetFileName(dataFile, "", "AVAIL", 3);
                FileOperations::CompactFile(index, dataFile);
                linesReclaimed = FileOperations::ReindexFile(header, dataFile, indexFile, 2);
                FileOperations::ReindexFile(header, dataFile, secIndexFile, 2);
                FileOperations::ReindexFile(header, dataFile, secIndexFile2, 2);
                ofstream outputAvail(availListFile, std::ios::out | std::ios::trunc);
                outputAvail.close();
                cout << "{\"result\":\"OK\", \"records-reclaimed\": "<< linesReclaimed << "}" << endl;
            } else if (flag2 == "-reindex") {
                string availListFile = FileOperations::GetFileName(dataFile, "", "AVAIL", 3);
                linesReclaimed = FileOperations::ReindexFile(header, dataFile, indexFile, 1);
                FileOperations::ReindexFile(header, dataFile, secIndexFile, 1);
                FileOperations::ReindexFile(header, dataFile, secIndexFile2, 1);
                ofstream outputAvail(availListFile, std::ios::out | std::ios::trunc);
                outputAvail.close();
                cout << "{\"result\":\"OK\", \"indexes-processed\": "<< linesReclaimed << "}" << endl;
            } else if (flag2 == "-describe") {
                FileOperations::convertoJson(dataFile);
            } else if (flag2 == "-GET") {
                string header = FileOperations::getHeader(dataFile);
                fileMetaData metaData = FileOperations::getMetaData(header);
                string indexFile = FileOperations::GetFileName(dataFile, "", "IDX", 3);
                vector<fileStructure> fileInformation = FileOperations::setFileStructure(header);
                vector<indexStruct> indexList = FileOperations::setIndexToMemory(dataFile, indexFile);
                FileOperations::getData(dataFile, fileInformation, indexList);
            }
        } else if (argc == 5) {
            string flag1 = argv[1];
            string dataFile = argv[2];
            string flag2 = argv[3];
            fileOK = ErrorHandler::checkFile(dataFile, "BIN");
            if (!fileOK) return 0;
            if (flag2 == "-load") {
                //-----------------------------------------------LOAD------------------------------------------------
                string mainFile = argv[2];
                string dataFile = argv[4];
                string line;
                fileOK = ErrorHandler::checkFile(mainFile, "BIN");
                if (!fileOK) return 0;
                fileOK = ErrorHandler::checkFile(dataFile, "CSV");
                if (!fileOK) return 0;
                
                string header = FileOperations::getHeader(mainFile);
                fileMetaData metaData = FileOperations::getMetaData(header);
                string indexFile = FileOperations::GetFileName(dataFile, "", "IDX", 3);
                string secIndexFile = FileOperations::GetFileName(dataFile, metaData.secondaryKey, "SDX", 4);
                string secIndexFile2 = FileOperations::GetFileName(dataFile, metaData.secondaryKey2, "SDX", 4);
                string availListFile = FileOperations::GetFileName(dataFile, metaData.primaryKey, "AVAIL", 4);
                vector<fileStructure> fileStructureList = FileOperations::setFileStructure(header);
                bool isValid = ErrorHandler::checkFields(FileOperations::getHeader(dataFile), mainFile, fileStructureList, 0);
                if (!isValid) return 0;
                int initialOffset = FileOperations::getHeader(mainFile).length();
                int recordLength = metaData.recordLength;
                int pkPosition = 0;
                int skPosition = 0;
                int numberOfRecords = FileOperations::setRecords(dataFile, mainFile, header, fileStructureList);
                pkPosition = FileOperations::getFieldPosition(metaData.primaryKey, fileStructureList);
                FileOperations::setIndex(dataFile, indexFile, pkPosition, initialOffset, recordLength);
                skPosition = FileOperations::getFieldPosition(metaData.secondaryKey, fileStructureList);
                FileOperations::setIndex(dataFile, secIndexFile, skPosition, initialOffset, recordLength);
                skPosition = FileOperations::getFieldPosition(metaData.secondaryKey2, fileStructureList);
                FileOperations::setIndex(dataFile, secIndexFile2, skPosition, initialOffset, recordLength);
                ofstream outputAvail(availListFile, std::ios::out | std::ios::trunc);
                outputAvail.close();
                cout << "{" << "\"result\": " << "\"OK\", " << "\"records\": \"" << numberOfRecords << "\"]}" << endl;
            } else if(flag2 == "-DELETE") {
                string flag4 = argv[4];
                string primaryKey = flag4.substr(4, flag4.length());
                flag4 = flag4.substr(0, 3);
                string indexFile = FileOperations::GetFileName(dataFile, "", "IDX", 3);
                vector<indexStruct> indexList = FileOperations::setIndexToMemory(dataFile, indexFile);
                int offset = FileOperations::getOffset(primaryKey, indexList);
                if (offset == -1) {
                    cerr << "{\"result\":\"not found\"}" << endl;
                    return 0;
                }
                    
                FileOperations::deleteRecord(offset, dataFile);
                cout << "{\"result\":\"OK\"}" << endl;
            } else {
                cerr << "{" << "\"result\": \"ERROR\", \"error\": \"The entered flags are invalid\"}" << endl;
            }
        } else if (argc == 6) {
            string dataFile = argv[2];
            string flag2 = argv[3];
            string flag3 = argv[4];
            string flag4 = argv[5];
            fileOK = ErrorHandler::checkFile(dataFile, "BIN");
            if (fileOK == false)
                return 0;
            string flag3Temp = flag3.substr(0, 3);
            string keyValue = flag4.substr(7);
            flag4 = flag4.substr(0, 6);
            if (flag3 == "-pk") {
                string indexFile = FileOperations::GetFileName(dataFile, "", "IDX", 3);
                vector<indexStruct> indexList = FileOperations::setIndexToMemory(dataFile, indexFile);
                ifstream inputFile(dataFile);
                int offset = FileOperations::getOffset(keyValue, indexList);
                if (offset == -1) {
                    cerr << "{\"result\":\"not found\"}" << endl;
                    return 0;
                }
                string header = "";
                getline(inputFile, header, '\n');
                vector<fileStructure> fileInformation = FileOperations::setFileStructure(header);
                FileOperations::getDataByKey(keyValue, dataFile, indexFile, fileInformation, indexList);
            } else if (flag3Temp == "-sk") {
                string secondaryKey = flag3.substr(4, flag3.length());
                bool keyOk = ErrorHandler::CheckKeys(secondaryKey, FileOperations::getHeader(dataFile));
                if (!keyOk) return 0;
                string csvFileName = FileOperations::GetFileName(dataFile, "", "CSV", 3);
                string header = FileOperations::getHeader(csvFileName);
                string indexFile = FileOperations::GetFileName(dataFile, secondaryKey, "SDX", 4);
                vector<indexStruct> indexList = FileOperations::setIndexToMemory(dataFile, indexFile);
                string header2 = FileOperations::getHeader(dataFile);
                vector<fileStructure> fileInformation = FileOperations::setFileStructure(header2);
                int offset = FileOperations::getOffset(keyValue, indexList);
                if (offset == -1) {
                    cout << "[]" << endl;
                    return 0;
                }
                FileOperations::getDataByKey(keyValue, dataFile, indexFile, fileInformation, indexList);
            } else {
                cerr << "{" << "\"result\": \"ERROR\", \"error\": \"The entered flags are invalid\"}" << endl;
                return 0;
            }
        } else {
            string mainFile = argv[2];
            bool fileOK = ErrorHandler::checkFile(mainFile, "BIN");
            if (!fileOK) return 0;
            string flag2 = argv[3];
            string header = FileOperations::getHeader(mainFile);
            fileMetaData metaData = FileOperations::getMetaData(header);
            string indexFile = FileOperations::GetFileName(mainFile, "", "IDX", 3);
            vector<indexStruct> indexList = FileOperations::setIndexToMemory(mainFile, indexFile);
            vector<fileStructure> fileStructureList = FileOperations::setFileStructure(header);
            int recordLength = metaData.recordLength;
            int fieldPosition = 0;
            string key = "";
            if (flag2 == "-POST") {
                string dataInput = "";
                for (int i = 4; i < argc; i++) {
                    dataInput = dataInput + argv[i];
                }
                string jsonString = dataInput.substr(7, dataInput.length() - 8);
                string flag3 = dataInput.substr(0, 5);
                fileOK = ErrorHandler::checkFields(jsonString, "", fileStructureList, 1);
                if (fileOK == false)
                    return 0;
                if (flag3 == "-data") {
                    string record = FileOperations::parseJson(jsonString, fileStructureList);
                    string key;
                    stringstream ss(record);
                    getline(ss, key, ' ');
                    int repeated = FileOperations::getOffset(key, indexList);
                    if (repeated != -1) {
                        cerr << "{\"result\": \"ERROR\", \"error\": \"Primary key already exists\"}" << endl;
                        return 0;
                    }
                    fileMetaData metaData = FileOperations::getMetaData(header);
                    FileOperations::insert(header, record, mainFile);
                    int offset = FileOperations::reindex(recordLength, indexList);
                    fieldPosition = FileOperations::getFieldPosition(metaData.primaryKey, fileStructureList);
                    key = FileOperations::getKey(jsonString, fileStructureList, fieldPosition);
                    FileOperations::updateIndex(key, offset, indexFile, true, 0);

                    fieldPosition = FileOperations::getFieldPosition(metaData.secondaryKey, fileStructureList);
                    key = FileOperations::getKey(jsonString, fileStructureList, fieldPosition);
                    indexFile = FileOperations::GetFileName(mainFile, metaData.secondaryKey, "SDX", 4);
                    FileOperations::updateIndex(key, offset, indexFile, true, 0);

                    fieldPosition = FileOperations::getFieldPosition(metaData.secondaryKey2, fileStructureList);
                    key = FileOperations::getKey(jsonString, fileStructureList, fieldPosition);
                    indexFile = FileOperations::GetFileName(mainFile, metaData.secondaryKey2, "SDX", 4);
                    FileOperations::updateIndex(key, offset, indexFile, true, 0);
                    cout << "{" << "\"result\": " << "\"OK\"}" << endl;
                } else {
                    cerr << "{" << "\"result\": \"ERROR\", \"error\": \"The entered flags are invalid\"}" << endl;
                    return 0;
                }
            } else if (flag2 == "-PUT") {
                string flag4 = argv[4];
                string primaryKey = flag4.substr(4, flag4.length());
                flag4 = flag4.substr(0, 3);
                if (flag4 == "-pk") {
                    int offset = FileOperations::getOffset(primaryKey, indexList);
                    if (offset == -1) {
                        cerr << "{\"result\":\"not found\"}" << endl;
                        return 0;
                    }
                    string dataInput = "";
                    for (int i = 5; i < argc; i++) {
                        dataInput = dataInput + argv[i];
                    }
                    string jsonString = dataInput.substr(7, dataInput.length() - 8);
                    string flag5 = dataInput.substr(0, 5);
                    fileOK = ErrorHandler::checkFields(jsonString, "", fileStructureList, 1);
                    if (fileOK == false)
                        return 0;
                    if (flag5 == "-data") {
                        string record = FileOperations::parseJson(jsonString, fileStructureList);
                        ofstream outputFile(mainFile, std::ios::in | std::ios::out);
                        outputFile.seekp(offset);
                        outputFile.write(record.c_str(), recordLength);
                        outputFile.close();
                        fileMetaData metaData = FileOperations::getMetaData(header);
                        fieldPosition = FileOperations::getFieldPosition(metaData.primaryKey, fileStructureList);
                        key = FileOperations::getKey(jsonString, fileStructureList, fieldPosition);
                        FileOperations::updateIndex(key, offset, indexFile, false, 5);
                        fieldPosition = FileOperations::getFieldPosition(metaData.secondaryKey, fileStructureList);
                        key = FileOperations::getKey(jsonString, fileStructureList, fieldPosition);
                        indexFile = FileOperations::GetFileName(mainFile, metaData.secondaryKey, "SDX", 4);
                        FileOperations::updateIndex(key, offset, indexFile, false, 5);
                        fieldPosition = FileOperations::getFieldPosition(metaData.secondaryKey2, fileStructureList);
                        key = FileOperations::getKey(jsonString, fileStructureList, fieldPosition);
                        indexFile = FileOperations::GetFileName(mainFile, metaData.secondaryKey2, "SDX", 4);
                        FileOperations::updateIndex(key, offset, indexFile, false, 5);
                        cout << "{" << "\"result\": " << "\"OK\"}" << endl;
                    } else {
                        cerr << "{" << "\"result\": \"ERROR\", \"error\": \"The entered flags are invalid\"}" << endl;
                        return 0;
                    }
                } else {
                    cerr << "{" << "\"result\": \"ERROR\", \"error\": \"The entered flags are invalid\"}" << endl;
                    return 0;
                }
            } else if(flag2 == "-GET") {
                string dataInput;
                for (int i = 5; i < argc; i++) {
                    dataInput = dataInput + argv[i] + " ";
                }
                string secondaryKey = dataInput.substr(7, dataInput.length() - 8);
                string flag3 = dataInput.substr(0, 6);
                if (flag3 == "-value") {
                    string flag4 = argv[4];
                    string flag4Temp = flag4.substr(0, 3);
                    string keyName = flag4.substr(4, flag4.length());
                    bool keyOk = false;
                    keyOk = ErrorHandler::CheckKeys(keyName, FileOperations::getHeader(mainFile));
                    if (keyOk == false) return 0;
                    if (flag4Temp == "-sk") {
                        cout << secondaryKey << endl;
                        string csvFileName = FileOperations::GetFileName(mainFile, "", "CSV", 3);
                        string header = FileOperations::getHeader(csvFileName);
                        string indexFile = FileOperations::GetFileName(mainFile, keyName, "SDX", 4);
                        vector<indexStruct> indexList = FileOperations::setIndexToMemory(mainFile, indexFile);
                        string header2 = FileOperations::getHeader(mainFile);
                        vector<fileStructure> fileInformation = FileOperations::setFileStructure(header2);
                        int offset = FileOperations::getOffset(secondaryKey, indexList);
                        if (offset == -1) {
                            cout << "[]" << endl;
                            return 0;
                        }
                        FileOperations::getDataByKey(secondaryKey, mainFile, indexFile, fileInformation, indexList);
                    } else {
                        cerr << "{" << "\"result\": \"ERROR\", \"error\": \"The entered flags are invalid\"}" << endl;
                        return 0;
                    }
                } else {
                    cerr << "{" << "\"result\": \"ERROR\", \"error\": \"The entered flags are invalid\"}" << endl;
                    return 0;
                }
            } else {
                cerr << "{" << "\"result\": \"ERROR\", \"error\": \"The entered flags are invalid\"}" << endl;
                return 0;
            }
        }        
    } else {
        cerr << "{" << "\"result\": \"ERROR\", \"error\": \"Flag entered does not exist\"}" << endl;
        return 0;
    }
    return 0;
}





