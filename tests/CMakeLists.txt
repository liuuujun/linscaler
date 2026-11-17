foreach(name legacy core integer sets mapping affine check_failure)
    add_executable(test_${name} test_${name}.cpp)
    target_link_libraries(test_${name} PRIVATE linscaler::linscaler)
    linscaler_development_target(test_${name})
    add_test(NAME ${name} COMMAND test_${name})
endforeach()

# Reuse the same behavioral tests in the lowest supported language mode.
# Wrappers verify the effective language mode before compiling each test source.
function(linscaler_cxx11_compatibility_target target)
    set_target_properties(${target} PROPERTIES
        CXX_STANDARD 11 CXX_STANDARD_REQUIRED YES CXX_EXTENSIONS OFF)
    if(MSVC)
        # MSVC has no C++11 mode. Its lowest selectable mode is C++14;
        # only GCC/Clang runs below establish strict C++11 compatibility.
        set_target_properties(${target} PROPERTIES CXX_STANDARD 14)
    elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE -pedantic-errors)
    endif()
endfunction()
foreach(name core integer mapping affine)
    set(source "${CMAKE_CURRENT_BINARY_DIR}/test_${name}_cxx11.cpp")
    file(WRITE "${source}"
        "#include \"${CMAKE_CURRENT_SOURCE_DIR}/cxx11_mode.hpp\"\n#include \"${CMAKE_CURRENT_SOURCE_DIR}/test_${name}.cpp\"\n")
    add_executable(test_${name}_cxx11 "${source}")
    if(name STREQUAL "mapping" OR name STREQUAL "affine")
        target_link_libraries(test_${name}_cxx11 PRIVATE linscaler::transform)
    else()
        target_link_libraries(test_${name}_cxx11 PRIVATE linscaler::core)
    endif()
    linscaler_development_target(test_${name}_cxx11)
    linscaler_cxx11_compatibility_target(test_${name}_cxx11)
    add_test(NAME ${name}_cxx11 COMMAND test_${name}_cxx11)
endforeach()
# Proves checks are active even in Release / NDEBUG builds.
set_tests_properties(check_failure PROPERTIES WILL_FAIL TRUE)
set_tests_properties(sets PROPERTIES TIMEOUT 120)
file(GLOB_RECURSE public_headers CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/linscaler/*.hpp")
set(header_sources header_main.cpp)
foreach(header IN LISTS public_headers)
    file(RELATIVE_PATH relative "${PROJECT_SOURCE_DIR}/include" "${header}")
    string(MAKE_C_IDENTIFIER "${relative}" identifier)
    set(source "${CMAKE_CURRENT_BINARY_DIR}/${identifier}.cpp")
    file(WRITE "${source}" "#include <${relative}>\nvoid ${identifier}() {}\n")
    list(APPEND header_sources "${source}")
endforeach()
add_executable(test_headers ${header_sources})
target_link_libraries(test_headers PRIVATE linscaler::linscaler)
linscaler_development_target(test_headers)
add_test(NAME headers COMMAND test_headers)

set(core_headers
    core.hpp version.hpp basic/integer.hpp basic/precision.hpp
    core/linsp.hpp core/linsp_io.hpp)
set(core_header_sources core_header_main.cpp core_header_peer.cpp)
foreach(header IN LISTS core_headers)
    string(MAKE_C_IDENTIFIER "${header}" identifier)
    set(source "${CMAKE_CURRENT_BINARY_DIR}/core_${identifier}.cpp")
    file(WRITE "${source}"
        "#include \"${CMAKE_CURRENT_SOURCE_DIR}/cxx11_mode.hpp\"\n#include <linscaler/${header}>\nvoid core_${identifier}() {}\n")
    list(APPEND core_header_sources "${source}")
endforeach()
add_executable(test_core_headers_cxx11 ${core_header_sources})
target_link_libraries(test_core_headers_cxx11 PRIVATE linscaler::core)
linscaler_development_target(test_core_headers_cxx11)
linscaler_cxx11_compatibility_target(test_core_headers_cxx11)
add_test(NAME core_headers_cxx11 COMMAND test_core_headers_cxx11)

set(transform_headers
    transform/mapper.hpp transform/rational_index_map.hpp
    transform/rational_affine_map.hpp transform/floating_affine_map.hpp
    transform/grid_map.hpp)
set(transform_header_sources transform_header_main.cpp transform_header_peer.cpp)
foreach(header IN LISTS transform_headers)
    string(MAKE_C_IDENTIFIER "${header}" identifier)
    set(source "${CMAKE_CURRENT_BINARY_DIR}/${identifier}_cxx11.cpp")
    file(WRITE "${source}"
        "#include \"${CMAKE_CURRENT_SOURCE_DIR}/cxx11_mode.hpp\"\n#include <linscaler/${header}>\nvoid ${identifier}_cxx11() {}\n")
    list(APPEND transform_header_sources "${source}")
endforeach()
add_executable(test_transform_headers_cxx11 ${transform_header_sources})
target_link_libraries(test_transform_headers_cxx11 PRIVATE linscaler::transform)
linscaler_development_target(test_transform_headers_cxx11)
linscaler_cxx11_compatibility_target(test_transform_headers_cxx11)
add_test(NAME transform_headers_cxx11 COMMAND test_transform_headers_cxx11)
add_test(NAME package_consumer COMMAND ${CMAKE_COMMAND}
    "-DPROJECT_SOURCE=${PROJECT_SOURCE_DIR}"
    "-DPROJECT_BINARY=${PROJECT_BINARY_DIR}"
    "-DCONFIG=$<CONFIG>"
    "-DGENERATOR=${CMAKE_GENERATOR}"
    "-DPLATFORM=${CMAKE_GENERATOR_PLATFORM}"
    "-DCOMPILER=${CMAKE_CXX_COMPILER}"
    "-DMAKE_PROGRAM=${CMAKE_MAKE_PROGRAM}"
    "-DLINKER_FLAGS=${CMAKE_EXE_LINKER_FLAGS}"
    -P "${PROJECT_SOURCE_DIR}/cmake/test-package.cmake")
set_tests_properties(package_consumer PROPERTIES TIMEOUT 120)

