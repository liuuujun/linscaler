foreach(name grid_mapping block_sampling affine_mapping)
    add_executable(example_${name} ${name}.cpp)
    target_link_libraries(example_${name} PRIVATE linscaler::linscaler)
    linscaler_development_target(example_${name})
    if(LINSCALER_BUILD_TESTS)
        if(name STREQUAL "grid_mapping")
            add_test(NAME example_${name} COMMAND ${CMAKE_COMMAND}
                "-DEXAMPLE=$<TARGET_FILE:example_${name}>"
                -P "${PROJECT_SOURCE_DIR}/cmake/test-grid-mapping.cmake")
        else()
            add_test(NAME example_${name} COMMAND example_${name})
        endif()
    endif()
endforeach()

