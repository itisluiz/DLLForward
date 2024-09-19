cmake_minimum_required(VERSION 3.21)
project("dllforward" VERSION 1.3 LANGUAGES CXX)

## Project's base, containing all that's to be inherited
set(PROJECT_BASE ${PROJECT_NAME}-base)
add_library(${PROJECT_BASE} INTERFACE)
target_compile_features(${PROJECT_BASE} INTERFACE cxx_std_17)
target_include_directories(${PROJECT_BASE} INTERFACE "include")

## QoL definitions
target_compile_definitions(${PROJECT_BASE} INTERFACE PROJECT_VERSION="${PROJECT_VERSION}" BUILD_SHARED_LIBS=$<BOOL:${BUILD_SHARED_LIBS}>)

## Project's dependencies adding and linking
if(NOT TARGET CLI11)
	add_subdirectory("lib/cli11")
endif()

target_link_libraries(${PROJECT_BASE} INTERFACE CLI11 imagehlp)

## Source globbing, exclude main file
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "src/*.cxx" "src/*.cc")
list(FILTER PROJECT_SOURCES EXCLUDE REGEX "src/main.cc$")
target_sources(${PROJECT_BASE} INTERFACE ${PROJECT_SOURCES})

## Project's resource files
set(PROJECT_RESOURCES "res/app.rc" "res/app.ico")

## Project output, end-product
set(PROJECT_TARGET ${PROJECT_NAME})
# add_library(${PROJECT_TARGET} "src/main.cc" ${PROJECT_RESOURCES})
add_executable(${PROJECT_TARGET} "src/main.cc" ${PROJECT_RESOURCES})
target_link_libraries(${PROJECT_TARGET} PUBLIC ${PROJECT_BASE})
