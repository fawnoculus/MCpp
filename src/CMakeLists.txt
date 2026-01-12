file(GLOB_RECURSE COMMON_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Common-Lib/**.c*")
add_library(Common-Lib STATIC ${COMMON_LIB_SOURCES})
target_compile_features(Common-Lib PUBLIC cxx_std_23)
target_include_directories(Common-Lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Common-Lib")
target_link_libraries(Common-Lib PUBLIC spdlog glm asio)

file(GLOB_RECURSE SERVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Server/**.c*")
add_executable(Server ${SERVER_SOURCES})
target_include_directories(Server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Server")
target_compile_features(Server PUBLIC cxx_std_23)
target_link_libraries(Server PUBLIC spdlog glm asio Common-Lib)

file(GLOB_RECURSE CLIENT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/Client/**.c*")
add_executable(Client ${CLIENT_SOURCES})
target_include_directories(Client PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/Client" ${stb_SOURCE_DIR})
target_compile_features(Client PUBLIC cxx_std_23)
target_link_libraries(Client PUBLIC spdlog glm asio Common-Lib glfw Vulkan::Headers imgui Vulkan::Vulkan Vulkan::glslang)

set_target_properties(Common-Lib Server Client PROPERTIES FOLDER "MCpp")
