cmake_minimum_required(VERSION 3.1)
project (motion_search VERSION 0.0.1 DESCRIPTION "compute motion search for video complexity estimation")
set (CMAKE_CXX_STANDARD 14)
include(GNUInstallDirs)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_OSX_ARCHITECTURES "x86_64")

add_compile_options(-fms-extensions)

# Create library : libmotion_search
add_library(motion_search
    "motion_search/BaseVideoSequenceReader.cpp"
#    "motion_search/BaseVideoSequenceReader.h"
    "motion_search/ComplexityAnalyzer.cpp"
#    "motion_search/ComplexityAnalyzer.h"
    "motion_search/EOFException.cpp"
#    "motion_search/EOFException.h"
#    "motion_search/IVideoSequenceReader.h"
    "motion_search/MotionVectorField.cpp"
#    "motion_search/MotionVectorField.h"
    "motion_search/YUVFrame.cpp"
#    "motion_search/YUVFrame.h"
    "motion_search/YUVSequenceReader.cpp"
#    "motion_search/YUVSequenceReader.h"
#    "motion_search/combine.bat"
#    "motion_search/common.h"
    "motion_search/frame.c"
#    "motion_search/frame.h"
    "motion_search/moments.c"
#    "motion_search/moments.h"
    "motion_search/motion_search.c"
#    "motion_search/motion_search.h"
)

add_executable(motion-search
    "motion_search/main.cpp"
)

target_link_libraries (motion-search LINK_PUBLIC motion_search)

set_target_properties(motion_search PROPERTIES PUBLIC_HEADER motion_search/motion_search.h)

target_include_directories(motion_search PRIVATE
  ${CMAKE_SOURCE_DIR}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions -fpermissive -Wall -Wpedantic -mssse3 -msse4.1 -mavx2 -mxsave")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fexceptions -fpermissive -Wall -Wpedantic -mssse3 -msse4.1 -mavx2 -mxsave")

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

install(TARGETS motion_search
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_BINARY_DIR}/motion_search.pc DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig)
