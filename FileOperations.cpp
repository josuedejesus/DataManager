#include "FileOperations.h"


void FileOperations::formatField(std::string& field, int length) {
    field.resize(length, ' ');
}

/**
* Set Indedx
* It writes all the indexes and the offset associated with it to the dile
*/

void FileOperations::setIndex(string dataFile, string indexFile, int keyPosition, int initialOffset, int recordLength) {
    ifstream inputFile(dataFile);
    ofstream outputFile(indexFile);
    int offset = initialOffset;
    int length = 0;
    string line;
    getline(inputFile, line, '\n');
    while (getline(inputFile, line, '\n')) {
        int pos = 0;
        stringstream ss(line);
        string field;
        offset = offset + length + 1;
        while(getline(ss, field, ',')) {
            if (pos == keyPosition) {
                outputFile.write(field.c_str(), field.length());
                outputFile.write(",", 1);
                outputFile.write(to_string(offset).c_str(), to_string(offset).length());
                outputFile.write("\n", 1);
            }
            pos++;
        }
        length = recordLength;
    }
}

/**
* Insert
* Takes a record and inserts it at the end of the file
*/

bool FileOperations::insert(string header, string record, string mainFile) {
    ofstream outputFile(mainFile, std::ios::app);
    outputFile.write(record.c_str(), record.length());
    //outputFile.write(reinterpret_cast<const char*>(&record), record.length());
    outputFile.write("\n", 1);
    outputFile.close();
    return false;
}

/**
* set Records
* Takes the csv file and creates a fixed length record for each line, then sends it to the insert fuction to write it on the file
*/

int FileOperations::setRecords(string dataFile, string mainFile, string header, vector<fileStructure> fileStructureList) {
    ofstream outputFile(mainFile);
    outputFile.write(header.c_str(), header.length());
    outputFile.write("\n", 1);
    outputFile.close();
    ifstream inputFile(dataFile);
    string line2, headerCSV, field;
    int numberOfRecords = 0;
    getline(inputFile, headerCSV, '\n');
    while (getline(inputFile, line2, '\n')) {
        string record = "";
        stringstream ss(line2);
        for (fileStructure fs : fileStructureList) {
            getline(ss, field, ',');
            if (field == "") {
                if (fs.type == "int") {
                    field = "0";
                }
                if (fs.type == "double") {
                    field = "0";
                }
            }
            FileOperations::formatField(field, fs.length);
            record = record + field;
        }
        FileOperations::insert("", record, mainFile);
        numberOfRecords++;
    }
    inputFile.close();
    return numberOfRecords;
}

/**
* parseJson
* Takes the a string and extracts each field returning a formated record string
*/

string FileOperations::parseJson(string jsonInput, vector<fileStructure> fileStructureList) {
    stringstream ss(jsonInput);
    string fieldName, fieldData, field, record;
    int i = 0;
    while(getline(ss, field, ',')) {
        stringstream ss2(field);
        getline(ss2, fieldName, ':');
        getline(ss2, fieldData, ',');
        formatField(fieldData, fileStructureList[i].length);
        record = record + fieldData;
        i++;
    }
    return record;
}

/**
* set Records
* Takes the csv file and creates a fixed length record for each line, then sends it to the insert fuction to write it on the file
*/

/**
* getKey
* Returns the key from the index files
*/
string FileOperations::getKey(string jsonInput, vector<fileStructure> fileStructureList, int keyPosition) {
    stringstream ss(jsonInput);
    string fieldName, fieldData, field, key;
    int i = 0;
    while(getline(ss, field, ',')) {
        stringstream ss2(field);
        getline(ss2, fieldName, ':');
        getline(ss2, fieldData, ',');
        if (i == keyPosition) {
            key = fieldData;
        }
        i++;
    }
    return key;
}

/**
* getMetaData
* Receives the file header and extracts its contents returning a struct that helps us get the metadata from the file
*/

fileMetaData FileOperations::getMetaData(string headerData) {
    fileMetaData meta;
    int counter = 0;
    string fieldsNumber, recordLength, index, secIndex, secIndex2, recordsNumber;
    string dataSection = "";
    stringstream ss(headerData);
    while(getline(ss, dataSection, '/')) {
        stringstream ss2(dataSection);
        if (counter == 0) {
            ss2 >> fieldsNumber >> recordLength >> index >> secIndex >> secIndex2 >> recordsNumber;
            meta.fieldsNumber = stoi(fieldsNumber);
            meta.recordLength = stoi(recordLength);
            meta.primaryKey = index;
            meta.secondaryKey = secIndex;
            meta.secondaryKey2 = secIndex2;
            meta.recordsNumber = stoi(recordsNumber);
        }
        counter++;    
    }
    return meta;
}

/**
* updateIndex
* Rebuilds the index data for each index file
*/

void FileOperations::updateIndex(string key, int offset, string indexFileName, bool append, int lineNumber) {
    //ofstream outputFile(indexFileName, std::ios::out);
    ofstream outputFile;
    
    std::ios_base::openmode mode = std::ios::binary;
    string index = key + "," + to_string(offset);
    if (append) {
        mode |= std::ios::app;
    } else {
        mode |= std::ios::in | std::ios::out;
    }        
    outputFile.open(indexFileName, mode);
    if (outputFile.is_open()) {
        if (!append) {
            vector<indexStruct> indexList = setIndexToMemory("", indexFileName);
            for (indexStruct is : indexList) {
                if (is.offset == offset) {
                    is.key = key;
                }
                index = is.key + "," + to_string(is.offset);
                outputFile.write(index.c_str(), index.length());
                outputFile.write("\n", 1);
            }
        } else {
            outputFile.write(index.c_str(), index.length());
            outputFile.write("\n", 1);
        }
    }
    outputFile.close();
}

/**
* reindex
* Return the initial offset
*/

int FileOperations::reindex(int recordLength, vector<indexStruct> indexList) {
    int offset = indexList[indexList.size() - 1].offset + recordLength + 1;
    //cout << offset << endl;
    return offset;
}

/**
* getFileName
* Get the input that has the name of the file being used and return different names according to the needs
*/

string FileOperations::GetFileName(string fileName, string secondary, string type, int charToRemove) {
    string newFile = "";
    string name = "";
    if (type == "BIN") {
        newFile = fileName.substr(0, fileName.length() - charToRemove);
        stringstream ss(newFile);
        getline(ss, newFile, '.');
        getline(ss, newFile, '.');
        newFile = "." + newFile + ".bin";
    } else if (type == "IDX") {
        newFile = fileName.substr(0, fileName.length() - charToRemove);
        stringstream ss(newFile);
        getline(ss, newFile, '.');
        getline(ss, newFile, '.');
        newFile = "." + newFile + ".idx";
    } else if (type == "CSV") {
        newFile = fileName.substr(0, fileName.length() - charToRemove);
        stringstream ss(newFile);
        getline(ss, newFile, '.');
        getline(ss, newFile, '.');
        newFile = "." + newFile + ".csv";
    } else if (type == "SDX") {
        newFile = fileName.substr(0, fileName.length() - charToRemove);
        stringstream ss(newFile);
        getline(ss, newFile, '.');
        getline(ss, newFile, '.');
        newFile = "." + newFile + "-" + secondary + ".sdx";
    } else if (type == "AVAIL") {
        newFile = fileName.substr(0, fileName.length() - charToRemove);
        stringstream ss(newFile);
        getline(ss, newFile, '.');
        getline(ss, newFile, '.');
        newFile = "." + newFile + "-avail-list" + ".bin";
    }
    return newFile;
}

/**
* setIndexToMemory
* Gets the index files and sets them to memory, returns a vector containing a struct that contains key and offset
*/

vector<indexStruct> FileOperations::setIndexToMemory(string dataFile, string indexFile) {
    string availListName = GetFileName(dataFile, "", "AVAIL", 4);
    Stack availList = getAvailList(availListName);
    indexStruct recordIndex;
    vector<indexStruct> keyVec;
    ifstream inputFile(indexFile);
    string line, key, offset;
    while(getline(inputFile, line, '\n')) {
            stringstream ss(line);
            getline(ss, key, ',');
            getline(ss, offset, ',');
            if (stoi(offset) != availList.top()) {
                recordIndex.key = key;
                recordIndex.offset = stoi(offset);
                keyVec.push_back(recordIndex);
            } else {
                availList.pop();
            }
    }
    return keyVec;
}

/**
* InsertRecord
* Helps the setRecords fuction to insert each record to the file
*/

bool FileOperations::insertRecord(string fileName, string record, vector<fileStructure> fileInformation, vector<indexStruct> indexList) {
    bool notRepeated = true;
    ofstream outputFile(fileName, std::ios::binary | std::ios::ate | std::ios::app);
    bool insertionOK = true;
    stringstream ss(record);
    string field;
    string formatedRecord;
    int pos = 0;
    while (getline(ss, field, ',')) {
        stringstream ss2(field);
        string name, data;
        getline(ss2, name, ':');
        getline(ss2, data, ',');
        if (pos == 0) {
            for (indexStruct i : indexList) {
                if (i.key == data) {
                    notRepeated = false;
                }
            }
        }
        formatField(data, fileInformation[pos].length);
        formatedRecord = formatedRecord + data;
        pos++;
    }
    if (notRepeated == false) {
        cerr << "{\"result\": \"ERROR\", \"error\": \"The primary key already exists\"}" << endl;
        return notRepeated;
    }
    else {
        outputFile.write(formatedRecord.c_str(), 49);
        outputFile.write("\n", 1);
        outputFile.close();
        return insertionOK;
    }
}

/**
* GetFieldPosition
* Return the position of the field of the record
*/

int FileOperations::getFieldPosition(string keyName, vector<fileStructure> fileStructureList) {
    int keyPosition = 0, i = 0;
    string field;
    for (fileStructure fs : fileStructureList) {
        if (fs.name == keyName) {
            keyPosition = i;
        }
        i++;
    }
    return keyPosition;
}

/**
* SetFileStructure
* Receives the header and returns a vector containing the structure that will be used
*/

vector<fileStructure> FileOperations::setFileStructure(string header) {
    fileStructure fileStruct;
    vector<fileStructure> fileInformation;
    int counter = 0;
    string name, type, length;
    string dataSection = "";
    stringstream ss(header);
    while(getline(ss, dataSection, '/')) {
        stringstream ss2(dataSection);
        if (counter != 0) {
            ss2 >> name >> type >> length;
            fileStruct.name = name;
            fileStruct.type = type;
            fileStruct.length = stoi(length);
            fileInformation.push_back(fileStruct);
        }
        counter++;    
    }
    return fileInformation;
}

/**
* ConvertToJson
* Takes the Json data we enter and formats it in order to extract each field
*/

void FileOperations::convertoJson(string dataFile) {
    cout << "{" << endl;
    cout << "  \"fields\": [" << endl;
    string header = getHeader(dataFile);
    vector<fileStructure> fileInformation = setFileStructure(header);
    size_t i = 0;
    for (fileStructure f : fileInformation) {
        if (i != fileInformation.size() - 1) {
            cout << "    {\"name\": \"" << f.name << "\", \"type\": \"" << f.type << "\", \"length\": " << f.length << "}, " << endl;
        } else {
            cout << "    {\"name\": \"" << f.name << "\", \"type\": \"" << f.type << "\", \"length\": " << f.length << "} " << endl;
        }
        i++;
    }
    cout << "  ]," << endl;
    ifstream inputFile(dataFile);
    string line = "", field = "", primaryKey = "", secondaryKey = "", secondaryKey2 = "";
    int recordsNumber = FileOperations::getNumberOfRecords(dataFile);
    getline(inputFile, line, '/');
    stringstream ss(line);
    int counter = 0;
    while (getline(ss, field, ' ')) {
        if (counter == 2) {
            primaryKey = field;
        }
        if (counter == 3) {
            secondaryKey = field;
        }
        if (counter == 4) {
            secondaryKey2 = field;
        }
        counter++;
    }
    cout << "  \"primary-key\": " << "\"" << primaryKey << "\"," << endl;
    cout << "  \"secondary-key\": " << "[\"" << secondaryKey << "\", " << "\"" << secondaryKey2 << "\"]," << endl;
    cout << "  \"records\": " << recordsNumber << endl;
    cout << "]" << endl;
}

/**
* TrimField
* Removes inecessary black spaces in the records
*/

string FileOperations::trimField(const string& field) {
    string trimmedField = field;
    size_t lastNonSpace = trimmedField.find_last_not_of(' ');
    if (lastNonSpace != std::string::npos) {
        trimmedField.erase(lastNonSpace + 1);
    } else {
        trimmedField.clear();
    }
    return trimmedField;
}

/**
* JsonHelper
* Receives the data from ConvertToJson and print the data in Json format to the console
*/

void FileOperations::jsonHelper(string name, string data, int type, bool isLast) {
    if (type == 0) {
        if (isLast == true)
            cout << "   \"" << name << "\": " << "\"" << data << "\"" << endl;
        else
            cout << "   \"" << name << "\": " << "\"" << data << "\"" << "," << endl;
    } else {
        if (isLast == true)
            cout << "   \"" << name << "\": " << data << endl;
        else
            cout << "   \"" << name << "\": " << data << "," << endl;
    }
    
}

/**
* GetHeader
* Read the main file and returns the header
*/

string FileOperations::getHeader(string fileName) {
    string headerData = "";
    ifstream inputFile(fileName);
    getline(inputFile, headerData, '\n');
    return headerData;
}

/**
* GetData
* Reads the file and sends the parsed data to the Json helper in order to print the data to console
*/

void FileOperations::getData(string fileName, vector<fileStructure> fileInformation, vector<indexStruct> indexList) {
    string indexFile = FileOperations::GetFileName(fileName, "", "IDX", 4);
    ifstream inputFile(fileName, std::ios::binary);
    int numberOfRecords = indexList.size();
    cout << "[" << endl;
    int pos = 1;
    int numberOfFields = fileInformation.size();
    for (indexStruct is :  indexList) {
        bool isLast = false;
        int posField = 1;
        int offset = is.offset;
        cout << " {" << endl;
        for (fileStructure f : fileInformation) {
            inputFile.seekg(offset);
            string field(f.length, '\0');
            inputFile.read(&field[0], f.length);
            offset = offset + f.length;
            if (posField == numberOfFields) {
                isLast = true;
            }
            if (f.type == "char") {
                jsonHelper(f.name, trimField(field), 0, isLast);
            } else {
                jsonHelper(f.name, trimField(field), 1, isLast);
            }
            posField++;
        }
        if (pos == numberOfRecords) {
            cout << " }" << endl;
        } else {
            cout << " }," << endl;
        }
        pos++;
    }
    cout << "]" << endl;
}

/**
* GetNumberOfRecords
* Returns the number of records inside the file
*/

int FileOperations::getNumberOfRecords(string fileName) {
    int numberOfRecords = 0;
    string line = "";
    ifstream inputFile(fileName);
    getline(inputFile, line, '\n');
    while (getline(inputFile, line, '\n')) {
        numberOfRecords++;
    }
    return numberOfRecords;
}

/**
* GetDataByKey
* Looks for the key it takes as a parameter and return the data associated with it
*/

void FileOperations::getDataByKey(string key , string dataFile, string indexFile, vector<fileStructure> fileInformation, vector<indexStruct> indexList) {
    ifstream inputFile(dataFile);
    int numberOfRecords = 0;
    for (indexStruct is :  indexList) {
        if (is.key == key) {
            numberOfRecords++;
        }
    }
    cout << "[" << endl;
    int pos = 1;
    int numberOfFields = fileInformation.size();
    for (indexStruct is :  indexList) {
        if (is.key == key) {
            bool isLast = false;
            int posField = 1;
            int offset = is.offset;
            cout << " {" << endl;
            for (fileStructure f : fileInformation) {
                inputFile.seekg(offset);
                string field(f.length, '\0');
                inputFile.read(&field[0], f.length);
                offset = offset + f.length;
                if (posField == numberOfFields) {
                    isLast = true;
                }
                if (f.type == "char") {
                    jsonHelper(f.name, trimField(field), 0, isLast);
                } else {
                    jsonHelper(f.name, trimField(field), 1, isLast);
                }
                posField++;
            }
            if (pos == numberOfRecords) {
                cout << " }" << endl;
            } else {
                cout << " }," << endl;
            }
            pos++;
        }
    }
    cout << "]" << endl;
}

/**
* GetOffset
* Return the offset for the key
*/

int FileOperations::getOffset(string key, vector<indexStruct> indexList) {
    int offset = -1;
    for (indexStruct fs : indexList) {
        if (fs.key == key) {
            offset = fs.offset;
            break;
        }
    }
    return offset;
}

/**
* DeleteRecord
* Stores the deleted record to the avail list file
*/

void FileOperations::deleteRecord(int offset, string fileName) {
    string availListName = GetFileName(fileName, "", "AVAIL", 4);
    ofstream outputFile(availListName, std::ios::binary | std::ios::app);
    //outputFile.write(reinterpret_cast<const char*>(&offset), sizeof(int));
    outputFile.write(to_string(offset).c_str(), to_string(offset).length());
    outputFile.write("\n", 1);
    outputFile.close();
}

/**
* GetAvailList
* Reads the avail list file and returns a stack containing the references for the deleted records
*/

Stack FileOperations::getAvailList(string fileName) {
    set<int, std::greater<int>> temp;
    Stack availList;
    ifstream inputFile(fileName, std::ios::binary);
    string offset;
    while(getline(inputFile, offset, '\n')) {
        temp.insert(stoi(offset));
    }
    availList.push(-1);
    for (int value : temp) {
        availList.push(value);
    }
    return availList;
}

/**
* CompactFile
* Copies the data from the main file excluding the deleted records to a temporary file and deletes the original one, them renames the temp one
*/

void FileOperations::CompactFile(vector<indexStruct> index, string mainFile) {
    string header = FileOperations::getHeader(mainFile);
    fileMetaData metaData = FileOperations::getMetaData(header);
    int recordLength = metaData.recordLength + 1;
    string tempFile = mainFile.substr(0, mainFile.length() - 4);
    tempFile = tempFile + "-temp.bin";
    ifstream inputFile(mainFile);
    ofstream outputTemp(tempFile, std::ios::out);
    outputTemp.write(header.c_str(), header.length());
    outputTemp.write("\n", 1);
    for (indexStruct is : index) {
        inputFile.seekg(is.offset);
        string record(recordLength, '\0');
        inputFile.read(&record[0], recordLength);
        outputTemp.write(record.c_str(), record.length());
    }
    inputFile.close();
    outputTemp.close();
    remove(mainFile.c_str());
    rename(tempFile.c_str(), mainFile.c_str());
}

/**
* ReindexFile
* Regenerates the indexes for the records after being deleted
*/

int FileOperations::ReindexFile(string header, string mainFile, string indexFile, int cases) {
    string tempFile = mainFile.substr(0, mainFile.length() - 4);
    tempFile = tempFile + "-temp.bin";
    ofstream outputTemp(tempFile, std::ios::out);
    vector<indexStruct> index = setIndexToMemory(mainFile, indexFile);
    string availListFile = GetFileName(mainFile, "", "AVAIL", 4 );
    int linesReclaimed = FileOperations::getAvailList(availListFile).getSize() - 1;
    /*ofstream outputFile(indexFile, std::ios::out); 
    int offset = header.length() + 1;*/
    string data;
    fileMetaData metaData = FileOperations::getMetaData(header);
    int recordLength = metaData.recordLength + 1;

    if (cases == 1) {
        for (indexStruct is : index) {
            data = is.key + "," + to_string(is.offset);
            outputTemp.write(data.c_str(), data.length());
            outputTemp.write("\n", 1);
        }
        outputTemp.close();
        remove(indexFile.c_str());
        rename(tempFile.c_str(), indexFile.c_str());
    } else if (cases == 2) {
        ofstream outputFile(indexFile, std::ios::out); 
        int offset = header.length() + 1;
        for (indexStruct is : index) {
            data = is.key + "," + to_string(offset);
            outputFile.write(data.c_str(), data.length());
            outputFile.write("\n", 1);
            offset = offset + recordLength;
        }
        outputTemp.close();
        outputFile.close();
        remove(tempFile.c_str());
        //rename(tempFile.c_str(), mainFile.c_str());
    }
    return linesReclaimed;
}