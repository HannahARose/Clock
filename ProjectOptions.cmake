include(cmake/SystemLink.cmake)
include(cmake/LibFuzzer.cmake)
include(CMakeDependentOption)
include(CheckCXXCompilerFlag)

include(CheckCXXSourceCompiles)

macro(Clock_supports_sanitizers)
  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES
                                                   ".*GNU.*") AND NOT WIN32)

    message(
      STATUS
        "Sanity checking UndefinedBehaviorSanitizer, it should be supported on this platform"
    )
    set(TEST_PROGRAM "int main() { return 0; }")

    # Check if UndefinedBehaviorSanitizer works at link time
    set(CMAKE_REQUIRED_FLAGS "-fsanitize=undefined")
    set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=undefined")
    check_cxx_source_compiles("${TEST_PROGRAM}" HAS_UBSAN_LINK_SUPPORT)

    if(HAS_UBSAN_LINK_SUPPORT)
      message(
        STATUS
          "UndefinedBehaviorSanitizer is supported at both compile and link time."
      )
      set(SUPPORTS_UBSAN ON)
    else()
      message(
        WARNING "UndefinedBehaviorSanitizer is NOT supported at link time.")
      set(SUPPORTS_UBSAN OFF)
    endif()
  else()
    set(SUPPORTS_UBSAN OFF)
  endif()

  if((CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*" OR CMAKE_CXX_COMPILER_ID MATCHES
                                                   ".*GNU.*") AND WIN32)
    set(SUPPORTS_ASAN OFF)
  else()
    if(NOT WIN32)
      message(
        STATUS
          "Sanity checking AddressSanitizer, it should be supported on this platform"
      )
      set(TEST_PROGRAM "int main() { return 0; }")

      # Check if AddressSanitizer works at link time
      set(CMAKE_REQUIRED_FLAGS "-fsanitize=address")
      set(CMAKE_REQUIRED_LINK_OPTIONS "-fsanitize=address")
      check_cxx_source_compiles("${TEST_PROGRAM}" HAS_ASAN_LINK_SUPPORT)

      if(HAS_ASAN_LINK_SUPPORT)
        message(
          STATUS "AddressSanitizer is supported at both compile and link time.")
        set(SUPPORTS_ASAN ON)
      else()
        message(WARNING "AddressSanitizer is NOT supported at link time.")
        set(SUPPORTS_ASAN OFF)
      endif()
    else()
      set(SUPPORTS_ASAN ON)
    endif()
  endif()
endmacro()

macro(Clock_setup_options)
  option(Clock_ENABLE_HARDENING "Enable hardening" ON)
  option(Clock_ENABLE_COVERAGE "Enable coverage reporting" OFF)
  cmake_dependent_option(
    Clock_ENABLE_GLOBAL_HARDENING
    "Attempt to push hardening options to built dependencies"
    ON
    Clock_ENABLE_HARDENING
    OFF)

  clock_supports_sanitizers()

  if(NOT PROJECT_IS_TOP_LEVEL OR Clock_PACKAGING_MAINTAINER_MODE)
    option(Clock_ENABLE_IPO "Enable IPO/LTO" OFF)
    option(Clock_WARNINGS_AS_ERRORS "Treat Warnings As Errors" OFF)
    option(Clock_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(Clock_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer" OFF)
    option(Clock_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(Clock_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer" OFF)
    option(Clock_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(Clock_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(Clock_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(Clock_ENABLE_CLANG_TIDY "Enable clang-tidy" OFF)
    option(Clock_ENABLE_CPPCHECK "Enable cpp-check analysis" OFF)
    option(Clock_ENABLE_PCH "Enable precompiled headers" OFF)
    option(Clock_ENABLE_CACHE "Enable ccache" OFF)
  else()
    option(Clock_ENABLE_IPO "Enable IPO/LTO" ON)
    option(Clock_WARNINGS_AS_ERRORS "Treat Warnings As Errors" ON)
    option(Clock_ENABLE_USER_LINKER "Enable user-selected linker" OFF)
    option(Clock_ENABLE_SANITIZER_ADDRESS "Enable address sanitizer"
           ${SUPPORTS_ASAN})
    option(Clock_ENABLE_SANITIZER_LEAK "Enable leak sanitizer" OFF)
    option(Clock_ENABLE_SANITIZER_UNDEFINED "Enable undefined sanitizer"
           ${SUPPORTS_UBSAN})
    option(Clock_ENABLE_SANITIZER_THREAD "Enable thread sanitizer" OFF)
    option(Clock_ENABLE_SANITIZER_MEMORY "Enable memory sanitizer" OFF)
    option(Clock_ENABLE_UNITY_BUILD "Enable unity builds" OFF)
    option(Clock_ENABLE_CLANG_TIDY "Enable clang-tidy" ON)
    option(Clock_ENABLE_CPPCHECK "Enable cpp-check analysis" ON)
    option(Clock_ENABLE_PCH "Enable precompiled headers" OFF)
    option(Clock_ENABLE_CACHE "Enable ccache" ON)
  endif()

  if(NOT PROJECT_IS_TOP_LEVEL)
    mark_as_advanced(
      Clock_ENABLE_IPO
      Clock_WARNINGS_AS_ERRORS
      Clock_ENABLE_USER_LINKER
      Clock_ENABLE_SANITIZER_ADDRESS
      Clock_ENABLE_SANITIZER_LEAK
      Clock_ENABLE_SANITIZER_UNDEFINED
      Clock_ENABLE_SANITIZER_THREAD
      Clock_ENABLE_SANITIZER_MEMORY
      Clock_ENABLE_UNITY_BUILD
      Clock_ENABLE_CLANG_TIDY
      Clock_ENABLE_CPPCHECK
      Clock_ENABLE_COVERAGE
      Clock_ENABLE_PCH
      Clock_ENABLE_CACHE)
  endif()

  if(PROJECT_IS_TOP_LEVEL)
    set(Clock_ENABLE_Doxygen ON)
  endif()

  clock_check_libfuzzer_support(LIBFUZZER_SUPPORTED)
  if(LIBFUZZER_SUPPORTED
     AND (Clock_ENABLE_SANITIZER_ADDRESS
          OR Clock_ENABLE_SANITIZER_THREAD
          OR Clock_ENABLE_SANITIZER_UNDEFINED))
    set(DEFAULT_FUZZER ON)
  else()
    set(DEFAULT_FUZZER OFF)
  endif()

  option(Clock_BUILD_FUZZ_TESTS "Enable fuzz testing executable"
         ${DEFAULT_FUZZER})
endmacro()

macro(Clock_global_options)
  if(Clock_ENABLE_IPO)
    include(cmake/InterproceduralOptimization.cmake)
    clock_enable_ipo()
  endif()

  clock_supports_sanitizers()

  if(Clock_ENABLE_HARDENING AND Clock_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)

    if(NOT SUPPORTS_UBSAN
       OR Clock_ENABLE_SANITIZER_UNDEFINED
       OR Clock_ENABLE_SANITIZER_ADDRESS
       OR Clock_ENABLE_SANITIZER_THREAD
       OR Clock_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()

    message(
      "${Clock_ENABLE_HARDENING} ${ENABLE_UBSAN_MINIMAL_RUNTIME} ${Clock_ENABLE_SANITIZER_UNDEFINED}"
    )
    clock_enable_hardening(Clock_options ON ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()
endmacro()

macro(Clock_local_options)
  if(PROJECT_IS_TOP_LEVEL)
    include(cmake/StandardProjectSettings.cmake)
  endif()

  add_library(Clock_warnings INTERFACE)
  add_library(Clock_options INTERFACE)

  include(cmake/CompilerWarnings.cmake)
  clock_set_project_warnings(
    Clock_warnings
    ${Clock_WARNINGS_AS_ERRORS}
    ""
    ""
    ""
    "")

  if(Clock_ENABLE_USER_LINKER)
    include(cmake/Linker.cmake)
    clock_configure_linker(Clock_options)
  endif()

  include(cmake/Sanitizers.cmake)
  clock_enable_sanitizers(
    Clock_options
    ${Clock_ENABLE_SANITIZER_ADDRESS}
    ${Clock_ENABLE_SANITIZER_LEAK}
    ${Clock_ENABLE_SANITIZER_UNDEFINED}
    ${Clock_ENABLE_SANITIZER_THREAD}
    ${Clock_ENABLE_SANITIZER_MEMORY})

  set_target_properties(Clock_options PROPERTIES UNITY_BUILD
                                                 ${Clock_ENABLE_UNITY_BUILD})

  if(Clock_ENABLE_PCH)
    target_precompile_headers(
      Clock_options
      INTERFACE
      <vector>
      <string>
      <utility>)
  endif()

  if(Clock_ENABLE_CACHE)
    include(cmake/Cache.cmake)
    clock_enable_cache()
  endif()

  include(cmake/Doxygen.cmake)

  if(Clock_ENABLE_Doxygen)
    clock_enable_doxygen("awesome-sidebar")
  endif()

  include(cmake/StaticAnalyzers.cmake)

  if(Clock_ENABLE_CLANG_TIDY)
    clock_enable_clang_tidy(Clock_options ${Clock_WARNINGS_AS_ERRORS})
  endif()

  if(Clock_ENABLE_CPPCHECK)
    clock_enable_cppcheck(
      ${Clock_WARNINGS_AS_ERRORS} "" # override cppcheck options
    )
  endif()

  if(Clock_ENABLE_COVERAGE)
    include(cmake/Tests.cmake)
    clock_enable_coverage(Clock_options)
  endif()

  if(Clock_WARNINGS_AS_ERRORS)
    check_cxx_compiler_flag("-Wl,--fatal-warnings" LINKER_FATAL_WARNINGS)

    if(LINKER_FATAL_WARNINGS)
      # This is not working consistently, so disabling for now
      # target_link_options(Clock_options INTERFACE -Wl,--fatal-warnings)
    endif()
  endif()

  if(Clock_ENABLE_HARDENING AND NOT Clock_ENABLE_GLOBAL_HARDENING)
    include(cmake/Hardening.cmake)
    if(NOT SUPPORTS_UBSAN
       OR Clock_ENABLE_SANITIZER_UNDEFINED
       OR Clock_ENABLE_SANITIZER_ADDRESS
       OR Clock_ENABLE_SANITIZER_THREAD
       OR Clock_ENABLE_SANITIZER_LEAK)
      set(ENABLE_UBSAN_MINIMAL_RUNTIME FALSE)
    else()
      set(ENABLE_UBSAN_MINIMAL_RUNTIME TRUE)
    endif()
    clock_enable_hardening(Clock_options OFF ${ENABLE_UBSAN_MINIMAL_RUNTIME})
  endif()

endmacro()
