cmake_minimum_required(VERSION 3.26)

add_executable(${PROJECT_NAME}
	#WIN32 # If you need a terminal for debug, please comment this statement 
	"LoginBili.cpp" 
	"LoginWindow.cpp"
	"main.cpp" 
	"ScannerGui.cpp" 
	"ThreadDownload.cpp" 
	"ThreadGetScreen.cpp" 
	"ThreadLocalServer.cpp" 
	"ThreadSacn.cpp" 
	"ThreadStreamProcess.cpp" 
)

target_include_directories(${PROJECT_NAME} PUBLIC 
	../TrrJson
	../MHY_Scanner_Core
)