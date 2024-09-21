/*
    Josh Khovick Fermano
    2023
*/

#include <iostream>
#include <fstream>

using namespace std;

typedef struct Encoding {
    char symbol;
    string encodedSymbol;
} Encoding;

class Decompressor {
private:
    string compressedFileName;
    string decompressedFileName;
    Encoding encodings[256];

public:
    void loadEncodingDatabase() {
        ifstream encodingDatabase("Compressed.txt");

        if (!encodingDatabase.is_open()) {
            cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
            cout << "\t\t\t\t\t\t\t\tError opening encoding database." << endl;
            return;
        }
        int encodingCount;
        encodingDatabase >> encodingCount;

        for (int i = 0; i < encodingCount; ++i) {
            char symbol;
            string encodedSymbol;
            encodingDatabase >> symbol >> encodedSymbol;
            encodings[i] = {symbol, encodedSymbol};
        }
        encodingDatabase.close();
    }

    void expandCompressedData() {
        ifstream compressedFile("CompressedData.txt");
        ofstream tempFile("temp.txt");

        if (!compressedFile.is_open() || !tempFile.is_open()) {
            cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
            cout << "\t\t\t\t\t\t\t\tError opening the file." << endl;
            return;
        }
        string line;
        while (getline(compressedFile, line)) {
            string expandedLine = "";
            for (size_t i = 0; i < line.length(); i++) {
                if (line[i] >= '3' && line[i] <= '9') {
                    int repeatCount = line[i] - '0';
                    char nextChar = line[i + 1];
                    for (int j = 0; j < repeatCount; j++) {
                        expandedLine += nextChar;
                    }
                    i++;
                } else {
                    expandedLine += line[i];
                }
            }
            tempFile << expandedLine << endl;
        }
        compressedFile.close();
        tempFile.close();
    }

    void decodeData() {
        expandCompressedData();
        loadEncodingDatabase();

        ifstream tempFile("temp.txt");
        ofstream decompressedFile("decompressed.txt");

        if (!tempFile.is_open() || !decompressedFile.is_open()) {
            cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
            cout << "\t\t\t\t\t\t\t\tError opening input or output file." << endl;
            return;
        }
        string line;
        while (getline(tempFile, line)) {
            string decodedLine = "";
            string encodedWord = "";

            for (size_t i = 0; i < line.length(); ++i) {
                if (line[i] == ' ') {
                    string decodedWord = "";
                    string encodedSymbol = "";

                    for (size_t j = 0; j < encodedWord.length(); j++) {
                        encodedSymbol += encodedWord[j];
                        for (int k = 0; k < 256; ++k) {
                            if (encodedSymbol == encodings[k].encodedSymbol) {
                                decodedWord += encodings[k].symbol;
                                encodedSymbol = "";
                                break;
                            }
                        }
                    }
                    decodedLine += decodedWord + " ";
                    encodedWord = "";
                } else {
                    encodedWord += line[i];
                }
            }
            if (!encodedWord.empty()) {
                string decodedWord = "";
                string encodedSymbol = "";

                for (size_t j = 0; j < encodedWord.length(); j++) {
                    encodedSymbol += encodedWord[j];
                    for (int k = 0; k < 256; ++k) {
                        if (encodedSymbol == encodings[k].encodedSymbol) {
                            decodedWord += encodings[k].symbol;
                            encodedSymbol = "";
                            break;
                        }
                    }
                }
                decodedLine += decodedWord;
            }
            decompressedFile << decodedLine << endl;
        }
        tempFile.close();
        decompressedFile.close();
        if (remove("temp.txt") != 0) {
            cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
            cout << "\t\t\t\t\t\t\t\tError deleting temporary file." << endl;
        }
    }
};

int main() {
    system("color b1");
    Decompressor decompressor;
    string compressedFileName = "CompressedData";
    string decompressedFileName = "Decompressed.txt";
    string userResponse;

    ifstream checkFile(compressedFileName + ".txt");
    if (!checkFile.good()) {
        system("cls");
        cout << "\n";
        cout << "\t\t\t\t\t\t-----------------------------------------------------------------------------------------------" << endl;
        cout << "\t\t\t\t\t\t|                                                                                             |" << endl;
        cout << "\t\t\t\t\t\t|                            WELCOME TO FILE DECOMPRESSION MACHINE                            |" << endl;
        cout << "\t\t\t\t\t\t|                                                                                             |" << endl;
        cout << "\t\t\t\t\t\t-----------------------------------------------------------------------------------------------" << endl;
        cout << "\t\t\t\t\t\t|                                                                                             |" << endl;
        cout << "\t\t\t\t\t\t|    CREATED BY:                                                                              |" << endl;
        cout << "\t\t\t\t\t\t|    JOSH KHOVICK FERMANO                                                                     |" << endl;
        cout << "\t\t\t\t\t\t|                                                                                             |" << endl;
        cout << "\t\t\t\t\t\t|                                                                                             |" << endl;
        cout << "\t\t\t\t\t\t-----------------------------------------------------------------------------------------------" << endl;
        cout << "\n";
        cout << "\t\t\t\t\t\t\t\t\t\tCOMPRESSED FILE DOES NOT EXIST!" << endl;
        system("pause");
        return 1;
    }
    checkFile.close();

    system("cls");
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
    cout << "\t\t\t\t\t\t\t\tDo you want to rename the decompressed file? (Y/N): ";
    cin >> userResponse;
    if (userResponse == "Y" || userResponse == "y") {
        system("cls");
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
        cout << "\t\t\t\t\t\t\t\tEnter the new file name: ";
        cin >> decompressedFileName;
        if (decompressedFileName.size() >= 4 && decompressedFileName.substr(decompressedFileName.size() - 4) != ".txt") {
            decompressedFileName += ".txt";
        }
    }
    decompressor.decodeData();

    system("cls");
    cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
    cout << "\t\t\t\t\t\t\t\tDo you want to delete the compressed file? (Y/N): ";
    cin >> userResponse;
    if (userResponse == "Y" || userResponse == "y") {
        ifstream checkCompressedFile(compressedFileName + ".txt");
        if (checkCompressedFile.good()) {
            checkCompressedFile.close();
            if (remove((compressedFileName + ".txt").c_str()) != 0) {
                system("cls");
                cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
                cout << "\t\t\t\t\t\t\t\tError deleting file." << endl;
                system("pause");
                return 1;
            }
            system("cls");
            cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
            cout << "\t\t\t\t\t\t\t\tCompressed file deleted successfully." << endl;
            system("pause");
        } else {
            system("cls");
            cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
            cout << "\t\t\t\t\t\t\t\tCompressed file does not exist or has already been deleted." << endl;
            system("pause");
        }
    }
    if (decompressedFileName != "decompressed.txt") {
        if (rename("decompressed.txt", decompressedFileName.c_str()) != 0) {
            system("cls");
            cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
            cout << "\t\t\t\t\t\t\t\tError renaming the file." << endl;
            system("pause");
            return 1;
        }
        system("cls");
        cout << "\n\n\n\n\n\n\n\n\n\n\n\n";
        cout << "\t\t\t\t\t\t\t\tFile Decompressed successfully." << endl;
    }
    return 0;
}