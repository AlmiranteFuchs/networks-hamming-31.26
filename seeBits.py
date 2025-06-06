# ver_hamming_bits.py
import struct

with open("aaa.hamming", "rb") as f:
    i = 0
    while chunk := f.read(4):
        val = struct.unpack("<I", chunk)[0]  # Little-endian uint32_t
        bits = f"{val:032b}"[:-1]  # Remove bit extra se quiser só 31 bits
        print(f"Chunk {i:02d}: {bits}")
        i += 1
