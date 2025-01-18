cmake_minimum_required(VERSION 3.16)
project(Eter_UI)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_PREFIX_PATH "/opt/homebrew/lib/cmake")

find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
        source.cpp
        Eter_UI.cpp
        CardLabel.cpp
        BoardCell.cpp
)

set(HEADERS
        Eter_UI.h
        CardLabel.h
        BoardCell.h
)

set(UI_FILES
        Eter_UI.ui
)

set(RESOURCE_FILES
        Eter_UI.qrc
)

qt6_wrap_ui(UI_HEADERS ${UI_FILES})

qt6_add_resources(RESOURCES ${RESOURCE_FILES})

add_executable(${PROJECT_NAME} ${SOURCES} ${UI_HEADERS} ${RESOURCES})

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES} ${HEADERS} ${UI_HEADERS} ${RESOURCES})
target_link_libraries(${PROJECT_NAME} Qt6::Core Qt6::Gui Qt6::Widgets)
