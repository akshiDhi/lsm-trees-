# LSM Tree Storage Engine

A simplified implementation of a Log-Structured Merge Tree (LSM Tree) designed for high-performance write-heavy workloads.


- In-memory MemTable
- Immutable SSTables on disk
- Compaction mechanism
- Efficient write path
- Basic read/write API


Modern databases like LevelDB and RocksDB use LSM Trees to handle large-scale data efficiently. This project demonstrates how LSM Trees work internally.


- Language: C++ / Python
- Storage: File-based persistence

## Architecture

### 1. MemTable
- In-memory data structure (e.g., skip list / map)
- Handles fast writes

### 2. SSTables
- Immutable sorted files on disk
- Enable efficient sequential reads

### 3. Compaction
- Merges SSTables
- Removes duplicates and stale data

## Workflow

1. Write → MemTable
2. Flush → SSTable
3. Background compaction
4. Read → Check MemTable + SSTables

##  Advantages

- High write throughput
- Sequential disk access
- Scalable storage design

##  Future Improvements

- Bloom filters for faster reads
- Multi-level compaction strategy
- Concurrency support
