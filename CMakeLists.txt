cmake_minimum_required(VERSION 3.5)

project(QtUnblockNeteaseMusic VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets LinguistTools)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets LinguistTools)

set(TS_FILES
        locale/QtUnblockNeteaseMusic_en.ts
        locale/QtUnblockNeteaseMusic_zh_CN.ts
)

set(APP_SOURCES
        src/config.cpp
        src/config.h
        src/main.cpp
        src/mainwindow.cpp
        src/mainwindow.h
        src/mainwindow.ui
)
source_group(src FILES ${APP_SOURCES})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(UTILS_SOURCES
        utils/winutils.cpp
        utils/winutils.h
    )
    source_group(utils FILES ${UTILS_SOURCES})

    set(RESOURCES
        res/QtUnblockNeteaseMusic.rc
    )
    source_group(res FILES ${RESOURCES})
endif()

qt_add_executable(QtUnblockNeteaseMusic
    MANUAL_FINALIZATION
    ${APP_SOURCES}
    ${UTILS_SOURCES}
    ${RESOURCES}
)

qt_add_resources(QtUnblockNeteaseMusic res
    # resource path = FILES - BASE + PREFIX
    PREFIX "/res"
    BASE "res"
    FILES res/icon.ico res/icon.png
)

qt_add_translations(QtUnblockNeteaseMusic
    TS_FILES ${TS_FILES}
)

target_link_libraries(QtUnblockNeteaseMusic PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

set_target_properties(QtUnblockNeteaseMusic PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(QtUnblockNeteaseMusic)
