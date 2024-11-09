cmake_minimum_required(VERSION 3.7.0)

project(SpriteChat VERSION 2.12.0.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

option(KAL_SPRITECHAT_BUILD_TESTS "(${PROJECT_NAME}) Build tests" ON)
option(KAL_SPRITECHAT_ENABLE_DISCORD_RPC "(${PROJECT_NAME}) Enable Discord RPC" ON)

if(NOT KAL_BUILD_OUTPUT)
set(KAL_BUILD_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/bin")
endif()

find_package(QT NAMES Qt6)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Network Widgets Concurrent WebSockets UiTools)

if(NOT KAL_QT_VERSION)
set(KAL_QT_VERSION 6.5.3)
endif()

if(NOT Qt${QT_VERSION_MAJOR}_VERSION VERSION_EQUAL KAL_QT_VERSION)
message(FATAL_ERROR "Supported version is Qt ${KAL_QT_VERSION}, found: ${Qt${QT_VERSION_MAJOR}_VERSION}")
endif()

qt_add_executable(${PROJECT_NAME}
  ${PROJECT_NAME}.qrc
  src/animationlayer.cpp src/animationlayer.h
  src/animationloader.cpp src/animationloader.h
  src/aoapplication.cpp src/aoapplication.h
  src/aoblipplayer.cpp src/aoblipplayer.h
  src/aobutton.cpp src/aobutton.h
  src/aocharbutton.cpp src/aocharbutton.h
  src/aoclocklabel.cpp src/aoclocklabel.h
  src/aoemotebutton.cpp src/aoemotebutton.h
  src/aoemotepreview.cpp src/aoemotepreview.h
  src/aoevidencebutton.cpp src/aoevidencebutton.h
  src/aoevidencedisplay.cpp src/aoevidencedisplay.h
  src/aoimage.cpp src/aoimage.h
  src/aomusicplayer.cpp src/aomusicplayer.h
  src/aopacket.cpp src/aopacket.h
  src/aosfxplayer.cpp src/aosfxplayer.h
  src/aotextarea.cpp src/aotextarea.h
  src/aotextboxwidgets.cpp src/aotextboxwidgets.h
  src/aoutils.cpp src/aoutils.h
  src/charselect.cpp
  src/chatlogpiece.cpp src/chatlogpiece.h
  src/courtroom.cpp src/courtroom.h
  src/datatypes.h
  src/debug_functions.cpp src/debug_functions.h
  src/demoserver.cpp src/demoserver.h
  src/discord_rich_presence.cpp src/discord_rich_presence.h
  src/emotes.cpp
  src/eventfilters.cpp src/eventfilters.h
  src/evidence.cpp
  src/file_functions.cpp src/file_functions.h
  src/gui_utils.h
  src/hardware_functions.cpp src/hardware_functions.h
  src/lobby.cpp src/lobby.h
  src/main.cpp
  src/moderation_functions.cpp src/moderation_functions.h
  src/network/serverinfo.cpp src/network/serverinfo.h
  src/network/websocketconnection.cpp src/network/websocketconnection.h
  src/networkmanager.cpp src/networkmanager.h
  src/options.cpp src/options.h
  src/packet_distribution.cpp
  src/path_functions.cpp
  src/screenslidetimer.cpp src/screenslidetimer.h
  src/scrolltext.cpp src/scrolltext.h
  src/serverdata.cpp src/serverdata.h
  src/text_file_functions.cpp
  src/widgets/aooptionsdialog.cpp src/widgets/aooptionsdialog.h
  src/widgets/direct_connect_dialog.cpp src/widgets/direct_connect_dialog.h
  src/widgets/moderator_dialog.cpp src/widgets/moderator_dialog.h
  src/widgets/playerlistwidget.cpp src/widgets/playerlistwidget.h
  src/widgets/server_editor_dialog.cpp src/widgets/server_editor_dialog.h
)

if(WIN32)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/resource/icon.rc")
    target_sources(${PROJECT_NAME} PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
  endif()
endif()

target_include_directories(${PROJECT_NAME} PUBLIC src external)
target_link_directories(${PROJECT_NAME} PUBLIC external)
target_link_libraries(${PROJECT_NAME} PUBLIC
  Qt${QT_VERSION_MAJOR}::Core
  Qt${QT_VERSION_MAJOR}::Gui
  Qt${QT_VERSION_MAJOR}::Network
  Qt${QT_VERSION_MAJOR}::Widgets
  Qt${QT_VERSION_MAJOR}::Concurrent
  Qt${QT_VERSION_MAJOR}::WebSockets
  Qt${QT_VERSION_MAJOR}::UiTools
  bass
  bassopus
)

if(KAL_SPRITECHAT_ENABLE_DISCORD_RPC)
  target_compile_definitions(${PROJECT_NAME} PRIVATE KAL_SPRITECHAT_ENABLE_DISCORD_RPC)
  target_link_libraries(${PROJECT_NAME} PUBLIC discord-rpc)
endif()

set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_NAME "${PROJECT_NAME}")
set_target_properties(${PROJECT_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY $<1:${KAL_BUILD_OUTPUT}>)

if(WIN32)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/resource/${PROJECT_NAME}.rc")
    target_sources(${PROJECT_NAME} PRIVATE ${APP_ICON_RESOURCE_WINDOWS})
  endif()
endif()

if(KAL_SPRITECHAT_BUILD_TESTS)
  message(WARNING "${PROJECT_NAME} tests are not yet available.")
endif()
