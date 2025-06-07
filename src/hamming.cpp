#include "hamming.h"
#include "decoder.h"
#include "encoder.h"
#include <bitset>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <vector>

// --- Encode File Function ---
void encodeFileHamming3126(const std::string &filename) {
  // Input File
  std::ifstream infile(filename, std::ios::binary);
  if (!infile) {
    std::cerr << "Failed to open file.\n";
  }

  // Output file
  std::ofstream outfile(filename + ".hamming", std::ios::binary);
  if (!outfile) {
    std::cerr << "Failed to open output file.\n";
    return;
  }

  std::vector<bool> bit_buffer;
  unsigned char byte;

  while (infile.read(reinterpret_cast<char *>(&byte), 1)) {
    for (int i = 7; i >= 0; --i) {
      bit_buffer.push_back((byte >> i) & 1);
    }

    // Process full 26-bit chunks
    while (bit_buffer.size() >= CHUNK_SIZE) {
      std::bitset<CHUNK_SIZE> chunk;
      for (int j = 0; j < CHUNK_SIZE; ++j) {
        chunk[CHUNK_SIZE - 1 - j] = bit_buffer[j];
      }

      std::bitset<ENCODED_SIZE> encoded = encodeHamming3126(chunk);
      bit_buffer.erase(bit_buffer.begin(), bit_buffer.begin() + CHUNK_SIZE);

      // Escreve no arquivo como binário
      // uint32_t encoded_val = static_cast<uint32_t>(encoded.to_ulong());
      // outfile.write(reinterpret_cast<char *>(&encoded_val),
      // sizeof(uint32_t));

      // Escreve como txt
      outfile << encoded.to_string() << " ";
    }
  }

  // Handle remaining bits (pad to 26)
  int padding_count = 0;
  if (!bit_buffer.empty()) {
    std::bitset<CHUNK_SIZE> last_chunk;
    for (size_t j = 0; j < bit_buffer.size(); ++j) {
      last_chunk[CHUNK_SIZE - 1 - j] = bit_buffer[j];
    }
    padding_count = CHUNK_SIZE - bit_buffer.size(); // Number of zeros added

    std::bitset<ENCODED_SIZE> encoded = encodeHamming3126(last_chunk);
    // uint32_t encoded_val = static_cast<uint32_t>(encoded.to_ulong());

    outfile << encoded.to_string() << " ";
    // outfile.write(reinterpret_cast<char *>(&encoded_val), sizeof(uint32_t));
  }

  //   Append metadata chunk with padding count (5 bits stored in LSBs)
  std::bitset<CHUNK_SIZE> padding_chunk;
  std::bitset<5> padding_bits(padding_count);
  for (int i = 0; i < 5; ++i) {
    padding_chunk[i] = padding_bits[i]; // LSBs
  }

  std::bitset<ENCODED_SIZE> encoded_padding = encodeHamming3126(padding_chunk);
  //uint32_t encoded_val = static_cast<uint32_t>(encoded_padding.to_ulong());
  //outfile.write(reinterpret_cast<char *>(&encoded_val), sizeof(uint32_t));

  outfile << encoded_padding.to_string() << " ";

  outfile.close();
}

// ################################
// #           Decoded            #
// ################################

void decodeFileHamming3126(const std::string &filename) {

  // Input File (encoded as text)
  std::ifstream input(filename);
  if (!input) {
      std::cerr << "Error opening input file: " << filename << "\n";
      return;
  }

  std::string baseFilename = filename;
  const std::string suffix = ".hamming";
  if (baseFilename.size() >= suffix.size() &&
      baseFilename.compare(baseFilename.size() - suffix.size(), suffix.size(), suffix) == 0) {
      baseFilename.erase(baseFilename.size() - suffix.size());
  }

  // Output File (decoded binary)
  std::ofstream output(baseFilename + ".dec", std::ios::binary);
  if (!output) {
      std::cerr << "Error creating output file.\n";
      return;
  }

  std::vector<std::string> chunks;
  std::string chunk;

  // Read all bitset strings split by space
  while (input >> chunk) {
      chunks.push_back(chunk);
  }

  if (chunks.empty()) {
      std::cerr << "No data found in input file.\n";
      return;
  }

  // Last chunk is padding metadata
  std::bitset<ENCODED_SIZE> padding_encoded(chunks.back());
  std::bitset<CHUNK_SIZE> padding_decoded = decodeHamming3126(padding_encoded);
  uint32_t padding_size = padding_decoded.to_ulong();

  std::vector<bool> bit_buffer;

  for (size_t i = 0; i < chunks.size() - 1; ++i) {
      std::bitset<ENCODED_SIZE> encoded(chunks[i]);
      std::bitset<CHUNK_SIZE> decoded = decodeHamming3126(encoded);

      if (i < chunks.size() - 2) {
          // Full chunk
          for (int b = CHUNK_SIZE - 1; b >= 0; --b) {
              bit_buffer.push_back(decoded[b]);
          }
      } else {
          // Second to last chunk, apply padding
          for (int b = CHUNK_SIZE - 1; b >= static_cast<int>(padding_size); --b) {
              bit_buffer.push_back(decoded[b]);
          }
      }

      // Convert every 8 bits into a byte
      while (bit_buffer.size() >= 8) {
          unsigned char byte = 0;
          for (int b = 0; b < 8; ++b) {
              byte |= (bit_buffer[b] << (7 - b));
          }
          output.write(reinterpret_cast<char *>(&byte), 1);
          bit_buffer.erase(bit_buffer.begin(), bit_buffer.begin() + 8);
      }
  }

  output.close();
}