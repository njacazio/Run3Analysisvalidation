#!/usr/bin/python3

from sys import argv

from ROOT import (
    TCanvas,
    TFile,
    TLatex,
    TLegend,
    TPaveText,
    gPad,
    gStyle,
    gSystem,
)


def get(f, hn, d="", V=True):
    if V:
        print("Getting", hn, "from", f[0].GetName(), "and", f[1].GetName())
    d += hn
    h = [f[0].Get(d), f[1].Get(d)]
    for i in [0, 1]:
        if not h[i]:
            f[i].ls()
            print(hn, h)
            raise ValueError(f"{hn} not found in {f[i].GetName()}")
    # Set style of histos
    h[0].SetTitle("Run2")
    h[0].SetLineColor(2)
    h[0].SetLineStyle(7)
    h[0].SetLineWidth(2)
    h[1].SetTitle("Run3")
    h[0].SetDirectory(0)
    h[1].SetDirectory(0)
    return h


ldrawn = []


def drawrange(x, y, xtit="", ytit=""):
    if not isinstance(x, list):
        if xtit == "":
            xtit = x.GetXaxis().GetTitle()
        x = [x.GetXaxis().GetBinLowEdge(
            1), x.GetXaxis().GetBinUpEdge(x.GetNbinsX())]
    ldrawn.append(gPad.DrawFrame(
        x[0], y[0], x[1], y[1], ";" + xtit + ";" + ytit))
    ldrawn[-1].GetYaxis().CenterTitle()


latexdrawn = []


def draw(h, opt="", copy=False, labelize=True):
    hd = h
    if copy:
        print("Drawing", h, "as a copy")
        hd = h.DrawCopy(opt)
        if isinstance(copy, str):
            hd.SetName(hd.GetName() + copy)
        else:
            hd.SetName(hd.GetName() + "copy")
        hd.SetDirectory(0)
    else:
        h.Draw(opt)
    if labelize and "same" not in opt:
        latex = TLatex(0.2, 0.95, hd.GetName())
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
    leg = TLegend(0.7, 0.7, 0.9, 0.9)
    for i in h:
        leg.AddEntry(i, "", "f")
    # leg.SetHeader("Legend","C"); // option "C" allows to center the header
    legends.append(leg)


def drawtwo(h, logx=False, logy=False, project=True, ratio=True, V=True):
    def equals(h1, h2, Ck):
        cmd = ["h1." + Ck, "h2." + Ck]
        cmdR = [eval(cmd[0]), eval(cmd[1])]
        V = cmdR[0] == cmdR[1]
        if not V:
            raise ValueError(h1, "differs from", h2, Ck, cmd, cmdR)

    for i in "GetName GetNbinsX GetXaxis().GetTitle".split():
        equals(h[0], h[1], i + "()")
    nextpad()
    if logy:
        gPad.SetLogy()
    if logx:
        gPad.SetLogx()
    hrun2 = h[0]
    hrun3 = h[1]
    if project and "TH2" in h[0].ClassName():
        hrun2 = h[0].ProjectionY("Y1_" + h[0].GetName())
        hrun3 = h[1].ProjectionY("Y2_" + h[1].GetName())
    if V:
        print("Drawing", h)
    if "TH2" in hrun2.ClassName() and not project:
        draw(hrun2, "COLZ")
        nextpad()
        if logy:
            gPad.SetLogy()
        if logx:
            gPad.SetLogx()
        draw(hrun3, "COLZ")
    else:
        draw(hrun2)
        draw(hrun3, "same")
        drawcounts([hrun2, hrun3])
    print("Entries of", hrun2.GetName(), hrun2.GetEntries(), "vs", hrun3.GetEntries())
    if "TH1" in h[0].ClassName():
        legends[-1].Draw()
    if ratio:
        nextpad()
        drawrange(hrun2, [0.5, 1.5], ytit="Run2/Run3")
        hratio = draw(hrun2, "same", copy=True, labelize=False)
        hratio.SetTitle(hratio.GetTitle() + " Ratio")
        hratio.Divide(hrun3)
        gPad.Update()


canvaslist = []


def makecanvas(cname, ctit, sizex=1600, sizey=1600):
    canvaslist.append(TCanvas(cname, ctit, sizex, sizey))
    return canvaslist[-1]


def compare(filerun3, filerun1, avoid=""):
    f = [TFile(filerun3, "READ"), TFile(filerun1, "READ")]
    gStyle.SetOptStat(0)
    gStyle.SetOptTitle(0)
    dirlist = f[0].GetListOfKeys()
    hlist = []
    for i in dirlist:
        if i.GetName() in avoid:
            continue
        print("Directory", i)
        d = f[0].Get(i.GetName()).GetListOfKeys()
        for j in d:
            print(j)
            hlist.append(f"{i.GetName()}/{j.GetName()}")
    print(hlist)
    h = [get(f, i) for i in hlist]

    makelegend(h[0])

    for i in h:
        makecanvas(i[0].GetName(), i[0].GetName()).Divide(2, 1)
        drawtwo(i)

    gSystem.ProcessEvents()
    canvaslist[0].SaveAs("plots.pdf[")
    for i in canvaslist:
        i.SaveAs("plots.pdf")
    canvaslist[-1].SaveAs("plots.pdf]")
    if "-b" not in input():
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
