#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
using namespace std;
//  HUFFMAN NODE
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
//MIN HEAP COMPARATOR
class Compare {
public:
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};
// BIT WRITER 
class BitWriter {
private:
    ofstream& out;
    unsigned char buffer;
    int bitCount;
public:
    BitWriter(ofstream& file) : out(file) {
        buffer = 0;
        bitCount = 0;
    }
    void writeBit(int bit) {
        buffer = (buffer << 1) | (bit & 1);
        bitCount++;
        if (bitCount == 8) {
            out.put(static_cast<char>(buffer));
            buffer = 0;
            bitCount = 0;
        }
    }
    void writeCode(const string& code) {
        for (char bit : code) {
            writeBit(bit == '1' ? 1 : 0);
        }
    }
    void flush() {
        if (bitCount > 0) {
            buffer <<= (8 - bitCount);
            out.put(static_cast<char>(buffer));
            buffer = 0;
            bitCount = 0;
        }

        out.flush();
    }
};
// FILE CHECK
bool fileExists(string filename) {
    ifstream file(filename.c_str(), ios::binary);
    return file.good();
}
//  DELETE TREE
void deleteTree(HuffmanNode* root) {
    if (root == nullptr)
        return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}

//  BUILD HUFFMAN TREE 
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
        HuffmanNode* parent = new HuffmanNode(0, left->frequency + right->frequency);
        parent->left = left;
        parent->right = right;
        minHeap.push(parent);
    }
    return minHeap.top();
}
// DISPLAY HUFFMAN TREE
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

// GENERATE HUFFMAN CODES 
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

//  DISPLAY HUFFMAN CODES 
void showCodes(unsigned long long frequency[], string codes[]) {
    cout << "\n";
    cout << "Byte\t\tFrequency\tHuffman Code\n";
    cout << "\n";
    for (int i = 0; i < 256; i++) {
        if (frequency[i] > 0) {
            string name;
            if (i >= 32 && i <= 126)
                name = string("'") + static_cast<char>(i) + "'";
            else
                name = "Byte " + to_string(i);
            cout << name << "\t\t"<< frequency[i] << "\t\t"<< codes[i] << "\n";
        }
    }
}
int main() {
    int choice;
    string filename, outputFile, content;
    char ch, ans;
    do {
        cout << "\nSMARTFILE\n";
        cout << "1. Create New File\n";
        cout << "2. Read Existing File\n";
        cout << "3. Copy File\n";
        cout << "4. Calculate Byte Frequency\n";
        cout << "5. Build Huffman Tree\n";
        cout << "6. Generate Huffman Codes\n";
        cout << "7. Test BitWriter\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        // CREATE NEW FILE
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
        //  READ EXISTING FILE 
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
            cout << "\nFile Content\n";
            while (fin.get(ch))
                cout.put(ch);
            fin.close();
            cout << "\n\n";
        }
        // COPY FILE 
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
                cout << "\nBYTE FREQUENCIES\n";
                for (int i = 0; i < 256; i++) {
                    if (frequency[i] > 0) {
                        string name;
                        if (i >= 32 && i <= 126)
                            name = string("'") + static_cast<char>(i) + "'";
                        else
                            name = "Byte " + to_string(i);

                        cout << name << " : "
                             << frequency[i] << "\n";
                    }
                }
            }
            else {
                HuffmanNode* root = buildHuffmanTree(frequency);

                if (root == nullptr) {
                    cout << "File is empty. No tree or codes.\n";
                    continue;
                }

                if (choice == 5) {
                    cout << "\nHUFFMAN TREE\n";
                    displayTree(root);
                }
                else {
                    string codes[256];
                    generateCodes(root, "", codes);
                    cout << "\nHUFFMAN CODES\n";
                    showCodes(frequency, codes);
                }
                deleteTree(root);
            }
        }
        //TEST BIT WRITER
        else if (choice == 7) {
            cout << "Enter output file name: ";
            cin >> outputFile;
            if (fileExists(outputFile)) {
                cout << "File already exists. Overwrite? (y/n): ";
                cin >> ans;
                if (ans != 'y' && ans != 'Y') {
                    cout << "Operation cancelled.\n";
                    continue;
                }
            }
            ofstream fout(outputFile.c_str(), ios::binary);
            if (!fout) {
                cout << "Error creating output file!\n";
                continue;
            }
            string bits;
            cout << "Enter bits (example: 101101): ";
            cin >> bits;
            bool valid = !bits.empty();
            for (char bit : bits) {
                if (bit != '0' && bit != '1') {
                    valid = false;
                    break;
                }
            }
            if (!valid) {
                cout << "Enter only 0 and 1!\n";
                fout.close();
                continue;
            }
            BitWriter writer(fout);
            writer.writeCode(bits);
            writer.flush();
            fout.close();
            cout << "Bits packed into binary file successfully!\n";
            cout << "Bits entered: " << bits.length() << "\n";
            cout << "Bytes written: "
                 << (bits.length() + 7) / 8 << "\n";
        }
        else if (choice == 8) {
            cout << "Exiting SmartFile...\n";
        }
        else {
            cout << "Invalid choice!\n";
        }
    } while (choice != 8);
    return 0;
}
