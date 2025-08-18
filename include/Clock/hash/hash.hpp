/**
 * @file hash.hpp
 * @brief Header file for hash functions.
 */

/* Revision History
 * - 2025-08-18 Initial revision
 */

#ifndef CLOCK_HASH_HPP_
#define CLOCK_HASH_HPP_

#include <string>

namespace clk::hash {
/**
 * @brief Computes the hash of a file.
 * @param file_path The path to the file to be hashed.
 * @note This hash is not cryptographically secure and should not be used for
 * security purposes, only to detect typical edits to a file.
 */
std::string hashFile(const std::string &file_path);
}// namespace clk::hash

#endif// CLOCK_HASH_HPP_
