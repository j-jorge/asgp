cmake_minimum_required(VERSION 2.6)
project(rp-android-launcher)

set( RP_ANDROID_LAUNCHER_TARGET_NAME andy-super-great-park )

include_directories( . ${BEAR_ENGINE_INCLUDE_DIRECTORY} )

#-------------------------------------------------------------------------------
set( RP_ANDROID_LAUNCHER_SOURCE_FILES
  code/android_logger.cpp
  code/android_resource_pool.cpp
  code/game.cpp
  code/main.cpp

  code/SDL_android_main.cpp
  )

add_library(
  ${RP_ANDROID_LAUNCHER_TARGET_NAME}
  SHARED
  ${RP_ANDROID_LAUNCHER_SOURCE_FILES} )

#-------------------------------------------------------------------------------
install(
  TARGETS ${RP_ANDROID_LAUNCHER_TARGET_NAME}
  DESTINATION ${RP_INSTALL_ANDROID_LAUNCHER_DIR}
  )

set(
  RP_ANDROID_LAUNCHER_LINK_LIBRARIES
  bear_engine
  ${CLAW_APPLICATION_LIBRARIES}
  boost_thread
  boost_regex
  boost_signals
  boost_filesystem
  boost_system
  )

if(WIN32)
  set(
    RP_ANDROID_LAUNCHER_LINK_LIBRARIES
    ${RP_ANDROID_LAUNCHER_LINK_LIBRARIES}
    ${SDL_LIBRARY}
    intl )
endif(WIN32)

link_directories( ${RP_ANDROID_LAUNCHER_LINK_DIRECTORIES} )

target_link_libraries(
  ${RP_ANDROID_LAUNCHER_TARGET_NAME}
  ${RP_ANDROID_LAUNCHER_LINK_LIBRARIES}
  intl
  GLESv1_CM
  GLESv2
  supc++
  gnustl_shared
  log
  )
