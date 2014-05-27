# Specify the version being used
cmake_minimum_required(VERSION 2.6)
# Project name
project(tree_gen)

# Library imports
find_package( OpenThreads )
find_package( osg )
find_package( osgDB )
find_package( osgGA )
find_package( osgFX )
find_package( osgParticle )
find_package( osgUtil )
find_package( osgViewer )

# Setting CFLAGS and LIBS
add_definitions(-std=c++11)
add_definitions(-g)
add_definitions(-lm)
add_definitions(-Wall)
# add_definitions(-Werror)
add_definitions(-rdynamic)
add_definitions(-O3)
# add_definitions(-Wno-unused-variable)

# Self-defined library imports
add_library(graphics_engine engines/graphics/graphics.cc)
add_library(stem classes/stem.cc)

macro( config_project PROJNAME LIBNAME )
  include_directories( ${${LIBNAME}_INCLUDE_DIR} )
  target_link_libraries( ${PROJNAME} ${${LIBNAME}_LIBRARY} )
endmacro()

# Link libraries to executable
add_executable(tree-gen main.cc)
target_link_libraries(tree-gen graphics_engine)
target_link_libraries(tree-gen stem)

config_project( tree-gen OPENTHREADS )
config_project( tree-gen OSG )
config_project( tree-gen OSGDB )
config_project( tree-gen OSGGA )
config_project( tree-gen OSGFX )
config_project( tree-gen OSGPARTICLE )
config_project( tree-gen OSGUTIL )
config_project( tree-gen OSGVIEWER )

install(TARGETS tree-gen DESTINATION bin)
