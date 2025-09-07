cmake_minimum_required(VERSION 3.28)

# Project
project(Enhanced_Freecam)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# SDL
find_package(SDL2 REQUIRED)

# OpenGL
find_package(OpenGL REQUIRED)

# Bin
set(IMGUI_DIR ${PROJECT_SOURCE_DIR}/dep/imgui)

set(SOURCES_IMGUI
	${IMGUI_DIR}/imconfig.h
	${IMGUI_DIR}/imgui.cpp
	${IMGUI_DIR}/imgui.h
	${IMGUI_DIR}/imgui_demo.cpp
	${IMGUI_DIR}/imgui_draw.cpp
	${IMGUI_DIR}/imgui_impl_opengl3.cpp
	${IMGUI_DIR}/imgui_impl_opengl3.h
	${IMGUI_DIR}/imgui_impl_opengl3_loader.h
	${IMGUI_DIR}/imgui_impl_sdl2.cpp
	${IMGUI_DIR}/imgui_impl_sdl2.h
	${IMGUI_DIR}/imgui_internal.h
	${IMGUI_DIR}/imgui_tables.cpp
	${IMGUI_DIR}/imgui_widgets.cpp
	${IMGUI_DIR}/imstb_rectpack.h
	${IMGUI_DIR}/imstb_textedit.h
	${IMGUI_DIR}/imstb_truetype.h
)

add_executable(${PROJECT_NAME} ${SOURCES_IMGUI})
add_subdirectory(${PROJECT_SOURCE_DIR}/src)
set_target_properties(${PROJECT_NAME} PROPERTIES OUTPUT_NAME Enhanced-Freecam)

if (WIN32)
	# Keep TTY in debug
	set_target_properties(${PROJECT_NAME} PROPERTIES
		LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE"
		LINK_FLAGS_RELEASE "/SUBSYSTEM:windows /ENTRY:mainCRTStartup"
		LINK_FLAGS_RELWITHDEBINFO "/SUBSYSTEM:windows /ENTRY:mainCRTStartup"
		LINK_FLAGS_MINSIZEREL "/SUBSYSTEM:windows /ENTRY:mainCRTStartup"
	)

	# Desktop Window Manager
	find_library(LIB_DWM dwmapi)

	if (LIB_DWM)
		target_link_libraries(${PROJECT_NAME} PRIVATE ${LIB_DWM})
	else()
		message(FATAL_ERROR "DWM lib not found")
	endif()

	# WinRT
	target_link_libraries(${PROJECT_NAME} PRIVATE windowsapp)
endif()

if (MSVC)
	# Visual Studio source group
	# https://github.com/TheLartians/GroupSourcesByFolder.cmake
	set(SOURCE_GROUP_DELIMITER "/")
	set(last_dir "")
	set(files "")
	get_target_property(sources ${PROJECT_NAME} SOURCES)
	foreach(file ${sources})
		file(RELATIVE_PATH relative_file "${PROJECT_SOURCE_DIR}" ${file})
		get_filename_component(dir "${relative_file}" PATH)
		if(NOT "${dir}" STREQUAL "${last_dir}")
			if(files)
				source_group("${last_dir}" FILES ${files})
			endif()
			set(files "")
		endif()
		set(files ${files} ${file})
		set(last_dir "${dir}")
	endforeach()

	if(files)
		source_group("${last_dir}" FILES ${files})
	endif()
endif()

target_include_directories(${PROJECT_NAME} PRIVATE 
	${PROJECT_SOURCE_DIR}/src
	${PROJECT_SOURCE_DIR}/dep/imgui
	${PROJECT_SOURCE_DIR}/dep/json/include)

target_compile_definitions(${PROJECT_NAME} PRIVATE $<$<CONFIG:Debug>:EF_DEBUG>)



target_link_libraries(${PROJECT_NAME} PRIVATE
	SDL2::SDL2
	OpenGL::GL
)