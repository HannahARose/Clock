#include <cstddef>
#include <cstdint>
#include <fmt/base.h>
#include <iterator>

[[nodiscard]] auto sumValues(const uint8_t *data, size_t size)
{
  constexpr auto SCALE = 1000;

  int value = 0;
  for (std::size_t offset = 0; offset < size; ++offset) {
    value +=
      static_cast<int>(*std::next(data, static_cast<long>(offset))) * SCALE;
  }
  return value;
}

// Fuzzer that attempts to invoke undefined behavior for signed integer overflow
// cppcheck-suppress unusedFunction symbolName=LLVMFuzzerTestOneInput
extern "C" int llvmFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
  fmt::print("Value sum: {}, len{}\n", sumValues(Data, Size), Size);
  return 0;
}
