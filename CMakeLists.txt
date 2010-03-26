# some project info
project(Sedna)
cmake_minimum_required(VERSION 2.6)

if (COMMAND cmake_policy)
    cmake_policy(SET CMP0003 NEW)
    cmake_policy(SET CMP0005 NEW)
endif (COMMAND cmake_policy)

# define CMAKE_VERSION if it isn't defined
if (NOT CMAKE_VERSION)
    set(CMAKE_VERSION "${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}.${CMAKE_PATCH_VERSION}")
    message(STATUS "CMAKE_VERSION was not defined, setting to ${CMAKE_VERSION}")
endif (NOT CMAKE_VERSION )
message(STATUS "CMake version is -- ${CMAKE_VERSION}")

# prohibit in-source-builds
if (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})
    message(STATUS "In-source-builds are not allowed")
    message(FATAL_ERROR "Create a separate build directory (e.g \"build\") and execute \"cmake <path_to_sedna_sources>\" from it")
endif (${CMAKE_BINARY_DIR} STREQUAL ${CMAKE_SOURCE_DIR})

# set default build type if it's supported
if (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)
   message(STATUS "Build type is not defined -- assuming Release")
   set(CMAKE_BUILD_TYPE Release CACHE STRING "Choose the type of build, options are: None(CMAKE_CXX_FLAGS or CMAKE_C_FLAGS used) Debug Release RelWithDebInfo MinSizeRel." FORCE)
endif (NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE)

# path to find additional cmake modules
set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

# display system name
message(STATUS "System name is determined -- ${CMAKE_SYSTEM_NAME}")
if (UNIX)
    message(STATUS "System is of UNIX type -- yes")
else (UNIX)
    message(STATUS "System is of UNIX type -- no")
endif (UNIX)

include(AddToConfig)

# Determine features and options to be used

option(EL_DEBUG "Debug logging" OFF)
option(RCV_TEST "Recovery testing facilities" OFF)
option(JAVA_DRIVER "Compile Sedna Java Driver" ON)
option(MAKE_DOC "Compile Documentation files" ON)
option(SQL_CONNECTION "SQL connection facilities" OFF)
option(STATIC_SYS_LIBS "Use static system libraries for some platforms (ignored on Darwin, SunOS, Windows)" OFF)
option(SE_ENABLE_GCOV "Enable gcov coverage tests" OFF)
option(ENABLE_DTSEARCH "Enable dtsearch full-text search integration" OFF)
option(ENABLE_FTSEARCH "Enable internal full-text search" ON)
option(ENABLE_TRIGGERS "Enable triggers support" ON)
set(ENHANCE_TERM Libedit CACHE STRING "Enhance terminal with completion, history etc. Possible values: Readline -- use GNU Readline library, Libedit -- use libedit, None -- Don't do anything")
option(GCC_USE_M32 "Enable 32-bit build on 64-bit architecture via -m32 flag (GCC only)" OFF)

# this contains path to dtsearch includes to build Sedna with dtsearch support
set(DTSEARCH_PREFIX "${CMAKE_SOURCE_DIR}/libs" CACHE PATH "Path to dtsearch sources to build Sedna with dtsearch support (dtsearch lib, commmon and includes are expected to be there)")

# static linking with MS via /MT[d]
option(MS_LINK_RUNTIME_STATIC "Enbales linking with MSVC runtime via /MT (ignored on non-Windows platforms)" ON)

# disable static linking feature on all platforms, except Linux and FreeBSD
if (STATIC_SYS_LIBS AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")
    message(STATUS "Disabling static linking on ${CMAKE_SYSTEM_NAME}")
    set(STATIC_SYS_LIBS FALSE CACHE BOOL "Use static system libraries for some platforms (ignored on Darwin, SunOS, Windows)" FORCE)
endif (STATIC_SYS_LIBS AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "Linux" AND NOT ${CMAKE_SYSTEM_NAME} MATCHES "FreeBSD")

# set SEDNA_BUILD and SEDNA_VERSION
if (EXISTS ${CMAKE_SOURCE_DIR}/ver AND NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/ver)
    file(READ ${CMAKE_SOURCE_DIR}/ver SEDNA_VERSION)
    message(STATUS "Sedna release version is -- ${SEDNA_VERSION}")
else (EXISTS ${CMAKE_SOURCE_DIR}/ver AND NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/ver)
    message(FATAL_ERROR "Cannot determine Sedna release version to build -- cannot find ${CMAKE_SOURCE_DIR}/ver file")
endif (EXISTS ${CMAKE_SOURCE_DIR}/ver AND NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/ver)

if (EXISTS ${CMAKE_SOURCE_DIR}/build AND NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
    file(READ ${CMAKE_SOURCE_DIR}/build SEDNA_BUILD)
    message(STATUS "Sedna build version is -- ${SEDNA_BUILD}")
else (EXISTS ${CMAKE_SOURCE_DIR}/build AND NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/build)
    message(STATUS "Sedna build version is undefined -- assume 0")
    set(SEDNA_BUILD 0)
endif (EXISTS ${CMAKE_SOURCE_DIR}/build AND NOT IS_DIRECTORY ${CMAKE_SOURCE_DIR}/build)

set(SEDNA_BUILD_VERSION ${SEDNA_VERSION}.${SEDNA_BUILD})
message(STATUS "Full Sedna version is -- ${SEDNA_BUILD_VERSION}")

# debug logging
if (EL_DEBUG)
    add_definitions(-DEL_DEBUG=1)
else (EL_DEBUG)
    add_definitions(-DEL_DEBUG=0)
endif (EL_DEBUG)

# static linking for all executables
if (STATIC_SYS_LIBS)
    message(STATUS "using static linking for executables -- yes")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
else (STATIC_SYS_LIBS)
    message(STATUS "using static linking for executables -- no")
endif (STATIC_SYS_LIBS)

# check for dtsearch prereqs
if (ENABLE_DTSEARCH)
    if (UNIX)
        find_library(Z_LIB z)
        if (Z_LIB)
            message(STATUS "Found zlib -- ${Z_LIB}")
        else (Z_LIB)
            message(STATUS "Cannot find zlib -- disabling dtsearch")
            set(ENABLE_DTSEARCH OFF)
        endif (Z_LIB)
    endif (UNIX)

    find_library(DTSEARCH_LIB NAMES dtsearch dten600 PATHS ${DTSEARCH_PREFIX}/dtsearch/lib)
    if (DTSEARCH_LIB)
        message(STATUS "Found dtsearch library -- ${DTSEARCH_LIB}")
    else (DTSEARCH_LIB)
        message(STATUS "Cannot find dtsearch library")
        set(ENABLE_DTSEARCH OFF)
    endif (DTSEARCH_LIB)

    find_path(DTSEARCH_COMMON dtsviewr.cpp PATHS ${DTSEARCH_PREFIX}/dtsearch/common)
    if (NOT DTSEARCH_COMMON)
        message(STATUS "Cannot find dtsearch common files -- disabling dtsearch")
        set(ENABLE_DTSEARCH OFF)
    else (NOT DTSEARCH_COMMON)
        message(STATUS "Found dtsearch common files -- ${DTSEARCH_COMMON}")
    endif (NOT DTSEARCH_COMMON)

    find_path(DTSEARCH_INCLUDES dtsearch/include/dt_util.h PATHS ${DTSEARCH_PREFIX})
    if (NOT DTSEARCH_INCLUDES)
        message(STATUS "Cannot find dtsearch includes -- disabling dtsearch")
        set(ENABLE_DTSEARCH OFF)
    else (NOT DTSEARCH_INCLUDES)
        message(STATUS "Found dtsearch includes in -- ${DTSEARCH_INCLUDES}")
    endif (NOT DTSEARCH_INCLUDES)
endif (ENABLE_DTSEARCH)

# add sedna features as macro-definitions in sources
if (ENABLE_DTSEARCH)
    add_definitions(-DSE_ENABLE_DTSEARCH)
    message(STATUS "dtsearch support -- enabled")
else (ENABLE_DTSEARCH)
    message(STATUS "dtsearch support -- disabled")
endif (ENABLE_DTSEARCH)

if (ENABLE_FTSEARCH)
    message(STATUS "ftsearch support -- enabled")
    add_definitions(-DSE_ENABLE_FTSEARCH)
else (ENABLE_FTSEARCH)
    message(STATUS "ftsearch support -- disabled")
endif (ENABLE_FTSEARCH)

if (ENABLE_TRIGGERS)
    message(STATUS "triggers support -- enabled")
    add_definitions(-DSE_ENABLE_TRIGGERS)
else (ENABLE_TRIGGERS)
    message(STATUS "triggers support -- disabled")
endif (ENABLE_TRIGGERS)

if (RCV_TEST)
    message(STATUS "recovery testing -- enabled")
    add_definitions(-DRCV_TEST_CRASH)
else (RCV_TEST)
    message(STATUS "recovery testing -- disabled")
endif (RCV_TEST)

# sql connection prereq -- odbc
if (SQL_CONNECTION AND UNIX)
    find_path(ODBC_HEADERS sql.h)
    if (NOT ODBC_HEADERS)
        message(STATUS "Cannot find ODBC headers -- disabling SQL Connection")
        set(SQL_CONNECTION OFF)
    else (NOT ODBC_HEADERS)
        message(STATUS "Found ODBC headers in -- ${ODBC_HEADERS}")
    endif (NOT ODBC_HEADERS)
endif (SQL_CONNECTION AND UNIX)

if (SQL_CONNECTION)
    message(STATUS "SQL connection feature -- enabled")
    add_definitions(-DSQL_CONNECTION)
else (SQL_CONNECTION)
    message(STATUS "SQL connection feature -- disabled")
endif (SQL_CONNECTION)

# don't use RPATH since we don't have an install target for now
set(CMAKE_SKIP_BUILD_RPATH ON)

# ACTIVE_CONFIGURATION is used by some source files (notable se_trn to enable/disable manual se_trn)
AddDefinitionDebRel(ACTIVE_CONFIGURATION \"Debug\" \"Release\")

# DEBUGI defines internal debugging ouput for some modules
AddDefinitionDebRel(DEBUGI 1 0)

add_definitions(-DSEDNA_VERSION="${SEDNA_VERSION}")
add_definitions(-DSEDNA_BUILD="${SEDNA_BUILD}")

# include directories
include_directories(${CMAKE_SOURCE_DIR}/kernel) # main relatieve include resolver
include_directories(${CMAKE_SOURCE_DIR}/driver/c) # for sp_defs.h
include_directories(${CMAKE_BINARY_DIR}/kernel) # for generated files
include_directories(${CMAKE_SOURCE_DIR}/libs) # for sedna additional libraries

# include stdint for MSVC compilers
if (MSVC)
    include_directories(${CMAKE_SOURCE_DIR}/libs/compat/stdint)
endif (MSVC)

# stdint definitions (see c++ standard)
add_definitions(-D__STDC_CONSTANT_MACROS)
add_definitions(-D__STDC_FORMAT_MACROS)
add_definitions(-D__STDC_LIMIT_MACROS)

# perform some platform specific definitions setup
include(CMakeDefinitions.cmake)

# perform some system libraries stuff configuration
include(CMakeLibraries.cmake)

# perform some compiler configuration
include(CMakeCompilers.cmake)

# custom script to build "tree"-like libraries
include(RecurseSrcs)

# script to configure binary tree
include(ConfigOutput)

# finally, start traversing project sources
# first, build some libraries
add_subdirectory(libs)

# import expat library and headers
include_directories(${CUSTOM_EXPAT_INCLUDE})
add_library(expat STATIC IMPORTED)
set_target_properties(expat PROPERTIES
    IMPORTED_LOCATION ${CUSTOM_EXPAT_LIB}/${CMAKE_STATIC_LIBRARY_PREFIX}expat${CMAKE_STATIC_LIBRARY_SUFFIX})

# then start bulduing Sedna
add_subdirectory(kernel)

# hot-backup utility
add_subdirectory(hbp)

# driver
add_subdirectory(driver)

# sedna terminal
add_subdirectory(term)

# export utility
add_subdirectory(export)

# docs
if (MAKE_DOC)
    add_subdirectory(doc)
endif (MAKE_DOC)

# configure some files
ConfigOutputTree()

# installing some additional files
install(FILES etc/sednaconf.xml.sample DESTINATION etc)

install(DIRECTORY examples/ DESTINATION examples PATTERN "*.sh" EXCLUDE PATTERN "*.bat" EXCLUDE)
if ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
    install(DIRECTORY examples/ DESTINATION examples FILES_MATCHING PATTERN "*.bat")
else ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")
    install(DIRECTORY examples/ DESTINATION examples FILES_MATCHING PATTERN "*.sh")
endif ("${CMAKE_SYSTEM_NAME}" MATCHES "Windows")

install(FILES "include/sedna_ef.h" DESTINATION include)
install(FILES "share/sedna_auth_md.xml" DESTINATION share)
install(FILES AUTHORS COPYRIGHT HISTORY LICENSE README DESTINATION .)

# data and cfg dirtectories
install(DIRECTORY DESTINATION data)
install(DIRECTORY DESTINATION cfg)