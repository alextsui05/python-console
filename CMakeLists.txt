cmake_minimum_required( VERSION 2.8 )
project( PythonInterpreter )

find_package( PythonLibs REQUIRED )
include_directories( ${PYTHON_INCLUDE_DIRS} )

add_executable( test test.cpp )

target_link_libraries( test ${PYTHON_LIBRARIES} )
