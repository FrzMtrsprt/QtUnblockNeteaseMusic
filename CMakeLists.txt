cmake_minimum_required(VERSION 3.5)

project(QtUnblockNeteaseMusic VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets Network LinguistTools)

# SingleApplication
set(QAPPLICATION_CLASS QApplication)
add_subdirectory(src/third-party/SingleApplication)

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
    src/tray.cpp
    src/tray.h
)
source_group("Source Files" FILES ${APP_SOURCES})

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(UTILS_SOURCES
        src/utils/winutils.cpp
        src/utils/winutils.h
    )
    source_group("Source Files" FILES ${UTILS_SOURCES})

    set(RESOURCES
        res/QtUnblockNeteaseMusic.rc
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
    PREFIX /
    FILES res/icon.png
)

qt_add_translations(QtUnblockNeteaseMusic
    TS_FILES ${TS_FILES}
)

target_link_libraries(QtUnblockNeteaseMusic PRIVATE Qt6::Widgets)
target_link_libraries(QtUnblockNeteaseMusic PRIVATE SingleApplication::SingleApplication)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(QtUnblockNeteaseMusic PRIVATE dwmapi shlwapi uxtheme wininet)
endif()

set_target_properties(QtUnblockNeteaseMusic PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(QtUnblockNeteaseMusic)
