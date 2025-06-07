#include "encoder.h"
#include <bitset>

std::bitset<31> encodeHamming3126(const std::bitset<26> &data) {
  std::bitset<31> encoded;

  // Fill data into 31 skipping parity positions
  int data_it = 0;
  for (int i = 1; i < 32; i++) {
    if (i == 1 || i == 2 || i == 4 || i == 8 || i == 16) {
      continue;
    }

    encoded[i - 1] = data[data_it++];
  }

  // Count for parity bit
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
        parity_count += encoded[pos - 1];
      }
    }

    encoded[parity_bit - 1] = (parity_count % 2 != 0);
  }

  return encoded;
}