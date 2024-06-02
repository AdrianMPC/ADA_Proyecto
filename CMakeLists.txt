cmake_minimum_required(VERSION 3.10)

# Nombre del proyecto
project(ADA_proyecto)

# Especificar el estándar de C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Incluir directorios con archivos de cabecera
include_directories(${PROJECT_SOURCE_DIR})

# Especificar los archivos fuente
set(SOURCES
    btree.cpp
    cuckohashing.cpp
    firstload.cpp
    main.cpp # Este archivo contiene la función main()
)

# Especificar los archivos de cabecera (opcional)
set(HEADERS
    btree.h
    cuckohashing.h
    IFileUtils.h
    personamodelo.h
)

# Cambiamos el nombre del ejecutable a algo más cool luego, atte. adrian
set(EXECUTABLE_NAME "adaBD")

# Crear el ejecutable con el nombre especificado
add_executable(${EXECUTABLE_NAME} ${SOURCES} ${HEADERS})

# Configurar las propiedades del ejecutable
set_target_properties(${EXECUTABLE_NAME} PROPERTIES OUTPUT_NAME ${EXECUTABLE_NAME})
