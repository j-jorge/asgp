cmake_minimum_required(VERSION 2.6)
project(rp-android-launcher)

set( RP_ANDROID_LAUNCHER_TARGET_NAME libandy-super-great-park.so )

include_directories( . ${BEAR_ENGINE_INCLUDE_DIRECTORY} )

add_custom_command(
  OUTPUT ${RP_ANDROID_LAUNCHER_TARGET_NAME}
  COMMAND sh make.sh
  DEPENDS rp
  )
