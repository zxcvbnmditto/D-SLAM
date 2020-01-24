from __future__ import absolute_import, division, print_function

import sys
sys.path.append("./ThirdParty/monodepth2")

import torch
import matplotlib.pyplot as plt
import PIL.Image as pil
import numpy as np
import os

import networks
from torchvision import transforms
import torchvision


# import ThirdParty.monodepth2.networks as networks

# Define model

# Load Weight

# torch.jit.trace

# save


def load_example_image(loaded_dict_enc):
    image_path = "ThirdParty/monodepth2/assets/test_image.jpg"
    input_image = pil.open(image_path).convert('RGB')
    original_width, original_height = input_image.size

    feed_height = loaded_dict_enc['height']
    feed_width = loaded_dict_enc['width']
    input_image_resized = input_image.resize(
        (feed_width, feed_height), pil.LANCZOS)

    input_image_pytorch = transforms.ToTensor()(input_image_resized).unsqueeze(0)
    return input_image_pytorch


def convert_pretrained():
    # Pretrained Model Name
    model_name = "mono_640x192"

    # Pretrained Weights
    encoder_path = os.path.join("models", model_name, "encoder.pth")
    depth_decoder_path = os.path.join("models", model_name, "depth.pth")
    t_encoder_path = os.path.join("models", model_name, "t_encoder.pt")
    t_depth_decoder_path = os.path.join("models", model_name, "t_depth.pt")

    # Model Architechture
    encoder = networks.ResnetEncoder(18, False)
    depth_decoder = networks.DepthDecoder(
        num_ch_enc=encoder.num_ch_enc, scales=range(4))

    # Load pretrained weights into model
    loaded_dict_enc = torch.load(encoder_path, map_location='cpu')
    filtered_dict_enc = {
        k: v for k, v in loaded_dict_enc.items() if k in encoder.state_dict()}
    encoder.load_state_dict(filtered_dict_enc)

    loaded_dict = torch.load(depth_decoder_path, map_location='cpu')
    depth_decoder.load_state_dict(loaded_dict)

    import pdb; pdb.set_trace()
    encoder.eval()
    depth_decoder.eval()

    example_image = load_example_image(loaded_dict_enc)
    with torch.no_grad():
        example_features = encoder(example_image)
        outputs = depth_decoder(example_features)

        import pdb; pdb.set_trace()
        # Trace
        # encoder_module = torch.jit.trace(encoder.encoder, example_image)
        depth_decoder_module = torch.jit.trace(depth_decoder, example_features)
        # Serialize
        # encoder_module.save(t_encoder_path)
        depth_decoder_module.save(t_depth_decoder_path)


def main():
    convert_pretrained()


if __name__ == "__main__":
    main()
