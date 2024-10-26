cmake_minimum_required(VERSION 3.26)

add_library(MHY_Scanner_Core STATIC 
    CryptoKit.cpp
    HttpClient.cpp
    LiveStreamLink.cpp
    Mihoyosdk.cpp
    OfficialApi.cpp
    QRScanner.cpp
    ScreenScan.cpp
    ConfigDate.cpp
    BH3Api.hpp
    "CookieParser.hpp"
    CreateUUID.hpp
    #BiliGameApi.cpp
    #VideoProcessor.cpp
)