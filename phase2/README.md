
# SmartFile – File Compression and Decompression

## Objective
To develop a C++ application for lossless file compression and decompression using Huffman Coding, reducing file size while preserving the original data.

## About the Project
SmartFile is a C++ based project that applies Data Structures and Algorithms (DSA) to file compression using Huffman Trees, Min-Heap, and bit-level processing.

## Features
- Huffman Coding for lossless compression.
- Min-Heap-based Priority Queue for Huffman Tree construction.
- Bit-level data encoding and decoding.
- Custom `.sfile` format for storing compressed data and metadata.
- RAW mode when compression is not beneficial.
- File-size and storage-saving statistics.

## Technologies Used
- C++
- Data Structures and Algorithms (DSA)
- Object-Oriented Programming (OOP)
- Binary File Handling

## How It Works
1. Reads the input file in binary mode.
2. Calculates the frequency of each byte.
3. Constructs a Huffman Tree using a Min-Heap.
4. Generates binary codes for each byte.
5. Encodes and stores data in the `.sfile` format.
6. Uses saved metadata to reconstruct the original file during decompression.

## Team BitMasters
- Rishi Negi 
- Vardan Bansal
- Priyansh Rautela
- Pragya Saksham

## Project Status
The project is under development. Huffman Tree construction, byte-frequency calculation, code generation, and initial bit-level writing have been implemented. Decompression, metadata handling, integration, and complete testing remain in progress.
