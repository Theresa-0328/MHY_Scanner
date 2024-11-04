cmake_minimum_required(VERSION 3.26)

add_executable(${PROJECT_NAME}
	"${CMAKE_SOURCE_DIR}/src/Resources/MHY_Scanner.rc"
    "${CMAKE_SOURCE_DIR}/src/Resources/MHY_Scanner.qrc" 
    "Main.cpp" 
    "QRCodeForStream.cpp"
    "QRCodeForScreen.cpp" 
    "WindowAbout.cpp" 
    "WindowMain.cpp" 
    "WindowMain.h" 
    "WindowAbout.h" 
    "WindowLogin.h"
    "WindowLogin.cpp" 
    "WindowGeeTest.h" 
    "WindowGeeTest.cpp"
)