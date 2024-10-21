cmake_minimum_required(VERSION 3.26)

add_executable(${PROJECT_NAME}
	"LoginBili.cpp" 
	"LoginWindow.cpp"
	"main.cpp" 
	"ScannerGui.cpp" 
	#"ThreadDownload.cpp" 
	"ThreadLocalServer.cpp" 
	#"ThreadSacn.cpp" 
	"QRCodeForStream.cpp"
	"QRCodeForScreen.cpp" 
	../rc/ScannerGui.rc 
	"WindowAbout.cpp" 
	"WindowAbout.h" 
	"WindowLogin.h"
	"WindowLogin.cpp")