file(GLOB_RECURSE headers "*.h" "*.hpp")
file(GLOB_RECURSE sources "*.cpp")
file(GLOB_RECURSE inlines "*.ini")
source_group("" FILES ${headers} ${sources} ${inlines})

include_directories(.)

add_library(RhythmEngine STATIC
            ${headers}
            ${sources}
            ${inlines}
)

set_property(TARGET RhythmEngine PROPERTY FOLDER "Engine")