#include <iostream>
#include <fstream>
#include <string>
using namespace std;
bool fileExists(string filename) {
    ifstream file(filename.c_str(), ios::binary);
    return file.good();
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
        cout << "5. Exit\n";
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
            while (fin.get(ch)) {
                cout.put(ch);
            }
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
                cout << "Source and destination must differ!\n";
                continue;
            }
            if (fileExists(outputFile)) {
                cout << "Destination file exists. Overwrite? (y/n): ";
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
            while (fin.get(ch)) {
                fout.put(ch);
            }
            fin.close();
            fout.close();
            cout << "File copied successfully!\n";
        }
        else if (choice == 4) {
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
            cout << "\nOriginal File Size: "<< fileSize << " bytes\n";
            cout << "\nByte Frequency Table:\n";
            cout << "Byte\tFrequency\n";
            for (int i = 0; i < 256; i++) {
                if (frequency[i] > 0) {
                    if (i >= 32 && i <= 126) {
                        cout << "'" << static_cast<char>(i)
                             << "'";
                    }
                    else {
                        cout << i;
                    }
                    cout << "\t" << frequency[i] << "\n";
                }
            }
            cout << "\nFrequency calculation completed.\n";
        }
        else if (choice == 5) {
            cout << "Exiting SmartFile...\n";
        }
        else {
            cout << "Invalid choice!\n";
        }
    } while (choice != 5);
    return 0;
}
