// WEEK 6 – HUFFMAN COMPRESSION
// Extracted from the uploaded SmartFile source.
// compressFile depends on the earlier HuffmanNode/tree/code functions,
// BitWriter from Week 5, and the SmartFile header writer from Week 7.

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
