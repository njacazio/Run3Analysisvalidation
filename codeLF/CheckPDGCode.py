#!/usr/bin/env python3

"""
Script to show the pdg codes of the generated particles
"""

from sys import argv
from utils import get_obj, draw
from ROOT import TCanvas, TObjArray
import ROOT


def main(input_file):
    pdgDB = ROOT.TDatabasePDG.Instance()
    h = get_obj(input_file, "pdg", "generator-histogram")
    for i in range(1, h.GetNbinsX()+1):
        axis = h.GetXaxis()
        if axis.GetBinLabel(i) != "":
            pdgCode = int(axis.GetBinLabel(i))
            part = pdgDB.GetParticle(pdgCode)
            axis.SetBinLabel(i, part.GetName())
            axis.SetRange(1, i+1)
        else:
            break

    can = TCanvas("pdg", "pdg")
    draw(h, "")
    input()
    return can


main(argv[1])
