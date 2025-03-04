# CMakeList.txt : CMake project for RunIt, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("RunIt")

# Add source to this project's executable.
add_executable (RunIt "source/Main.cpp"  "source/Application.cpp")

target_precompile_headers ( RunIt PRIVATE source/pch.hpp )

# Add GLFW
include ( "FindOpenGL" )
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory ( external/glfw )

# Add ImGUI
target_compile_definitions ( RunIt
	PUBLIC
	IMGUI_DEFINE_MATH_OPERATORS
)

target_include_directories ( RunIt 
	PUBLIC
	"external/imgui"
	"external/imgui/backends"
	"external/imgui/misc/cpp"
	"external/imgui-filebrowser"
)

target_sources ( RunIt 
	PRIVATE
	"external/imgui/imgui.cpp"
	"external/imgui/imgui_demo.cpp"
	"external/imgui/imgui_draw.cpp"
	"external/imgui/imgui_widgets.cpp"
	"external/imgui/imgui_tables.cpp"
	"external/imgui/misc/cpp/imgui_stdlib.cpp"
	"external/imgui/backends/imgui_impl_opengl3.cpp"
	"external/imgui/backends/imgui_impl_glfw.cpp"
 )

# Add nlohmann json
add_subdirectory ( external/json-3.11.3 )
target_link_libraries ( RunIt PRIVATE nlohmann_json)

target_link_libraries ( RunIt 
	PRIVATE 
	OpenGL::GL
	glfw
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET RunIt PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
