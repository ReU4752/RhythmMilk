# Target name
set(target Client)

# Get now architecture.
if(WIN32)
	if( CMAKE_SIZEOF_VOID_P EQUAL 4 )
        set(WIN_ARCHITECTURE x86)
	elseif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
	    set(WIN_ARCHITECTURE x64)
	endif()
endif()

# Includes
include(CheckIncludeFileCxx)
include_directories("${PROJECT_SOURCE_DIR}/RhythmEngine")

## Find and include the rendering files to use
if(WIN32)
	# Find the DirectX Library and link.

	# Can't use "$ENV{ProgramFiles(x86)}" to avoid violating CMP0053.  See
    # http://public.kitware.com/pipermail/cmake-developers/2014-October/023190.html
    set(PROGRAMFILES_x86 "ProgramFiles(x86)")
	if ("$ENV{${PROGRAMFILES_x86}}")
        set(PROGRAMFILES "$ENV{${PROGRAMFILES_x86}}")
    else()
        set(PROGRAMFILES "$ENV{ProgramFiles}")
    endif()

	find_path(DirectX_ROOT_DIR
		Include/d3d9.h
		PATHS
			"$ENV{DXSDK_DIR}"
            "${PROGRAMFILES}/Microsoft DirectX SDK (June 2010)"
            "${PROGRAMFILES}/Microsoft DirectX SDK (February 2010)"
            "${PROGRAMFILES}/Microsoft DirectX SDK (March 2009)"
            "${PROGRAMFILES}/Microsoft DirectX SDK (August 2008)"
            "${PROGRAMFILES}/Microsoft DirectX SDK (June 2008)"
            "${PROGRAMFILES}/Microsoft DirectX SDK (March 2008)"
            "${PROGRAMFILES}/Microsoft DirectX SDK (November 2007)"
            "${PROGRAMFILES}/Microsoft DirectX SDK (August 2007)"
            "${PROGRAMFILES}/Microsoft DirectX SDK"
		DOC "DirectX SDK root directory."
	)
	if(DirectX_ROOT_DIR)
		set(DirectX_INC_SEARCH_PATH "${DirectX_ROOT_DIR}/Include")
		set(DirectX_LIB_SEARCH_PATH "${DirectX_ROOT_DIR}/Lib/${WIN_ARCHITECTURE}")
		set(DirectX_BIN_SEARCH_PATH "${DirectX_ROOT_DIR}/Utilities/bin/${WIN_ARCHITECTURE}")
	endif()

	# With VS 2011 and Windows 8 SDK, the DirectX SDK is included as part of
    # the Windows SDK.
	#
    # See also:
    # - http://msdn.microsoft.com/en-us/library/windows/desktop/ee663275.aspx
	if(MSVC)
		set(USE_WINSDK_HEADERS TRUE)
	endif()

	# Find a header in the DirectX SDK
	macro(find_dxsdk_header var_name header)
        set(include_dir_var "DirectX_${var_name}_INCLUDE_DIR")
        set(include_found_var "DirectX_${var_name}_INCLUDE_FOUND")
        find_path(${include_dir_var} ${header}
            HINTS ${DirectX_INC_SEARCH_PATH}
            DOC "The directory where ${header} resides"
            CMAKE_FIND_ROOT_PATH_BOTH
        )
        if (${include_dir_var})
            set(${include_found_var} TRUE)
				find_package_message(${var_name}_INC "Found ${header} header: ${${include_dir_var}}/${header}" "[${${include_dir_var}}]")
        endif()
        mark_as_advanced(${include_found_var})
    endmacro()

	# Find a header in the Windows SDK
    macro (find_winsdk_header var_name header)
        if (USE_WINSDK_HEADERS)
            # Windows SDK
            set(include_dir_var "DirectX_${var_name}_INCLUDE_DIR")
            set(include_found_var "DirectX_${var_name}_INCLUDE_FOUND")
				CHECK_INCLUDE_FILE_CXX(${header} ${include_found_var})
            set(${include_dir_var})
            mark_as_advanced(${include_found_var})
        else()
            find_dxsdk_header(${var_name} ${header})
        endif()
    endmacro()

	find_winsdk_header(D3D d3d.h)

	if (MSVC)
        set(DirectX_D3D8_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/thirdparty/dxsdk_aug2007)
        set(DirectX_D3D8_INCLUDE_FOUND TRUE)
        mark_as_advanced(DirectX_D3D8_INCLUDE_FOUND)
    else ()
        find_dxsdk_header(D3D8 d3d8.h)
    endif ()

	find_winsdk_header(D3D9 d3d9.h)
    find_winsdk_header(D3D11_4 d3d11_4.h)
	find_winsdk_header(D2D1_1 d2d1_1.h)

	find_program (DirectX_FXC_EXECUTABLE fxc
        HINTS ${DirectX_BIN_SEARCH_PATH}
        DOC "Path to fxc.exe executable."
    )

	include_directories(${DirectX_INC_SEARCH_PATH})
else()
	# Find the DirectX Library and link.

	find_package(OpenGL REQUIRED)

	if(OPENGL_FOUND)
		message("OpenGL found.")
	else(OPENGL_FOUND)
		message("OpenGL not found.")
	endif()

	include_directories("${PROJECT_SOURCE_DIR}/Dependencies/glfw/include")
endif()

# Sources
message("Current sources directory : ${CMAKE_CURRENT_SOURCE_DIR}")

file(GLOB_RECURSE sources
	"${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

# Build executable
add_executable(${target}
	${sources})

# Project options
set_property(TARGET ${target}
	PROPERTY
	FOLDER
	"Executables"
)

# Link libraries
target_link_libraries(${target} 
	RhythmEngine
)

## Rendering library
if(WIN32)
	target_link_libraries(${target}
		${DirectX_LIB_SEARCH_PATH}
		d3d9.lib d3d11.lib d3dx11.lib
	)
else()
	target_link_libraries(${target}
		glfw
	)
endif()