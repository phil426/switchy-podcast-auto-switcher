cmake_minimum_required(VERSION 3.16)
project(obs-auto-cam-switcher VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# Locate OBS Studio
# Strategy 1: system-installed (find_package)
# Strategy 2: OBS_SOURCE_DIR points to a cloned/extracted obs-studio tree
# ---------------------------------------------------------------------------
option(OBS_SOURCE_DIR "" "Path to obs-studio source (used when OBS is not installed)")

if(OBS_SOURCE_DIR)
  message(STATUS "[switchy] Using OBS from: ${OBS_SOURCE_DIR}")
  add_library(OBS::libobs INTERFACE IMPORTED)
  target_include_directories(OBS::libobs INTERFACE
    "${OBS_SOURCE_DIR}/libobs"
    "${OBS_SOURCE_DIR}"
  )
  add_library(OBS::obs-frontend-api INTERFACE IMPORTED)
  target_include_directories(OBS::obs-frontend-api INTERFACE
    "${OBS_SOURCE_DIR}/UI/obs-frontend-api"
    "${OBS_SOURCE_DIR}/libobs"
  )
  # On CI we skip linking (header-check build only) or link against system OBS
  # Real linking happens at OBS plugin load time; the .so/.dll is loaded by OBS
  # so undefined OBS symbols are resolved at runtime.
  set(OBS_SKIP_LINK TRUE)
else()
  find_package(libobs QUIET)
  find_package(obs-frontend-api QUIET)
  if(NOT libobs_FOUND)
    message(FATAL_ERROR
      "[switchy] OBS not found. Either:
"
      "  1. Install OBS Studio and ensure libobs is on the CMake path, OR
"
      "  2. Pass -DOBS_SOURCE_DIR=/path/to/obs-studio
"
    )
  endif()
endif()

# ---------------------------------------------------------------------------
# Qt (try Qt6, fall back to Qt5)
# ---------------------------------------------------------------------------
find_package(Qt6 COMPONENTS Core Widgets QUIET)
if(Qt6_FOUND)
  set(QT_NS Qt6)
else()
  find_package(Qt5 COMPONENTS Core Widgets QUIET)
  if(Qt5_FOUND)
    set(QT_NS Qt5)
  else()
    message(WARNING "[switchy] Qt not found — building without UI (headless mode).")
    set(QT_NS "")
  endif()
endif()

if(QT_NS)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)
  set(CMAKE_AUTORCC ON)
endif()

# ---------------------------------------------------------------------------
# Plugin sources
# ---------------------------------------------------------------------------
set(PLUGIN_SOURCES
  src/plugin-main.cpp
  src/audio-monitor.cpp
  src/switch-engine.cpp
  src/config.cpp
  src/utils.cpp
)

if(QT_NS)
  list(APPEND PLUGIN_SOURCES
    src/ui/dock-widget.cpp
    src/ui/settings-dialog.cpp
  )
endif()

add_library(obs-auto-cam-switcher MODULE ${PLUGIN_SOURCES})

target_include_directories(obs-auto-cam-switcher PRIVATE src src/ui)

target_link_libraries(obs-auto-cam-switcher PRIVATE OBS::libobs OBS::obs-frontend-api)
if(QT_NS)
  target_link_libraries(obs-auto-cam-switcher PRIVATE ${QT_NS}::Core ${QT_NS}::Widgets)
endif()

set_target_properties(obs-auto-cam-switcher PROPERTIES PREFIX "")

# ---------------------------------------------------------------------------
# Allow undefined OBS symbols when building against headers only (runtime link)
# ---------------------------------------------------------------------------
if(OBS_SKIP_LINK AND UNIX AND NOT APPLE)
  target_link_options(obs-auto-cam-switcher PRIVATE -Wl,--allow-shlib-undefined)
elseif(OBS_SKIP_LINK AND APPLE)
  target_link_options(obs-auto-cam-switcher PRIVATE -undefined dynamic_lookup)
endif()

# ---------------------------------------------------------------------------
# Install
# ---------------------------------------------------------------------------
if(APPLE)
  set(OBS_PLUGIN_DIR "$ENV{HOME}/Library/Application Support/obs-studio/plugins/obs-auto-cam-switcher")
elseif(WIN32)
  set(OBS_PLUGIN_DIR "$ENV{APPDATA}/obs-studio/plugins/obs-auto-cam-switcher")
else()
  set(OBS_PLUGIN_DIR "$ENV{HOME}/.config/obs-studio/plugins/obs-auto-cam-switcher")
endif()

install(TARGETS obs-auto-cam-switcher LIBRARY DESTINATION "${OBS_PLUGIN_DIR}/bin/64bit")
install(DIRECTORY data/ DESTINATION "${OBS_PLUGIN_DIR}/data")
