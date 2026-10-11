cmake_minimum_required(VERSION 3.14)

project (OpenVGExamples VERSION 1.0.0)

# Build policy + static analysis. Registered ONLY when this is the top-level
# project: the libraries are vendored through FetchContent, and a second
# `cppcheck` custom target would make CMake fail that build outright. None of
# this is required to build the examples - with the tools absent and every
# option at its default the build is unchanged.
if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
	list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake)
	include(cppcheck)
	include(warnings)
endif()

include(FetchContent)

# macOS is not supported: the EGL stack has no macOS backend.
if(APPLE)
	message(STATUS "OpenVG: macOS is not supported (no EGL backend) - nothing to build.")
	return()
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/VG_Binaries)
set(LIBRARY_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/lib)

# Force single output directory (no Debug/Release subdirectories)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/VG_Binaries)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

foreach(OUTPUTCONFIG ${CMAKE_CONFIGURATION_TYPES})
    string(TOUPPER ${OUTPUTCONFIG} OUTPUTCONFIG)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/VG_Binaries)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${OUTPUTCONFIG} ${CMAKE_SOURCE_DIR}/lib)
endforeach()

# OpenVG provider - whichever pair the platform provides:
#   * the platform's own EGL + OpenVG (for example embedded GPUs such as the
#     Vivante GPU of the STM32MP1), or
#   * McNopper/EGL together with OpenVG_Library, the software implementation
#     used on desktop Windows and Linux.
set(VG_LIBRARIES "")
if(WIN32 OR UNIX)
	find_library(OPENVG_LIBRARY NAMES OpenVG)
	find_library(EGL_LIBRARY NAMES EGL)
	find_path(VG_INCLUDE_ROOT VG/openvg.h)
	find_path(EGL_INCLUDE_ROOT EGL/egl.h)
endif()

if(OPENVG_LIBRARY AND EGL_LIBRARY AND VG_INCLUDE_ROOT AND EGL_INCLUDE_ROOT)
	message(STATUS "OpenVG: using the platform EGL/OpenVG pair")
	set(VG_PROVIDER "SYSTEM")
	set(VG_LIBRARIES ${OPENVG_LIBRARY} ${EGL_LIBRARY})
	set(VG_INCLUDE_DIRS ${VG_INCLUDE_ROOT} ${EGL_INCLUDE_ROOT})
else()
	message(STATUS "OpenVG: no platform EGL/OpenVG pair - using McNopper/EGL and OpenVG_Library")
	set(VG_PROVIDER "LIBRARY")

	# OpenVG_Library: prefer the sibling local checkout (multi-repo development
	# layout), and fall back to the published release when it is not present.
	# It brings McNopper/EGL with it.
	if(EXISTS "${CMAKE_SOURCE_DIR}/../OpenVG_Library/CMakeLists.txt")
		FetchContent_Declare(
			openvg_library
			SOURCE_DIR ${CMAKE_SOURCE_DIR}/../OpenVG_Library
		)
	else()
		FetchContent_Declare(
			openvg_library
			GIT_REPOSITORY https://github.com/McNopper/OpenVG_Library.git
			GIT_TAG        v1.0.0
		)
	endif()
	FetchContent_MakeAvailable(openvg_library)

	set(VG_LIBRARIES OpenVG)
	set(VG_INCLUDE_DIRS "")
endif()

# GLUS in the OpenVG flavor: the EGL window and the *_vg.c helpers.
set(OpenGL "VG" CACHE STRING "GLUS flavor" FORCE)

# GLUS: prefer the sibling local checkout (multi-repo development layout), and
# fall back to the published release when it is not present.
if(EXISTS "${CMAKE_SOURCE_DIR}/../GLUS/CMakeLists.txt")
    FetchContent_Declare(
        glus
        SOURCE_DIR ${CMAKE_SOURCE_DIR}/../GLUS
    )
else()
    FetchContent_Declare(
        glus
        GIT_REPOSITORY https://github.com/McNopper/GLUS.git
        GIT_TAG        v1.1.6
    )
endif()
FetchContent_MakeAvailable(glus)

# GLUS compiles its EGL window against the VG/EGL headers and resolves the
# entry points through the same libraries as the examples. The platform
# libraries resolve the native window and timer symbols inside GLUS itself,
# so they are PUBLIC dependencies too: the linker needs them after the static
# GLUS library, which is exactly how CMake orders them.
IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(GLUS_PLATFORM_LIBRARIES X11 Xrandr dl pthread m)
ELSEIF(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(GLUS_PLATFORM_LIBRARIES winmm)
ENDIF()

if(VG_INCLUDE_DIRS)
    target_include_directories(GLUS PUBLIC ${VG_INCLUDE_DIRS})
endif()
target_link_libraries(GLUS PUBLIC ${VG_LIBRARIES} ${GLUS_PLATFORM_LIBRARIES})

file(GLOB EXAMPLES_LIST LIST_DIRECTORIES true RELATIVE ${CMAKE_SOURCE_DIR} "Example*")
foreach (EXAMPLE ${EXAMPLES_LIST})
	message(STATUS "Add Subdirectory: " ${EXAMPLE})
	add_subdirectory(${EXAMPLE})
endforeach()

# Warnings are applied per example target and only here - AFTER the dependency
# setup above - so GLUS, EGL and OpenVG_Library keep building exactly as
# before, and every warning that appears is ours. At the default settings this
# loop adds no flags at all.
if(COMMAND project_enable_warnings)
	foreach(_ex IN LISTS EXAMPLES_LIST)
		get_filename_component(_ex_target ${_ex} NAME)
		if(TARGET ${_ex_target})
			project_enable_warnings(${_ex_target})
		endif()
	endforeach()
endif()
