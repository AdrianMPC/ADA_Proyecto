cmake_minimum_required(VERSION 3.20)
# Specify the project name
project(ADA_proyecto)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories with header files
include_directories(${PROJECT_SOURCE_DIR})
include_directories(${PROJECT_SOURCE_DIR}/models)

# Specify the source files
set(SOURCES
    src/btree.cpp
    src/firstload.cpp
    src/cuckohashing.cpp 
    main.cpp
    # Este archivo contiene la función main()
)

# Specify the header files (optional, for better IDE integration)
set(HEADERS
   models/btree.h
   models/cuckohashing.h
   models/i-file-utils.h
   models/personamodelo.h
   models/dni-pos.h
)

# Set the executable name
set(EXECUTABLE_NAME "adaBD")

# Create the executable with the specified name
add_executable(${EXECUTABLE_NAME} ${SOURCES} ${HEADERS})
