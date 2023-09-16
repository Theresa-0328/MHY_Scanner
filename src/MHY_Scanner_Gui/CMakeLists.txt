cmake_minimum_required(VERSION 3.26)

set(SRC_CPP
	#WIN32 # If you need a terminal for debug, please comment this statement 
	"LoginBili.cpp" 
	"LoginWindow.cpp"
	"main.cpp" 
	"ScannerGui.cpp" 
	#"ThreadDownload.cpp" 
	"ThreadLocalServer.cpp" 
	"ThreadSacn.cpp" 
	"QRCodeForStream.cpp"
	"QRCodeForScreen.cpp" 
	../rc/ScannerGui.rc
	"ConfigDate.cpp"
)

add_definitions(-DTESTSPEED)
add_definitions(-DSHOW)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_executable(${PROJECT_NAME} 
		#WIN32 
		${SRC_CPP})
else()
    add_executable(${PROJECT_NAME} 
		${SRC_CPP})
endif()

target_include_directories(${PROJECT_NAME} PUBLIC 
	../TrrJson
	../MHY_Scanner_Core
)