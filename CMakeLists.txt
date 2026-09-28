cmake_minimum_required(VERSION 3.20)

project(PA3_Computacion_Grafica LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# FreeGLUT viene incluido dentro del proyecto para evitar descargas con FetchContent.
set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(FREEGLUT_BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)

add_subdirectory(third_party/freeglut EXCLUDE_FROM_ALL)

add_executable(PA3_Computacion_Grafica
        main.cpp
)

target_include_directories(PA3_Computacion_Grafica
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party/freeglut/include
)

target_link_libraries(PA3_Computacion_Grafica
        PRIVATE
        freeglut_static
        opengl32
        glu32
        winmm
        gdi32
)
