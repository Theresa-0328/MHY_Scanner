option(BUILD_QrCodeGeneratorDemo OFF)

include_directories(${CMAKE_SOURCE_DIR}/3rdparty/QR-Code-generator/cpp)

add_library(QR_Code_generator STATIC
    ${CMAKE_SOURCE_DIR}/3rdparty/QR-Code-generator/cpp/qrcodegen.cpp
)

if(BUILD_QrCodeGeneratorDemo)
    add_executable(QrCodeGeneratorDemo
        ${CMAKE_SOURCE_DIR}/3rdparty/QR-Code-generator/cpp/QrCodeGeneratorDemo.cpp
    )

    target_link_libraries(QrCodeGeneratorDemo
        PRIVATE
        QR_Code_generator
    )
endif()
