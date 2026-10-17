#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
using namespace std;

class HuffmanNode {
public:
    unsigned char data;
    unsigned long long frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(unsigned char ch, unsigned long long freq) {
        data = ch;
        frequency = freq;
        left = nullptr;
        right = nullptr;
    }
};

class Compare {
public:
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

bool fileExists(string filename) {
    ifstream file(filename.c_str(), ios::binary);
    return file.good();
}

void deleteTree(HuffmanNode* root) {
    if (root == nullptr)
        return;

    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

HuffmanNode* buildHuffmanTree(unsigned long long frequency[]) {
    priority_queue<HuffmanNode*,vector<HuffmanNode*>, Compare> minHeap;
    for (int i = 0; i < 256; i++) {
        if (frequency[i] > 0) {
            minHeap.push(new HuffmanNode(
                static_cast<unsigned char>(i), frequency[i]));
        }
    }
    if (minHeap.empty())
        return nullptr;
    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top();
        minHeap.pop();
        HuffmanNode* right = minHeap.top();
        minHeap.pop();
        HuffmanNode* parent = new HuffmanNode(
            0, left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        minHeap.push(parent);
    }
    return minHeap.top();
}
// Display tree with branches
void displayTree(HuffmanNode* root, string prefix = "",bool isLeft = true, bool isRoot = true) {
    if (root == nullptr)
        return;
    cout << prefix;
    if (!isRoot)
        cout << (isLeft ? "|-- " : "\\-- ");
    if (root->left == nullptr && root->right == nullptr) {
        if (root->data >= 32 && root->data <= 126)
            cout << "'" << static_cast<char>(root->data) << "'";
        else
            cout << "Byte " << static_cast<int>(root->data);

        cout << " [" << root->frequency << "]\n";
    }
    else {
        cout << "* [" << root->frequency << "]\n";
    }
    string nextPrefix = prefix;
    if (!isRoot)
        nextPrefix += (isLeft ? "|   " : "    ");
    displayTree(root->left, nextPrefix, true, false);
    displayTree(root->right, nextPrefix, false, false);
}
// Generate Huffman codes
void generateCodes(HuffmanNode* root, string code,string codes[]) {
    if (root == nullptr)
        return;
    if (root->left == nullptr && root->right == nullptr) {
        codes[root->data] = code.empty() ? "0" : code;
        return;
    }
    generateCodes(root->left, code + "0", codes);
    generateCodes(root->right, code + "1", codes);
}
void showCodes(unsigned long long frequency[],
               string codes[]) {
    cout << "\n\n";
    cout << "Byte \t\tFrequency\t\tHuffman Code|\n";
    cout << "\n";

    for (int i = 0; i < 256; i++) {
        if (frequency[i] > 0) {
            string name;
            if (i >= 32 && i <= 126)
                name = string("'") + static_cast<char>(i) + "'";
            else
                name = "Byte " + to_string(i);
            cout << " " << name;
            int padding = 15 - static_cast<int>(name.length());
            for (int j = 0; j < padding; j++)
                cout << " ";
            cout << " " << frequency[i];
            int freqPadding = 11 -static_cast<int>(to_string(frequency[i]).length());
            for (int j = 0; j < freqPadding; j++)
                cout << " ";
            cout << " " << codes[i] << "\n";
        }
    }

    cout << "\n";
}
int main() {
    int choice;
    string filename, outputFile, content;
    char ch, ans;
    do {
        cout << "\tSMARTFILE\n";
        cout << "1. Create New File\n";
        cout << "2. Read Existing File\n";
        cout << "3. Copy File\n";
        cout << "4. Calculate Byte Frequency\n";
        cout << "5. Build Huffman Tree\n";
        cout << "6. Generate Huffman Codes\n";
        cout << "7. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (choice == 1) {
            cout << "Enter new file name: ";
            cin >> filename;

            if (fileExists(filename)) {
                cout << "File already exists!\n";
                continue;
            }

            ofstream fout(filename.c_str(), ios::binary);

            if (!fout) {
                cout << "Error creating file!\n";
                continue;
            }

            cout << "Enter file content: ";
            cin >> ws;
            getline(cin, content);

            fout << content;
            fout.close();

            cout << "File created successfully!\n";
        }

        else if (choice == 2) {
            cout << "Enter file name: ";
            cin >> filename;

            if (!fileExists(filename)) {
                cout << "File does not exist!\n";
                continue;
            }

            ifstream fin(filename.c_str(), ios::binary);

            if (!fin) {
                cout << "Error opening file!\n";
                continue;
            }

            cout << "\n File Content\n";
            while (fin.get(ch))
                cout.put(ch);

            fin.close();
            cout << "\n\n";
        }
        else if (choice == 3) {
            cout << "Enter source file name: ";
            cin >> filename;

            if (!fileExists(filename)) {
                cout << "Source file does not exist!\n";
                continue;
            }

            cout << "Enter destination file name: ";
            cin >> outputFile;

            if (filename == outputFile) {
                cout << "File names must differ!\n";
                continue;
            }

            if (fileExists(outputFile)) {
                cout << "Destination exists. Overwrite? (y/n): ";
                cin >> ans;

                if (ans != 'y' && ans != 'Y') {
                    cout << "Copy cancelled.\n";
                    continue;
                }
            }

            ifstream fin(filename.c_str(), ios::binary);
            ofstream fout(outputFile.c_str(), ios::binary);

            if (!fin || !fout) {
                cout << "Error opening files!\n";
                continue;
            }

            while (fin.get(ch))
                fout.put(ch);

            fin.close();
            fout.close();

            cout << "File copied successfully!\n";
        }

        else if (choice >= 4 && choice <= 6) {
            cout << "Enter file name: ";
            cin >> filename;

            if (!fileExists(filename)) {
                cout << "File does not exist!\n";
                continue;
            }

            unsigned long long frequency[256] = {0};
            unsigned long long fileSize = 0;

            ifstream fin(filename.c_str(), ios::binary);

            if (!fin) {
                cout << "Error opening file!\n";
                continue;
            }

            while (fin.get(ch)) {
                unsigned char byte =
                    static_cast<unsigned char>(ch);

                frequency[byte]++;
                fileSize++;
            }

            fin.close();

            cout << "\nOriginal File Size: "
                 << fileSize << " bytes\n";

            if (choice == 4) {
                cout << "\nByte Frequencies:\n";

                for (int i = 0; i < 256; i++) {
                    if (frequency[i] > 0)
                        cout << "Byte " << i << " : "
                             << frequency[i] << "\n";
                }
            }
            else {
                HuffmanNode* root = buildHuffmanTree(frequency);

                if (root == nullptr) {
                    cout << "File is empty. No tree or codes.\n";
                    continue;
                }

                if (choice == 5) {
                    cout << "\n----- HUFFMAN TREE -----\n";
                    displayTree(root);
                }
                else {
                    string codes[256];

                    generateCodes(root, "", codes);

                    cout << "\n----- HUFFMAN CODES -----\n";
                    showCodes(frequency, codes);
                }

                deleteTree(root);
            }
        }

        else if (choice == 7) {
            cout << "Exiting SmartFile...\n";
        }

        else {
            cout << "Invalid choice!\n";
        }

    } while (choice != 7);

    return 0;
}