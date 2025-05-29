#!/bin/sh
cmake -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=cmake/ToolchainLinuxGCC9.cmake
cmake --build build