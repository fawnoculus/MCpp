cmake_minimum_required(VERSION 3.30)

include(FetchContent)

set_property(GLOBAL PROPERTY CXX_STANDARD 23)
set_property(GLOBAL PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(MCPP_VERSION_MAJOR 0)
set(MCPP_VERSION_MINOR 0)
set(MCPP_VERSION_PATCH 0)

project(MCpp LANGUAGES CXX C
        DESCRIPTION "Minecraft Clone in C++"
        VERSION ${MCPP_VERSION_MAJOR}.${MCPP_VERSION_MINOR}.${MCPP_VERSION_PATCH}
)

message(STATUS "[MCpp] Version: ${MCPP_VERSION_MAJOR}.${MCPP_VERSION_MINOR}.${MCPP_VERSION_PATCH}")

add_compile_definitions(
        VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
        MCPP_VERSION_MAJOR=${MCPP_VERSION_MAJOR}
        MCPP_VERSION_MINOR=${MCPP_VERSION_MINOR}
        MCPP_VERSION_PATCH=${MCPP_VERSION_PATCH}
)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION FALSE)
    set(GLM_ENABLE_FAST_MATH OFF)
else ()
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)
    set(GLM_ENABLE_FAST_MATH ON)
endif ()

message(STATUS "[MCpp] Getting spdlog ...")
set(SPDLOG_USE_STD_FORMAT true)
FetchContent_Declare(spdlog
        GIT_REPOSITORY https://github.com/gabime/spdlog
        GIT_TAG v1.16.0
)
FetchContent_MakeAvailable(spdlog)
set_target_properties(spdlog PROPERTIES FOLDER "spdlog")

message(STATUS "[MCpp] Getting glm ...")
set(GLM_ENABLE_CXX_20 ON FORCE CACHE BOOL "" FORCE)
set(GLM_BUILD_LIBRARY ON FORCE CACHE BOOL "" FORCE)
set(GLM_TEST_ENABLE OFF FORCE CACHE BOOL "" FORCE)
set(GLM_PERF_TEST_ENABLE OFF FORCE CACHE BOOL "" FORCE)
set(GLM_BUILD_TESTS OFF FORCE CACHE BOOL "" FORCE)
set(GLM_BUILD_INSTALL OFF FORCE CACHE BOOL "" FORCE)
set(GLM_ENABLE_SIMD_SSE2 OFF CACHE BOOL "" FORCE)
set(GLM_ENABLE_SIMD_SSE3 OFF CACHE BOOL "" FORCE)
set(GLM_ENABLE_SIMD_SSSE3 OFF CACHE BOOL "" FORCE)
set(GLM_ENABLE_SIMD_SSE4_1 OFF CACHE BOOL "" FORCE)
set(GLM_ENABLE_SIMD_SSE4_2 OFF CACHE BOOL "" FORCE)
set(GLM_ENABLE_SIMD_AVX OFF CACHE BOOL "" FORCE)
set(GLM_ENABLE_SIMD_AVX2 ON CACHE BOOL "" FORCE) # enable AVX2 optimisations, disable everything else
set(GLM_ENABLE_SIMD_NEON OFF CACHE BOOL "" FORCE)
set(GLM_FORCE_PURE OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glm
        GIT_REPOSITORY https://github.com/g-truc/glm
        GIT_TAG 1.0.2
)
FetchContent_MakeAvailable(glm)
set_target_properties(glm uninstall
        test-bug_ms_vec_static
        test-core_cpp_constexpr test-core_cpp_defaulted_ctor test-core_force_aligned_gentypes test-core_force_ctor_init test-core_force_arch_unknown test-core_force_compiler_unknown test-core_force_explicit_ctor test-core_force_inline test-core_force_intrinsics test-core_force_platform_unknown test-core_force_pure test-core_force_unrestricted_gentype test-core_force_xyzw_only test-core_force_quat_wxyz test-core_type_aligned test-core_type_cast test-core_type_ctor test-core_type_int test-core_type_length test-core_type_mat2x2 test-core_type_mat2x3 test-core_type_mat2x4 test-core_type_mat3x2 test-core_type_mat3x3 test-core_type_mat3x4 test-core_type_mat4x2 test-core_type_mat4x3 test-core_type_mat4x4 test-core_type_vec1 test-core_type_vec2 test-core_type_vec3 test-core_type_vec4 test-core_func_common test-core_func_exponential test-core_func_geometric test-core_func_integer test-core_func_integer_bit_count test-core_func_integer_find_lsb test-core_func_integer_find_msb test-core_func_matrix test-core_func_noise test-core_func_packing test-core_func_trigonometric test-core_func_vector_relational test-core_func_swizzle test-core_setup_force_cxx_unknown test-core_setup_force_cxx98 test-core_setup_force_cxx03 test-core_setup_force_size_t_length test-core_setup_message test-core_setup_platform_unknown test-core_setup_precision
        test-ext_matrix_relational test-ext_matrix_transform test-ext_matrix_common test-ext_matrix_integer test-ext_matrix_int2x2_sized test-ext_matrix_int2x3_sized test-ext_matrix_int2x4_sized test-ext_matrix_int3x2_sized test-ext_matrix_int3x3_sized test-ext_matrix_int3x4_sized test-ext_matrix_int4x2_sized test-ext_matrix_int4x3_sized test-ext_matrix_int4x4_sized test-ext_matrix_uint2x2_sized test-ext_matrix_uint2x3_sized test-ext_matrix_uint2x4_sized test-ext_matrix_uint3x2_sized test-ext_matrix_uint3x3_sized test-ext_matrix_uint3x4_sized test-ext_matrix_uint4x2_sized test-ext_matrix_uint4x3_sized test-ext_matrix_uint4x4_sized test-ext_quaternion_common test-ext_quaternion_exponential test-ext_quaternion_geometric test-ext_quaternion_relational test-ext_quaternion_transform test-ext_quaternion_trigonometric test-ext_quaternion_type test-ext_scalar_common test-ext_scalar_constants test-ext_scalar_int_sized test-ext_scalar_uint_sized test-ext_scalar_integer test-ext_scalar_ulp test-ext_scalar_reciprocal test-ext_scalar_relational test-ext_vec1 test-ext_vector_bool1 test-ext_vector_common test-ext_vector_iec559 test-ext_vector_int1_sized test-ext_vector_int2_sized test-ext_vector_int3_sized test-ext_vector_int4_sized test-ext_vector_integer test-ext_vector_integer_sized test-ext_vector_uint1_sized test-ext_vector_uint2_sized test-ext_vector_uint3_sized test-ext_vector_uint4_sized test-ext_vector_reciprocal test-ext_vector_relational test-ext_vector_ulp
        test-gtc_bitfield test-gtc_color_space test-gtc_constants test-gtc_epsilon test-gtc_integer test-gtc_matrix_access test-gtc_matrix_integer test-gtc_matrix_inverse test-gtc_matrix_transform test-gtc_noise test-gtc_packing test-gtc_quaternion test-gtc_random test-gtc_round test-gtc_reciprocal test-gtc_type_aligned test-gtc_type_precision test-gtc_type_ptr test-gtc_ulp test-gtc_vec1
        test-gtx test-gtx_associated_min_max test-gtx_closest_point test-gtx_color_encoding test-gtx_color_space_YCoCg test-gtx_color_space test-gtx_common test-gtx_compatibility test-gtx_component_wise test-gtx_easing test-gtx_euler_angle test-gtx_extend test-gtx_extended_min_max test-gtx_exterior_product test-gtx_fast_exponential test-gtx_fast_square_root test-gtx_fast_trigonometry test-gtx_functions test-gtx_gradient_paint test-gtx_handed_coordinate_space test-gtx_hash test-gtx_integer test-gtx_intersect test-gtx_io test-gtx_iteration test-gtx_load test-gtx_log_base test-gtx_matrix_cross_product test-gtx_matrix_decompose test-gtx_matrix_factorisation test-gtx_matrix_interpolation test-gtx_matrix_major_storage test-gtx_matrix_operation test-gtx_matrix_query test-gtx_matrix_transform_2d test-gtx_norm test-gtx_normal test-gtx_normalize_dot test-gtx_orthonormalize test-gtx_optimum_pow test-gtx_pca test-gtx_perpendicular test-gtx_polar_coordinates test-gtx_projection test-gtx_quaternion test-gtx_dual_quaternion test-gtx_range test-gtx_rotate_normalized_axis test-gtx_rotate_vector test-gtx_scalar_multiplication test-gtx_scalar_relational test-gtx_spline test-gtx_string_cast test-gtx_structured_bindings test-gtx_texture test-gtx_type_aligned test-gtx_type_trait test-gtx_vec_swizzle test-gtx_vector_angle test-gtx_vector_query test-gtx_wrap
        test-perf_matrix_div test-perf_matrix_inverse test-perf_matrix_mul test-perf_matrix_mul_vector test-perf_matrix_transpose test-perf_vector_mul_matrix
        PROPERTIES FOLDER "glm"
)

message(STATUS "[MCpp] Getting asio ...")
FetchContent_Declare(asio
        GIT_REPOSITORY https://github.com/chriskohlhoff/asio
        GIT_COMMIT 55684d42ac00021b4c31ba8571aca414c863ead5
)
FetchContent_MakeAvailable(asio)
find_package(Threads REQUIRED)
file(GLOB_RECURSE ASIO_SOURCES "${asio_SOURCE_DIR}/include/**.h*")
add_library(asio STATIC "${ASIO_SOURCES}")
target_include_directories(asio PUBLIC "${asio_SOURCE_DIR}/include")
target_link_libraries(asio INTERFACE Threads::Threads)
set_target_properties(asio PROPERTIES LINKER_LANGUAGE CXX FOLDER "asio")

message(STATUS "[MCpp] Getting glfw ...")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
FetchContent_Declare(glfw
        GIT_REPOSITORY https://github.com/glfw/glfw
        GIT_TAG 3.4
)
FetchContent_MakeAvailable(glfw)

message(STATUS "[MCpp] Getting stb ...")
FetchContent_Declare(stb
        GIT_REPOSITORY https://github.com/nothings/stb
        GIT_COMMIT f1c79c02822848a9bed4315b12c8c8f3761e1296
)
FetchContent_MakeAvailable(stb)

message(STATUS "[MCpp] Getting imgui ...")
FetchContent_Declare(imgui
        GIT_REPOSITORY https://github.com/ocornut/imgui
        GIT_TAG v1.92.5#-docking
)
FetchContent_MakeAvailable(imgui)

message(STATUS "[MCpp] Finding Vulkan Sdk ...")
find_package(Vulkan REQUIRED COMPONENTS glslang SPIRV-Tools)
if (NOT Vulkan_FOUND)
    message(FATAL_ERROR "Failed to Find Vulkan SDK! Did you set VULKAN_SDK in your Environment Variables?")
endif ()
if (NOT Vulkan_SPIRV-Tools_FOUND)
    message(FATAL_ERROR "Failed to Find Vulkan SPIRV-Tools! Did you set VULKAN_SDK in your Environment Variables?")
endif ()

message(STATUS "[MCpp] Getting shaderc ...")
set(SHADERC_SKIP_INSTALL ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_TESTS ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_EXECUTABLES ON CACHE BOOL "" FORCE)
set(SHADERC_SKIP_COPYRIGHT_CHECK ON CACHE BOOL "" FORCE)
set(SHADERC_ENABLE_WGSL_OUTPUT OFF CACHE BOOL "" FORCE)
add_library(glslang ALIAS Vulkan::glslang)
add_library(SPIRV-Tools ALIAS Vulkan::SPIRV-Tools)
FetchContent_Declare(shaderc
        GIT_REPOSITORY https://github.com/google/shaderc
        GIT_TAG v2025.5
)
FetchContent_MakeAvailable(shaderc)
set_target_properties(shaderc shaderc_shared shaderc_util shaderc_combined
        shaderc-pkg-config shaderc_combined-pkg-config shaderc_static-pkg-config
        add-copyright build-version testdata
        glslc
        PROPERTIES FOLDER "shaderc"
)


file(GLOB_RECURSE COMMON_LIB_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/Common-Lib/**.c*")
add_library(Common-Lib STATIC "${COMMON_LIB_SOURCES}")
target_compile_features(Common-Lib PUBLIC cxx_std_23)
target_include_directories(Common-Lib PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Common-Lib")
target_link_libraries(Common-Lib PUBLIC spdlog glm asio)

file(GLOB_RECURSE SERVER_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/Server/**.c*")
file(GLOB_RECURSE SERVER_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/Server/**.h*")
add_executable(Server "${SERVER_SOURCES};${SERVER_HEADERS}")
target_include_directories(Server PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Server")
target_compile_features(Server PUBLIC cxx_std_23)
target_link_libraries(Server PUBLIC spdlog glm asio Common-Lib)

file(GLOB_RECURSE CLIENT_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/Client/**.c*")
file(GLOB_RECURSE CLIENT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/src/Client/**.h*")
add_executable(Client "${CLIENT_SOURCES};${CLIENT_HEADERS}")
target_include_directories(Client PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/src/Client")
target_compile_features(Client PUBLIC cxx_std_23)
target_link_libraries(Client PUBLIC spdlog glm asio Common-Lib glfw ${Vulkan_LIBRARIES} )#shaderc) TODO: fix shaderc not compiling
target_include_directories(Client PUBLIC ${Vulkan_INCLUDE_DIRS} ${imgui_SOURCE_DIR} "${imgui_SOURCE_DIR}/backends" ${stb_SOURCE_DIR})
target_link_directories(Client PUBLIC ${Vulkan_LIBRARY_DIRS} ${imgui_SOURCE_DIR} "${imgui_SOURCE_DIR}/backends" ${stb_SOURCE_DIR})

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
