cmake_minimum_required(VERSION 3.26)

add_executable(${PROJECT_NAME}
    ../Resources/MHY_Scanner.rc 
    "LoginBili.cpp" 
    "LoginWindow.cpp"
    "Main.cpp" 
    "ThreadLocalServer.cpp" 
    "QRCodeForStream.cpp"
    "QRCodeForScreen.cpp" 
    "WindowAbout.cpp" 
    "WindowMain.cpp" 
    "WindowMain.h" 
    "WindowAbout.h" 
    "WindowLogin.h"
    "WindowLogin.cpp" 
    "WindowGeeTest.h" 
    "WindowGeeTest.cpp")