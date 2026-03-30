# Dependency Management Utils (DMU)
#
# CMake

# make a dependency available for this cmake project
function(depends)
    set(options

        # use the CMake FindPackage functionality (NO_MODULE)
        PACKAGE
        # use the CMake Module FindPackage
        MODULE
        # import precompiled libraries
        PRECOMPILED
        # header only (INTERFACE)
        HEADERONLY

        # package is whether found from source or from install paths
        FROM_SOURCE

        # download files or clone from URL and use as PACKAGE
        FETCH

        # add_subdirectory
        # the subdirectory can either be a git submodule, a sub folder or a completely external folder
        # according to the CMake documentation, the external folder needs a build directory specified
        SUBDIRECTORY
        SUBMODULE

        STATIC # does nothing
        SHARED # default to STATIC
        # configure files (https://cmake.org/cmake/help/latest/command/configure_file.html)
        CONFIGURE_DEPENDS
    )
    set(keywords

        # package source directory
        DIRECTORY
        # package configuration file (*Config.cmake, *-config.cmake)
        CONFIG
        # package required version
        VERSION

        # generated target name
        OUTPUT_TARGET
        # libraries debug suffix (*d.lib)
        DEBUG_SUFFIX

        # link from which to download an archive or installation files
        DL_URL
        # link from which to clone a source repository
        GIT_URL
    )
    set(multikeywords

        # libraries path
        DIRECTORIES
        # libraries name
        FILES
        # include directories
        INCLUDE_PATHS
        # optional components
        COMPONENTS
        # optional additional dependencies
        ADDITIONAL_DEPENDENCIES
        # files to be copied (alias ADDITIONAL_DEPENDENCIES)
        CONFIGURE_FILES
    )
    cmake_parse_arguments(arg
        "${options}"
        "${keywords}"
        "${multikeywords}"
        ${ARGN}
    )

    if (${arg_PACKAGE})
        list(APPEND arg_ADDITIONAL_DEPENDENCIES ${arg_CONFIGURE_FILES})
        depends_package("${arg_FROM_SOURCE}"
            "${arg_DIRECTORY}"
            "${arg_CONFIG}"
            "${arg_VERSION}"
            "${arg_COMPONENTS}"
            "${arg_ADDITIONAL_DEPENDENCIES}"
        )
    elseif (${arg_MODULE})
        depends_module("${arg_CONFIG}"
            "${arg_VERSION}"
)
    elseif (${arg_PRECOMPILED})
        depends_precompiled("${arg_SHARED}"
            "${arg_CONFIGURE_DEPENDS}"
            "${arg_OUTPUT_TARGET}"
            "${arg_DIRECTORIES}"
            "${arg_FILES}"
            "${arg_DEBUG_SUFFIX}"
            "${arg_INCLUDE_PATHS}"
            "${arg_ADDITIONAL_DEPENDENCIES}"
        )
    elseif (${arg_HEADERONLY})
        depends_headeronly("${arg_DIRECTORY}"
            "${arg_OUTPUT_TARGET}"
        )
    elseif (${arg_FETCH})
        depends_fetch("${arg_DL_URL}" "${arg_GIT_URL}")
    elseif (${arg_SUBDIRECTORY})
        depends_subdirectory("${arg_DIRECTORY}")
    elseif (${arg_SUBMODULE})
        depends_subdirectory("${arg_DIRECTORY}")
    else()
        message(FATAL_ERROR "A dependency type must be specified")
    endif()
endfunction()



# package based external dependency
# should include a *Config.cmake or *-config.cmake file
#
# Based on nvpro_core2 integration
# https://github.com/nvpro-samples/nvpro_core2
function(depends_package
    FROM_SOURCE
    SOURCE_DIR
    CONFIG_NAME
    PACKAGE_VERSION
    COMPONENTS
    ADDITIONAL_DEPENDENCIES
)
    string(TOLOWER ${CONFIG_NAME} CONFIG_NAME_LOWER)

    if (FROM_SOURCE AND NOT SOURCE_DIR)
        message(FATAL_ERROR "Source directory must be specified with DIRECTORY keyword if FROM_SOURCE option is enabled")
    endif()

    if (NOT FROM_SOURCE)
        # if FROM_SOURCE is not specified, the dependency is mostly likely installed or
        # near the default paths (environment PATH, CMake paths, ...)

        set(SOURCE_DIR ${CONFIG_NAME})
        message("\nLooking for ${CONFIG_NAME} in default PATHs...")

    else()
        # find the dependency sources near the current CMakeLists.txt

        cmake_path(IS_ABSOLUTE SOURCE_DIR IS_ABS)
        if (NOT ${IS_ABS})
            message("\nLooking for ${SOURCE_DIR} near ${CMAKE_SOURCE_DIR}...")
            find_path(${SOURCE_DIR}-DIR
                NAMES .
                PATHS ${CMAKE_CURRENT_LIST_DIR}/${SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/${SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/../${SOURCE_DIR}
                    ${CMAKE_SOURCE_DIR}/../../${SOURCE_DIR}
                NO_DEFAULT_PATH
                NO_PACKAGE_ROOT_PATH
                NO_CMAKE_PATH
                NO_CMAKE_ENVIRONMENT_PATH
                NO_SYSTEM_ENVIRONMENT_PATH
                NO_CMAKE_SYSTEM_PATH
                NO_CMAKE_INSTALL_PREFIX
                REQUIRED
            )
            message("${SOURCE_DIR} was found : ${${SOURCE_DIR}-DIR}")
        else()
            message("\nLooking for ${CONFIG_NAME} in ${SOURCE_DIR}...")
            set(${SOURCE_DIR}-DIR ${SOURCE_DIR})
            message("${SOURCE_DIR} is absolute : ${${SOURCE_DIR}-DIR}")
        endif()


        # find a CMake config file in order to use find_package in package mode instead of module

        file(GLOB_RECURSE CONFIG_FILES
            "${${SOURCE_DIR}-DIR}/**/${CONFIG_NAME}Config.cmake"
            "${${SOURCE_DIR}-DIR}/**/${CONFIG_NAME_LOWER}-config.cmake"
            )
        if (NOT CONFIG_FILES)
            message(FATAL_ERROR "Failed to find ${CONFIG_NAME}Config.cmake or ${CONFIG_NAME_LOWER}-config.cmake in ${${SOURCE_DIR}-DIR}")
        endif()
        list(LENGTH CONFIG_FILES CONFIG_FILES_LENGTH)
        message("Found ${CONFIG_FILES_LENGTH} config files :")
        foreach(FILE IN LISTS CONFIG_FILES)
            message("\t- ${FILE}")
        endforeach()
        list(GET CONFIG_FILES 0 CONFIG_FILE)
        message("Using Config file : " ${CONFIG_FILE})


        cmake_path(REMOVE_FILENAME CONFIG_FILE OUTPUT_VARIABLE CONFIG_PATH)
        set(${CONFIG_NAME}_DIR INTERNAL FORCE ${CONFIG_PATH})
        set(${CONFIG_NAME}_ROOT ${CONFIG_PATH})

    endif()
    
    if (COMPONENTS)
        find_package(${CONFIG_NAME} ${PACKAGE_VERSION} COMPONENTS ${COMPONENTS} REQUIRED NO_MODULE GLOBAL)
        set(${CONFIG_NAME}-COMPLETE)
        foreach(COMP IN LISTS COMPONENTS)
            list(APPEND ${CONFIG_NAME}-COMPLETE "${CONFIG_NAME}::${COMP}")
        endforeach()
        set(${CONFIG_NAME}-COMPLETE ${${CONFIG_NAME}-COMPLETE} CACHE INTERNAL "${CONFIG_NAME} package variable with all the specified components")
        message("The variable ${CONFIG_NAME}-COMPLETE can be used to link all the components at once")
    else()
        find_package(${CONFIG_NAME} ${PACKAGE_VERSION} REQUIRED NO_MODULE GLOBAL)
    endif()
    
    message("${CONFIG_NAME} is ready to link : see ${SOURCE_DIR} documentation to get the target name\n")
    
    message("Defined variables :")
    set("${CONFIG_NAME}_VARS" "")
    dump_cmake_variables("${CONFIG_NAME}" ${CONFIG_NAME}_VARS)
    # save variables to cache
    foreach(VAR IN LISTS ${CONFIG_NAME}_VARS)
        set("${VAR}" "${${VAR}}" CACHE INTERNAL "${${VAR}}")
    endforeach()

    foreach (ADD_LIB IN LISTS ADDITIONAL_DEPENDENCIES)
        file(GLOB_RECURSE LIB_PATH ${${SOURCE_DIR}-DIR}/${ADD_LIB})
        configure_file(${LIB_PATH} ${CMAKE_BINARY_DIR}/${ADD_LIB} COPYONLY)
    endforeach()

endfunction()




# module based external dependency
function(depends_module
    CONFIG_NAME
    PACKAGE_VERSION
)
    find_package(${CONFIG_NAME} ${PACKAGE_VERSION} REQUIRED GLOBAL)
    message("Defined variables :")
    set("${CONFIG_NAME}_VARS" "")
    dump_cmake_variables("${CONFIG_NAME}" ${CONFIG_NAME}_VARS)
endfunction()




# precompiled library
# default to STATIC
# look for FOLDER_NAMES in default path and cmake paths if not specified
function(depends_precompiled
    SHARED
    CONFIGURE_DEPENDS
    TARGET_NAME
    FOLDER_NAMES
    LIB_FILES
    DEBUG_SUFFIX
    INCLUDE_PATHS
    ADDITIONAL_DEPENDENCIES
)
    # extract variables from arguments
    list(LENGTH LIB_FILES LIB_FILES_LENGTH)
    list(LENGTH FOLDER_NAMES FOLDER_NAMES_LENGTH)
    if (${LIB_FILES_LENGTH} GREATER 0)
        list(GET FOLDER_NAMES 0 FOLDER_NAME)
        if (FOLDER_NAMES_LENGTH GREATER 1)
            list(GET FOLDER_NAMES 1 SHARED_FOLDER_NAME)
        endif()
        list(GET LIB_FILES 0 LIB_FILE)
        if (LIB_FILES_LENGTH GREATER 1)
            list(GET LIB_FILES 1 SHARED_LIB_FILE)
        endif()
    else()
        message(FATAL_ERROR "At least one directory or file name must be specified")
    endif()


    # find the library folder

    find_lib_directory(${FOLDER_NAME})

    message("${FOLDER_NAME}-DIR : ${${FOLDER_NAME}-DIR}")

    # do nothing if target already exists
    if (TARGET ${TARGET_NAME})
        message("${TARGET_NAME} already exists and is ready to link\n")
        return()
    endif()
    
    # adding a pre compiled static/shared library (interface)
    # https://cmake.org/cmake/help/latest/command/add_library.html#imported-libraries
    if (NOT ${SHARED})
        add_library(${TARGET_NAME} STATIC IMPORTED GLOBAL)
    else()
        add_library(${TARGET_NAME} SHARED IMPORTED GLOBAL)
    endif()
    set_target_properties(${TARGET_NAME} PROPERTIES
        LINKER_LANGUAGE CXX
    )

    # include paths can be specified if the headers are somewhat hidden

    if (INCLUDE_PATHS)
        cmake_path(IS_ABSOLUTE INCLUDE_PATHS IS_ABS)
        if (${IS_ABS})
            target_include_directories(${TARGET_NAME} INTERFACE ${INCLUDE_PATHS})
        else()
            foreach(path IN LISTS INCLUDE_PATHS)
                target_include_directories(${TARGET_NAME} INTERFACE ${${FOLDER_NAME}-DIR}/${path})
            endforeach()
        endif()
    else()
        target_include_directories(${TARGET_NAME} INTERFACE ${${FOLDER_NAME}-DIR})
    endif()
    get_target_property(SHOW_INCLUDE_PATHS ${TARGET_NAME} INTERFACE_INCLUDE_DIRECTORIES)
    message("${TARGET_NAME} include directories : ${SHOW_INCLUDE_PATHS}")

    if (NOT CONFIGURE_DEPENDS)
        target_link_directories(${TARGET_NAME} INTERFACE ${${FOLDER_NAME}-DIR})
    endif()


    # find the static library file in Debug configuration (take one among a list if multiple libraries have been found)

    file(GLOB_RECURSE LIBD_NAMES "${${FOLDER_NAME}-DIR}/${LIB_FILE}${DEBUG_SUFFIX}.${IMPLIB_EXTENSION}")
    if (NOT LIBD_NAMES)
        file(GLOB_RECURSE LIBD_NAMES "${${FOLDER_NAME}-DIR}/**/${LIB_FILE}${DEBUG_SUFFIX}.${IMPLIB_EXTENSION}")
    endif()
    if (NOT LIBD_NAMES)
        message(FATAL_ERROR "Failed to find ${LIB_FILE}${DEBUG_SUFFIX}.${IMPLIB_EXTENSION} in ${${FOLDER_NAME}-DIR}")
    endif()

    list(LENGTH LIBD_NAMES LIBD_LENGTH)
    message("Found ${LIBD_LENGTH} Debug static libraries :")
    foreach(LIB IN LISTS LIBD_NAMES)
        message("\t- ${LIB}")
    endforeach()
    list(GET LIBD_NAMES 0 LIBD_NAME)
    message("Using : ${LIBD_NAME}")

    cmake_path(REMOVE_FILENAME LIBD_NAME OUTPUT_VARIABLE LIBD_PATH)
    target_link_directories(${TARGET_NAME} INTERFACE ${LIBD_PATH})


    # find the static library file in Release configuration (take on among a list if multiple libraries have been found)

    file(GLOB_RECURSE LIB_NAMES "${${FOLDER_NAME}-DIR}/${LIB_FILE}.${IMPLIB_EXTENSION}")
    if (NOT LIB_NAMES)
        file(GLOB_RECURSE LIB_NAMES "${${FOLDER_NAME}-DIR}/**/${LIB_FILE}.${IMPLIB_EXTENSION}")
    endif()
    if (NOT LIB_NAMES)
        message(FATAL_ERROR "Failed to find ${LIB_FILE}.${IMPLIB_EXTENSION} in ${${FOLDER_NAME}-DIR}")
    endif()

    list(LENGTH LIB_NAMES LIBD_LENGTH)
    message("Found ${LIBD_LENGTH} Release static libraries :")
    foreach(LIB IN LISTS LIB_NAMES)
        message("\t- ${LIB}")
    endforeach()
    list(GET LIB_NAMES 0 LIB_NAME)
    message("Using : ${LIB_NAME}")

    cmake_path(REMOVE_FILENAME LIB_NAME OUTPUT_VARIABLE LIB_PATH)
    target_link_directories(${TARGET_NAME} INTERFACE ${LIB_PATH})

    
    # see the documentation for imported libraries
    # https://cmake.org/cmake/help/latest/command/add_library.html#imported-libraries
    if (NOT ${SHARED})
        message("Found static Debug library : ${LIBD_NAME}")
        message("Found static library : ${LIB_NAME}")
        set_target_properties(${TARGET_NAME} PROPERTIES
            IMPORTED_LOCATION_DEBUG "${LIBD_NAME}"
            IMPORTED_LOCATION_RELEASE "${LIB_NAME}"
        )

    else()
        set_target_properties(${TARGET_NAME} PROPERTIES
            IMPORTED_IMPLIB_DEBUG "${LIBD_NAME}"
            IMPORTED_IMPLIB_RELEASE "${LIB_NAME}"
        )

        if (${FOLDER_NAMES_LENGTH} EQUAL 2)
            list(LENGTH LIB_FILES LIB_FILES_LENGTH)
            if (${LIB_FILES_LENGTH} LESS 2)
                message(FATAL_ERROR "Two paths were given but only 1 library names (or 0)")
            endif()

            set_target_properties(${TARGET_NAME} PROPERTIES
                IMPORTED_LOCATION_DEBUG "${SHARED_FOLDER_NAME}/${SHARED_LIB_FILE}${DEBUG_SUFFIX}.${DYNLIB_EXTENSION}"
                IMPORTED_LOCATION_RELEASE "${SHARED_FOLDER_NAME}/${SHARED_LIB_FILE}.${DYNLIB_EXTENSION}"
            )
            
            if (CONFIGURE_DEPENDS)
                configure_file(${SHARED_FOLDER_NAME}/${SHARED_LIB_FILE}.${DYNLIB_EXTENSION} ${CMAKE_BINARY_DIR}/${SHARED_LIB_FILE}.${DYNLIB_EXTENSION} COPYONLY)
            endif()

            foreach (ADD_LIB IN LISTS ADDITIONAL_DEPENDENCIES)
                configure_file(${SHARED_FOLDER_NAME}/${ADD_LIB} ${CMAKE_BINARY_DIR}/${ADD_LIB} COPYONLY)
                install(FILES ${SHARED_FOLDER_NAME}/${ADD_LIB} TYPE BIN)
            endforeach()

            install(FILES ${SHARED_FOLDER_NAME}/${SHARED_LIB_FILE}.${DYNLIB_EXTENSION} TYPE BIN)

        else()
            # find the dynamic library file in Debug configuration (take on among a list if multiple libraries have been found)

            file(GLOB_RECURSE SHARED_LIBD_NAMES "${${FOLDER_NAME}-DIR}/${LIB_FILE}${DEBUG_SUFFIX}.${DYNLIB_EXTENSION}")
            if (NOT SHARED_LIBD_NAMES)
                file(GLOB_RECURSE SHARED_LIBD_NAMES "${${FOLDER_NAME}-DIR}/**/${LIB_FILE}${DEBUG_SUFFIX}.${DYNLIB_EXTENSION}")
            endif()
            if (NOT SHARED_LIBD_NAMES)
                message(FATAL_ERROR "Failed to find ${LIB_FILE}${DEBUG_SUFFIX}.${DYNLIB_EXTENSION} in ${${FOLDER_NAME}-DIR}")
            endif()

            list(LENGTH SHARED_LIBD_NAMES LIBD_LENGTH)
            message("Found ${LIBD_LENGTH} Debug dynamic libraries :")
            foreach(LIB IN LISTS SHARED_LIBD_NAMES)
                message("\t- ${LIB}")
            endforeach()
            list(GET SHARED_LIBD_NAMES 0 SHARED_LIBD_NAME)
            message("Using : ${SHARED_LIBD_NAME}")


            # find the dynamic library file in Release configuration (take on among a list if multiple libraries have been found)

            file(GLOB_RECURSE SHARED_LIB_NAMES "${${FOLDER_NAME}-DIR}/${LIB_FILE}.${DYNLIB_EXTENSION}")
            if (NOT SHARED_LIB_NAMES)
                file(GLOB_RECURSE SHARED_LIB_NAMES "${${FOLDER_NAME}-DIR}/**/${LIB_FILE}.${DYNLIB_EXTENSION}")
            endif()
            if (NOT SHARED_LIB_NAMES)
                message(FATAL_ERROR "Failed to find ${LIB_FILE}.${DYNLIB_EXTENSION} in ${${FOLDER_NAME}-DIR}")
            endif()

            list(LENGTH SHARED_LIB_NAMES LIBD_LENGTH)
            message("Found ${LIBD_LENGTH} Release dynamic libraries :")
            foreach(LIB IN LISTS SHARED_LIB_NAMES)
                message("\t- ${LIB}")
            endforeach()
            list(GET SHARED_LIB_NAMES 0 SHARED_LIB_NAME)
            message("Using : ${SHARED_LIB_NAME}")

            # only the dynamic library in Release configuration will be configured
            if (CONFIGURE_DEPENDS)
                configure_file(${SHARED_LIB_NAME} ${CMAKE_BINARY_DIR}/${LIB_FILE}.${DYNLIB_EXTENSION} COPYONLY)
            endif()

            foreach (ADD_LIB IN LISTS ADDITIONAL_DEPENDENCIES)
                get_filename_component(LIB_DIR SHARED_LIB_NAME DIRECTORY)
                configure_file(${LIB_DIR}/${ADD_LIB} ${CMAKE_BINARY_DIR}/${ADD_LIB} COPYONLY)
                install(FILES ${LIB_DIR}/${ADD_LIB} TYPE BIN)
            endforeach()

            install(FILES ${SHARED_LIB_NAME} TYPE BIN)

        endif()
        
    endif()

    set_target_properties(${TARGET_NAME} PROPERTIES
        MAP_IMPORTED_CONFIG_MINSIZEREL Release
        MAP_IMPORTED_CONFIG_RELWITHDEBINFO Release
    )

    message("${TARGET_NAME} is ready to be linked as ${TARGET_NAME}\n")

endfunction()



# header only library
function(depends_headeronly DIRECTORY OUTPUT_TARGET)
    # do nothing if target already exists
    if (TARGET ${OUTPUT_TARGET})
        message("${OUTPUT_TARGET} already exists and is ready to link\n")
        return()
    endif()

    find_lib_directory(${DIRECTORY} REQUIRED)

    # only header files, no source files
    add_library(${OUTPUT_TARGET} INTERFACE)
    target_include_directories(${OUTPUT_TARGET} INTERFACE ${${DIRECTORY}-DIR})

    message("${${DIRECTORY}-DIR} is ready to be linked as ${OUTPUT_TARGET}\n")
endfunction()



function(depends_fetch DL_URL GIT_URL)
    message("Cloning from ${GIT_URL}...")
    # TODO
endfunction()



function(depends_subdirectory DIR)

    # if DIR is not a subfolder, add an external subdirectory

    find_path(IN_TREE_DIR
        NAMES .
        PATHS ${CMAKE_CURRENT_LIST_DIR}/${DIR}
        NO_DEFAULT_PATH
        NO_PACKAGE_ROOT_PATH
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH
        NO_CMAKE_INSTALL_PREFIX
        OPTIONAL
    )
    if (NOT (${IN_TREE_DIR} STREQUAL "IN_TREE_DIR-NOTFOUND"))
        add_subdirectory(${DIR})
        return()
    endif()

    # https://cmake.org/cmake/help/latest/command/add_subdirectory.html
    find_path(OUT_TREE_DIR
        NAMES .
        PATHS ${CMAKE_SOURCE_DIR}/${DIR}
            ${CMAKE_SOURCE_DIR}/../${DIR}
            ${CMAKE_SOURCE_DIR}/../../${DIR}
        NO_DEFAULT_PATH
        NO_PACKAGE_ROOT_PATH
        NO_CMAKE_PATH
        NO_CMAKE_ENVIRONMENT_PATH
        NO_SYSTEM_ENVIRONMENT_PATH
        NO_CMAKE_SYSTEM_PATH
        NO_CMAKE_INSTALL_PREFIX
        REQUIRED
    )
    add_subdirectory(${OUT_TREE_DIR} ${CMAKE_CURRENT_BINARY_DIR}/${DIR})

endfunction()




function(find_lib_directory LIB_NAME)
    set(options REQUIRED OPTIONAL)
    set(keywords)
    set(multikeywords)
    cmake_parse_arguments(arg "${options}" "${keywords}" "${multikeywords}")

    # LIB_NAME is considered to be the name of the folder containing the said library
    set(FOLDER_NAME ${LIB_NAME})

    cmake_path(IS_ABSOLUTE FOLDER_NAME IS_ABS)

    # look for the directory in the absolute path
    if (${IS_ABS})
        message("\nLooking for ${LIB_NAME} in ${FOLDER_NAME}...")
        cmake_path(CONVERT "${FOLDER_NAME}" TO_CMAKE_PATH_LIST FOLDER_NAME NORMALIZE)
        find_path(${FOLDER_NAME}-DIR
            NAMES .
            PATHS ${FOLDER_NAME}
            NO_DEFAULT_PATH
            NO_PACKAGE_ROOT_PATH
            NO_CMAKE_PATH
            NO_CMAKE_ENVIRONMENT_PATH
            NO_SYSTEM_ENVIRONMENT_PATH
            NO_CMAKE_SYSTEM_PATH
            NO_CMAKE_INSTALL_PREFIX
            arg_REQUIRED
        )
    # look near the current top level workspace directory
    else()
        message("\nLooking for ${FOLDER_NAME} near ${CMAKE_SOURCE_DIR}...")
        find_path(${FOLDER_NAME}-DIR
            NAMES .
            PATHS ${CMAKE_CURRENT_LIST_DIR}/${FOLDER_NAME}
                ${CMAKE_SOURCE_DIR}/${FOLDER_NAME}
                ${CMAKE_SOURCE_DIR}/../${FOLDER_NAME}
                ${CMAKE_SOURCE_DIR}/../../${FOLDER_NAME}
            NO_DEFAULT_PATH
            NO_PACKAGE_ROOT_PATH
            NO_CMAKE_PATH
            NO_CMAKE_ENVIRONMENT_PATH
            NO_SYSTEM_ENVIRONMENT_PATH
            NO_CMAKE_SYSTEM_PATH
            NO_CMAKE_INSTALL_PREFIX
            arg_REQUIRED
        )
    endif()
        
    # in case the library has been installed, it can be found in the CMake default paths
    if (${${FOLDER_NAME}-DIR} STREQUAL "${FOLDER_NAME}-DIR-NOTFOUND")
        message("\nLooking for ${FOLDER_NAME} in default and cmake paths...")
        set(FOLDER_NAME ${TARGET_NAME})
        find_path(${FOLDER_NAME}-DIR
            NAMES ${FOLDER_NAME}
            arg_REQUIRED
        )
    endif()

    if ((${${FOLDER_NAME}-DIR} STREQUAL "${FOLDER_NAME}-DIR-NOTFOUND") AND arg_REQUIRED)
        message(FATAL_ERROR "Could not find the specified library")
    endif()
    
    set(${FOLDER_NAME}-DIR ${${FOLDER_NAME}-DIR} PARENT_SCOPE)

endfunction()



if (WIN32)
    set(IMPLIB_EXTENSION "lib")
    set(DYNLIB_EXTENSION "dll")
else()
    message(FATAL_ERROR "No other platforms supported yet")
    # TODO
endif()



# https://stackoverflow.com/a/9328525
function(dump_cmake_variables)
    get_cmake_property(_variableNames VARIABLES)
    list (SORT _variableNames)
    foreach (_variableName ${_variableNames})
        if (ARGV0)
            unset(MATCHED)
            string(REGEX MATCH ${ARGV0} MATCHED ${_variableName})
            if (NOT MATCHED)
                continue()
            endif()
        endif()
        message(STATUS "${_variableName}=${${_variableName}}")
        if (ARGV1)
            list(APPEND ${ARGV1} ${_variableName})
        endif()
    endforeach()
    if (ARGV1)
        set(${ARGV1} "${${ARGV1}}" PARENT_SCOPE)
    endif()
endfunction()