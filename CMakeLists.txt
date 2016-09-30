cmake_minimum_required(VERSION 2.6)
project(roller-painting)

set( GAME_ROOT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}" )

#-------------------------------------------------------------------------------
if( NOT DEFINED BEAR_ROOT_DIRECTORY )

  #The path to the directory containing the source code of the Bear engine.
  set(
    BEAR_ROOT_DIRECTORY
    "${CMAKE_CURRENT_SOURCE_DIR}/../bear"
    )
endif()

set( CMAKE_MODULE_PATH "${BEAR_ROOT_DIRECTORY}/cmake-helper" )

include( bear-config )
include( gettext )
include( compiler-defaults )
include( docbook-to-man )

#-------------------------------------------------------------------------------
# where the game resources are installed
if( NOT RP_INSTALL_DATA_DIR )
  set( RP_INSTALL_DATA_DIR share/super-great-park )
endif( NOT RP_INSTALL_DATA_DIR )

# where the game executables are installed
if( NOT RP_INSTALL_EXECUTABLE_DIR )
  set( RP_INSTALL_EXECUTABLE_DIR bin )
endif( NOT RP_INSTALL_EXECUTABLE_DIR )

# where the game libraries (the items) are installed
if( NOT RP_INSTALL_CUSTOM_LIBRARY_DIR )
  if( WIN32 )
    set( RP_INSTALL_CUSTOM_LIBRARY_DIR
      "${RP_INSTALL_EXECUTABLE_DIR}"
      )
  else( WIN32 )
    set( RP_INSTALL_CUSTOM_LIBRARY_DIR lib )
  endif( WIN32 )
endif( NOT RP_INSTALL_CUSTOM_LIBRARY_DIR )

# Directories where the executable and libraries are built
set( RP_EXECUTABLE_OUTPUT_PATH "${CMAKE_CURRENT_BINARY_DIR}/bin" )
set( RP_LIBRARY_OUTPUT_PATH "${RP_EXECUTABLE_OUTPUT_PATH}" )

set( EXECUTABLE_OUTPUT_PATH "${RP_EXECUTABLE_OUTPUT_PATH}" )
set( LIBRARY_OUTPUT_PATH "${RP_LIBRARY_OUTPUT_PATH}" )

#-------------------------------------------------------------------------------
# common flags
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )

#-------------------------------------------------------------------------------
# check SDL2
include( FindSDL2 )

if( NOT SDL2_FOUND )
  message( FATAL_ERROR "SDL2 library must be installed." )
else( NOT SDL2_FOUND )
  #-----------------------------------------------------------------------------
  # Include directories for SDL
  include_directories(
    ${SDL2_INCLUDE_DIR}
    )
endif( NOT SDL2_FOUND )

#-------------------------------------------------------------------------------
# various libraries

#-------------------------------------------------------------------------------
# Claw
find_package( libclaw 1.7.4 )

#-------------------------------------------------------------------------------
# Include directories for Claw
include_directories(
  ${CLAW_INCLUDE_DIRECTORY}
  ${CLAW_GRAPHIC_INCLUDE_DIRECTORY}
  )

#-------------------------------------------------------------------------------
# Link directories for Claw
link_directories(
  ${CLAW_LINK_DIRECTORY}
  ${CLAW_GRAPHIC_LINK_DIRECTORY}
  )

#-------------------------------------------------------------------------------
# Boost
include(FindBoost)

find_package( Boost 1.55 REQUIRED )

if( NOT Boost_FOUND )
  message( FATAL_ERROR 
    "You must have Boost library installed (at least 1.55)" )
endif( NOT Boost_FOUND )

#-------------------------------------------------------------------------------
# Include directories for Boost
include_directories(
  ${Boost_INCLUDE_DIR}
  )

#-------------------------------------------------------------------------------
# Link directories for Boost
link_directories(
  ${Boost_LIBRARY_DIRS}
  )

#-------------------------------------------------------------------------------
find_package( RapidJSON )

#-------------------------------------------------------------------------------
# Link directories for the game
link_directories(
  ${LIBRARY_OUTPUT_PATH}
  )

#-------------------------------------------------------------------------------
include_directories( "${BEAR_ENGINE_INCLUDE_DIRECTORY}" )
include_directories( "${BEAR_ITEMS_INCLUDE_DIRECTORY}" )

# Link directories for the Bear engine
link_directories(
  ${BEAR_ENGINE_LIBRARY_DIRECTORY}
  )

#-------------------------------------------------------------------------------
if( RP_TAG )
  add_definitions( "-DRP_TAG=${RP_TAG}" )
endif()

#-------------------------------------------------------------------------------
add_definitions( "-DRP_DEMO=0" )

#-------------------------------------------------------------------------------
if( BUILD_PLATFORM STREQUAL "android" )
  subdirs(
    lib
    data
    android
    desktop
    )
else()
  subdirs(
    launcher
    lib
    data
    release
    desktop
    )
endif()

option(
  RP_BEAR_FACTORY_ENABLED
  "Tells to install the files of Andy's Super Great Park for the Bear Factory"
  FALSE
  )

if ( RP_BEAR_FACTORY_ENABLED )
  install(
    DIRECTORY "lib/item-description"
    DESTINATION "share/bear-factory/super-great-park"
    FILE_PERMISSIONS OWNER_READ OWNER_WRITE
    GROUP_READ
    WORLD_READ
    DIRECTORY_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
    PATTERN "*~" EXCLUDE
  )
endif()

