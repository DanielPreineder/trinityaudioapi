cmake_minimum_required(VERSION 3.16)
project(CarbonTrinityAudioAPI VERSION 1.0.0)

if(NOT DEFINED ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "Missing required environment variable CCP_EVE_PERFORCE_BRANCH_PATH")
elseif(NOT IS_ABSOLUTE $ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "CCP_EVE_PERFORCE_BRANCH_PATH needs to be an absolute path")
elseif(NOT EXISTS $ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "CCP_EVE_PERFORCE_BRANCH_PATH is pointing at a non-existing location $ENV{CCP_EVE_PERFORCE_BRANCH_PATH}")
elseif(NOT IS_DIRECTORY $ENV{CCP_EVE_PERFORCE_BRANCH_PATH})
    message(FATAL_ERROR "CCP_EVE_PERFORCE_BRANCH_PATH needs to be a directory")
else()
    message(STATUS "Using CCP_EVE_PERFORCE_BRANCH_PATH located at $ENV{CCP_EVE_PERFORCE_BRANCH_PATH}")
endif()

file(TO_CMAKE_PATH "$ENV{CCP_EVE_PERFORCE_BRANCH_PATH}" BRANCH_ROOT_DIR)
list(INSERT CMAKE_MODULE_PATH 0 ${BRANCH_ROOT_DIR}/cmake)

option(USE_VCPKG OFF)

if(USE_VCPKG)
    include(cmake/CcpGlobalSettings.cmake)
    include(cmake/CcpTargetConfigurations.cmake)
else()
    include(CcpGlobalSettings)
    include(CcpPackageConfigHelpers)
endif()

set(PUBLIC_HEADER_FILES
    include/IAudioInputMgr.h
    include/ITr2AudEmitter.h
    include/ITr2Audio.h
)

add_library(CarbonTrinityAudioAPI INTERFACE)

target_include_directories(CarbonTrinityAudioAPI
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_sources(CarbonTrinityAudioAPI
    INTERFACE
        ${PUBLIC_HEADER_FILES}
)

get_target_property(_SOURCES CarbonTrinityAudioAPI SOURCES)
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}"
        PREFIX "Sources"
        FILES ${_SOURCES}
)

# Provide an install target if this is the top level project only
if(CMAKE_CarbonTrinityAudioAPI STREQUAL CarbonTrinityAudioAPI)
    # Determine if/when to generate/build documentation. The rules are as follows:
    # - If running locally - Build documentation is OFF by default
    # - If running on a build agent (TeamCity) - Build documentation is ON by default
    # - If -DBUILD_DOCUMENTATION=ON/OFF flag is explicitly set, use it.
    set(BUILD_DOCUMENTATION_DEFAULT_FLAG OFF)
    if(DEFINED ENV{TEAMCITY_VERSION})
        set(BUILD_DOCUMENTATION_DEFAULT_FLAG ON)
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

        # Regenerate with source changes
        add_custom_command(OUTPUT ${DOXYGEN_INDEX_FILE}
                DEPENDS ${_SOURCES}
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
                PYTHONPATH="$<TARGET_FILE_DIR:CarbonTrinityAudioAPI>\;${CMAKE_SOURCE_DIR}/python"
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

    if(USE_VCPKG)
        set(INSTALL_BIN_ONLY OFF CACHE BOOL "Do not Install configuration files meant to be read by projects using this project as a dependency (Cmake Config/ header files etc . . .). Set this to ON when installing this component to the perforce vendor folder")
        if(INSTALL_BIN_ONLY)
            # Install rule to ensure that our runtime files are in the expected, platform-specific folder
            install(
                    TARGETS CarbonTrinityAudioAPI
                    EXPORT CarbonTrinityAudioAPI Target
                    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
                    RUNTIME DESTINATION bin/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
            )
        else()
            # Install rule to ensure that our runtime and linker files are in the expected, platform-specific folders
            install(
                    TARGETS CarbonTrinityAudioAPI
                    EXPORT CarbonTrinityAudioAPITarget
                    CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
                    LIBRARY DESTINATION lib/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
                    ARCHIVE DESTINATION lib/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
                    RUNTIME DESTINATION bin/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
            )
            # Install rule for available public headers
            install(DIRECTORY include DESTINATION . FILES_MATCHING PATTERN "*.h")

            install(
                    EXPORT CarbonTrinityAudioAPITarget
                    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/CarbonTrinityAudioAPI
                    FILE CarbonTrinityAudioAPI.cmake)
            install(
                    FILES CarbonTrinityAudioAPIConfig.cmake
                    DESTINATION ${CMAKE_INSTALL_PREFIX}/share/CarbonTrinityAudioAPI)
        endif()
    else()
            # Install rule to ensure that our runtime and linker files are in the expected, platform-specific folders
        install(
                TARGETS CarbonTrinityAudioAPI
                EXPORT CarbonTrinityAudioAPITarget
                CONFIGURATIONS ${CMAKE_CONFIGURATION_TYPES}
                LIBRARY DESTINATION lib/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
                ARCHIVE DESTINATION lib/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
                RUNTIME DESTINATION bin/${CCP_PLATFORM}/${CCP_ARCHITECTURE}/${CCP_TOOLSET}/
        )
        # Install rule for available public headers
        install(FILES ${PUBLIC_HEADER_FILES} DESTINATION include)

        configure_ccp_vendor_config_file(
                TARGET CarbonTrinityAudioAPI 
                DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/CarbonTrinityAudioAPIConfig.cmake"
                DEPENDENCIES ""
        )
        install(FILES ${CMAKE_CURRENT_BINARY_DIR}/CarbonTrinityAudioAPIConfig.cmake DESTINATION .)

        # Install python package
        install(DIRECTORY python/ DESTINATION python
                FILES_MATCHING
                PATTERN "*.py"
                PATTERN "package.yaml"
        )
    endif()
endif()
