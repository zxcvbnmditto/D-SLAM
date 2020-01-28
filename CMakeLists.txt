cmake_minimum_required(VERSION 2.8)
project( VSLAM_2020 )

find_package( OpenCV REQUIRED )
find_package( Torch REQUIRED )

message(${TORCH_LIBRARIES})

include_directories(${OpenCV_INCLUDE_DIRS})

add_executable( VIDEO_CAPTURE_DEMO src/video_capture_demo.cpp )
target_link_libraries( VIDEO_CAPTURE_DEMO ${OpenCV_LIBS} )

add_executable( GET_DEPTH_FROM_DIR src/get_depth_from_dir.cpp )
target_link_libraries( GET_DEPTH_FROM_DIR ${OpenCV_LIBS} ${TORCH_LIBRARIES})