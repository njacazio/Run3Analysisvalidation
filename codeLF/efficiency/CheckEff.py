#!/usr/bin/env python3

"""
Script to compute efficiencies
"""

from sys import argv
from utils import get_obj, draw
from ROOT import TCanvas, TObjArray, TFile
import ROOT


nums = {}
dens = {}


def get_num_den(input_file, draw_src=False):
    pdgDB = ROOT.TDatabasePDG.Instance()

    def getanddrawsrc(name, dir):
        h = get_obj(input_file, name, dir)
        if draw_src:
            can = TCanvas(name, name)
            h.Draw()
            can.Update()
            print("Drawing source", name)
            input()
        return h
    pdg = getanddrawsrc("pdg", "reconstructed-histogram")
    num = getanddrawsrc("pt", "reconstructed-histogram")
    den = getanddrawsrc("pt", "generator-histogram")
    global nums
    global dens
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
        if nums.setdefault(pname, None) is None:
            nums[pname] = num.ProjectionX(pname + "_num", i, i)
        else:
            nums[pname].Add(num.ProjectionX(pname + "_num" + input_file, i, i))
        if dens.setdefault(pname, None) is None:
            dens[pname] = den.ProjectionX(pname + "_den", i, i)
        else:
            dens[pname].Add(den.ProjectionX(pname + "_den" + input_file, i, i))


def main(input_files, show=False):
    if "-b" in input_files:
        input_files.remove("-b")
    for i in input_files:
        get_num_den(i)
    effs = []
    for i in nums:
        n = nums[i]
        d = dens[i]
        name = n.GetName().replace("_num", "")
        effs.append(n.Clone("Eff_" + name))
        effs[-1].SetTitle(name)
        effs[-1].Divide(n, d, 1, 1, "B")
    fout = TFile("/tmp/Eff.root", "RECREATE")
    fout.cd()
    for i in nums:
        nums[i].Write()
        dens[i].Write()
    for i in effs:
        i.Write()
        if show:
            can = TCanvas(i.GetName(), i.GetName())
            frame = can.DrawFrame(
                0, 0, 5, 1, ";#it{p}_{T} (GeV/#it{c});Efficiency " + i.GetTitle())
            frame
            draw(i, "same")
            print(i)
            print("Press enter to continue")
            input()
    fout.Close()


main(argv[1:])
