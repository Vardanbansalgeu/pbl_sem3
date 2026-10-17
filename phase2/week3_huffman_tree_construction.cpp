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
// Min-heap comparator
class Compare {
public:
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};
// Check whether a file exists
bool fileExists(string filename) {
    ifstream file(filename.c_str(), ios::binary);
    return file.good();
}
// Delete dynamically allocated tree nodes
void deleteTree(HuffmanNode* root) {
    if (root == nullptr)
        return;
    deleteTree(root->left);
    deleteTree(root->right);
    delete root;
}
// Display the Huffman Tree
void displayTree(HuffmanNode* root, string prefix = "" ,bool isLeft = true) {
    if (root == nullptr)
        return;
    cout << prefix;
    if (root->left == nullptr && root->right == nullptr) {
        if (root->data >= 32 && root->data <= 126)
            cout << "'" << static_cast<char>(root->data) << "'";
        else
            cout << "Byte " << static_cast<int>(root->data);
        cout << " (" << root->frequency << ")\n";
    }
    else {
        cout << "* (" << root->frequency << ")\n";
    }
    displayTree(root->left, prefix + "  ", true);
    displayTree(root->right, prefix + "  ", false);
}
// Build Huffman Tree
HuffmanNode* buildHuffmanTree(
    unsigned long long frequency[]) {
    priority_queue<HuffmanNode*,vector<HuffmanNode*>, Compare> minHeap;
    // Insert bytes having non-zero frequency
    for (int i = 0; i < 256; i++) {
        if (frequency[i] > 0) {
            minHeap.push(new HuffmanNode(static_cast<unsigned char>(i),frequency[i])
            );
        }
    }
    if (minHeap.empty())
        return nullptr;
    // Combine the two lowest-frequency nodes
    while (minHeap.size() > 1) {
        HuffmanNode* left = minHeap.top();
        minHeap.pop();
        HuffmanNode* right = minHeap.top();
        minHeap.pop();
        HuffmanNode* parent = new HuffmanNode(0, left->frequency + right->frequency
        );
        parent->left = left;
        parent->right = right;
        minHeap.push(parent);
    }
    return minHeap.top();
}
int main() {
    int choice;
    string filename, outputFile, content;
    char ch, ans;
    do {
        cout << "\tSMARTFILE\n";
        cout << "1.Create New File\n";
        cout << "2.Read Existing File\n";
        cout << "3.Copy File\n";
        cout << "4.Calculate Byte Frequency\n";
        cout << "5.Build Huffman Tree\n";
        cout << "6.Exit\n";
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
            cout << "Enter file name to read: ";
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
            cout << "\tFile Content\n";
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
            if(filename == outputFile) {
                cout << "File names must differ!\n";
                continue;
            }
            if(fileExists(outputFile)) {
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
        else if (choice == 4 || choice == 5) {
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
                cout << "\nByte Frequency Table:\n";

                for (int i = 0; i < 256; i++) {
                    if (frequency[i] > 0) {
                        cout << "Byte " << i
                             << " = " << frequency[i] << "\n";
                    }
                }
            }
            else {
                HuffmanNode* root =
                    buildHuffmanTree(frequency);

                if (root == nullptr) {
                    cout << "Empty file. No tree to build.\n";
                }
                else {
                    cout << "\nHuffman Tree "
                         << "(* represents internal node):\n";

                    displayTree(root);

                    cout << "\nHuffman Tree created successfully!\n";

                    deleteTree(root);
                }
            }
        }

        else if (choice == 6) {
            cout << "Exiting SmartFile...\n";
        }
        else {
            cout << "Invalid choice!\n";
        }
    } while (choice != 6);
    return 0;
}
