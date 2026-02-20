cmake_minimum_required(VERSION 3.16)
project(obs-auto-cam-switcher VERSION 1.0.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---------------------------------------------------------------------------
# OBS Studio
#   Option A (system install, e.g. installed via OBS PPA on Linux):
#       find_package(libobs) works automatically.
#   Option B (CI / source only):
#       -DOBS_SOURCE_DIR=/path/to/obs-studio
#       We include our bundled src/obs-config.h stub so no OBS cmake build needed.
# ---------------------------------------------------------------------------
set(OBS_SOURCE_DIR "" CACHE PATH "obs-studio source tree")

if(OBS_SOURCE_DIR)
  message(STATUS "[switchy] Building against OBS source headers: ${OBS_SOURCE_DIR}")

  add_library(OBS::libobs INTERFACE IMPORTED)
  target_include_directories(OBS::libobs INTERFACE
    "${OBS_SOURCE_DIR}/libobs"   # obs.h, obs-module.h, obs-data.h, blog.h …
    "${OBS_SOURCE_DIR}"          # fallback root
    "${CMAKE_SOURCE_DIR}/src"    # our bundled obs-config.h stub lives here
  )

  add_library(OBS::obs-frontend-api INTERFACE IMPORTED)
  target_include_directories(OBS::obs-frontend-api INTERFACE
    "${OBS_SOURCE_DIR}/UI/obs-frontend-api"
    "${OBS_SOURCE_DIR}/libobs"
    "${CMAKE_SOURCE_DIR}/src"
  )

  # Plugin resolves OBS symbols at load time — no link-time OBS libs needed
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
  if(Qt5_FOUND)
    set(QT_NS Qt5)
  else()
    message(WARNING "[switchy] Qt not found — building headless (no UI)")
    set(QT_NS "")
  endif()
endif()

if(QT_NS)
  set(CMAKE_AUTOMOC ON)
  set(CMAKE_AUTOUIC ON)
  set(CMAKE_AUTORCC ON)
endif()

# ---------------------------------------------------------------------------
# Sources
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

# Runtime-link OBS symbols (resolved at plugin load time by OBS)
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
  set(_PLUG "$ENV{HOME}/Library/Application Support/obs-studio/plugins/obs-auto-cam-switcher")
elseif(WIN32)
  set(_PLUG "$ENV{APPDATA}/obs-studio/plugins/obs-auto-cam-switcher")
else()
  set(_PLUG "$ENV{HOME}/.config/obs-studio/plugins/obs-auto-cam-switcher")
endif()
install(TARGETS obs-auto-cam-switcher LIBRARY DESTINATION "${_PLUG}/bin/64bit")
install(DIRECTORY data/ DESTINATION "${_PLUG}/data")
