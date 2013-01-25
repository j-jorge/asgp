if( WIN32 )
  install(
    FILES asgp.ico ReadMe.txt ChangeLog
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_WRITE
    GROUP_READ
    WORLD_READ
    )
endif()
