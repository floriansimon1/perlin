set(SFML_COMPONENTS system window graphics)
set(SFML_TARGETS sfml-system sfml-window sfml-graphics)

find_package(SFML REQUIRED ${SFML_COMPONENTS} CONFIG)

add_executable(perlin main.cpp)

target_link_libraries(perlin ${SFML_TARGETS})
