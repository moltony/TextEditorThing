cmake_minimum_required(VERSION 3.12)
project(TextEditorThing LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(wxWidgets REQUIRED COMPONENTS core base)
if(wxWidgets_USE_FILE)
    include(${wxWidgets_USE_FILE})
endif()

file(GLOB_RECURSE SRC_LIST CONFIGURE_DEPENDS "src/*.cpp")
add_executable(TextEditorThing ${SRC_LIST})
target_link_libraries(TextEditorThing ${wxWidgets_LIBRARIES})
