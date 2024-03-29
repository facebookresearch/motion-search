cmake_minimum_required(VERSION 3.1)
project (motion_search VERSION 0.0.1 DESCRIPTION "compute motion search for video complexity estimation")
set (CMAKE_CXX_STANDARD 17)
include(GNUInstallDirs)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)
include(ClangFormat)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_OSX_ARCHITECTURES "x86_64")

add_compile_options(-fms-extensions)

# Create library : libmotion_search_lib
add_library(motion_search_lib
    "motion_search/cpu.c"
    "motion_search/frame.c"
    "motion_search/memory.cpp"
    "motion_search/moments.disp.cpp"
    "motion_search/BaseVideoSequenceReader.cpp"
    "motion_search/Y4MSequenceReader.cpp"
    "motion_search/YUVSequenceReader.cpp"
    "motion_search/ComplexityAnalyzer.cpp"
    "motion_search/EOFException.cpp"
    "motion_search/MotionVectorField.cpp"
    "motion_search/YUVFrame.cpp"
    "motion_search/moments.c"
    "motion_search/motion_search.c"
)
target_clangformat_setup(motion_search_lib)

#
# Create library : libmotion_search_lib_sse2
#

set(SOURCES_SSE2
    "motion_search/asm/moments.x86.sse2.c")
set_source_files_properties(${SOURCES_SSE2}
    COMPILE_FLAGS "-msse2")
add_library(motion_search_lib_sse2 ${SOURCES_SSE2})
target_clangformat_setup(motion_search_lib_sse2)

#
# create binary
#

add_executable(motion_search
    "motion_search/main.cpp"
)
target_clangformat_setup(motion_search)


target_link_libraries (motion_search LINK_PUBLIC
    motion_search_lib
    motion_search_lib_sse2)

set_target_properties(motion_search_lib PROPERTIES PUBLIC_HEADER motion_search/motion_search.h)

target_include_directories(motion_search_lib PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/motion_search
)
target_include_directories(motion_search_lib_sse2 PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/motion_search
)
target_include_directories(motion_search PRIVATE
  ${CMAKE_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/motion_search
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -fpermissive -Wall -Wpedantic -mxsave")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions -fpermissive -Wall -Wpedantic -mxsave")

configure_file(motion_search.pc.in motion_search.pc @ONLY)

# build settings :  make release, make debug

ADD_CUSTOM_TARGET(debug
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Debug ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Creating the executable in the debug mode.")

ADD_CUSTOM_TARGET(release
  COMMAND ${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE=Release ${CMAKE_SOURCE_DIR}
  COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR} --target all
  COMMENT "Creating the executable in the release mode.")

install(TARGETS motion_search_lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(TARGETS motion_search_lib_sse2
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_BINARY_DIR}/motion_search.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
