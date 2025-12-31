cmake_minimum_required(VERSION 3.26)
project(MHY_Scanner LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(MAJOR_VERSION 1)
set(MINOR_VERSION 1)
set(MICRO_VERSION 14)
set(_VERSION_ "v${MAJOR_VERSION}.${MINOR_VERSION}.${MICRO_VERSION}")
add_definitions(-DSCAN_VER="${_VERSION_}")

option(Unit_Tests "Unit Tests" ON)
option(USE_OPENSSL "OPENSSL" ON)
option(DEV "DEV" OFF)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    add_compile_options(/Zi /sdl /arch:AVX2 /utf-8)
    #add_compile_options("/analyze:external-")
    if(CMAKE_BUILD_TYPE STREQUAL "Release")
        add_link_options(/DEBUG /OPT:REF,ICF,LBR)
    endif()
endif()

find_program(NUGET_EXE NAMES nuget PATHS ${CMAKE_CURRENT_SOURCE_DIR})
if(NOT NUGET_EXE)
    message("NUGET.EXE not found.")
    message(FATAL_ERROR "Please install this executable, and run CMake again.")
endif()

execute_process(COMMAND ${NUGET_EXE} install "Microsoft.Web.WebView2"-ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages)
execute_process(COMMAND ${NUGET_EXE} install "Microsoft.Windows.ImplementationLibrary" -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/packages) 

include_directories(${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2/build/native/include)
link_directories(${CMAKE_BINARY_DIR}/packages/Microsoft.Web.WebView2//build/native/x64)
include_directories(${CMAKE_BINARY_DIR}/packages/Microsoft.Windows.ImplementationLibrary/include)

include_directories(./3rdparty/NlohmannJson/include)

#opencv
include_directories(./3rdparty/opencv/include)
link_directories(./3rdparty/opencv/staticlib)

#boost
include_directories(./3rdparty/boost)

#ffmpeg
include_directories(./3rdparty/ffmpeg/include)
link_directories(D:/Dev/ffmpeg/lib)
link_directories(./3rdparty/ffmpeg/lib)

#curl
include_directories(./3rdparty/libcurl-vc17-X64-release-static-zlib-static-ipv6-sspi-schannel/include)
link_directories(./3rdparty/libcurl-vc17-X64-release-static-zlib-static-ipv6-sspi-schannel/lib)
add_definitions(-DCURL_STATICLIB)

include_directories(./3rdparty/abogus_cpp)
link_directories(./3rdparty/abogus_cpp)
#OpenSSL
if(USE_OPENSSL)
    include_directories(D:/Dev/OpenSSL-Win64/include)
    link_directories(D:/Dev/OpenSSL-Win64/lib)
    add_definitions(-DUSE_OPENSSL)
endif()

#QT
find_package(Qt6 COMPONENTS Widgets REQUIRED)

#QR-Code-generator
include_directories(./3rdparty/QR-Code-generator/cpp)
add_library(QR_Code_generator STATIC 3rdparty/QR-Code-generator/cpp/qrcodegen.cpp)
#add_executable(QrCodeGeneratorDemo 3rdparty/QR-Code-generator/cpp/QrCodeGeneratorDemo.cpp)
#target_link_libraries(QrCodeGeneratorDemo QR_Code_generator)

if(NOT DEV)
    add_definitions(-DTESTSPEED)
    add_definitions(-DSHOW)
endif()

include_directories(src/TrrJson)
include_directories(src/Core)

add_subdirectory(src/UI)
add_subdirectory(src/Core)
add_subdirectory(src/TrrJson)

if(NOT DEV)
    #target_link_options(MHY_Scanner PRIVATE "/SUBSYSTEM:WINDOWS")
    set_target_properties(MHY_Scanner PROPERTIES WIN32_EXECUTABLE TRUE)
endif()
#target_compile_definitions

if(Unit_Tests)
    enable_testing()
    add_subdirectory(tests)
endif()

set(FFMPEG_LIBRARIES)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    list(APPEND FFMPEG_LIBRARIES 
    libavcodec
    libavdevice
    libavfilter
    libavformat
    libavutil
    libpostproc
    libswresample
    libswscale
    )
else()
    list(APPEND FFMPEG_LIBRARIES 
    avcodec
    avdevice
    avfilter
    avformat
    avutil
    postproc
    swresample
    swscale
    )
endif()

set(PROJECT_LIBRARIES
    MHY_Scanner_Core
    TrrJson
	abogus_lib

    # windows
    Ws2_32
    wldap32
    winmm
    Crypt32
    Normaliz
    Dbghelp

    # Qt6
    Qt6::Core
    Qt6::Widgets

    # opencv
    #ade$<$<CONFIG:Debug>:d>
    IlmImf$<$<CONFIG:Debug>:d>
    ittnotify$<$<CONFIG:Debug>:d>
    libjpeg-turbo$<$<CONFIG:Debug>:d>
    libopenjp2$<$<CONFIG:Debug>:d>
    libpng$<$<CONFIG:Debug>:d>
    libprotobuf$<$<CONFIG:Debug>:d>
    libtiff$<$<CONFIG:Debug>:d>
    libwebp$<$<CONFIG:Debug>:d>
    opencv_img_hash480$<$<CONFIG:Debug>:d>
    opencv_world480$<$<CONFIG:Debug>:d>
    #quirc$<$<CONFIG:Debug>:d>
    zlib$<$<CONFIG:Debug>:d>
    
    # ffmpeg
    ${FFMPEG_LIBRARIES}

    # curl
    libcurl_a

    QR_Code_generator

    WebView2LoaderStatic
)

#target_link_libraries(QrCodeGeneratorDemo ${PROJECT_LIBRARIES})

if(USE_OPENSSL)
    target_link_libraries(MHY_Scanner_Core PRIVATE 
        # OpenSSL
        libssl_static
        libcrypto_static)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_LIBRARIES})

if(Unit_Tests)
    target_link_libraries(test_core PRIVATE ${PROJECT_LIBRARIES})
    target_link_libraries(test_gui PRIVATE ${PROJECT_LIBRARIES})
    target_link_libraries(test_video PRIVATE ${PROJECT_LIBRARIES})
    target_link_libraries(test_video_hw PRIVATE ${PROJECT_LIBRARIES})
    target_link_libraries(test_streamlink PRIVATE ${PROJECT_LIBRARIES})

endif()

set(Install_Directory "${CMAKE_INSTALL_PREFIX}/${PROJECT_NAME}_${_VERSION_}")

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
	${CMAKE_BINARY_DIR}/bin/${PROJECT_NAME}.pdb
	${QT_DLL_FILES}
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
    src/ScanModel
    DESTINATION
    ${Install_Directory}
)