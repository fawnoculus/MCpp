cmake_minimum_required(VERSION 3.30)

set_property(GLOBAL PROPERTY CXX_STANDARD 23)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(MCPP_VERSION_MAJOR 0)
set(MCPP_VERSION_MINOR 0)
set(MCPP_VERSION_PATCH 0)

project(MCpp LANGUAGES CXX C
        DESCRIPTION "Minecraft Clone in C++"
        VERSION ${MCPP_VERSION_MAJOR}.${MCPP_VERSION_MINOR}.${MCPP_VERSION_PATCH}
)

message(STATUS "[MCpp] Version: ${MCPP_VERSION_MAJOR}.${MCPP_VERSION_MINOR}.${MCPP_VERSION_PATCH}")

add_compile_definitions(
        MCPP_VERSION_MAJOR=${MCPP_VERSION_MAJOR}
        MCPP_VERSION_MINOR=${MCPP_VERSION_MINOR}
        MCPP_VERSION_PATCH=${MCPP_VERSION_PATCH}
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
else ()
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
endif ()

message(STATUS "[MCpp] Linking Resources into Build-Directory")
file(CREATE_LINK "${CMAKE_CURRENT_SOURCE_DIR}/resources" "${CMAKE_CURRENT_BINARY_DIR}/resources" RESULT result COPY_ON_ERROR SYMBOLIC)
if(result EQUAL 0)
    message(STATUS "[MCpp] successfully created symlink")
else ()
    message(WARNING "[MCpp] an error occurred while creating symlink: ${result}")
    message(WARNING "[MCpp] The compiled binaries might not find the resources")
endif ()

include("Dependencies.cmake")
include_directories("src")

if (DEFINED WIN32)
    # Stop Microsoft from whining
    add_compile_definitions(_CRT_SECURE_NO_WARNINGS)

    if (DEFINED MSVC) # If using the Visual Studio Compiler
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

        if (CMAKE_BUILD_TYPE STREQUAL "Debug") # Enable Console for Client on Debug Builds
            set_target_properties(Client PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
        else ()
            set_target_properties(Client PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
        endif ()

        set_target_properties(Server PROPERTIES LINK_FLAGS "/SUBSYSTEM:CONSOLE /ENTRY:mainCRTStartup")
    endif ()
endif ()
