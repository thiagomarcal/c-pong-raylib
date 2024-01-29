cmake_minimum_required(VERSION 3.27)
project(c_pong_raylib C)

set(CMAKE_C_STANDARD 11)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

FILE(GLOB SOURCE_FILES src/*.c src/*/*.c src/*/*/*.c)
FILE(GLOB HEADER_FILES src/*.h src/*/*.h src/*/*/*.h)

# Dependencies
# raylib
set(RAYLIB_VERSION 4.5.0)
find_package(raylib ${RAYLIB_VERSION} QUIET) # QUIET or REQUIRED
if (NOT raylib_FOUND) # If there's none, fetch and build raylib
    include(FetchContent)
    FetchContent_Declare(
            raylib
            DOWNLOAD_EXTRACT_TIMESTAMP OFF
            URL https://github.com/raysan5/raylib/archive/refs/tags/${RAYLIB_VERSION}.tar.gz
    )
    FetchContent_GetProperties(raylib)
    if (NOT raylib_POPULATED) # Have we downloaded raylib yet?
        set(FETCHCONTENT_QUIET NO)
        FetchContent_Populate(raylib)
        set(BUILD_EXAMPLES OFF CACHE BOOL "" FORCE) # don't build the supplied examples
        add_subdirectory(${raylib_SOURCE_DIR} ${raylib_BINARY_DIR})
    endif()
endif()

# raygui
set(RAYGUI_VERSION 3.1)
FetchContent_Declare(
        raygui
        URL https://github.com/raysan5/raygui/archive/refs/tags/${RAYGUI_VERSION}.tar.gz
)
FetchContent_GetProperties(raygui)
if (NOT raygui_POPULATED)
    FetchContent_Populate(raygui)
endif()

include_directories(${raygui_SOURCE_DIR}/src)

include_directories(.)
include_directories(src)
include_directories(external)

link_directories(external/lib)

add_definitions(-DSOME_DEFINITION)

add_executable(${PROJECT_NAME}
        ${SOURCE_FILES} ${HEADER_FILES})


target_include_directories(${PROJECT_NAME} PRIVATE external/include external/lib ${raygui_SOURCE_DIR}/src)

target_link_libraries(${PROJECT_NAME} raylib)