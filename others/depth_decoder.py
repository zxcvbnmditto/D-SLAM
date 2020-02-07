# Copyright Niantic 2019. Patent Pending. All rights reserved.
#
# This software is licensed under the terms of the Monodepth2 licence
# which allows for non-commercial use only, the full terms of which are made
# available in the LICENSE file.

from __future__ import absolute_import, division, print_function

import numpy as np
import torch
import torch.nn as nn

from collections import OrderedDict
from layers import *


class DepthDecoder(nn.Module):
    def __init__(self, num_ch_enc, scales=range(4), num_output_channels=1, use_skips=True):
        super(DepthDecoder, self).__init__()

        self.num_output_channels = num_output_channels
        self.use_skips = use_skips
        self.upsample_mode = 'nearest'
        self.scales = scales

        self.num_ch_enc = num_ch_enc
        self.num_ch_dec = np.array([16, 32, 64, 128, 256])

        # decoder
        self.convs = OrderedDict()
        for i in range(4, -1, -1):
            # upconv_0
            num_ch_in = self.num_ch_enc[-1] if i == 4 else self.num_ch_dec[i + 1]
            num_ch_out = self.num_ch_dec[i]
            self.convs[("upconv", i, 0)] = ConvBlock(num_ch_in, num_ch_out)
            print("upconv0 {}".format(i))

            # upconv_1
            num_ch_in = self.num_ch_dec[i]
            if self.use_skips and i > 0:
                num_ch_in += self.num_ch_enc[i - 1]
            num_ch_out = self.num_ch_dec[i]
            self.convs[("upconv", i, 1)] = ConvBlock(num_ch_in, num_ch_out)
            print("upconv1 {}".format(i))

        for s in self.scales:
            print("disconv {}".format(s))
            self.convs[("dispconv", s)] = Conv3x3(self.num_ch_dec[s], self.num_output_channels)

        self.decoder = nn.ModuleList(list(self.convs.values()))
        self.sigmoid = nn.Sigmoid()

    def forward_original(self, input_features):
        """ Original forward
        """
        print("Decode Original")
        self.outputs = {}

        # decoder
        x = input_features[-1]
        for i in range(4, -1, -1):
            x = self.convs[("upconv", i, 0)](x)
            x = [upsample(x)]
            if self.use_skips and i > 0:
                x += [input_features[i - 1]]
            x = torch.cat(x, 1)
            x = self.convs[("upconv", i, 1)](x)
            if i in self.scales:
                self.outputs[("disp", i)] = self.sigmoid(self.convs[("dispconv", i)](x))

        return self.outputs

    def forward(self, a1, a2, a3, a4, a5):
        """ Revised forward
            Accomodate for torch.jit.trace
                => Remove all conditions
                => Loop Unrolled
                => Return Type Tuple
        """
        print("Decode Trace")

        outputs = []
        input_features = [a1, a2, a3, a4, a5]
        x = input_features[-1]

        x = self.convs[("upconv", 4, 0)](x)
        x = [upsample(x)]
        x += [input_features[4 - 1]]
        x = torch.cat(x, 1)
        x = self.convs[("upconv", 4, 1)](x)
        # No outputs.append

        x = self.convs[("upconv", 3, 0)](x)
        x = [upsample(x)]
        x += [input_features[3 - 1]]
        x = torch.cat(x, 1)
        x = self.convs[("upconv", 3, 1)](x)
        outputs.append(self.sigmoid(self.convs[("dispconv", 3)](x)))

        x = self.convs[("upconv", 2, 0)](x)
        x = [upsample(x)]
        x += [input_features[2 - 1]]
        x = torch.cat(x, 1)
        x = self.convs[("upconv", 2, 1)](x)
        outputs.append(self.sigmoid(self.convs[("dispconv", 2)](x)))

        x = self.convs[("upconv", 1, 0)](x)
        x = [upsample(x)]
        x += [input_features[1 - 1]]
        x = torch.cat(x, 1)
        x = self.convs[("upconv", 1, 1)](x)
        outputs.append(self.sigmoid(self.convs[("dispconv", 1)](x)))

        x = self.convs[("upconv", 0, 0)](x)
        x = [upsample(x)]
        # No  x += [input_features[i - 1]]
        x = torch.cat(x, 1)
        x = self.convs[("upconv", 0, 1)](x)
        outputs.append(self.sigmoid(self.convs[("dispconv", 0)](x)))

        return tuple(outputs)  # Return Type: Tensor or Tuple for jit.trace