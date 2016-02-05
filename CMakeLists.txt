project(STORMEXTRACT)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${STORMEXTRACT_BINARY_DIR}/bin")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${STORMEXTRACT_BINARY_DIR}/lib")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${STORMEXTRACT_BINARY_DIR}/bin")

set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")

if (NOT EXISTS "${STORMEXTRACT_SOURCE_DIR}/CascLib/CMakeLists.txt")
    message(FATAL_ERROR
"Missing dependency: CascLib
storm-extract requires the CascLib library.
It is provided as a GIT submodule of this repository.
Did you forgot to execute the following commands?
   git submodule init
   git submodule update")
endif()


add_subdirectory(CascLib)

include_directories("${STORMEXTRACT_SOURCE_DIR}/src/"
                    "${STORMEXTRACT_SOURCE_DIR}/CascLib/src/"
                    "${STORMEXTRACT_SOURCE_DIR}/include/"
)

add_executable(storm-extract src/storm-extract.cpp)
target_link_libraries(storm-extract casc)

# Set the RPATH
if (APPLE)
    set_target_properties(storm-extract PROPERTIES LINK_FLAGS "-Wl,-rpath,@loader_path/.")
elseif (UNIX)
    set_target_properties(storm-extract PROPERTIES INSTALL_RPATH ".")
endif()
