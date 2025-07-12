set( CMAKE_FOLDER "External/DevIL" )

cmake_minimum_required(VERSION 2.6)

project(ImageLib)

#External Libraries for DevIL.
#add_subdirectory( Libpng ) # Add PNG lib
#add_subdirectory( Libjpeg ) # Add JPG lib

add_subdirectory(src-IL)
add_subdirectory(src-ILU)
add_subdirectory(src-ILUT)

