cmake_minimum_required(VERSION 3.26)

set(BUILD_GMOCK OFF)

add_subdirectory(../3rdparty/googletest googletest)

add_executable(test_core test_core.cpp)

target_link_libraries(test_core PUBLIC gtest gtest_main)
target_include_directories(test_core PUBLIC ../3rdparty/googletest-1.13.0/include)
target_include_directories(test_core PUBLIC ../src/MHY_Scanner_Core)

add_test(NAME test_core COMMAND test_core)
set_tests_properties(test_core PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src)

add_executable(test_gui test_gui.cpp ../src/MHY_Scanner_Gui/ThreadSacn.cpp)

target_link_libraries(test_gui PUBLIC gtest gtest_main)
target_include_directories(test_gui PUBLIC ../3rdparty/googletest-1.13.0/include)
target_include_directories(test_gui PUBLIC ../src/MHY_Scanner_Gui)
target_include_directories(test_gui PUBLIC ../src/MHY_Scanner_Core)

add_test(NAME test_gui COMMAND test_gui)
set_tests_properties(test_gui PROPERTIES WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/src)
set_tests_properties(test_gui PROPERTIES ENVIRONMENT "PATH=C:/Qt/6.2.3/msvc2019_64/bin;$ENV{PATH}")