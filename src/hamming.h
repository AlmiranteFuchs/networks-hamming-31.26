#pragma once

#include <bitset>
#include <string>
#include <vector>

// --- Constants ---
constexpr int CHUNK_SIZE = 26;
constexpr int ENCODED_SIZE = 31;
constexpr int SIZE_META_BITS = 32; // Metadata size

void encodeFileHamming3126(const std::string &filename);
void decodeFileHamming3126(const std::string &filename);