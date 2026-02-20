cmake_minimum_required(VERSION 3.26)
project(MHY_Scanner
	VERSION 1.1.14
	LANGUAGES CXX
)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(UNIT_TESTS "Unit Tests" OFF)
option(DEV "DEV" OFF)

add_definitions(-DMHY_Scanner_VERSION="${PROJECT_VERSION}")

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    add_compile_options(/Zi /sdl /arch:AVX2 /utf-8)
    #add_compile_options("/analyze:external-")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_link_options(/DEBUG /OPT:REF,ICF,LBR)
    endif()
endif()

include(FetchContent)
include(ExternalProject)

include(cmake/install_nuget.cmake)
include(cmake/install_ffmpeg.cmake)
include(cmake/build_QR-Code-generator.cmake)

FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)
FetchContent_MakeAvailable(nlohmann_json)

find_package(OpenCV REQUIRED opencv_world)
include_directories(${OpenCV_INCLUDE_DIRS})

set(OPENSSL_USE_STATIC_LIBS TRUE)
find_package(OpenSSL REQUIRED)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

#boost
include_directories(${CMAKE_SOURCE_DIR}/3rdparty/boost)

#curl
include_directories(./3rdparty/libcurl-vc17-X64-release-static-zlib-static-ipv6-sspi-schannel/include)
link_directories(./3rdparty/libcurl-vc17-X64-release-static-zlib-static-ipv6-sspi-schannel/lib)
add_definitions(-DCURL_STATICLIB)

include_directories(./3rdparty/abogus_cpp)
link_directories(./3rdparty/abogus_cpp)

if(NOT DEV)
    add_definitions(-DTESTSPEED)
    add_definitions(-DSHOW)
endif()

include_directories(src/TrrJson)
include_directories(src/Core)

add_subdirectory(src/TrrJson)

add_executable(${PROJECT_NAME}
	"${CMAKE_SOURCE_DIR}/src/Resources/MHY_Scanner.rc"
	"${CMAKE_SOURCE_DIR}/src/Resources/MHY_Scanner.qrc" 
	"${CMAKE_SOURCE_DIR}/src/UI/Main.cpp" 
	"${CMAKE_SOURCE_DIR}/src/UI/QRCodeForStream.cpp"
	"${CMAKE_SOURCE_DIR}/src/UI/QRCodeForStream.h"
	"${CMAKE_SOURCE_DIR}/src/UI/QRCodeForScreen.cpp" 
	"${CMAKE_SOURCE_DIR}/src/UI/QRCodeForScreen.h" 
	"${CMAKE_SOURCE_DIR}/src/UI/WindowAbout.cpp" 
	"${CMAKE_SOURCE_DIR}/src/UI/WindowAbout.h" 
	"${CMAKE_SOURCE_DIR}/src/UI/WindowMain.cpp" 
	"${CMAKE_SOURCE_DIR}/src/UI/WindowMain.h" 
	"${CMAKE_SOURCE_DIR}/src/UI/WindowLogin.h"
	"${CMAKE_SOURCE_DIR}/src/UI/WindowLogin.cpp" 
	"${CMAKE_SOURCE_DIR}/src/UI/WindowGeeTest.h" 
	"${CMAKE_SOURCE_DIR}/src/UI/WindowGeeTest.cpp"
	"${CMAKE_SOURCE_DIR}/src/Core/CryptoKit.cpp"
	"${CMAKE_SOURCE_DIR}/src/Core/CryptoKit.h"
    "${CMAKE_SOURCE_DIR}/src/Core/HttpClient.cpp"
    "${CMAKE_SOURCE_DIR}/src/Core/HttpClient.h"
    "${CMAKE_SOURCE_DIR}/src/Core/LiveStreamLink.cpp"
    "${CMAKE_SOURCE_DIR}/src/Core/LiveStreamLink.h"
    "${CMAKE_SOURCE_DIR}/src/Core/Mihoyosdk.cpp"
    "${CMAKE_SOURCE_DIR}/src/Core/Mihoyosdk.h"
    "${CMAKE_SOURCE_DIR}/src/Core/QRScanner.cpp"
    "${CMAKE_SOURCE_DIR}/src/Core/QRScanner.h"
    "${CMAKE_SOURCE_DIR}/src/Core/ScreenScan.cpp"
    "${CMAKE_SOURCE_DIR}/src/Core/ScreenScan.h"
    "${CMAKE_SOURCE_DIR}/src/Core/ConfigDate.cpp"
    "${CMAKE_SOURCE_DIR}/src/Core/ConfigDate.h"
    "${CMAKE_SOURCE_DIR}/src/Core/BH3Api.hpp"
    "${CMAKE_SOURCE_DIR}/src/Core/CookieParser.hpp"
    "${CMAKE_SOURCE_DIR}/src/Core/CreateUUID.hpp"
	"${CMAKE_SOURCE_DIR}/src/Core/ScannerBase.hpp"
)

if(NOT DEV)
    #target_link_options(${PROJECT_NAME} PRIVATE "/SUBSYSTEM:WINDOWS")
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
#target_compile_definitions

set(PROJECT_LIBRARIES
    TrrJson
	abogus_lib

    # windows
    Ws2_32
    wldap32
    Normaliz
    Dbghelp

    # Qt6
    Qt6::Core
    Qt6::Widgets

    # curl
    libcurl_a

    QR_Code_generator
    WebView2LoaderStatic
	nlohmann_json::nlohmann_json
	${OpenCV_LIBS}
	${FFMPEG_LIBS}
	OpenSSL::Crypto
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIBRARIES})

file(GLOB FFMPEG_DLLS
	"${FFMPEG_BIN_DIR}/avcodec-*.dll"
	"${FFMPEG_BIN_DIR}/avformat-*.dll"
	"${FFMPEG_BIN_DIR}/avutil-*.dll"
	"${FFMPEG_BIN_DIR}/swscale-*.dll"
	"${FFMPEG_BIN_DIR}/swresample-*.dll"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
	COMMAND ${CMAKE_COMMAND} -E copy_if_different
	${FFMPEG_DLLS}
	"$<TARGET_FILE_DIR:${PROJECT_NAME}>"
)

# unit tests
if(UNIT_TESTS)
    enable_testing()
    add_subdirectory(tests)
    target_link_libraries(test_core PRIVATE ${PROJECT_LIBRARIES})
    target_link_libraries(test_gui PRIVATE ${PROJECT_LIBRARIES})
    target_link_libraries(test_video PRIVATE ${PROJECT_LIBRARIES})
    target_link_libraries(test_video_hw PRIVATE ${PROJECT_LIBRARIES})
    target_link_libraries(test_streamlink PRIVATE ${PROJECT_LIBRARIES})
endif()

# install
set(Install_Directory "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}_${PROJECT_VERSION}")

install(TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION ${Install_Directory}
)

set(QT_DLL_FILES
    "$ENV{QTDIR}/bin/Qt6Core.dll"
    "$ENV{QTDIR}/bin/Qt6Widgets.dll"
    "$ENV{QTDIR}/bin/Qt6Gui.dll"
)

install(
    FILES 
	${CMAKE_BINARY_DIR}/${PROJECT_NAME}.pdb
	${QT_DLL_FILES}
	${FFMPEG_DLLS}
    DESTINATION 
	${Install_Directory}
)

install(
    FILES
    $ENV{QTDIR}/plugins/platforms/qdirect2d.dll
    $ENV{QTDIR}/plugins/platforms/qminimal.dll
    $ENV{QTDIR}/plugins/platforms/qoffscreen.dll
    $ENV{QTDIR}/plugins/platforms/qwindows.dll
    DESTINATION
    ${Install_Directory}/plugins/platforms
)

install(
    FILES
    $ENV{QTDIR}/plugins/styles/qmodernwindowsstyle.dll
    DESTINATION
    ${Install_Directory}/plugins/styles
)

install(
    DIRECTORY
    ScanModel
    DESTINATION
    ${Install_Directory}
)