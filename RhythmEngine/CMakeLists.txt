file(GLOB TARGET_SOURCES
     "*.hpp"
     "*.cpp")

file(GLOB RENDERER_SOURCES
     "Renderer/*.hpp"
     "Renderer/*.cpp")

source_group("" FILES ${TARGET_SOURCES})
source_group("Renderer" FILES ${RENDERER_SOURCES})

include_directories(.)

add_library(RhythmEngine STATIC ${TARGET_SOURCES} ${RENDERER_SOURCES})

set_property(TARGET RhythmEngine PROPERTY FOLDER "Engine")