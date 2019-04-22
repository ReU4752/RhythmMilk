find_package(OpenGL REQUIRED)

if(OPENGL_FOUND)
  message("OpenGL found.")
else(OPENGL_FOUND)
  message("OpenGL not found.")
endif()

# Bass audio library
if( WIN32 )
    message("Window platform")
	set(BASS_INCLUDE ${PROJECT_SOURCE_DIR}/Dependencies/bass/Window/c)	
	if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
        set(BASS_LIB ${PROJECT_SOURCE_DIR}/Dependencies/bass/Window/c/bass.lib)
	elseif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	    set(BASS_LIB ${PROJECT_SOURCE_DIR}/Dependencies/bass/Window/c/x64/bass.lib)
	endif()
endif()
	

include_directories("${PROJECT_SOURCE_DIR}/RhythmEngine")
include_directories("${PROJECT_SOURCE_DIR}/Dependencies/glfw/include")
include_directories(${OPENGL_INCLUDE_DIR})
include_directories(${BASS_INCLUDE})

add_executable(Metronome WIN32 Main.cpp)

target_link_libraries(Metronome RhythmEngine)
target_link_libraries(Metronome ${OPENGL_gl_LIBRARY} glfw)
target_link_libraries(Metronome ${BASS_LIB})

set_property(TARGET Metronome PROPERTY FOLDER "Executables")