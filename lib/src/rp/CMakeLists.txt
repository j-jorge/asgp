cmake_minimum_required(VERSION 2.6)
project(rp)

set( RP_TARGET_NAME rp )

include_directories(
  "${CMAKE_CURRENT_SOURCE_DIR}/.."
  ${BEAR_ENGINE_INCLUDE_DIRECTORY}
  ${BEAR_ITEMS_INCLUDE_DIRECTORY}
  )

link_directories(
  ${BEAR_ENGINE_LINK_DIRECTORY}
  )

#-------------------------------------------------------------------------------
set( RP_SOURCE_FILES
  code/add_ingame_layers.cpp
  code/attractable_item.cpp
  code/balloon.cpp
  code/bird.cpp
  code/bird_support.cpp
  code/bomb.cpp
  code/bonus.cpp
  code/boss.cpp
  code/boss_controller.cpp
  code/boss_teleport.cpp
  code/cable.cpp
  code/callback_queue.cpp
  code/cannonball.cpp
  code/cart_controller.cpp
  code/cart.cpp
  code/client_config.cpp
  code/config_file.cpp
  code/config_save.cpp
  code/crate.cpp
  code/cursor.cpp
  code/decorative_balloon.cpp
  code/end.cpp
  code/entity.cpp
  code/explosion.cpp
  code/event_tagger.cpp
  code/game_key.cpp
  code/game_variables.cpp
  code/help_button.cpp
  code/hole.cpp
  code/http_request.cpp
  code/init.cpp
  code/interactive_item.cpp
  code/level_exit.cpp
  code/level_scheduler.cpp
  code/level_selector.cpp
  code/level_settings.cpp
  code/level_variables.cpp
  code/obstacle.cpp
  code/pause_game.cpp
  code/plank.cpp
  code/plunger.cpp
  code/serial_switcher.cpp
  code/show_key_layer.cpp
  code/switching.cpp
  code/tar.cpp
  code/tnt.cpp
  code/util.cpp
  code/wall.cpp
  code/zeppelin.cpp

  ad/code/show_interstitial.cpp

  android/code/back_button_home_item.cpp

  events/code/tag_event.cpp
  events/code/tag_level_event.cpp
  
  layer/code/help_layer.cpp
  layer/code/key_layer.cpp
  layer/code/misc_layer.cpp
  layer/code/pause_layer.cpp
  layer/code/status_layer.cpp
  layer/status/code/background_component.cpp
  layer/status/code/balloon_component.cpp
  layer/status/code/bonus_component.cpp
  layer/status/code/boss_component.cpp
  layer/status/code/cannonball_component.cpp
  layer/status/code/lives_component.cpp
  layer/status/code/floating_score_component.cpp
  layer/status/code/plunger_component.cpp
  layer/status/code/score_component.cpp
  layer/status/code/status_component.cpp
  layer/status/code/time_component.cpp
  
  message/code/help_layer_starting_message.cpp
  message/code/key_layer_starting_message.cpp
  message/code/pause_message.cpp

  power_up/code/has_extra_plungers.cpp
  
  transition_effect/code/level_ending_effect.cpp
  transition_effect/code/level_starting_effect.cpp
  )

if( BUILD_PLATFORM STREQUAL "android" )
  set(
    RP_SOURCE_FILES
    ${RP_SOURCE_FILES}
    
    android/code/get_activity_method.cpp
    android/code/java_activity.cpp
    android/code/jni_call_bridge.cpp
    android/code/method_info.cpp
    android/code/native_call.cpp
    )

  set( RP_LINK_TYPE STATIC )
else()
  set( RP_LINK_TYPE SHARED )
endif()

add_library( ${RP_TARGET_NAME} ${RP_LINK_TYPE} ${RP_SOURCE_FILES} )

install(
  TARGETS ${RP_TARGET_NAME}
  DESTINATION ${RP_INSTALL_CUSTOM_LIBRARY_DIR}
  )

target_link_libraries(
  ${RP_TARGET_NAME}
  bear_engine
  bear_gui
  bear_generic_items
  ${Boost_THREAD_LIBRARY}
  ${CLAW_CONFIGURATION_FILE_LIBRARIES}
  )
