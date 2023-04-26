add_executable( UserClipPlane UserClipPlane_cube.c )
target_link_libraries( UserClipPlane Common )

configure_file(posx.tga ${CMAKE_CURRENT_BINARY_DIR}/posx.tga COPYONLY)
configure_file(posy.tga ${CMAKE_CURRENT_BINARY_DIR}/posy.tga COPYONLY)
configure_file(posz.tga ${CMAKE_CURRENT_BINARY_DIR}/posz.tga COPYONLY)
configure_file(negx.tga ${CMAKE_CURRENT_BINARY_DIR}/negx.tga COPYONLY)
configure_file(negy.tga ${CMAKE_CURRENT_BINARY_DIR}/negy.tga COPYONLY)
configure_file(negz.tga ${CMAKE_CURRENT_BINARY_DIR}/negz.tga COPYONLY)
