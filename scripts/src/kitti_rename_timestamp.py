import os
import re
import cv2
import argparse


def parse_args():
    parser = argparse.ArgumentParser()
    parser.add_argument("--input_dir", type=str,
                        help="Path to directory with images to be renamed",
                        default="/vslam/data/kitti-rgb-odometry/sequences/01/image_2")

    parser.add_argument("--timestamp_file", type=str,
                        help="Path to timestamp file",
                        default="/vslam/data/kitti-rgb-odometry/sequences/01/times.txt")

    parser.add_argument("--output_dir", type=str,
                        help="Path to directory to store new timestamp named file",
                        default="/vslam/data/kitti-rgb-output/01/rgb")

    return parser.parse_args()


def main():
    args = parse_args()
    input_dir = args.input_dir
    timestamp_file = args.timestamp_file
    output_dir = args.output_dir

    if not os.path.isdir(output_dir):
        os.makedirs(output_dir)

    imgs = sorted(os.listdir(input_dir))
    f = open(timestamp_file, "r")
    for line, old_filename in zip(f, imgs):
        new_filename = str(float(line)) + ".png"
        data = cv2.imread(os.path.join(input_dir, old_filename))
        cv2.imwrite(os.path.join(output_dir, new_filename), data)
        print(new_filename, old_filename)


if __name__ == "__main__":
    main()
