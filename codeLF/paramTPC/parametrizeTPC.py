#!/usr/bin/env python3

from utils import get_obj, draw, TFile
from ROOT import TGraphErrors, TCanvas, TF1
from numpy import sqrt, power, log
from sys import argv
import ROOT
from ROOT import gROOT, TColor, TObjArray

gROOT.LoadMacro("BB.h+g")
from ROOT import BB, BB2, BBReso


def makeparam(input_file="../AnalysisResults_TPC.root", part="Pr", mass=0.938272):
    h = get_obj(input_file,
                "htpcsignal" + part,
                "TPCpidqa-signalwTOF-task/")
    can = TCanvas(part, part)
    draw(h, "COLZ")
    # h.GetZaxis().SetRangeUser(2, 100)
    profile = h.ProfileX("profile", 1, -1, "o")
    draw(profile, "same")

    slices = TObjArray()
    h.FitSlicesY(0, 0, -1, 0, "QNR", slices)
    mean = slices.At(1)
    sigma = slices.At(2)
    draw(mean, "same")
    draw(sigma, "same")
    draw(BBReso(), "same")

    g = TGraphErrors()
    g.SetName("bg")
    g.SetTitle(part)
    g.GetXaxis().SetTitle("#beta#gamma")
    g.GetYaxis().SetTitle("d#it{E}/d#it{x}")
    for i in range(1, h.GetNbinsX()+1):
        x = h.GetXaxis().GetBinCenter(i)
        x /= mass
        ex = h.GetXaxis().GetBinWidth(i)
        ex /= mass
        y = profile.GetBinContent(i)
        ey = profile.GetBinError(i)
        g.SetPoint(g.GetN(), x, y)
        g.SetPointError(g.GetN()-1, ex, ey)
    gcan = TCanvas("g", "g")
    draw(g)
    fun = [BB(), BB2(), BB2("Unfitted")]
    g.Fit(fun[1])
    draw(fun[1], "same")
    fun[2].SetLineColor(1)
    draw(fun[2], "same")
    p = "{"
    for i in range(fun[1].GetNpar()):
        p += f"{fun[1].GetParameter(i)},"
    p += "}"
    p = p.replace(",}", "};")
    print(f"std::vector<float> p = {p}")
    input()
    f = TFile(f"/tmp/bg_{part}.root", "RECREATE")
    f.cd()
    g.Write()
    bb = ROOT.o2.pid.tpc.BetheBloch()
    for i in range(0, bb.GetParameters().size()):
        bb.SetParameter(i, fun[1].GetParameter(i))
    bb.Write()
    f.Close()
    return can, gcan


def plotbg(part="Pi Ka Pr"):
    h = [get_obj(f"/tmp/bg_{i}.root", "bg", "") for i in part.split()]
    cols = ['#e41a1c', '#377eb8', '#4daf4a', '#984ea3']
    for i in enumerate(h):
        i[1].SetLineColor(TColor.GetColor(cols[i[0]]))
    can = TCanvas("bg", "bg")
    opt = ""
    for i in h:
        draw(i, opt)
        opt += "same"
    input()
    return can


if __name__ == "__main__":
    pass
    # makeparam("Pi", 0.139570)
    # makeparam("Ka", 0.493677)
    makeparam(argv[1], part="Pr", mass=0.938272)
    # makeparam(part="Pr", mass=0.938272)
    # makeparam(part="De", mass=1.875613)
    # c = plotbg()
