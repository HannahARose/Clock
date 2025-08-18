/**
 * @file hash.cpp
 * @brief Implementation of hash functions.
 */

/* Revision History
 * - 2025-08-18 Initial revision
 */
#include <Clock/hash/hash.hpp>

#include <array>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <ios>
#include <stdexcept>
#include <string>
#include <vector>

#include "base64.hpp"
#include <Clock/hash/xxh.h>

namespace clk::hash {
std::string hashFile(const std::string &file_path)
{
  std::ifstream file(file_path, std::ios::binary);
  if (!file) { throw std::runtime_error("Failed to open file"); }

  constexpr std::streamsize CHUNK_SIZE = 512;
  std::array<char, CHUNK_SIZE> buffer = {};

  XXHash64 myhash(0);
  while (!file.eof()) {
    file.read(buffer.data(), CHUNK_SIZE);
    myhash.add(buffer.data(), CHUNK_SIZE);
  }
  const uint64_t result = myhash.hash();
  std::vector<char> hash_buffer(sizeof(result));
  std::memcpy(hash_buffer.data(), &result, sizeof(result));
  const std::string hash_str(hash_buffer.begin(), hash_buffer.end());

  return base64::to_base64(hash_str);
}
}// namespace clk::hash