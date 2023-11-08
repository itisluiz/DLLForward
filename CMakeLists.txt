cmake_minimum_required (VERSION 3.21)
project("dllforward" VERSION 0.1 LANGUAGES CXX)

## interface
set(PROJECT_INTERFACE ${PROJECT_NAME}_interface)
add_library(${PROJECT_INTERFACE} INTERFACE)

# settings
target_compile_features(${PROJECT_INTERFACE} INTERFACE cxx_std_17)

# definitions
add_compile_definitions(-DCMAKE_PROJECT_VERSION="${CMAKE_PROJECT_VERSION}")

# sources
file(GLOB_RECURSE PROJECT_SOURCES "src/*.cpp" "src/*.cc")
target_sources(${PROJECT_INTERFACE} INTERFACE ${PROJECT_SOURCES})

# includes
target_include_directories(${PROJECT_INTERFACE} INTERFACE "include")

# architecture
if(CMAKE_SIZEOF_VOID_P EQUAL 4)
	set(TARGET_TRIPLET x86-${TARGET_TRIPLET_OSNAME})
else()
	set(TARGET_TRIPLET x64-${TARGET_TRIPLET_OSNAME})
endif()

# linkage
if(DEPENDENCY_STATIC_LINKAGE)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
	set(VCPKG_TARGET_TRIPLET ${TARGET_TRIPLET}-static)
else()
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
	set(VCPKG_TARGET_TRIPLET ${TARGET_TRIPLET})
endif()

# depencencies
find_package(cxxopts REQUIRED)
target_link_libraries(${PROJECT_INTERFACE} INTERFACE cxxopts::cxxopts imagehlp.lib)

## output
set(PROJECT_TARGET ${PROJECT_NAME})

if(OUTPUT_BINARY_TYPE STREQUAL "win32")
	add_executable(${PROJECT_TARGET} WIN32)
elseif(OUTPUT_BINARY_TYPE STREQUAL "dll")
	add_library(${PROJECT_TARGET} SHARED)
elseif(OUTPUT_BINARY_TYPE STREQUAL "lib")
	add_library(${PROJECT_TARGET} STATIC)
else()
	add_executable(${PROJECT_TARGET})
endif()

# linking
target_link_libraries(${PROJECT_TARGET} ${PROJECT_INTERFACE})
