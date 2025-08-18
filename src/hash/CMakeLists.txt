include(GenerateExportHeader)

add_library(hash STATIC hash.cpp)

add_library(Clock::hash ALIAS hash)

target_link_libraries(hash PRIVATE Clock_options Clock_warnings)

target_link_system_libraries(hash PRIVATE base64)

target_include_directories(
  hash PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")

target_include_directories(
  hash ${WARNING_GUARD} PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
                               $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(hash PUBLIC cxx_std_20)

set_target_properties(
  hash
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(hash EXPORT_FILE_NAME
                       ${PROJECT_BINARY_DIR}/include/Clock/hash_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(hash PUBLIC HASH_STATIC_DEFINE)
endif()
