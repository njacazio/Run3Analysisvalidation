#!/usr/bin/python3

from ROOT import (
    TFile,
    gROOT,
    gStyle,
    TLegend,
    TCanvas,
    gPad,
    gSystem,
    TLatex,
    TPaveText,
)
from sys import argv
from utils import get_obj, gettwo


ldrawn = []


def drawrange(x, y, xtit="", ytit=""):
    if type(x) != type([]):
        if xtit == "":
            xtit = x.GetXaxis().GetTitle()
        x = [x.GetXaxis().GetBinLowEdge(
            1), x.GetXaxis().GetBinUpEdge(x.GetNbinsX())]
    ldrawn.append(gPad.DrawFrame(
        x[0], y[0], x[1], y[1], ";" + xtit + ";" + ytit))
    ldrawn[-1].GetYaxis().CenterTitle()


latexdrawn = []


def draw(h, opt="", copy=False, labelize=True, puttitle=False):
    hd = h
    if copy:
        print("Drawing", h, "as a copy")
        hd = h.DrawCopy(opt)
        if type(copy) == type(""):
            hd.SetName(hd.GetName() + copy)
        else:
            hd.SetName(hd.GetName() + "copy")
        hd.SetDirectory(0)
    else:
        h.Draw(opt)
    if labelize and "same" not in opt:
        n = hd.GetName()
        if puttitle:
            n += f" {hd.GetTitle()}"
        latex = TLatex(0.2, 0.95, n)
        latex.SetNDC()
        latex.Draw()
        latexdrawn.append(latex)
    gPad.Update()
    ldrawn.append(hd)
    return hd


def drawcounts(h):
    txt = TPaveText(0.75, 0.55, 0.88, 0.7, "NDC")
    for i in h:
        txt.AddText("{}={}".format(i.GetTitle(), i.GetEntries()))
    txt.Draw()
    latexdrawn.append(txt)
    gPad.Update()


def drawdiff(h, opt="", useabs=True):
    hd = draw(h[0], opt=opt, copy=" Difference")
    if useabs:
        if "TH2" in h[0].ClassName():
            for i in range(0, h[0].GetNbinsX()):
                for j in range(0, h[0].GetNbinsY()):
                    hd.SetBinContent(
                        i + 1,
                        j + 1,
                        abs(
                            h[0].GetBinContent(i + 1, j + 1)
                            - h[1].GetBinContent(i + 1, j + 1)
                        ),
                    )
    else:
        hd.Add(h[1], -1)
    gPad.Modified()
    gPad.Update()


def nextpad():
    pn = gPad.GetName()
    cn = gPad.GetCanvas().GetName()
    pad = pn.replace(cn, "").replace("_", "")
    if pad == "":
        gPad.cd(1)
    else:
        gPad.GetCanvas().cd(int(pad) + 1)


legends = []


def makelegend(h):
    l = TLegend(0.7, 0.7, 0.9, 0.9)
    for i in h:
        l.AddEntry(i, "", "f")
    # l.SetHeader("Legend","C"); // option "C" allows to center the header
    legends.append(l)


def drawtwo(h, logx=False, logy=False, project=False, ratio=True, diff=True, V=True):
    th2mode = "TH2" in h[0].ClassName()
    if th2mode:
        makecanvas(h[0].GetName(), h[0].GetName(), nx=2, ny=2)
    else:
        makecanvas(h[0].GetName(), h[0].GetName(), nx=3, ny=1)

    def equals(h1, h2, Ck):
        cmd = ["h1." + Ck, "h2." + Ck]
        cmdR = [eval(cmd[0]), eval(cmd[1])]
        V = cmdR[0] == cmdR[1]
        if not V:
            raise ValueError(h1, "differs from", h2, Ck, cmd, cmdR)

    for i in "GetNbinsX GetXaxis().GetTitle GetYaxis().GetTitle".split():
        equals(h[0], h[1], i + "()")
    nextpad()
    if logy:
        gPad.SetLogy()
    if logx:
        gPad.SetLogx()
    if th2mode:
        gPad.SetLogz()
    hrun2 = h[0]
    hrun3 = h[1]
    if project and th2mode:
        hrun2 = h[0].ProjectionY("Y1_" + h[0].GetName())
        hrun3 = h[1].ProjectionY("Y2_" + h[1].GetName())
    if V:
        print("Drawing", h)
    if "TH2" in hrun2.ClassName():
        draw(hrun2, "COLZ", puttitle=True)
        drawcounts([hrun2])
        nextpad()
        if logy:
            gPad.SetLogy()
        if logx:
            gPad.SetLogx()
        if th2mode:
            gPad.SetLogz()
        draw(hrun3, "COLZ", puttitle=True)
        drawcounts([hrun3])
    else:
        draw(hrun2)
        draw(hrun3, "same")
        drawcounts([hrun2, hrun3])
    print(
        "Entries of", hrun2.GetName(), hrun2.GetEntries(), "vs", hrun3.GetEntries(),
    )
    if "TH1" in h[0].ClassName():
        legends[-1].Draw()
    opt = ""
    if th2mode:
        opt = "COLZ"
    if ratio:
        nextpad()
        # drawrange(hrun2, [0.5, 1.5], ytit="Run2/Run3")
        # hratio = draw(hrun2, opt + "same", copy=True, labelize=False)
        hratio = draw(hrun2, opt, copy=True, labelize=False)
        hratio.SetTitle(hratio.GetTitle() + " Ratio")
        if th2mode:
            hratio.GetZaxis().SetTitle("Run2/Run3")
        else:
            hratio.GetYaxis().SetTitle("Run2/Run3")
        hratio.Divide(hrun3)
        gPad.Update()
    if diff:
        nextpad()
        hdiff = draw(hrun2, opt, copy=True, labelize=False)
        hdiff.SetTitle(hdiff.GetTitle() + " Difference")
        if th2mode:
            hdiff.GetZaxis().SetTitle("Run2-Run3")
        else:
            hdiff.GetYaxis().SetTitle("Run2-Run3")
        hdiff.Add(hrun3, -1)
        gPad.Update()


canvaslist = []


def makecanvas(cname, ctit, sizex=1600, sizey=1600, nx=1, ny=1):
    c = TCanvas(cname, ctit, sizex, sizey)
    c.Divide(nx, ny)
    if nx > 1:
        for i in range(0, nx * ny):
            c.cd(i+1)
            gPad.SetLeftMargin(0.15)
            gPad.SetRightMargin(0.15)
    c.cd()
    canvaslist.append(c)
    return c

nsigma=" hnsigmaEl hnsigmaMu hnsigmaDe  hnsigmaTr  hnsigmaHe  hnsigmaAl"
exptime=" htimediffEl htimediffMu htimediffAl htimediffDe htimediffTr htimediffHe"
def compare(filerun3, filerun2, avoid="tofqa-task tofspectra-task" + exptime + nsigma, save=False):
    gStyle.SetOptStat(0)
    gStyle.SetOptTitle(0)
    hlist = []
    if 1:  # Getting list of histograms
        f = TFile(filerun3, "READ")
        dirlist = f.GetListOfKeys()
    for i in dirlist:
        if i.GetName() in avoid:
            continue
        print("Directory", i)
            d = f.Get(i.GetName()).GetListOfKeys()
        for j in d:
                if j.GetName() in avoid:
                    continue
            print(j)
            hlist.append(f"{i.GetName()}/{j.GetName()}")
    print(hlist)
    h = [gettwo(filerun2, filerun3, i) for i in hlist]

    makelegend(h[0])

    for i in h:
        drawtwo(i)

    gSystem.ProcessEvents()
    if save:
    canvaslist[0].SaveAs("plots.pdf[")
    for i in canvaslist:
        i.SaveAs("plots.pdf")
    canvaslist[-1].SaveAs("plots.pdf]")
    if "-b" not in argv:
        print("Press enter to continue")
        input()


if __name__ == "__main__":
    pass
    RUN3FILE = "AnalysisResults.root"
    RUN1FILE = "TOFPid.root"
    if len(argv) > 2:
        RUN3FILE = argv[1]
        RUN1FILE = argv[2]
    compare(RUN3FILE, RUN1FILE)
