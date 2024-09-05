cmake_minimum_required(VERSION 3.16)

project(QtUnblockNeteaseMusic
    VERSION 1.5.3
    DESCRIPTION "A desktop client for UnblockNeteaseMusic, made with Qt."
    HOMEPAGE_URL "https://github.com/FrzMtrsprt/QtUnblockNeteaseMusic"
    LANGUAGES CXX
)
set(PROJECT_AUTHOR "FrzMtrsprt")
configure_file("src/version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/version.h")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools Network)

# SingleApplication
set(QAPPLICATION_CLASS QApplication)
add_subdirectory("third-party/SingleApplication")

add_subdirectory("third-party/QSimpleUpdater")

file(GLOB TS_FILES
    "locale/*.ts"
)

file(GLOB APP_SOURCES
    "src/*.cpp"
    "src/config/*.cpp"
)
source_group("Source Files" FILES ${APP_SOURCES})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(UTILS_SOURCES
        "src/utils/winutils.cpp"
    )
    source_group("Source Files" FILES ${UTILS_SOURCES})

    set(RESOURCES
        "res/QtUnblockNeteaseMusic.rc"
    )
    source_group("Resources" FILES ${RESOURCES})
endif()

qt_add_executable(QtUnblockNeteaseMusic
    MANUAL_FINALIZATION
    ${APP_SOURCES}
    ${UTILS_SOURCES}
    ${RESOURCES}
)

qt_add_resources(QtUnblockNeteaseMusic res
    PREFIX "/res"
    BASE "res"
    FILES "res/icon.png"
)

qt_add_resources(QtUnblockNeteaseMusic certs
    PREFIX "/certs"
    BASE "third-party/server"
    FILES "third-party/server/ca.crt"
)

qt_add_translations(QtUnblockNeteaseMusic
    TS_FILES ${TS_FILES}
)

target_link_libraries(QtUnblockNeteaseMusic PRIVATE
    Qt6::Widgets
    Qt6::Network
    SingleApplication::SingleApplication
    QSimpleUpdater
)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(QtUnblockNeteaseMusic PRIVATE
        crypt32 uxtheme wininet
    )
endif()

set_target_properties(QtUnblockNeteaseMusic PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(QtUnblockNeteaseMusic)
