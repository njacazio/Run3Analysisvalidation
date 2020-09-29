#!/usr/bin/env python3

from parametrizeTPC import O2BB, O2BBReso, betheblocho2
from parametrizeTPC import masses
from ROOT import TCanvas, TColor, TLegend

cols = ['#e41a1c', '#377eb8', '#4daf4a', '#984ea3', '#ff7f00', '#ffff33']


def plotbb(mass):
    can = TCanvas("bg", "bg")
    can.Draw()
    frame = can.DrawFrame(0, 0, 5, 1000, ";p (GeV/c);Expected BB")
    fun = []
    leg = TLegend(.7, .7, .9, .9)
    for i in mass:
        f = O2BB(i)
        f.SetParameter(0, masses[i])
        print("before:")
        for j in [1, 2, 3, 4, 5]:
            print(f.GetName(), "at", j, "=", f.Eval(j))
        betheblocho2.PrintParametrization()
        param = [0.0320981, 19.9768, 2.52666e-16, 2.72123, 6.08092, 50, 2.3]
        for j in enumerate(param):
            betheblocho2.SetParameter(*j)
        leg.AddEntry(f)
        fun.append(f)
        print("after:")
        for j in [1, 2, 3, 4, 5]:
            print(f.GetName(), "at", j, "=", f.Eval(j))
        for j in range(f.GetNpar()):
            print(j, f.GetParameter(j))
        c = TColor.GetColor(cols[len(fun) - 1])
        f.SetLineColor(c)
        f.Draw("same")
    leg.Draw()
    can.Update()
    input()
    return frame


# plotbb(mass=["El", "Pi", "Ka", "Pr", "De"])
# plotbb(mass=["El", "Pi"])
plotbb(mass=["Pi", "El"])
# plotbb(mass=["El"])

betheblocho2.PrintParametrization()
