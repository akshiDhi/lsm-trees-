
// lsm_wasm.cpp
#include <string>
#include <fstream>
#include <cstring>
#include <iostream>
#include <emscripten/emscripten.h>

extern "C" {

std::string mem_keys[100];
std::string mem_vals[100];
int mem_size = 0;

std::string sstable_files[100];
int sstable_count = 0;

int memtable_threshold = 4;

int findInMemtable(const std::string &key) {
    for (int i = 0; i < mem_size; i++)
        if (mem_keys[i] == key)
            return i;
    return -1;
}

void WriteSSTable(const std::string &filename) {
    std::ofstream out(filename);
    for (int i = 0; i < mem_size; i++)
        out << mem_keys[i] << "=" << mem_vals[i] << "\n";
}

void Flush() {
    std::string filename = "sstable_" + std::to_string(sstable_count) + ".txt";
    WriteSSTable(filename);
    sstable_files[sstable_count++] = filename;
    mem_size = 0;
}

bool SearchSSTable(const std::string &filename, const std::string &key, std::string &value) {
    std::ifstream in(filename);
    std::string line;

    while (std::getline(in, line)) {
        int pos = line.find('=');
        if (pos == -1) continue;

        std::string k = line.substr(0, pos);
        std::string v = line.substr(pos + 1);

        if (k == key) {
            value = v;
            return true;
        }
    }
    return false;
}

void Compact() {
    std::string merged_file = "sstable_merged.txt";
    std::ofstream out(merged_file);

    std::string seen_keys[200];
    std::string seen_vals[200];
    int count = 0;

    for (int i = 0; i < sstable_count; i++) {
        std::ifstream in(sstable_files[i]);
        std::string line;

        while (std::getline(in, line)) {
            int pos = line.find('=');
            if (pos == -1) continue;

            std::string k = line.substr(0, pos);
            std::string v = line.substr(pos + 1);

            bool found = false;
            for (int j = 0; j < count; j++)
                if (seen_keys[j] == k) {
                    seen_vals[j] = v;
                    found = true;
                    break;
                }

            if (!found) {
                seen_keys[count] = k;
                seen_vals[count] = v;
                count++;
            }
        }
    }

    for (int i = 0; i < count; i++)
        out << seen_keys[i] << "=" << seen_vals[i] << "\n";

    sstable_files[0] = merged_file;
    sstable_count = 1;
}

EMSCRIPTEN_KEEPALIVE
void put(const char* key, const char* value) {
    int idx = findInMemtable(key);
    if (idx != -1) mem_vals[idx] = value;
    else {
        mem_keys[mem_size] = key;
        mem_vals[mem_size] = value;
        mem_size++;
    }
    if (mem_size >= memtable_threshold) Flush();
}

EMSCRIPTEN_KEEPALIVE
char* get(const char* key) {
    static std::string result;

    for (int i = 0; i < mem_size; i++)
        if (mem_keys[i] == key) {
            result = mem_vals[i];
            return (char*) result.c_str();
        }

    for (int i = sstable_count - 1; i >= 0; i--) {
        std::string value;
        if (SearchSSTable(sstable_files[i], key, value)) {
            result = value;
            return (char*) result.c_str();
        }
    }

    result = "NOTFOUND";
    return (char*) result.c_str();
}

EMSCRIPTEN_KEEPALIVE
void flush_table() { Flush(); }

EMSCRIPTEN_KEEPALIVE
void compact_table() { Compact(); }

}
