# Copyright (C) 2024 embeff GmbH
# SPDX-License-Identifier: Apache-2.0
# This file is part of the ExecutionPlatform distribution. More information at https://embeff.com/executionplatform/.

# Adds a .bin file to the given (elf) build target that can be used with `DUT Flash Firmware`.
#
function(ep_add_bin_output target)
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Obinary $<TARGET_FILE:${target}> $<TARGET_FILE_DIR:${target}>/${target}.bin
            COMMENT "Creating ${target}.bin" VERBATIM )
    add_custom_command(TARGET ${target} POST_BUILD
            COMMAND ${CMAKE_OBJCOPY} -Oihex $<TARGET_FILE:${target}> $<TARGET_FILE_DIR:${target}>/${target}.hex
            COMMENT "Creating ${target}.hex" VERBATIM )
endfunction(ep_add_bin_output)

# Creates an app with whole-archive linkage to target_bsp
#
# app is created as INTERFACE library, this way all object files will be linked
# to the executable. This provides all symbols that are neccessesary.
#
macro(ep_app_create app sources)
    add_library(${app} INTERFACE)
    target_sources(${app} INTERFACE ${sources})

    # Link app implementation and bsp as whole-archive
    target_link_libraries(${app} INTERFACE -Wl,--whole-archive,"$<TARGET_FILE:target_bsp>",--no-whole-archive)

    ep_app_link(${app} target_bsp)
endmacro(ep_app_create)

macro(ep_app_link app libs)
    add_dependencies(${app} ${libs})
    target_link_libraries(${app} INTERFACE ${app} ${libs})
endmacro(ep_app_link)


# Creates a executable that uses ep_app_generic and provides access to full bsp
#
# If you have multiple sources, make sure to quote the list on the calling side.
# This is intended for examples that demonstrate driver behavior. In this case
# the sources will need access to bsp internals.
#
macro(ep_setup_example name sources)
    ep_app_example( APP ep_app_generic
                    NAME ${name}
                    SOURCES ${sources})

endmacro(ep_setup_example)

# Creates a executable that uses specified ep_app and provides access to full bsp
#
function(ep_app_example)
    set(flags)
    set(args)
    set(listArgs APP NAME SOURCES)

    cmake_parse_arguments(arg "${flags}" "${args}" "${listArgs}" ${ARGN})

    if (NOT arg_SOURCES)
        message(FATAL_ERROR "[ep_app_example]: SOURCES is a required argument")
    endif()
    if (NOT arg_NAME)
        message(FATAL_ERROR "[ep_app_example]: NAME is a required argument")
    endif()
    if (NOT arg_APP)
        message(FATAL_ERROR "[ep_app_example]: APP is a required argument")
    endif()

    add_executable(${arg_NAME} ${arg_SOURCES})
    target_link_libraries(${arg_NAME} ${arg_APP})

    # We are accessing drivers here directly, so use all internal includes from bsp to make them available
    get_target_property(BSP_INCLUDES target_bsp INCLUDE_DIRECTORIES)
    target_include_directories(${arg_NAME} PRIVATE ${BSP_INCLUDES})

    # Same output directory, independent from where the example is deployed.
    set_target_properties(${arg_NAME}
        PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/example")
    ep_add_bin_output(${arg_NAME})

endfunction(ep_app_example)

function(add_gtest_example)
    set(flags)
    set(args)
    set(listArgs NAME SOURCES INCLUDE_DIRECTORIES)

    cmake_parse_arguments(arg "${flags}" "${args}" "${listArgs}" ${ARGN})

    if (NOT arg_SOURCES)
        message(FATAL_ERROR "[add_gtest_executable]: SOURCES is a required argument")
    endif()
    if (NOT arg_NAME)
        message(FATAL_ERROR "[add_gtest_executable]: NAME is a required argument")
    endif()

    if(CMAKE_CROSSCOMPILING)
        ep_app_example( APP ep_app_gtest
                        NAME ${arg_NAME}
                        SOURCES ${arg_SOURCES})
    else()
        add_executable(${arg_NAME} ${arg_SOURCES})
        target_link_libraries(${arg_NAME} PRIVATE GTest::gtest_main GTest::gmock)
    endif()

    if (arg_INCLUDE_DIRECTORIES)
        target_include_directories(${arg_NAME}  PRIVATE  ${arg_INCLUDE_DIRECTORIES} )
    endif()

endfunction(add_gtest_example)
