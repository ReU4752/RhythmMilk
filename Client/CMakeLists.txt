find_package(OpenGL REQUIRED)

if(OPENGL_FOUND)
  message("OpenGL found.")
else(OPENGL_FOUND)
  message("OpenGL not found.")
endif()

include_directories("${PROJECT_SOURCE_DIR}/RhythmEngine")
include_directories("${PROJECT_SOURCE_DIR}/Dependencies/glfw/include")
include_directories(${OPENGL_INCLUDE_DIR})

add_executable(Client main.cpp)

target_link_libraries(Client RhythmEngine)
target_link_libraries(Client ${OPENGL_gl_LIBRARY} glfw)
target_link_libraries(Client glfw)

set_property(TARGET Client PROPERTY FOLDER "Executables")