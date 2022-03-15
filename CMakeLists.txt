cmake_minimum_required(VERSION 3.18)

project(cppwebsockets)
find_package(PkgConfig)
pkg_check_modules(LIB_WEBSOCKETS REQUIRED libwebsockets)

set(CMAKE_CXX_FLAGS -DLWS_NO_EXTENSIONS )
include_directories(${CMAKE_INSTALL_PREFIX}/include)

add_library(cppwebsockets
  Util.cpp
  Util.h
  WebSocketServer.cpp
  WebSocketServer.h)

target_link_libraries(cppwebsockets PUBLIC
                      ${LIB_WEBSOCKETS_LIBRARIES})

install(FILES Util.h WebSocketServer.h DESTINATION ${CMAKE_INSTALL_PREFIX}/include/cppwebsockets)
install(TARGETS cppwebsockets DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
