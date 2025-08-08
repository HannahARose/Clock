cmake_minimum_required(VERSION 3.15...3.23)

project(CmakeConfigPackageTests LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

# ---- Test as standalone project the exported config package ----

if(PROJECT_IS_TOP_LEVEL OR TEST_INSTALLED_VERSION)
  enable_testing()

  find_package(Clock CONFIG REQUIRED) # for intro, project_options, ...

  if(NOT TARGET Clock_options)
    message(FATAL_ERROR "Required config package not found!")
    return() # be strictly paranoid for Template Janitor github action! CK
  endif()
endif()

# ---- Dependencies ----

include(${Catch2_SOURCE_DIR}/extras/Catch.cmake)

# ---- Subdirectories ----

add_subdirectory(misc_lib)
add_subdirectory(si3_sim)
