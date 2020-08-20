#!/usr/bin/env python3

"""
Script to compute efficiencies
"""

from sys import argv
from utils import get_obj, draw
from ROOT import TCanvas, TObjArray, TFile
import ROOT


def main(input_file, show="pi"):
    pdgDB = ROOT.TDatabasePDG.Instance()
    pdg = get_obj(input_file, "pdg", "reconstructed-histogram")
    num = get_obj(input_file, "pt", "reconstructed-histogram")
    den = get_obj(input_file, "pt", "generator-histogram")
    can = TCanvas("efficiency", "efficiency")
    num.Divide(num, den, 1, 1, "B")
    draw(num, "")
    effs = []
    for i in range(1, pdg.GetNbinsX()):
        l = pdg.GetXaxis().GetBinLabel(i)
        if l == "":
            continue
        pname = pdgDB.GetParticle(int(l))
        if not pname:
            print("Skipping", l)
            continue
        print(l, pname)
        pname = pname.GetName()
        effs.append(num.ProjectionX(pname, i, i))
    cans = []
    frames = []
    for i in effs:
        cans.append(TCanvas(i.GetName(), i.GetName()))
        frames.append(cans[-1].DrawFrame(0, 0, 20, 1,
                                        f";{i.GetXaxis().GetTitle()};Efficiency {i.GetName()}"))
        draw(i, "same")

    input()
    f = TFile("/tmp/Eff.root", "RECREATE")
    f.cd()
    for i in effs:
        i.Write()
    f.Close()
    return can


main(argv[1])
