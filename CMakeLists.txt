cmake_minimum_required(VERSION 3.26)
project(MHY_Scanner LANGUAGES CXX)


#set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:/Qt/6.2.3/msvc2019_64") # Qt Kit Dir
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

set(MAJOR_VERSION 1)
set(MINOR_VERSION 1)
set(MICRO_VERSION 3)
set(_VERSION_ "v${MAJOR_VERSION}.${MINOR_VERSION}.${MICRO_VERSION}")

option(Unit_Tests "Unit Tests" ON)
option(USE_OPENSSL "OPENSSL" ON)
option(DEV "DEV" OFF)

if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /sdl")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /sdl")
endif()

# Specify MSVC UTF-8 encoding   
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

#opencv
include_directories(./3rdparty/opencv/include)
link_directories(./3rdparty/opencv/staticlib)

#httplib
include_directories(./3rdparty/httplib)

#boost
include_directories(./3rdparty/boost)

#ffmpeg
include_directories(D:/Dev/ffmpeg/include)
link_directories(D:/Dev/ffmpeg/lib)

#curl
include_directories(./3rdparty/libcurl-vc17-X64-release-static-zlib-static-ipv6-sspi-schannel/include)
link_directories(./3rdparty/libcurl-vc17-X64-release-static-zlib-static-ipv6-sspi-schannel/lib)
add_definitions(-DCURL_STATICLIB)

#OpenSSL
if(USE_OPENSSL)
	include_directories(D:/Dev/OpenSSL-Win64/include)
	link_directories(D:/Dev/OpenSSL-Win64/lib)
	add_definitions(-DUSE_OPENSSL)
endif()

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_definitions(-DSCAN_VER="${_VERSION_}")

if(!DEV)
	add_definitions(-DTESTSPEED)
	add_definitions(-DSHOW)
endif()

add_subdirectory(src)

#target_compile_definitions

if(Unit_Tests)
	enable_testing()
	add_subdirectory(tests)
endif()

set(PROJECT_LIBRARIES
	MHY_Scanner_Core
	TrrJson

	# windows
	Ws2_32
	wldap32
	winmm
	Crypt32
	Normaliz

	# Qt6 Shared Library
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
	avcodec
	avdevice
	avfilter
	avformat
	avutil
	postproc
	swresample
	swscale

	# curl
	libcurl_a
)

if(USE_OPENSSL)
	target_link_libraries(MHY_Scanner_Core PRIVATE 
		# OpenSSL
		libssl_static
		libcrypto_static)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE 
	${PROJECT_LIBRARIES}
)

if(Unit_Tests)
	target_link_libraries(test_core PRIVATE ${PROJECT_LIBRARIES})
	target_link_libraries(test_gui PRIVATE ${PROJECT_LIBRARIES})
	target_link_libraries(test_video PRIVATE ${PROJECT_LIBRARIES})
	target_link_libraries(test_video_hw PRIVATE ${PROJECT_LIBRARIES})
endif()

install(TARGETS ${PROJECT_NAME}
	RUNTIME DESTINATION MHY_Scanner
)
install(
    DIRECTORY
    src/geetest
	src/ScanModel
    DESTINATION
    MHY_Scanner
)