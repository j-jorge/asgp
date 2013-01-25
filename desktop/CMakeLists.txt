install( DIRECTORY "icons" "pixmaps"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/share/"
  FILE_PERMISSIONS
  OWNER_READ OWNER_WRITE
  GROUP_READ
  WORLD_READ 
  DIRECTORY_PERMISSIONS  OWNER_READ OWNER_WRITE OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE )

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/applications/andy-super-great-park.desktop.in"
  "${CMAKE_CURRENT_BINARY_DIR}/applications/andy-super-great-park.desktop"
  IMMEDIATE @ONLY)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/applications/andy-super-great-park-demo.desktop.in"
  "${CMAKE_CURRENT_BINARY_DIR}/applications/andy-super-great-park-demo.desktop"
  IMMEDIATE @ONLY)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/menu/andy-super-great-park.in"
  "${CMAKE_CURRENT_BINARY_DIR}/menu/andy-super-great-park"
  IMMEDIATE @ONLY)

configure_file(
  "${CMAKE_CURRENT_SOURCE_DIR}/menu/andy-super-great-park-demo.in"
  "${CMAKE_CURRENT_BINARY_DIR}/menu/andy-super-great-park-demo"
  IMMEDIATE @ONLY)

install(
  FILES "${CMAKE_CURRENT_BINARY_DIR}/applications/andy-super-great-park.desktop"
  "${CMAKE_CURRENT_BINARY_DIR}/applications/andy-super-great-park-demo.desktop"
  DESTINATION "${CMAKE_INSTALL_PREFIX}/share/applications/"
  PERMISSIONS
  OWNER_READ OWNER_WRITE
  GROUP_READ
  WORLD_READ 
  )

subdirs(locale man)
