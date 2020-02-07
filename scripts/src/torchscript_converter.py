from __future__ import absolute_import, division, print_function

import sys
sys.path.append("../ThirdParty/monodepth2")

import torch.nn as nn
import torch
import matplotlib.pyplot as plt
import PIL.Image as pil
import numpy as np
import os

import networks
from torchvision import transforms
import torchvision
import argparse
import errno


def parse_args():
    parser = argparse.ArgumentParser(
        description='Simple script to convert Monodepth2 Model with torchscript')

    parser.add_argument('--model_name', type=str,
                        help='name of a pretrained model to use',
                        choices=[
                            "mono_640x192",
                            "mono_1024x320"],
                        default="mono_640x192")

    parser.add_argument('--model_path', type=str,
                        help='path to models',
                        default="/vslam/models")

    parser.add_argument('--sample_image', type=str,
                        help='path to sample img',
                        default="/vslam/data/depth/test_image.jpg")

    parser.add_argument('--encoder_name', type=str,
                        help='name of a encoder model to save',
                        default="t_encoder.pt")

    parser.add_argument('--decoder_name', type=str,
                        help='name of a decoder model to save',
                        default="t_decoder.pt")

    return parser.parse_args()


def load_example_image(example_img, loaded_dict_enc):
    image_path = example_img
    input_image = pil.open(image_path).convert('RGB')
    original_width, original_height = input_image.size

    feed_height = loaded_dict_enc['height']
    feed_width = loaded_dict_enc['width']
    input_image_resized = input_image.resize(
        (feed_width, feed_height), pil.LANCZOS)

    input_image_pytorch = transforms.ToTensor()(input_image_resized).unsqueeze(0)
    return input_image_pytorch


def check_tensor_eq(t1, t2):
    if not torch.all(torch.eq(t1, t2)):
        raise "Failure. Decoder Outputs does not match with origin implementation"


def verify_encoder(outputs, gt_outputs):
    for t1, t2 in zip(outputs, gt_outputs):
        check_tensor_eq(t1, t2)
    print("Encoder Input Verified. Test Passed.")


def verify_decoder(outputs, gt_outputs):
    for i, gt in enumerate(gt_outputs.values()):
        check_tensor_eq(outputs[i], gt)
    print("Decoder Input Verified. Test Passed.")


def convert_pretrained(model, model_path, example_img, save_enc_name, save_dec_name):
    # Pretrained Weights
    encoder_path = os.path.join(model_path, model, "encoder.pth")
    depth_decoder_path = os.path.join(model_path, model, "depth.pth")

    # Model Architechture
    encoder = networks.ResnetEncoder(18, False)
    depth_decoder = networks.DepthDecoder(
        num_ch_enc=encoder.num_ch_enc, scales=range(4))

    # Load pretrained weights into model
    try:
        loaded_dict_enc = torch.load(encoder_path, map_location='cpu')
    except FileNotFoundError as err:
        print("{} Cannot load encoder file {}".format(err, encoder_path))

    filtered_dict_enc = {
        k: v for k, v in loaded_dict_enc.items() if k in encoder.state_dict()}
    encoder.load_state_dict(filtered_dict_enc)

    try:
        loaded_dict = torch.load(depth_decoder_path, map_location='cpu')
    except FileNotFoundError as err:
        print("{} Cannot load decoder file {}".format(err, depth_decoder_path))

    depth_decoder.load_state_dict(loaded_dict)

    # Set to Eval mode
    encoder.eval()
    depth_decoder.eval()

    # Forward
    image = load_example_image(example_img, loaded_dict_enc)
    with torch.no_grad():
        # Encoder
        gt_features = encoder.forward_original(image)
        features = encoder(image)
        verify_encoder(features, gt_features)

        # Decoder
        gt_outputs = depth_decoder.forward_original(features)
        outputs = depth_decoder(*features)
        verify_decoder(outputs, gt_outputs)

    # JIT Trace
    encoder_module = torch.jit.trace(encoder, image)
    depth_decoder_module = torch.jit.trace(depth_decoder, features)

    # Serialize & Save
    t_encoder_path = os.path.join(model_path, model, save_enc_name)
    t_depth_decoder_path = os.path.join(model_path, model, save_dec_name)
    encoder_module.save(t_encoder_path)
    depth_decoder_module.save(t_depth_decoder_path)


def main():
    args = parse_args()
    convert_pretrained(args.model_name, args.model_path, args.sample_image,
                       args.encoder_name, args.decoder_name)


if __name__ == "__main__":
    main()
