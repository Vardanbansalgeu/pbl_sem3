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
    string filename, outputFile;
    string content;
    char ch;
    char ans;
    do {
        cout << "\t SMARTFILE\n";
        cout << "1. Create New File\n";
        cout << "2. Read Existing File\n";
        cout << "3. Copy File\n";
        cout << "4. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        if (choice == 1) {
            cout << "Enter new file name: ";
            cin >> filename;
            if (fileExists(filename)) {
                cout << "File already exists!\n";
                cout << "Choose another name.\n";
            }
            else {
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
                cout << "Content saved successfully!\n";
            }
        }
        else if (choice == 2) {
            cout << "Enter file name to read: ";
            cin >> filename;
            if (!fileExists(filename)) {
                cout << "File does not exist!\n";
            }
            else {
                ifstream fin(filename.c_str(), ios::binary);
                if (!fin) {
                    cout << "Error opening file!\n";
                    continue;
                }
                cout << "\tFile Content \n";
                while (fin.get(ch)) {
                    cout.put(ch);
                }
                fin.close();
                cout << "\n\n";
            }
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
                cout << "Destination file already exists!\n";
                cout << "Overwrite it? (y/n): ";
                cin >> ans;
                if (ans != 'y' && ans != 'Y') {
                    cout << "Copy cancelled.\n";
                    continue;
                }
            }
            ifstream fin(filename.c_str(), ios::binary);
            if (!fin) {
                cout << "Error opening source file!\n";
                continue;
            }
            ofstream fout(outputFile.c_str(), ios::binary);
            if (!fout) {
                cout << "Error opening destination file!\n";
                fin.close();
                continue;
            }
            while (fin.get(ch)) {
                fout.put(ch);
            }
            //.
            fin.close();
            fout.close();
            cout << "File copied successfully!\n";
        }
        else if (choice == 4) {
            cout << "Exiting SmartFile...\n";
        }
        else {
            cout << "Invalid choice! Try again.\n";
        }
    } while (choice != 4);
    return 0;
}
