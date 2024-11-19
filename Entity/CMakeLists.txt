cmake_minimum_required(VERSION 3.10)
project(Entity)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


if(MSVC)
    set(SFML_DIR "C:/Program Files/SFML/lib/cmake/SFML")
    set(SFML_STATIC_LIBRARIES TRUE)
    add_compile_options(
    $<$<CONFIG:>:/MT> #---------|
    $<$<CONFIG:Debug>:/MTd> #---|--  Statically link the runtime libraries
    $<$<CONFIG:Release>:/MT> #--|
    )
endif()

add_executable (Feucircu "Feucircu.cpp")
add_executable (Voiture "Voiture.cpp")
target_link_libraries(Feucircu sfml-graphics sfml-window)
target_link_libraries(Voiture sfml-graphics sfml-window)

find_package (Threads)
find_package(SFML 2.5 COMPONENTS window system graphics REQUIRED)

link_libraries (Threads::Threads)
