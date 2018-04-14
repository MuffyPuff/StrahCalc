cmake_minimum_required(VERSION 3.10)

project(StrahCalc)

#set(CMAKE_CXX_STANDARD 17)

find_package(Qt5Widgets)
 
include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})

include_directories("${PROJECT_SOURCE_DIR}/lib/klfbackend")
include_directories("${PROJECT_SOURCE_DIR}/lib/MufTranslate/include")
include_directories("${PROJECT_SOURCE_DIR}/lib/MufExprtkBackend/include")

#add_subdirectory(lib/MufExprtkBackend)

#disable all warning for third party libs
if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -w")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_PREFIX_PATH "C:\\Qt\\5.10.1\\msvc2015\\")
else(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    set(CMAKE_PREFIX_PATH "~/Qt/5.10.1/gcc_64/")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# set up a mapping so that the Release configuration for the Qt imported target is
# used in the COVERAGE CMake configuration.
set_target_properties(Qt5::Core PROPERTIES MAP_IMPORTED_CONFIG_COVERAGE "RELEASE")


set(QOBJECT_HEADERS
    src/global.h
    src/strahcalc.h
    src/mufmenu.h
    src/muflatex.h
    src/mufsymbols.h
    src/muffunctions.h
    src/mufsettings_w.h
    src/mufexprparser.h
    src/mufsymbollistview_w.h
)
 
set(HEADERS
    src/global.h
)
 
set(SRC_LIST
    src/main.cpp
    src/strahcalc.cpp
    src/strahcalcinit.cpp
    src/mufmenu.cpp
    src/muflatex.cpp
    src/mufsymbols.cpp
    src/muffunctions.cpp
    src/mufsettings_w.cpp
    src/mufexprparser.cpp
    src/mufsymbollistview_w.cpp
)


qt5_wrap_ui(UI_HEADERS src/strahcalc.ui)
qt5_wrap_ui(UI_HEADERS src/mufsettings_w.ui)
qt5_wrap_ui(UI_HEADERS src/mufsymbollistview_w.ui)
qt5_wrap_cpp(MOC_FILES ${OBJECT_HEADERS})
#qt5_wrap_cpp(MOC_FILES src/mufmenu.cpp)
#qt5_wrap_cpp(MOC_FILES src/muflatex.cpp)
#qt5_wrap_cpp(MOC_FILES src/strahcalc.cpp)
#qt5_wrap_cpp(MOC_FILES src/mufsymbols.cpp)
#qt5_wrap_cpp(MOC_FILES src/muffunctions.cpp)
#qt5_wrap_cpp(MOC_FILES src/strahcalcinit.cpp)
#qt5_wrap_cpp(MOC_FILES src/mufsettings_w.cpp)
#qt5_wrap_cpp(MOC_FILES src/mufexprparser.cpp)
#qt5_wrap_cpp(MOC_FILES src/mufsymbollistview_w.cpp)
    
#if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
#    set_target_properties(StrahCalc PROPERTIES LINK_FLAGS_DEBUG "/SUBSYSTEM:WINDOWS")
#    set_target_properties(StrahCalc PROPERTIES LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS")
#endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

include_directories(${Qt5Widgets_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS})
# Add compiler flags for building executables (-fPIE)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${Qt5Widgets_EXECUTABLE_COMPILE_FLAGS}")

install(FILES 
    DESTINATION include)

add_executable(StrahCalc
    ${SRC_LIST}
    ${MOC_FILES}
    ${UI_HEADERS})

target_link_libraries(StrahCalc
    ${PROJECT_SOURCE_DIR}/bin/libKLFBackend.so
    ${PROJECT_SOURCE_DIR}/bin/libMufTranslate.so
    ${PROJECT_SOURCE_DIR}/bin/libMufExprtkBackend.so
    ${Qt5Core_QTMAIN_LIBRARIES} Qt5::Widgets)
