/*
   Josh Khovick Fermano
   2023
*/

#include <iostream>
#include <fstream>

using namespace std;

typedef struct TreeNode {
    char data;
    int frequency;
    TreeNode* left;
    TreeNode* right;
    TreeNode(char data, int frequency) : data(data), frequency(frequency), left(NULL), right(NULL) {}
} TreeNode;

typedef struct QueueNode {
    TreeNode* node;
    QueueNode* next;
    QueueNode(TreeNode* n) : node(n), next(NULL) {}
} QueueNode;

class FileCompressor {
private:
    QueueNode* queueHead;
    string inputFile;
    string outputFile;
    string huffmanCodes[256];

    void enqueueSorted(TreeNode* node) {
        QueueNode* newNode = new QueueNode(node);
        if (!queueHead || queueHead->node->frequency >= newNode->node->frequency) {
            newNode->next = queueHead;
            queueHead = newNode;
        } else {
            QueueNode* current = queueHead;
            while (current->next && current->next->node->frequency < newNode->node->frequency) {
                current = current->next;
            }
            newNode->next = current->next;
            current->next = newNode;
        }
    }

    TreeNode* dequeue() {
        if (!queueHead) {
            return NULL;
        }
        QueueNode* temp = queueHead;
        queueHead = queueHead->next;
        TreeNode* node = temp->node;
        delete temp;
        return node;
    }

    void generateTree(TreeNode* root, string code) {
        if (!root) {
            return;
        }
        if (root->data != '$') {
            huffmanCodes[root->data] = code;
        }
        generateTree(root->left, code + "0");
        generateTree(root->right, code + "1");
    }

    void buildTree(int frequencies[]) {
        for (int i = 0; i < 256; i++) {
            if (frequencies[i] > 0) {
                enqueueSorted(new TreeNode(char(i), frequencies[i]));
            }
        }
        while (queueHead->next) {
            TreeNode* left = dequeue();
            TreeNode* right = dequeue();
            TreeNode* newNode = new TreeNode('$', left->frequency + right->frequency);
            newNode->left = left;
            newNode->right = right;
            enqueueSorted(newNode);
        }
        generateTree(queueHead->node, "");
    }

    void encode() {
        ifstream inputFileStream(inputFile);
        ofstream outputFileStream(outputFile + ".txt");
        ofstream asciiFileStream("Compressed.txt");

        if (!inputFileStream.is_open() || !outputFileStream.is_open() || !asciiFileStream.is_open()) {
            cout << "\n\n\n";
            cout << "\t\t\t\t\t\t\t\t\t\tError opening files" << endl;
            return;
        }
        int frequencies[256] = {0};
        char currentChar;

        while (inputFileStream.get(currentChar)) {
            if (currentChar != ' ' && currentChar != '\n') {
                frequencies[int(currentChar)]++;
            }
        }
        frequencies[int(' ')] = 1;
        buildTree(frequencies);
        int numCodes = 0;

        for (int i = 0; i < 256; i++) {
            if (frequencies[i] > 0) {
                numCodes++;
            }
        }
        asciiFileStream << numCodes - 1 << '\n';
        for (int i = 0; i < 256; i++) {
            if (frequencies[i] > 0 && char(i) != ' ') {
                asciiFileStream << char(i) << ' ' << huffmanCodes[i] << '\n';
            }
        }
        inputFileStream.clear();
        inputFileStream.seekg(0);
        string line;
        while (getline(inputFileStream, line)) {
            string encodedText = "";
            for (char ch : line) {
                if (ch == ' ') {
                    encodedText += ' ';
                } else {
                    encodedText += huffmanCodes[int(ch)];
                }
            }
            outputFileStream << encodedText << '\n';
        }
        inputFileStream.close();
        outputFileStream.close();
        asciiFileStream.close();
    }

    void moreCompress() {
        ifstream inputFileStream(outputFile + ".txt");
        ofstream tempFileStream("temp.txt");

        if (!inputFileStream.is_open() || !tempFileStream.is_open()) {
            cout << "\n\n\n";
            cout << "\t\t\t\t\t\t\t\t\t\tError opening files" << endl;
            return;
        }
        string line;
        while (getline(inputFileStream, line)) {
            string updatedLine = "";
            int count = 1;
            char prevChar = line[0];

            for (int i = 1; i < line.length(); ++i) {
                if (line[i] == prevChar) {
                    count++;
                } else {
                    if (count >= 3) {
                        while (count > 9) {
                            updatedLine += "9" + string(1, prevChar);
                            count -= 9;
                        }
                        updatedLine += to_string(count) + prevChar;
                    } else {
                        while (count--) {
                            updatedLine += prevChar;
                        }
                    }
                    prevChar = line[i];
                    count = 1;
                }
            }
            if (count >= 3) {
                while (count > 9) {
                    updatedLine += "9" + string(1, prevChar);
                    count -= 9;
                }
                updatedLine += to_string(count) + prevChar;
            } else {
                while (count--) {
                    updatedLine += prevChar;
                }
            }
            tempFileStream << updatedLine << '\n';
        }
        inputFileStream.close();
        tempFileStream.close();
        cout << "\n\n\n";
        if (remove((outputFile + ".txt").c_str()) != 0) {
            cout << "\t\t\t\t\t\t\t\t\t\tError deleting original compressed file" << endl;
            return;
        }
        if (rename("temp.txt", (outputFile + ".txt").c_str()) != 0) {
            cout << "\n\n\n";
            cout << "\t\t\t\t\t\t\t\t\t\tError renaming temp file" << endl;
            return;
        }
    }

public:
    FileCompressor(string& inputFileName, string& outputFileName) : inputFile(inputFileName), outputFile(outputFileName), queueHead(NULL) {}
    
    int countLines() {
        int count = 0;
        string line;
        ifstream inputFileStream(inputFile);

        if (!inputFileStream.is_open()) {
            cout << "\n\n\n";
            cout << "\t\t\t\t\t\t\t\t\t\tError opening file to count lines" << endl;
            return -1;
        }
        while (getline(inputFileStream, line)) {
            count++;
        }
        inputFileStream.close();
        return count;
    }
    
    void compressFile() {
        int totalLines = countLines();
        if (totalLines <= 0) {
            cout << "\n";
            cout << "\t\t\t\t\t\t\t\tError counting lines in file" << endl;
            return;
        }
        encode();
        moreCompress();
        cout << "\n";
        cout << "\t\t\t\t\t\t\t\tTotal lines compressed from the file: " << totalLines << endl;
        cout << "\t\t\t\t\t\t\t\tFile compressed successfully" << endl;
    }

    void handleFiles() {
        ifstream inputFileStream(inputFile);
        if (!inputFileStream.good()) {
            cout << "\n";
            cout << "\t\t\t\t\t\t\t\tNo Text File Found, Creating Default File..." << endl;
            cout << "\t\t\t\t\t\t\t\tEnter text(s) to compress:\n\t\t\t\t\t\t\t\t";
            string userInput;
            getline(cin, userInput);

            ofstream newInputFileStream(inputFile);
            if (newInputFileStream.is_open()) {
                newInputFileStream << userInput;
                newInputFileStream.close();
            } else {
                cout << "\n";
                cout << "\t\t\t\t\t\t\t\tError creating default file" << endl;
                return;
            }
        }
        compressFile();
        inputFileStream.close();

        char deleteInput;
        cout << "\n";
        cout << "\t\t\t\t\t\t\t\tDo you want to delete the Default file? (Y/N): ";
        cin >> deleteInput;

        if (tolower(deleteInput) == 'y') {
            if (remove(inputFile.c_str()) != 0) {
                cout << "\n";
                cout << "\t\t\t\t\t\t\t\tError deleting input file" << endl;
                return;
            } else {
                cout << "\t\t\t\t\t\t\t\tDefault File deleted successfully" << endl;
            }
        } else {
            cout << "\n";
            cout << "\t\t\t\t\t\t\t\tDefault File not deleted" << endl;
        }
    }
};

int main() {
    system("color b1");
        cout << "\n";
        cout << "\t\t\t\t\t\t-----------------------------------------------------------------------------------------------" << endl;
        cout << "\t\t\t\t\t\t|                                                                                             |" << endl;
        cout << "\t\t\t\t\t\t|                            WELCOME TO FILE COMPRESSION MACHINE                              |" << endl;
        cout << "\t\t\t\t\t\t|                                                                                             |" << endl;
        cout << "\t\t\t\t\t\t-----------------------------------------------------------------------------------------------" << endl;
        cout << "\t\t\t\t\t\t|                                                                                             |" << endl;
        cout << "\t\t\t\t\t\t|    CREATED BY:                                                                              |" << endl;
        cout << "\t\t\t\t\t\t|    JOSH KHOVICK FERMANO                                                                     |" << endl;
        cout << "\t\t\t\t\t\t|                                                                                             |" << endl;
        cout << "\t\t\t\t\t\t|                                                                                             |" << endl;
        cout << "\t\t\t\t\t\t-----------------------------------------------------------------------------------------------" << endl;
    string inputFileName = "Default.txt";
    string outputFileName = "CompressedData";
    FileCompressor fileCompressor(inputFileName, outputFileName);
    fileCompressor.handleFiles();
    return 0;
}
