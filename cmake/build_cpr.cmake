file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/cpr/include)
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/cpr/lib)
set(CPR_INSTALL_DIR ${CMAKE_BINARY_DIR}/cpr)

ExternalProject_Add(cpr_ext
	GIT_REPOSITORY https://github.com/libcpr/cpr.git
	GIT_TAG 1.14.1

	CMAKE_ARGS
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		-DCMAKE_INSTALL_PREFIX=${CMAKE_BINARY_DIR}/cpr
		-DCPR_USE_SYSTEM_CURL=OFF
		-DBUILD_SHARED_LIBS=OFF
		-DCPR_CURL_USE_LIBPSL=OFF

	BUILD_BYPRODUCTS
		${CMAKE_BINARY_DIR}/cpr/lib/cpr.lib
		${CMAKE_BINARY_DIR}/cpr/lib/libcurl.lib
		${CMAKE_BINARY_DIR}/cpr/lib/libcurl-d.lib
		${CMAKE_BINARY_DIR}/cpr/lib/zlibstatic.lib
		${CMAKE_BINARY_DIR}/cpr/lib/zlibstaticd.lib

	INSTALL_DIR ${CPR_INSTALL_DIR}
)
add_library(cpr STATIC IMPORTED GLOBAL)
set_target_properties(cpr PROPERTIES
	IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/cpr/lib/cpr.lib
)
target_include_directories(cpr INTERFACE ${CMAKE_BINARY_DIR}/cpr/include)

add_library(libcurl STATIC IMPORTED GLOBAL)
set_target_properties(libcurl PROPERTIES
	IMPORTED_LOCATION_RELEASE ${CMAKE_BINARY_DIR}/cpr/lib/libcurl.lib
	IMPORTED_LOCATION_DEBUG   ${CMAKE_BINARY_DIR}/cpr/lib/libcurl-d.lib
)

add_library(CURL::ZLIB STATIC IMPORTED GLOBAL)
set_target_properties(CURL::ZLIB PROPERTIES
	IMPORTED_LOCATION_RELEASE ${CMAKE_BINARY_DIR}/cpr/lib/zlibstatic.lib
	IMPORTED_LOCATION_DEBUG   ${CMAKE_BINARY_DIR}/cpr/lib/zlibstaticd.lib
)

target_compile_definitions(cpr INTERFACE CURL_STATICLIB)
target_link_libraries(cpr INTERFACE libcurl CURL::ZLIB)
add_dependencies(cpr cpr_ext)
add_library(cpr::cpr ALIAS cpr)