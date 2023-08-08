cmake_minimum_required(VERSION 3.26)
project(MHY_Scanner VERSION 1.1.0 LANGUAGES CXX)


#set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_PREFIX_PATH "C:/Qt/6.2.3/msvc2019_64") # Qt Kit Dir
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
#"cwd": "${workspaceRoot}",
#"currentDir": "${workspaceRoot}"

if(MSVC)
	set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL")
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /sdl")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /sdl")
endif()

add_definitions(-DCURL_STATICLIB)

#opencv
include_directories(./3rdparty/opencv/include)
link_directories(./3rdparty/opencv/staticlib)

#httplib
include_directories(./3rdparty/httplib)

#ffmpeg
include_directories(D:/Dev/ffmpeg/include)
link_directories(D:/Dev/ffmpeg/lib)

#OpenSSL
include_directories(D:/Dev/OpenSSL-Win64/include)
link_directories(D:/Dev/OpenSSL-Win64/lib)

#curl
include_directories(D:/Dev/curl-7.87.0/builds/libcurl-vc15-x64-release-static-ipv6-sspi-schannel/include)
link_directories(D:/Dev/curl-7.87.0/builds/libcurl-vc15-x64-release-static-ipv6-sspi-schannel/lib)

find_package(Qt6 COMPONENTS Widgets REQUIRED)

add_subdirectory(src)

target_link_libraries(${PROJECT_NAME} PRIVATE
	MHY_Scanner_Core
	TrrJson
	# windows
	Ws2_32
	wldap32
	winmm
	Crypt32
	Normaliz
	# Qt6 Shared Library
	Qt6::Widgets 
	# opencv
	ade
	IlmImf
	ittnotify
	libjpeg-turbo
	libopenjp2
	libpng
	libprotobuf
	libtiff
	libwebp
	opencv_img_hash480
	opencv_world480
	quirc
	zlib
	# ffmpeg
	avcodec
	avdevice
	avfilter
	avformat
	avutil
	postproc
	swresample
	swscale
	# OpenSSL
	libssl_static
	libcrypto_static
	# curl
	libcurl_a
)