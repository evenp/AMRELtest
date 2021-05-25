cmake_minimum_required(VERSION 3.5)

project(roadgt)

cmake_policy(SET CMP0057 NEW)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_FLAGS "-std=c++11")

#find_package(Qt5 5.7.0 REQUIRED COMPONENTS Core)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Core REQUIRED)
find_package(Qt5Gui REQUIRED)

set(CMAKE_AUTOMOC ON)

add_definitions(-O3)

# Input
include_directories(
    ${PROJECT_SOURCE_DIR}/GTInterface
    ${PROJECT_SOURCE_DIR}/ImageTools
    ${PROJECT_SOURCE_DIR}/PointCloud
)

# Add files
set(HEADERS GTInterface/gtcreator.h
           GTInterface/gtperftest.h
           GTInterface/gtwindow.h
           ImageTools/pt2i.h
           ImageTools/vr2i.h
           PointCloud/asarea.h
           PointCloud/astrack.h
           PointCloud/ipttile.h
           PointCloud/ipttileset.h
           PointCloud/pt2f.h
           PointCloud/pt3f.h
           PointCloud/pt3i.h
           PointCloud/terrainmap.h
           PointCloud/vr2f.h
)

set(SOURCES main.cpp
           GTInterface/gtcreator.cpp
           GTInterface/gtperftest.cpp
           GTInterface/gtwindow.cpp
           ImageTools/pt2i.cpp
           ImageTools/vr2i.cpp
           PointCloud/asarea.cpp
           PointCloud/astrack.cpp
           PointCloud/ipttile.cpp
           PointCloud/ipttileset.cpp
           PointCloud/pt2f.cpp
           PointCloud/pt3f.cpp
           PointCloud/pt3i.cpp
           PointCloud/terrainmap.cpp
           PointCloud/vr2f.cpp
)

# Create executable and link library
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})
target_link_libraries(${PROJECT_NAME} ${QT_LIBRARIES} Qt5::Core Qt5::Widgets  Qt5::Gui)

