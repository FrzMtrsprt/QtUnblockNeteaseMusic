cmake_minimum_required(VERSION 3.5)

project(QtUnblockNeteaseMusic)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets LinguistTools)

# SingleApplication
set(QAPPLICATION_CLASS QApplication)
add_subdirectory(third-party/SingleApplication)

set(TS_FILES
    locale/QtUnblockNeteaseMusic_en.ts
    locale/QtUnblockNeteaseMusic_zh_CN.ts
)

set(APP_SOURCES
    src/config/config.cpp
    src/config/config.h
    src/config/param.h
    src/configdialog.cpp
    src/configdialog.h
    src/configdialog.ui
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
    src/server.cpp
    src/server.h
    src/tray.cpp
    src/tray.h
    src/wizardpages.cpp
    src/wizardpages.h
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
    PREFIX /res
    BASE res
    FILES res/icon.png
)

qt_add_resources(QtUnblockNeteaseMusic certs
    PREFIX /certs
    BASE third-party/server
    FILES third-party/server/ca.crt
)

qt_add_translations(QtUnblockNeteaseMusic
    TS_FILES ${TS_FILES}
)

# Copy qtbase translations manually
add_custom_command(TARGET QtUnblockNeteaseMusic POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo
    "Copying qtbase translations"
)

file(GLOB QTBASE_TRANSLATIONS
    LIST_DIRECTORIES false
    ${Qt6Core_DIR}/../../../translations/qtbase_*.qm
)

if(MSVC)
    set(BUILD_TYPE
        $<$<CONFIG:Debug>:Debug>$<$<CONFIG:Release>:Release>$<$<CONFIG:RelWithDebInfo>:RelWithDebInfo>$<$<CONFIG:MinSizeRel>:MinSizeRel>
    )
    set(TRANSLATION_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR}/${BUILD_TYPE}/translations)
else()
    set(TRANSLATION_TARGET_DIR ${CMAKE_CURRENT_BINARY_DIR}/translations)
endif()

foreach(QTBASE_TRANSLATION ${QTBASE_TRANSLATIONS})
    get_filename_component(QTBASE_TRANSLATION_NAME ${QTBASE_TRANSLATION} NAME)
    string(REPLACE "qtbase_" "qt_" QT_TRANSLATION_NAME ${QTBASE_TRANSLATION_NAME})

    add_custom_command(TARGET QtUnblockNeteaseMusic POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${QTBASE_TRANSLATION} ${TRANSLATION_TARGET_DIR}/${QT_TRANSLATION_NAME}
    )
endforeach()

target_link_libraries(QtUnblockNeteaseMusic PRIVATE Qt6::Widgets)
target_link_libraries(QtUnblockNeteaseMusic PRIVATE SingleApplication::SingleApplication)

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_link_libraries(QtUnblockNeteaseMusic PRIVATE crypt32 uxtheme wininet)
endif()

set_target_properties(QtUnblockNeteaseMusic PROPERTIES
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

qt_finalize_executable(QtUnblockNeteaseMusic)
