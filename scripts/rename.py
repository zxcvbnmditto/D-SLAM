import os
import re
import cv2

def main():
    target_dir = "/vslam/data/kitti-rgb-odometry/sequences/00/image_2"
    tstamp_filename = "/vslam/data/kitti-rgb-odometry/sequences/00/times.txt"
    save_dir = "/vslam/data/kitti-rgb-output/rename"

    imgs = sorted(os.listdir(target_dir))
    f = open(tstamp_filename, "r")
    for line, old_filename in zip(f, imgs):
        new_filename = str(float(line)) + ".png"
        data = cv2.imread(os.path.join(target_dir, old_filename))
        cv2.imwrite(os.path.join(save_dir, new_filename), data)
        print(new_filename, old_filename)


if __name__ == "__main__":
    main()
