cmake_minimum_required(VERSION 3.24)
# Each invocation owns a fresh directory. No recursive deletion of caller paths.
string(RANDOM LENGTH 12 ALPHABET 0123456789abcdef run_id)
set(root "${PROJECT_BINARY}/package-check-${run_id}")
set(stage "${root}/stage")
set(relocated "${root}/relocated")
file(MAKE_DIRECTORY "${root}")
function(run)
    execute_process(COMMAND ${ARGV} RESULT_VARIABLE result OUTPUT_VARIABLE output ERROR_VARIABLE error)
    if(NOT result STREQUAL "0")
        message(FATAL_ERROR "Command failed: ${ARGV}\n${output}\n${error}")
    endif()
endfunction()
run("${CMAKE_COMMAND}" --install "${PROJECT_BINARY}" --config "${CONFIG}" --prefix "${stage}")
file(RENAME "${stage}" "${relocated}")
set(generator_args -G "${GENERATOR}")
if(PLATFORM)
    list(APPEND generator_args -A "${PLATFORM}")
endif()
get_filename_component(cmake_bin "${CMAKE_COMMAND}" DIRECTORY)
find_program(test_runner NAMES ctest HINTS "${cmake_bin}" NO_DEFAULT_PATH REQUIRED)
foreach(mode installed subdirectory)
    # Each configuration builds/runs the full C++17 consumer and C++11 core and
    # transform consumers (MSVC uses its lowest selectable mode).
    set(build "${root}/${mode}")
    run("${CMAKE_COMMAND}" -S "${PROJECT_SOURCE}/tests/consumer" -B "${build}"
        ${generator_args} "-DCMAKE_CXX_COMPILER=${COMPILER}"
        "-DCMAKE_MAKE_PROGRAM=${MAKE_PROGRAM}" "-DCMAKE_BUILD_TYPE=${CONFIG}"
        "-DCMAKE_EXE_LINKER_FLAGS=${LINKER_FLAGS}"
        "-DCMAKE_PREFIX_PATH=${relocated}" "-DCONSUMER_MODE=${mode}"
        "-DLINSCALER_SOURCE=${PROJECT_SOURCE}")
    run("${CMAKE_COMMAND}" --build "${build}" --config "${CONFIG}")
    run("${test_runner}" --test-dir "${build}" -C "${CONFIG}" --output-on-failure)
endforeach()

