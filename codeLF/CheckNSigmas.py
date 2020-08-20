#!/usr/bin/env python3

"""
Script to compare the nsigmas
"""

from sys import argv
from utils import get_obj, draw
from ROOT import TCanvas, TObjArray


def main(input_file, obj_name):
    obj_name = obj_name.split("/")
    dir_name = "/".join(obj_name[:-1])
    h = get_obj(input_file, obj_name[-1], dir_name)
    can = TCanvas("nsigma", "nsigma")
    can.SetLogz()
    draw(h, "COLZ")
    slices = TObjArray()
    h.FitSlicesY(0, 0, -1, 0, "QNR", slices)
    mean = slices.At(1)
    sigma = slices.At(2)
    draw(mean, "same")
    draw(sigma, "same")

    input()
    return can


main(argv[1], argv[2])
