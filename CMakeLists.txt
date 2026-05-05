# Copyright (c) 2026 CCP Games
cmake_minimum_required(VERSION 3.16)
project(CarbonTrinityAudioAPI)

include(cmake/CcpBuildConfigurations.cmake)
include(cmake/CcpTargetConfigurations.cmake)

add_library(TrinityAudioAPI INTERFACE)

target_sources(TrinityAudioAPI INTERFACE
    FILE_SET HEADERS
        BASE_DIRS include
        FILES
        include/IAudioInputMgr.h
        include/IStretchAudio.h
        include/ITr2AudEmitter.h
        include/ITr2AudGeometry.h
        include/ITr2Audio.h
)

# Provide an install target if this is the top level project only
if(CMAKE_PROJECT_NAME STREQUAL CarbonTrinityAudioAPI)
    # Determine if/when to generate/build documentation. The rules are as follows:
    # - If running locally - Build documentation is OFF by default
    # - If running on a build agent (TeamCity) - Build documentation is ON by default
    # - If -DBUILD_DOCUMENTATION=ON/OFF flag is explicitly set, use it.
    set(BUILD_DOCUMENTATION_DEFAULT_FLAG OFF)
    if(DEFINED ENV{TEAMCITY_VERSION})
        set(BUILD_DOCUMENTATION_DEFAULT_FLAG OFF)
    endif()
    message(STATUS "Document generation settings: default=${BUILD_DOCUMENTATION_DEFAULT_FLAG}, option BUILD_DOCUMENTATION=${BUILD_DOCUMENTATION}")
    option(BUILD_DOCUMENTATION "Build Documentation" ${BUILD_DOCUMENTATION_DEFAULT_FLAG})

    if(BUILD_DOCUMENTATION)
        # Find Doxygen, which is needed to parse C++ source files
        find_package(Doxygen)

        set(DOCUMENTATION_OUT_DIR ${CMAKE_CURRENT_BINARY_DIR}/doc)

        # Evaluate config file for Doxygen to input project values
        set(DOXYFILE_IN ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
        set(DOXYFILE_OUT ${DOCUMENTATION_OUT_DIR}/Doxyfile)
        configure_file(${DOXYFILE_IN} ${DOXYFILE_OUT} @ONLY)

        set(DOXYGEN_OUT_DIR ${DOCUMENTATION_OUT_DIR}/xml)
        set(DOXYGEN_INDEX_FILE ${DOXYGEN_OUT_DIR}/index.xml)

        # Regenerate with header-file changes
        add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
        DEPENDS ${PUBLIC_HEADER_FILES}
        COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYFILE_OUT}
        MAIN_DEPENDENCY ${DOXYFILE_OUT} ${DOXYFILE_IN}
        COMMENT "Running Doxygen"
        VERBATIM)

        add_custom_target(Doxygen ALL DEPENDS ${DOXYGEN_INDEX_FILE})

        # Set up sphinx build for Python autodoc comments.
        set(SPHINX_SOURCE ${CMAKE_CURRENT_SOURCE_DIR}/doc/source)
        set(SPHINX_BUILD ${DOCUMENTATION_OUT_DIR}/build)

        # Select the correct pythonInterpreter to use based on build system.
        if(WIN32)
            set(CCP_PYTHON_INTERPRETER "${BRANCH_ROOT_DIR}/eve/client/pythonInterpreter.bat")
        elseif(APPLE)
            set(CCP_PYTHON_INTERPRETER "${BRANCH_ROOT_DIR}/eve/client/pythonInterpreter.sh")
        endif()

        # Run Sphinx against configuration file and Doxygen output
        add_custom_target(Sphinx ALL
                COMMAND ${CMAKE_COMMAND}
                -E env
                PYTHONPATH="${CMAKE_SOURCE_DIR}/python"
                BUILDFLAVOR=$<LOWER_CASE:$<CONFIG>>
                ${CCP_PYTHON_INTERPRETER}
                -m sphinx build -E -b html
                -Dbreathe_projects.doxygen=${DOXYGEN_OUT_DIR}
                -c ${SPHINX_SOURCE}
                ${SPHINX_SOURCE} ${SPHINX_BUILD}
                WORKING_DIRECTORY ${BRANCH_ROOT_DIR}
                DEPENDS ${DOXYGEN_INDEX_FILE}
                COMMENT "Generating documentation with Sphinx")

        # Install rule for documentation
        install(DIRECTORY ${SPHINX_BUILD} DESTINATION documentation)
    endif()

    install(
            TARGETS TrinityAudioAPI
            EXPORT TrinityAudioAPITarget
            CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
            FILE_SET HEADERS
    )

    install(
            EXPORT TrinityAudioAPITarget
            NAMESPACE Carbon::
            DESTINATION share
            FILE carbon-trinityaudioapi.cmake)
endif()
