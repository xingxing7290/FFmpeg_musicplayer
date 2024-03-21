cmake_minimum_required(VERSION 3.5)

project(ffmpeg LANGUAGES C)

add_executable(ffmpeg main.c
    123.c
    4.cpp
    stdafx.cpp
    stdafx.h
    targetver.h)

include(GNUInstallDirs)
install(TARGETS ffmpeg
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
include_directories("${PROJECT_SOURCE_DIR}/lib/ffmpeg/include")
include_directories("${PROJECT_SOURCE_DIR}/lib/SDL2")

# file(GLOB_RECURSE ffmpeg_LIB "
#     ${PROJECT_SOURCE_DIR}/lib/ffmpeg/lib/avcodec.lib \
#     ${PROJECT_SOURCE_DIR}/lib/ffmpeg/lib/avformat.lib \
#     ${PROJECT_SOURCE_DIR}/lib/ffmpeg/lib/avutil.lib \
#     ${PROJECT_SOURCE_DIR}/lib/ffmpeg/lib/avfilter.lib \
#     ${PROJECT_SOURCE_DIR}/lib/ffmpeg/lib/avdevice.lib
#     ${PROJECT_SOURCE_DIR}/lib/ffmpeg/SDL2/*.lib")


set(LIBRARY1_PATH "C:/Users/Administrator/Documents/ffmpeg/lib/ffmpeg/lib/avcodec.lib")
set(LIBRARY2_PATH "C:/Users/Administrator/Documents/ffmpeg/lib/ffmpeg/lib/avformat.lib")
set(LIBRARY3_PATH "C:/Users/Administrator/Documents/ffmpeg/lib/ffmpeg/lib/avutil.lib")
set(LIBRARY4_PATH "C:/Users/Administrator/Documents/ffmpeg/lib/ffmpeg/lib/avfilter.lib")
set(LIBRARY5_PATH "C:/Users/Administrator/Documents/ffmpeg/lib/ffmpeg/lib/postproc.lib")
set(LIBRARY6_PATH "C:/Users/Administrator/Documents/ffmpeg/lib/ffmpeg/lib/avdevice.lib")
set(LIBRARY7_PATH "C:/Users/Administrator/Documents/ffmpeg/lib/SDL2/SDL2.lib")
set(LIBRARY8_PATH "C:/Users/Administrator/Documents/ffmpeg/lib/ffmpeg/lib/swresample.lib")
set(LIBRARY9_PATH "C:/Users/Administrator/Documents/ffmpeg/lib/ffmpeg/lib/swscale.lib")

# 设置DLL文件的路径
set(DLL_PATH1 "C:/Users/Administrator/Documents/ffmpeg/dll/avcodec-60.dll")
set(DLL_PATH2 "C:/Users/Administrator/Documents/ffmpeg/dll/avdevice-60.dll")
set(DLL_PATH3 "C:/Users/Administrator/Documents/ffmpeg/dll/avfilter-9.dll")
set(DLL_PATH4 "C:/Users/Administrator/Documents/ffmpeg/dll/avformat-60.dll")
set(DLL_PATH5 "C:/Users/Administrator/Documents/ffmpeg/dll/avutil-58.dll")
set(DLL_PATH6 "C:/Users/Administrator/Documents/ffmpeg/dll/postproc-57.dll")
set(DLL_PATH7 "C:/Users/Administrator/Documents/ffmpeg/dll/SDL2.dll")
set(DLL_PATH8 "C:/Users/Administrator/Documents/ffmpeg/dll/swresample-4.dll")
set(DLL_PATH9 "C:/Users/Administrator/Documents/ffmpeg/dll/swscale-7.dll")

target_link_libraries(
    ${PROJECT_NAME}
    ${LIBRARY1_PATH}
    ${LIBRARY2_PATH}
    ${LIBRARY3_PATH}
    ${LIBRARY4_PATH}
    ${LIBRARY5_PATH}
    ${LIBRARY6_PATH}
    ${LIBRARY7_PATH}
    ${LIBRARY8_PATH}
    ${LIBRARY9_PATH}
    ${DLL1_PATH}
    ${DLL2_PATH}
    ${DLL3_PATH}
    ${DLL4_PATH}
    ${DLL5_PATH}
    ${DLL6_PATH}
    ${DLL7_PATH}
    ${DLL8_PATH}
    ${DLL9_PATH}

)


