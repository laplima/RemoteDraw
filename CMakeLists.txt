cmake_minimum_required(VERSION 3.5)

project(RemoteDraw VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
        main.cpp
        mainwindow.cpp
        mainwindow.h
        mainwindow.ui
        renderarea.h renderarea.cpp
        PenCaseC.h PenCaseC.cpp
        PenCaseS.h PenCaseS.cpp
        PenCaseI.h PenCaseI.cpp
        orbworker.h orbworker.cpp)

set(RESOURCE_FILES rdraw.qrc)

# pkg-config
# find_package(PkgConfig REQUIRED)
find_package(fmt REQUIRED)
find_package(spdlog REQUIRED)

include_directories(
    "${PROJECT_SOURCE_DIR}"
    "$ENV{ACE_ROOT}"
    "$ENV{TAO_ROOT}"
    "$ENV{TAO_ROOT}/orbsvcs"
	"$ENV{COLIBRY}/include")

add_compile_options(-Wno-deprecated-declarations)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(RemoteDraw
        MANUAL_FINALIZATION
		${PROJECT_SOURCES} ${RESOURCE_FILES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET RemoteDraw APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(RemoteDraw SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(RemoteDraw
            ${PROJECT_SOURCES}
        )
    endif()
endif()


# Generate ${mylibs}

set(libs
    ACE TAO
    TAO_PortableServer
    TAO_AnyTypeCode
    TAO_CosNaming
    TAO_IORTable
    colibry_static)

# Platform-dependent additional libraries

if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(libs ${libs} )
elseif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(libs ${libs} )
endif()

set(mylibs)
foreach(lib_ ${libs})
    set(alib_ ${lib}-NOTFOUND)
    find_library(alib_ ${lib_} PATHS
        "$ENV{ACE_ROOT}/lib" "$ENV{COLIBRY}/lib" "/opt/local/lib")
    set(mylibs ${mylibs} ${alib_})
endforeach(lib_)

target_link_libraries(RemoteDraw PRIVATE Qt${QT_VERSION_MAJOR}::Widgets fmt::fmt ${mylibs})

set_target_properties(RemoteDraw PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

install(TARGETS RemoteDraw
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(RemoteDraw)
endif()
