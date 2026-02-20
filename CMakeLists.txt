cmake_minimum_required(VERSION 3.16)
project(obs-auto-cam-switcher VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# OBS Studio
#   Option A (system install): find_package(libobs) — default
#   Option B (CI / source):    -DOBS_SOURCE_DIR=<path>  -DOBS_BUILD_DIR=<path>
#     OBS_BUILD_DIR is needed for obs-config.h, which cmake generates during
#     OBS's own configure step (it is NOT present in the source tree).
# ---------------------------------------------------------------------------
set(OBS_SOURCE_DIR "" CACHE PATH "obs-studio source tree")
set(OBS_BUILD_DIR  "" CACHE PATH "obs-studio cmake build dir (for obs-config.h)")

if(OBS_SOURCE_DIR)
  message(STATUS "[switchy] OBS source : ${OBS_SOURCE_DIR}")
  message(STATUS "[switchy] OBS build  : ${OBS_BUILD_DIR}")

  # --- libobs ---
  add_library(OBS::libobs INTERFACE IMPORTED)
  target_include_directories(OBS::libobs INTERFACE
    "${OBS_SOURCE_DIR}/libobs"
    "${OBS_SOURCE_DIR}"
  )

  # Locate obs-config.h in the build tree (cmake may place it in different
  # subdirs depending on version: config/, cmake/, or root)
  if(OBS_BUILD_DIR)
    file(GLOB_RECURSE _OBS_CONFIG_H "${OBS_BUILD_DIR}/obs-config.h"
                                     "${OBS_BUILD_DIR}/**/obs-config.h")
    if(_OBS_CONFIG_H)
      list(GET _OBS_CONFIG_H 0 _FIRST)
      get_filename_component(_CFG_DIR "${_FIRST}" DIRECTORY)
      message(STATUS "[switchy] obs-config.h : ${_FIRST}")
      target_include_directories(OBS::libobs INTERFACE "${_CFG_DIR}")
    else()
      message(WARNING "[switchy] obs-config.h not found — build may fail")
    endif()
  endif()

  # --- obs-frontend-api ---
  add_library(OBS::obs-frontend-api INTERFACE IMPORTED)
  target_include_directories(OBS::obs-frontend-api INTERFACE
    "${OBS_SOURCE_DIR}/UI/obs-frontend-api"
    "${OBS_SOURCE_DIR}/libobs"
  )

  set(OBS_SKIP_LINK TRUE)

else()
  find_package(libobs REQUIRED)
  find_package(obs-frontend-api REQUIRED)
endif()

# ---------------------------------------------------------------------------
# Qt (Qt6 → Qt5 → headless)
# ---------------------------------------------------------------------------
find_package(Qt6 COMPONENTS Core Widgets QUIET)
if(Qt6_FOUND)
  set(QT_NS Qt6)
else()
  find_package(Qt5 COMPONENTS Core Widgets QUIET)
  set(QT_NS $<IF:$<BOOL:${Qt5_FOUND}>,Qt5,"">)
  if(NOT Qt5_FOUND)
    message(WARNING "[switchy] Qt not found — skipping UI sources")
    set(QT_NS "")
  endif()
endif()

if(QT_NS)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)
  set(CMAKE_AUTORCC ON)
endif()

# ---------------------------------------------------------------------------
# Plugin target
# ---------------------------------------------------------------------------
set(SRCS
  src/plugin-main.cpp
  src/audio-monitor.cpp
  src/switch-engine.cpp
  src/config.cpp
  src/utils.cpp
)
if(QT_NS)
  list(APPEND SRCS src/ui/dock-widget.cpp src/ui/settings-dialog.cpp)
endif()

add_library(obs-auto-cam-switcher MODULE ${SRCS})
target_include_directories(obs-auto-cam-switcher PRIVATE src src/ui)
target_link_libraries(obs-auto-cam-switcher PRIVATE OBS::libobs OBS::obs-frontend-api)
if(QT_NS)
  target_link_libraries(obs-auto-cam-switcher PRIVATE ${QT_NS}::Core ${QT_NS}::Widgets)
endif()
set_target_properties(obs-auto-cam-switcher PROPERTIES PREFIX "")

# Runtime-link OBS symbols (plugin is loaded by OBS, not linked against it)
if(OBS_SKIP_LINK)
  if(APPLE)
    target_link_options(obs-auto-cam-switcher PRIVATE -undefined dynamic_lookup)
  elseif(UNIX)
    target_link_options(obs-auto-cam-switcher PRIVATE -Wl,--allow-shlib-undefined)
  endif()
endif()

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------
if(APPLE)
  set(_PLUG_DIR "$ENV{HOME}/Library/Application Support/obs-studio/plugins/obs-auto-cam-switcher")
elseif(WIN32)
  set(_PLUG_DIR "$ENV{APPDATA}/obs-studio/plugins/obs-auto-cam-switcher")
else()
  set(_PLUG_DIR "$ENV{HOME}/.config/obs-studio/plugins/obs-auto-cam-switcher")
endif()
install(TARGETS obs-auto-cam-switcher LIBRARY DESTINATION "${_PLUG_DIR}/bin/64bit")
install(DIRECTORY data/ DESTINATION "${_PLUG_DIR}/data")
