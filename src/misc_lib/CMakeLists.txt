include(GenerateExportHeader)

add_library(
  misc_lib SHARED
  # date_time.cpp
  time.cpp)

add_library(Clock::misc_lib ALIAS misc_lib)

target_link_libraries(misc_lib PRIVATE Clock_options Clock_warnings)

target_link_libraries(
  misc_lib PUBLIC Boost::date_time Boost::multiprecision #  Boost::json
                                                         #  Boost::uuid
)

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
  target_compile_definitions(misc_lib PUBLIC MISC_LIB_STATIC_DEFINE)
endif()

clock_enable_clang_tidy(misc_lib ${Clock_WARNINGS_AS_ERRORS})

# Print out every link option being used
get_target_property(misc_lib_LINK_OPTIONS misc_lib LINK_OPTIONS)
message(WARNING "Link options for misc_lib:")
foreach(OPTION IN LISTS misc_lib_LINK_OPTIONS)
  message(STATUS "Link option: ${OPTION}")
endforeach()
