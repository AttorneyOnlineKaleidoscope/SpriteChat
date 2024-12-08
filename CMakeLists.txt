cmake_minimum_required(VERSION 3.7.0)

project(SpriteChat VERSION 2.12.0 LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

if(NOT KAL_BUILD_OUTPUT)
set(KAL_BUILD_OUTPUT "${CMAKE_CURRENT_SOURCE_DIR}/bin")
endif()

option(KAL_SPRITECHAT_BUILD_TESTS "(${PROJECT_NAME}) Build tests" ON)
option(KAL_SPRITECHAT_ENABLE_DISCORD_RPC "(${PROJECT_NAME}) Enable Discord RPC" ON)

find_package(QT NAMES Qt6)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Gui Network Widgets Concurrent WebSockets UiTools)

if(NOT KAL_QT_VERSION)
set(KAL_QT_VERSION 6.5.3)
endif()

if(NOT Qt${QT_VERSION_MAJOR}_VERSION VERSION_EQUAL KAL_QT_VERSION)
message(FATAL_ERROR "Supported version is Qt ${KAL_QT_VERSION}, found: ${Qt${QT_VERSION_MAJOR}_VERSION}")
endif()

configure_file(src/spritechatversion.cpp.cmakegen src/spritechatversion.cpp)

qt_add_executable(${PROJECT_NAME}
  ${PROJECT_NAME}.qrc
  resource/ui/loading_window.ui
  resource/ui/message_dialog.ui
  src/animationlayer.cpp src/animationlayer.h
  src/animationloader.cpp src/animationloader.h
  src/aoapplication.cpp src/aoapplication.h
  src/aoapplication_network.cpp
  src/aoapplication_pathsystem.cpp
  src/aoapplication_readwrite.cpp
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
  src/aosfxplayer.cpp src/aosfxplayer.h
  src/aotextarea.cpp src/aotextarea.h
  src/aotextboxwidgets.cpp src/aotextboxwidgets.h
  src/assetpathresolver.h src/assetpathresolver.cpp
  src/chatlogpiece.cpp src/chatlogpiece.h
  src/spritechatcommon.h
  src/courtroom.cpp src/courtroom.h
  src/courtroom_charselect.cpp
  src/courtroom_emotes.cpp
  src/courtroom_evidence.cpp
  src/datatypes.cpp
  src/datatypes.h
  src/debug_functions.cpp src/debug_functions.h
  src/discord_rich_presence.cpp src/discord_rich_presence.h
  src/effect.h src/effect.cpp
  src/eventfilters.cpp src/eventfilters.h
  src/file_functions.cpp src/file_functions.h
  src/gui/guiloader.h src/gui/guiloader.cpp
  src/gui/guiutils.h src/gui/guiutils.cpp
  src/hardware_functions.cpp src/hardware_functions.h
  src/lobby.cpp src/lobby.h
  src/main.cpp
  src/moderation_functions.cpp src/moderation_functions.h
  src/network/masterapigateway.h src/network/masterapigateway.cpp

  src/networkmanager.cpp src/networkmanager.h
  src/networksession.cpp src/networksession.h
  src/options.cpp src/options.h
  src/screenslidetimer.cpp src/screenslidetimer.h
  src/scrolltext.cpp src/scrolltext.h
  src/spritechatversion.h src/spritechatversion.cpp.cmakegen ${CMAKE_CURRENT_BINARY_DIR}/src/spritechatversion.cpp
  src/widget/optionswindow.cpp src/widget/optionswindow.h
  src/widget/directconnectdialog.cpp src/widget/directconnectdialog.h
  src/widget/flipbutton.h src/widget/flipbutton.cpp
  src/widget/loadingwindow.h src/widget/loadingwindow.cpp
  src/widget/messagedialog.h src/widget/messagedialog.cpp
  src/widget/moderatordialog.cpp src/widget/moderatordialog.h
  src/widget/playerlistwidget.cpp src/widget/playerlistwidget.h
  src/widget/servereditordialog.cpp src/widget/servereditordialog.h
)

add_custom_target(Changelog SOURCES
  changelog.md
)

if(WIN32)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET ${PROJECT_NAME} PROPERTY WIN32_EXECUTABLE true)
    set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/resource/${PROJECT_NAME}.rc")
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
  turnabout-core
  turnabout-audio
  turnabout-game
  turnabout-network
  turnabout-gui
  bass
  bassopus
)

set(KAL_NOQUOTE_QINFO "qInfo().noquote")
set(KAL_NOQUOTE_QWARNING "qWarning().noquote")
set(KAL_NOQUOTE_QCRITICAL "qCritical")
set(KAL_NOQUOTE_QDEBUG "qDebug().noquote")
target_compile_definitions(${PROJECT_NAME} PRIVATE kInfo=${KAL_NOQUOTE_QINFO})
target_compile_definitions(${PROJECT_NAME} PRIVATE kWarning=${KAL_NOQUOTE_QWARNING})
target_compile_definitions(${PROJECT_NAME} PRIVATE kCritical=${KAL_NOQUOTE_QCRITICAL})
target_compile_definitions(${PROJECT_NAME} PRIVATE kDebug=${KAL_NOQUOTE_QDEBUG})

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
