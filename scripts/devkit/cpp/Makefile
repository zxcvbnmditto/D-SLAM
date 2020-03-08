all: evaluate_odometry

evaluate_odometry: evaluate_odometry.cpp matrix.cpp matrix.h
	g++ -Wall -O3 -DNDEBUG -o $@ evaluate_odometry.cpp matrix.cpp

clean:
	rm -rfv evaluate_odometry
