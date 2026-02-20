set(FFMPEG_URL "https://github.com/BtbN/FFmpeg-Builds/releases/download/latest/ffmpeg-n8.0-latest-win64-gpl-shared-8.0.zip")
set(FFMPEG_ARCHIVE "ffmpeg.zip")
set(FFMPEG_DIR "${CMAKE_BINARY_DIR}/ffmpeg-n8.0-latest-win64-gpl-shared-8.0")

set(FFMPEG_ARCHIVE "${CMAKE_BINARY_DIR}/${FFMPEG_ARCHIVE}")

if(NOT EXISTS ${FFMPEG_DIR})
	message(STATUS "Downloading FFmpeg...")
	file(DOWNLOAD ${FFMPEG_URL} ${FFMPEG_ARCHIVE} SHOW_PROGRESS)
	file(ARCHIVE_EXTRACT INPUT ${FFMPEG_ARCHIVE} DESTINATION "${CMAKE_BINARY_DIR}")
endif()

set(FFMPEG_LIBS
	avcodec
	avdevice
	avfilter
	avformat
	avutil
	swscale
	swresample
)

set(FFMPEG_BIN_DIR ${FFMPEG_DIR}/bin)
set(FFMPEG_LIB_DIR ${FFMPEG_DIR}/lib)
set(FFMPEG_INCLUDE_DIR ${FFMPEG_DIR}/include)

link_directories(${FFMPEG_LIB_DIR})
include_directories(${FFMPEG_INCLUDE_DIR})
