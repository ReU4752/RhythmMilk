include_directories("${PROJECT_SOURCE_DIR}/RhythmEngine")

add_executable(Client main.cpp)

target_link_libraries(Client RhythmEngine)

set_property(TARGET Client PROPERTY FOLDER "Executables")