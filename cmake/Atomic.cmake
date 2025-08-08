# Handle atomic library linking more robustly
# First try using the standard CMake approach
find_package(Threads REQUIRED)

# Check if we need to link libatomic explicitly
include(CheckCXXSourceCompiles)
set(CMAKE_REQUIRED_QUIET TRUE)

# Test if atomic operations compile and link without libatomic
check_cxx_source_compiles(
  "
  #include <atomic>
  int main() {
    std::atomic<long long> x{0};
    return static_cast<int>(x.fetch_add(1));
  }"
  ATOMIC_WORKS_WITHOUT_LIB)

if(NOT ATOMIC_WORKS_WITHOUT_LIB)
  # Try with libatomic
  find_library(ATOMIC_LIB atomic)
  if(ATOMIC_LIB)
    message(STATUS "Linking atomic library: ${ATOMIC_LIB}")
    target_link_libraries(date_time_tests PRIVATE ${ATOMIC_LIB})
  else()
    # As a last resort, try linking with -latomic directly
    message(STATUS "Trying to link atomic library directly")
    target_link_libraries(date_time_tests PRIVATE atomic)
  endif()
else()
  message(STATUS "Atomic operations work without separate library")
endif()
