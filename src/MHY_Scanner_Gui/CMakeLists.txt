cmake_minimum_required(VERSION 3.26)

add_executable(${PROJECT_NAME}
	"LoginBili.cpp" 
	"LoginWindow.cpp"
	"Main.cpp" 
	"ScannerGui.cpp" 
	"ThreadLocalServer.cpp" 
	"QRCodeForStream.cpp"
	"QRCodeForScreen.cpp" 
	../rc/ScannerGui.rc 
	"WindowAbout.cpp" 
	"WindowAbout.h" 
	"WindowLogin.h"
	"WindowLogin.cpp" 
	"WindowGeeTest.h" 
	"WindowGeeTest.cpp")