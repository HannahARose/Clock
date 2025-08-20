include(GenerateExportHeader)

add_library(csv_lib STATIC csv_file.cpp time_series.cpp time_format.cpp)

add_library(Clock::csv_lib ALIAS csv_lib)

target_link_libraries(csv_lib PRIVATE Clock_options Clock_warnings)

target_link_system_libraries(
  csv_lib
  PRIVATE
  Boost::json
  Boost::tokenizer
  Boost::algorithm
  Clock::hash)

target_link_libraries(csv_lib PUBLIC Clock::misc_lib)

target_include_directories(
  csv_lib PRIVATE "${CMAKE_BINARY_DIR}/configured_files/include")

target_include_directories(
  csv_lib ${WARNING_GUARD}
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(csv_lib PUBLIC cxx_std_20)

set_target_properties(
  csv_lib
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(csv_lib EXPORT_FILE_NAME
                       ${PROJECT_BINARY_DIR}/include/Clock/csv_lib_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(csv_lib PUBLIC CSV_LIB_STATIC_DEFINE)
endif()
