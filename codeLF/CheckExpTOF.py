#!/usr/bin/python3

"""
Script to compare the computed and the standard expected times
"""

from ROOT import TFile, gPad, TCanvas, TH1
from sys import argv
from utils import get_obj


parts = ["El"]
parts = ["El", "Mu", "Pi", "Ka", "Pr", "De", "Tr", "He", "Al"]
canvases = []


def drawit(l, opt="COLZ"):
    for i in l:
        if i is None:
            continue
        canvases.append(TCanvas(i.GetName(), i.GetName()))
        gPad.SetLeftMargin(0.15)
        i.Draw(opt)
        axis = i.GetYaxis()
        axis.CenterTitle()
        bins = [i.FindFirstBinAbove(0, 2) - 1,
                i.FindLastBinAbove(0, 2) + 1]
        axis.SetRange(*bins)
        prof = i.ProfileX(i.GetName() + "prof")
        prof.SetLineColor(2)
        prof.SetLineWidth(2)
        prof.Draw("same")
        gPad.SetLogz()
        gPad.Update()


subdir = "expectedtofqa-task/"
hdiff = [get_obj(argv[1], f"h_p_expdiff_{i}", subdir) for i in parts]
drawit(hdiff)
hdiff_rel = [get_obj(argv[1], f"h_p_expdiff_{i}_Rel", subdir) for i in parts]
drawit(hdiff_rel)

input()

outpdf = "/tmp/ExpTimes.pdf"
canvases[0].SaveAs(outpdf+"[")
for i in canvases:
    i.SaveAs(outpdf)
canvases[0].SaveAs(outpdf+"]")
