cmake_minimum_required(VERSION 3.0.0)
project(projectName)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -g")

set(MY_APP_NAME game)
set(MY_APP_VERSION 0.0.1)

set(EXECUTABLE_OUTPUT_PATH ./build/)

include(FindPkgConfig)

AUX_SOURCE_DIRECTORY(src SOURCES)

set(LIBS)

find_package(OpenGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIR})
set(LIBS ${LIBS} ${OPENGL_LIBRARIES})

INCLUDE(cmake/FindSDL2.cmake)
include_directories(${SDL2_INCLUDE_DIR})
set(LIBS ${LIBS} ${SDL2_LIBRARY})

INCLUDE(cmake/FindGLM.cmake)
include_directories(${GLM_INCLUDE_DIRS})
set(LIBS ${LIBS} ${GLM_LIBRARIES})

INCLUDE(cmake/FindGLEW.cmake)
include_directories(${GLEW_INCLUDE_PATH})
set(LIBS ${LIBS} ${GLEW_LIBRARY})

INCLUDE(cmake/FindSOIL.cmake)
include_directories(${SOIL_INCLUDE_DIRS})
set(LIBS ${LIBS} ${SOIL_LIBRARIES})

INCLUDE(cmake/FindASSIMP.cmake)
include_directories(${ASSIMP_INCLUDE_DIR})
set(LIBS ${LIBS} ${ASSIMP_LIBRARIES})

add_executable(
  ${MY_APP_NAME}
  MACOSX_BUNDLE
  ${SOURCES}
  )

target_link_libraries(${MY_APP_NAME} ${LIBS})

IF(APPLE)
  set(MACOSX_BUNDLE true)
  set(MACOSX_BUNDLE_BUNDLE_NAME ${MY_APP_NAME})
  set(MACOSX_BUNDLE_INFO_STRING "${MY_APP_NAME} ${MY_APP_VERSION}")
  set(MACOSX_BUNDLE_LONG_VERSION_STRING "${MY_APP_VERSION}")
  set(MACOSX_BUNDLE_SHORT_VERSION_STRING "${MY_APP_VERSION}")
  set(MACOSX_BUNDLE_BUNDLE_VERSION "${MY_APP_VERSION}")
  # set(MACOSX_BUNDLE_ICON_FILE "${PROJECT_SOURCE_DIR}/${MY_APP_NAME}.icns")

  exec_program("mkdir -p ${EXECUTABLE_OUTPUT_PATH}/${MY_APP_NAME}.app/Contents/Resources")
  exec_program("cp -r ${PROJECT_SOURCE_DIR}/resources/ ./build/game.app/Contents/Resources")
ENDIF(APPLE)
