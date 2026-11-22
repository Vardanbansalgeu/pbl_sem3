# SmartFile

A lossless C++ command-line file compression and decompression tool built with static Huffman coding, bit-level stream I/O, a custom `.sfile` binary format, and a `RAW` fallback mode to prevent size expansion on small or high-entropy files.

## Features

- **Binary Frequency & Huffman Coding:** Counts byte frequencies across input files, constructs a min-heap Huffman tree, and generates prefix-free variable-length codes.
- **Bit-Level Stream Engine (`BitReader` / `BitWriter`):** Packs variable-length bit codes into byte streams for compression and unpacks bit streams during decompression.
- **Custom `.sfile` Format & `RAW` Fallback:** Stores compressed payloads with metadata in `.sfile`, falling back to `RAW` mode when compression would not save space.
- **Lossless Round-Trip Verification:** Restores original file bytes cleanly across compression and decompression workflows.

## Tech Stack

- **Language:** Standard C++ & STL (`priority_queue`, `vector`, file streams)
- **Interface:** Command-Line Interface (CLI)
- **Format:** Custom `.sfile` binary format with `RAW` fallback
