cmake_minimum_required(VERSION 3.0)
project( VSLAM_2020 )

find_package( OpenCV 4.1.0 REQUIRED )
find_package( Eigen3 3.1.0 REQUIRED)
find_package( Pangolin REQUIRED)
find_package( Torch REQUIRED )

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}  -Wall  -O3 -march=native ")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall   -O3 -march=native")

# Check C++11 or C++0x support
include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
   add_definitions(-DCOMPILEDWITHC11)
   message(STATUS "Using flag -std=c++11.")
elseif(COMPILER_SUPPORTS_CXX0X)
   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
   add_definitions(-DCOMPILEDWITHC0X)
   message(STATUS "Using flag -std=c++0x.")
else()
   message(FATAL_ERROR "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

set(ORB_SLAM_DIRECTORY ${PROJECT_SOURCE_DIR}/ThirdParty/orbslam2)

include_directories(
    ${OpenCV_INCLUDE_DIRS}
    ${ORB_SLAM_DIRECTORY}
    ${ORB_SLAM_DIRECTORY}/include
    ${EIGEN3_INCLUDE_DIR}
    ${Pangolin_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME} SHARED
    ${ORB_SLAM_DIRECTORY}/src/System.cc
    ${ORB_SLAM_DIRECTORY}/src/Tracking.cc
    ${ORB_SLAM_DIRECTORY}/src/LocalMapping.cc
    ${ORB_SLAM_DIRECTORY}/src/LoopClosing.cc
    ${ORB_SLAM_DIRECTORY}/src/ORBextractor.cc
    ${ORB_SLAM_DIRECTORY}/src/ORBmatcher.cc
    ${ORB_SLAM_DIRECTORY}/src/FrameDrawer.cc
    ${ORB_SLAM_DIRECTORY}/src/Converter.cc
    ${ORB_SLAM_DIRECTORY}/src/MapPoint.cc
    ${ORB_SLAM_DIRECTORY}/src/KeyFrame.cc
    ${ORB_SLAM_DIRECTORY}/src/Map.cc
    ${ORB_SLAM_DIRECTORY}/src/MapDrawer.cc
    ${ORB_SLAM_DIRECTORY}/src/Optimizer.cc
    ${ORB_SLAM_DIRECTORY}/src/PnPsolver.cc
    ${ORB_SLAM_DIRECTORY}/src/Frame.cc
    ${ORB_SLAM_DIRECTORY}/src/KeyFrameDatabase.cc
    ${ORB_SLAM_DIRECTORY}/src/Sim3Solver.cc
    ${ORB_SLAM_DIRECTORY}/src/Initializer.cc
    ${ORB_SLAM_DIRECTORY}/src/Viewer.cc
)

target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    ${EIGEN3_LIBS}
    ${Pangolin_LIBRARIES}
    ${TORCH_LIBRARIES}
    ${ORB_SLAM_DIRECTORY}/Thirdparty/DBoW2/lib/libDBoW2.so
    ${ORB_SLAM_DIRECTORY}/Thirdparty/g2o/lib/libg2o.so
    ${ORB_SLAM_DIRECTORY}/lib/libORB_SLAM2.so
)



add_executable( MONODEPTH_SLAM_DISK
    src/monodepth_slam_disk.cpp
    src/monodepth2.cpp
    src/monoslamData.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries( MONODEPTH_SLAM_DISK ${PROJECT_NAME})


add_executable( MONODEPTH_SLAM_CAMERA
    src/monodepth_slam_camera.cpp
    src/monodepth2.cpp
    src/monoslamData.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries( MONODEPTH_SLAM_CAMERA ${PROJECT_NAME})

add_executable( MONODEPTH_SLAM_MULTITHREAD
    src/monodepth_slam_multithread.cpp
    src/monodepth2.cpp
    src/monoslamData.cpp
)

target_include_directories(${PROJECT_NAME}
    PUBLIC
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries( MONODEPTH_SLAM_MULTITHREAD ${PROJECT_NAME})
