include(GenerateExportHeader)

add_library(misc_lib STATIC time.cpp)

add_library(Clock::misc_lib ALIAS misc_lib)

target_link_libraries(misc_lib PRIVATE Clock_options Clock_warnings)

target_link_libraries(misc_lib PUBLIC Boost::date_time)

target_include_directories(
  misc_lib ${WARNING_GUARD}
  PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
         $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/include>)

target_compile_features(misc_lib PUBLIC cxx_std_20)

set_target_properties(
  misc_lib
  PROPERTIES VERSION ${PROJECT_VERSION}
             CXX_VISIBILITY_PRESET hidden
             VISIBILITY_INLINES_HIDDEN YES)

generate_export_header(misc_lib EXPORT_FILE_NAME
                       ${PROJECT_BINARY_DIR}/include/Clock/misc_lib_export.hpp)

if(NOT BUILD_SHARED_LIBS)
  target_compile_definitions(misc_lib PUBLIC SAMPLE_LIBRARY_STATIC_DEFINE)
endif()
