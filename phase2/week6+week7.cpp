#include <iostream>
#include <fstream>
#include <string>
#include <queue>
#include <vector>
#include <cstdint>
#include <limits>
using namespace std;
class HuffmanNode {
public:
    unsigned char data;
    unsigned long long frequency;
    HuffmanNode* left;
    HuffmanNode* right;
    unsigned long long order; // Stable tie-breaker for rebuilding the tree
    HuffmanNode(unsigned char ch, unsigned long long freq, unsigned long long sequence = 0) {
        data = ch;
        frequency = freq;
        order = sequence;
        left = nullptr;
        right = nullptr;
    }
};

class Compare {
public:
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        if (a->frequency != b->frequency)
            return a->frequency > b->frequency;
        return a->order > b->order;
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
    unsigned long long sequence = 0;
    for (int i = 0; i < 256; i++) {
        if (frequency[i] > 0) {
            minHeap.push(new HuffmanNode(
                static_cast<unsigned char>(i), frequency[i], sequence++));
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
            0, left->frequency + right->frequency, sequence++);
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

// Week 5: pack individual bits into bytes (most-significant bit first).
class BitWriter {
    ofstream& output;
    unsigned char buffer;
    int bitsUsed;
    bool ok;
public:
    explicit BitWriter(ofstream& out)
        : output(out), buffer(0), bitsUsed(0), ok(true) {}

    void writeBit(int bit) {
        buffer = static_cast<unsigned char>((buffer << 1) | (bit & 1));
        ++bitsUsed;
        if (bitsUsed == 8) {
            output.put(static_cast<char>(buffer));
            if (!output) ok = false;
            buffer = 0;
            bitsUsed = 0;
        }
    }

    void writeCode(const string& code) {
        for (size_t i = 0; i < code.size(); ++i)
            writeBit(code[i] == '1' ? 1 : 0);
    }

    // Zero-pad the final byte on the right if fewer than 8 bits remain.
    bool flush() {
        if (bitsUsed != 0) {
            buffer = static_cast<unsigned char>(buffer << (8 - bitsUsed));
            output.put(static_cast<char>(buffer));
            if (!output) ok = false;
            buffer = 0;
            bitsUsed = 0;
        }
        output.flush();
        return ok && static_cast<bool>(output);
    }
};

// Week 7: write an unsigned 64-bit integer in portable little-endian order.
void writeU64(ofstream& out, uint64_t value) {
    for (int i = 0; i < 8; ++i) {
        out.put(static_cast<char>(value & 0xFF));
        value >>= 8;
    }
}

// SmartFile v1 format:
// 6 bytes "SFILE1" | 1 byte mode (1 = HUFFMAN) | 8 bytes original size
// | 256 * 8 bytes frequency table | packed Huffman payload.
// Week 8's decoder must use the same deterministic tree tie-breaking rule.
bool writeSmartFileHeader(ofstream& out, uint64_t originalSize,
                          const unsigned long long frequency[256]) {
    out.write("SFILE1", 6);
    out.put(static_cast<char>(1)); // HUFFMAN mode; RAW mode comes in Week 9
    writeU64(out, originalSize);
    for (int i = 0; i < 256; ++i)
        writeU64(out, static_cast<uint64_t>(frequency[i]));
    return static_cast<bool>(out);
}

// Weeks 6-7: create a .sfile containing header + compressed bits.
bool compressFile(const string& inputName, const string& outputName) {
    ifstream input(inputName.c_str(), ios::binary);
    if (!input) {
        cout << "Cannot open input file!\n";
        return false;
    }

    unsigned long long frequency[256] = {0};
    uint64_t originalSize = 0;
    char ch;
    while (input.get(ch)) {
        ++frequency[static_cast<unsigned char>(ch)];
        ++originalSize;
    }
    if (!input.eof()) {
        cout << "Error reading input file!\n";
        return false;
    }

    HuffmanNode* root = buildHuffmanTree(frequency);
    string codes[256];
    generateCodes(root, "", codes);

    ofstream output(outputName.c_str(), ios::binary | ios::trunc);
    if (!output) {
        cout << "Cannot create output file!\n";
        deleteTree(root);
        return false;
    }
    bool ok = writeSmartFileHeader(output, originalSize, frequency);
    if (ok) {
        input.clear();
        input.seekg(0, ios::beg);
        if (!input) ok = false;
    }
    if (ok) {
        BitWriter writer(output);
        while (input.get(ch))
            writer.writeCode(codes[static_cast<unsigned char>(ch)]);
        if (!input.eof()) ok = false;
        if (!writer.flush()) ok = false;
    }
    output.close();
    if (output.fail()) ok = false;
    deleteTree(root);
    if (!ok) {
        cout << "Compression failed (read/write error).\n";
        return false;
    }

    ifstream compressed(outputName.c_str(), ios::binary | ios::ate);
    if (!compressed) {
        cout << "Could not read compressed file size.\n";
        return false;
    }
    streampos endPos = compressed.tellg();
    if (endPos == streampos(-1)) return false;
    uint64_t compressedSize = static_cast<uint64_t>(endPos);
    cout << "Compression completed!\n";
    cout << "Output: " << outputName << "\n";
    cout << "Original size: " << originalSize << " bytes\n";
    cout << "Compressed size (including header): " << compressedSize << " bytes\n";
    cout << "Mode: HUFFMAN\n";
    if (compressedSize >= originalSize)
        cout << "Note: Huffman output is not smaller for this file. RAW mode is planned for Week 9.\n";
    return true;
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
        cout << "7. Compress File (.sfile)\n";
        cout << "8. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;

        if (!cin) {
            if (cin.eof()) break;
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Please enter a number from 1 to 8.\n";
            continue;
        }

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

                    cout << "\nHUFFMAN CODES\n";
                    showCodes(frequency, codes);
                }

                deleteTree(root);
            }
        }

        else if (choice == 7) {
            cout << "Enter input file name: ";
            cin >> filename;
            if (!fileExists(filename)) {
                cout << "Input file does not exist!\n";
                continue;
            }
            cout << "Enter output .sfile name: ";
            cin >> outputFile;
            if (filename == outputFile) {
                cout << "Input and output names must differ!\n";
                continue;
            }
            if (fileExists(outputFile)) {
                cout << "Output exists. Overwrite? (y/n): ";
                cin >> ans;
                if (ans != 'y' && ans != 'Y') {
                    cout << "Compression cancelled.\n";
                    continue;
                }
            }
            compressFile(filename, outputFile);
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
