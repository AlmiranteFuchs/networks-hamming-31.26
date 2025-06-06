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

      // Escreve no arquivo
      uint32_t encoded_val = static_cast<uint32_t>(encoded.to_ulong());
      outfile.write(reinterpret_cast<char *>(&encoded_val), sizeof(uint32_t));
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
    uint32_t encoded_val = static_cast<uint32_t>(encoded.to_ulong());
    outfile.write(reinterpret_cast<char *>(&encoded_val), sizeof(uint32_t));
  }

  std::cout << padding_count << "\n";

  //   Append metadata chunk with padding count (5 bits stored in LSBs)
  std::bitset<CHUNK_SIZE> padding_chunk;
  std::bitset<5> padding_bits(padding_count);
  for (int i = 0; i < 5; ++i) {
    padding_chunk[i] = padding_bits[i]; // LSBs
  }

  std::bitset<ENCODED_SIZE> encoded_padding = encodeHamming3126(padding_chunk);
  uint32_t encoded_val = static_cast<uint32_t>(encoded_padding.to_ulong());
  outfile.write(reinterpret_cast<char *>(&encoded_val), sizeof(uint32_t));

  outfile.close();

  // Debug
  //   std::ifstream check("aaa.hamming", std::ios::binary);
  //   uint32_t val;
  //   while (check.read(reinterpret_cast<char *>(&val), sizeof(uint32_t))) {
  //     std::bitset<31> bits(val);
  //     std::cout << bits << "\n";
  //   }
  //   check.close();
}

// ################################
// #           Decoded            #
// ################################

void decodeFileHamming3126(const std::string &filename) {

  // Input File (encoded)
  std::ifstream input(filename, std::ios::binary | std::ios::ate);
  if (!input) {
    std::cerr << "Error opening input file: " << filename << "\n";
    return;
  }

  // Output File (to decode)
  std::ofstream output(filename + ".decoded", std::ios::binary);
  if (!output) {
    std::cerr << "Error creating output file.\n";
    return;
  }

  // Get file size immediately
  std::streampos file_size = input.tellg();
  size_t total_uint32 = file_size / sizeof(uint32_t);

  // Now go to last uint32_t
  input.seekg(-static_cast<int>(sizeof(uint32_t)), std::ios::end);
  uint32_t encoded_val_padding;
  input.read(reinterpret_cast<char *>(&encoded_val_padding), sizeof(uint32_t));
  if (!input) {
    std::cerr << "Error reading padding.\n";
    return;
  }

  // Decode
  std::bitset<CHUNK_SIZE> decoded_padding =
      decodeHamming3126(encoded_val_padding);
  uint32_t padding_size = decoded_padding.to_ulong();

  // Reset to beginning if needed
  input.seekg(0, std::ios::beg);
  // Lets read the data normally till before the last one

  std::vector<bool> bit_buffer;
  size_t index = 0;
  uint32_t encoded_val;

  std::cout << total_uint32;

  // Read all encoded chunks except the last one (which is metadata)
  while (input.read(reinterpret_cast<char *>(&encoded_val), sizeof(uint32_t))) {
    std::bitset<ENCODED_SIZE> encoded(encoded_val);
    std::bitset<CHUNK_SIZE> decoded = decodeHamming3126(encoded);
    std::cout << decoded.to_string() << "\n";

    if (index < total_uint32 - 2) {
      // Add decoded bits to buffer
      for (int i = CHUNK_SIZE - 1; i >= 0; --i) {
        bit_buffer.push_back(decoded[i]);
      }
    } else if (index == total_uint32 - 2) {
      // Now we do everything again but we remove the calculated padding from
      // the bit_buffer
      for (int i = CHUNK_SIZE - 1; i >= static_cast<int>(padding_size); --i) {
        bit_buffer.push_back(decoded[i]);
      }
    }

    while (bit_buffer.size() >= 8) {
      unsigned char byte = 0;
      for (int i = 0; i < 8; ++i) {
        byte |= (bit_buffer[i] << (7 - i));
      }
      output.write(reinterpret_cast<char *>(&byte), 1);
      bit_buffer.erase(bit_buffer.begin(), bit_buffer.begin() + 8);
    }

    ++index;
  }

  // Debug
  //   std::ifstream check("aaa.hamming.decoded", std::ios::binary);
  //   if (!check) {
  //     std::cerr << "Cannot open decoded file for reading\n";
  //     return;
  //   }

  //   char c;
  //   while (check.get(c)) {
  //     std::bitset<8> bits(static_cast<unsigned char>(c));
  //     std::cout << bits << "\n";
  //   }
  //   check.close();
}