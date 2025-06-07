#include "src/hamming.h"
#include <iostream>
#include <string>

/*
 * #Author: Marcela Efi Fuchs S da Silva
 * #Date: 05/2025
 */

static void usage(char *progname) {
  std::cout << "Usage: " << progname << " [OPTION] <filename>\n"
            << "Hamming(31,26) encoder/decoder and corruption simulator.\n\n"
            << "Options:\n"
            << "  -e <filename>   Encode the input file using Hamming(31,26)\n"
            << "  -d <filename>   Decode a Hamming(31,26) encoded file\n"
            << "  -s <filename>   Simulate a 1-bit error in an encoded file\n"
            << "  -h              Show this help message and exit\n\n"
            << "Each option requires a filename as an argument.\n";
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    usage(argv[0]);
    return 1;
  }

  std::string option = argv[1];
  std::string filename = argv[2];

  switch (option[1]) {
  case 'h':
    usage(argv[0]);
    return 0;

  case 'e': {
    // std::cout << "Encoding file: " << filename << "\n";
    std::cout << "Encoding: '" << filename << "'\n";
    encodeFileHamming3126(filename);
    break;
  }
  case 'd':
    std::cout << "Decoding file: " << filename << "\n";
    decodeFileHamming3126(filename);
    break;

  case 's':
    std::cout << "Simulating corruption on file: NOT IMPLEMENTED" << filename << "\n";
    // simulateError(filename);
    break;

  default:
    std::cerr << "Error: Unknown option '" << option << "'\n\n";
    usage(argv[0]);
    return 1;
  }
  return 0;
}