#include "decoder.h"
#include <bitset>

std::bitset<26> decodeHamming3126(std::bitset<31> data) {
  std::bitset<5> error_bit = std::bitset<5>("00000");
  std::bitset<26> decoded;

  for (int parity_bit = 1; parity_bit < 17; parity_bit *= 2) {
    // Parity count
    int parity_count = 0;

    for (int pos = 1; pos < 32; pos++) {

      if (pos == parity_bit) {
        // Parity bit
        continue;
      }

      // Bitwise op
      if (pos & parity_bit) {
        parity_count += data[pos - 1];
      }
    }

    // If this happens the error exists
    if ((data[parity_bit - 1] + parity_count) % 2 != 0) {
      // XOR keeps track of the bitset that is failing
      error_bit ^= std::bitset<5>(parity_bit);
    }
  }

  if (error_bit != std::bitset<5>("00000")) {
    // Found error
    int error_position = static_cast<int>(error_bit.to_ulong());
    // Flip the bit at the error position (1-based indexing)
    data[error_position - 1] = !data[error_position - 1];
  }

  // Builds the 26 bitset with the correct block
  int decoded_index = 0;
  for (int pos = 1; pos <= 31; ++pos) {
    if (pos == 1 || pos == 2 || pos == 4 || pos == 8 || pos == 16) {
      continue; // skip parity bits
    }
    decoded[decoded_index++] = data[pos - 1];
  }

  return decoded;
}