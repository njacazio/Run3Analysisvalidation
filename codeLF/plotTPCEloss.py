#!/usr/bin/env python3

from ROOT import TFile, TCanvas, TColor
from ROOT import TLegend, gPad, TH1
from sys import argv

f = TFile(argv[1], "READ")

f.ls()

d = f.Get("TPCpidqa-expsignal-task")

d.ls()

P = {"El":"e", "Mu":"#mu", "Pi":"#pi","Ka":"K", "Pr":"p", "De":"d", "Tr":"t", "He":"^{3}He", "Al":"#alpha" }
parts = "El Mu Pi Ka Pr De Tr He Al".split()
h = d.Get("htpcsignal")
he = [d.Get("hexp" + i) for i in parts]

can = TCanvas("c", "c")
can.SetLogz()

h.SetBit(TH1.kNoStats)
h.SetTitle("TPC Signal")
h.Draw("colz")
col = ['#e41a1c','#377eb8','#4daf4a','#984ea3','#ff7f00','#ffff33','#a65628','#f781bf','#999999']

c = 0
p = []
for i in he:
  cc = TColor.GetColor(col[c])
  i.SetFillStyle(0)
  i.SetLineColor(cc)
  i.SetMarkerColor(cc)
  #i.Draw("same")
  p.append(i.ProfileX())
  p[-1].SetMarkerStyle(8)
  p[-1].SetTitle(P[parts[c]])
  p[-1].Draw("same")
  c += 1

leg = gPad.BuildLegend(.7, .7, .9,.9)
leg.SetNColumns(2)
can.Update()

input()








