// WEEK 5 – BIT-LEVEL I/O
// Extracted from the uploaded SmartFile source.
// This section defines BitWriter; it uses <fstream>, <string> and <cstddef>.

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
