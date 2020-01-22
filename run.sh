#/bin/bash
xhost +local:docker

# docker start vslam_test
# docker attach vslam_test

docker run -it \
           --name=vslam_test \
           -e DISPLAY=$DISPLAY \
           -v /tmp/.X11-unix:/tmp/.X11-unix \
           vslam_test:latest \
           /bin/bash