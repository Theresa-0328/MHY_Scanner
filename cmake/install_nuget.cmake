find_program(NUGET_EXE NAMES nuget PATHS ${CMAKE_BINARY_DIR})
if(NOT NUGET_EXE)
	set(NUGET_URL "https://dist.nuget.org/win-x86-commandline/latest/nuget.exe")
	set(NUGET_EXE "${CMAKE_BINARY_DIR}/nuget.exe")
		file(DOWNLOAD
		${NUGET_URL}
		${NUGET_EXE}
		SHOW_PROGRESS
		STATUS DOWNLOAD_STATUS
		LOG DOWNLOAD_LOG
	)
endif()

execute_process(COMMAND ${NUGET_EXE} install "Microsoft.Web.WebView2"-ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/nuget_packages)
execute_process(COMMAND ${NUGET_EXE} install "Microsoft.Windows.ImplementationLibrary" -ExcludeVersion -OutputDirectory ${CMAKE_BINARY_DIR}/nuget_packages)

include_directories(${CMAKE_BINARY_DIR}/nuget_packages/Microsoft.Web.WebView2/build/native/include)
link_directories(${CMAKE_BINARY_DIR}/nuget_packages/Microsoft.Web.WebView2/build/native/x64)
include_directories(${CMAKE_BINARY_DIR}/nuget_packages/Microsoft.Windows.ImplementationLibrary/include)

set(WEBVIEW2_LOADER_LIB "WebView2LoaderStatic")