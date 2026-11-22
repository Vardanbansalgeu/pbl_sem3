// WEEK 7 – .SFILE FORMAT / HEADER
// Extracted from the uploaded SmartFile source.
// This section defines the little-endian 64-bit writer and SmartFile header.

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
