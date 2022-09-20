# CMakeList.txt : CMake project for resurrection N, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Add source to this project's executable.
add_executable (target WIN32 
"FileProcessor.cpp" 
"FileProcessor.h"
"framework.h"
"NiDTextureExplorer.cpp"
"NiDTextureExplorer.h"
"NiDTextureExplorer.rc"
"resource.h"
"targetver.h")

add_definitions("-DUNICODE -D_UNICODE")


# TODO: Add tests and install targets if needed.
