#include "ErrorHandler.h"

bool ErrorHandler::checkFile(string fileName, string fileType) {
    bool fileOK = true;
    if (fileType == "JSON") {
        if (fileName.substr(fileName.length() - 4) != "json") {
            cerr << "{\"result\": \"ERROR\", \"error\": \"Format not recognized\"}" << endl;
            fileOK = false;
        }
    } else if (fileType == "CSV") {
        if (fileName.substr(fileName.length() - 3) != "csv") {
            cerr << "{\"result\": \"ERROR\", \"error\": \"Format not recognized\"}" << endl;
            fileOK = false;
        }
    } else if (fileType == "BIN") {
        if (fileName.substr(fileName.length() - 3) != "bin") {
            cerr << "{\"result\": \"ERROR\", \"error\": \"Format not recognized\"}" << endl;
            fileOK = false;
        }
    }
    if (fileOK == true) {
        ifstream fileInput(fileName);
        if (!fileInput) {
            cerr << "{\"result\": \"ERROR\", \"error\": \"" << fileType << " file not found\"}" << endl;
            fileOK = false;
        }
    }
    return fileOK;
}

bool ErrorHandler::checkFields(string lineData, string fileWrite, vector<fileStructure> fileInformation, int caso) {
    bool isValid = true;
    string field;
    int fieldNum1 = 0;
    string fieldNum2 = "0";
    if (caso == 0) {
        ifstream fileW(fileWrite);
        string lineFile1, lineFile2;
        getline(fileW, lineFile2, '\n');
        stringstream ss1(lineData);
        stringstream ss2(lineFile2);
        getline(ss2, fieldNum2, ' ');
        while (getline(ss1, field, ',')) {
            fieldNum1++;
        }
        if (fieldNum1 != stoi(fieldNum2)) {
            isValid = false;
        }

        if (isValid == true) {
            int j = 0;
            string line = "";
            stringstream ss3(lineData);
            while (getline(ss3, field, ',')) {
                if (field != fileInformation[j].name) {
                    isValid = false;
                    break;
                }
                j++;
            }
        }
    } else if (caso == 1) {
        stringstream ss(lineData);
        string field;
        int j = 0;
        while (getline(ss, field, ',')) {
            stringstream ss2(field);
            string name;
            getline(ss2, name, ':');
            if (name != fileInformation[j].name) {
                isValid = false;
                break;
            }
            j++;
        }
    }
    if (isValid == false) {
        cerr << "{" << "\"result\": \"ERROR\", \"error\": \"CSV fields do not match the file structure\"}" << endl;
    }
    return isValid;
}

bool ErrorHandler::CheckKeys(string key, string header) {
    fileMetaData metaData = FileOperations::getMetaData(header);
    bool isKey = false;
    int indexNum = 0;
    for (int i = 1; i <= 3; i++) {
        if (i == 1) {
            if (key == metaData.primaryKey) isKey = true;
            if (!isKey) indexNum = 0;
        } else if (i == 2) {
            if (key == metaData.secondaryKey) isKey = true;
            if (!isKey) indexNum = 2;
        } else if (i == 3) {
            if (key == metaData.secondaryKey2) isKey = true;
            if (!isKey) indexNum = 2;
        }
    }
    if (!isKey) {
        if (indexNum == 1) cout << "{\"result\": \"ERROR\", \"error\": \"primary index field does not exist\"}" << endl;
        if (indexNum == 2) cout << "{\"result\": \"ERROR\", \"error\": \"secondary index field does not exist\"}" << endl;
    }
    return isKey;
}
