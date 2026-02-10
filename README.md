# Huffman / LZW Archiver — Command Line Compression Tool

## Overview

This project implements a command-line archiver that compresses multiple files and directories into a single archive using either the **Huffman** or **LZW** compression algorithm. The program supports full archive lifecycle operations including creation, extraction, integrity verification, listing, and incremental updates.

The focus of the implementation is **correctness**, **algorithmic compression**, and **efficient memory usage**, while handling archives that may be larger than available RAM.

---

## Features

### Archive Creation (`zip`)
- Create an archive from a list of files and/or directories
- Supports wildcard patterns:
  - `*` — match multiple characters
  - `?` — match a single character
- Directories are processed **recursively**
- Empty directories are preserved in the archive
- Supports Huffman or LZW compression

---

### Extraction (`unzip`)
- Extract one or all files from an archive
- Supports wildcard selection for extracted files
- User specifies destination directory
- Restores full directory structure
- Preserves empty directories

---

### Archive Info (`info`)
- Lists all archived entries
- Shows:
  - original size
  - compressed size
  - compression ratio

---

### Integrity Check (`check`)
- Verifies archive consistency
- Detects corruption or modification
- Ensures each archived file can be safely reconstructed

---

### Update (`update`)
- Replaces an archived file with a newer version
- Faster than rebuilding the archive from scratch
- Appends updated data and refreshes archive index

---

## Compression Algorithms

The archiver supports:

- **Huffman coding**
- **LZW compression**

---

## Memory & Performance Requirements

- Archives may be larger than available RAM
- The implementation must:
  - NOT load the entire archive into memory
  - NOT operate purely byte-by-byte from disk without caching
- Uses buffered/block processing and in-memory indexing for speed
- Emphasis on operational efficiency and scalability

---

## Architecture

//TODO

---

## Testing

//TODO
---

## Example Commands

