#!/usr/bin/python3

"""
Set of utilities for analysis
"""

from ROOT import TFile, gPad


def get_obj(input_file, obj_name, path="", V=True):
    """
    Function to get the object inside a file
    """
    if type(input_file) == type(""):
        input_file = TFile(input_file, "READ")
    if obj_name == "":
        input_file.ls()
        if path != "":
            input_file.Get(path).ls()
        return
    if V:
        print("Getting", obj_name, "from", input_file.GetName())
    obj = input_file.Get(f"{path}{obj_name}")
    if not obj:
        print("Did not find", obj_name)
        return None
    if "TH" in obj.ClassName():
        obj.SetDirectory(0)
    return obj


def gettwo(run2, run3, hn, d="", V=True):
    if V:
        print("Getting", d, hn, "from", run2, "and", run3)
    hr2 = get_obj(run2, hn, d)
    hr3 = get_obj(run3, hn, d)
    # Set style of histos
    hr2.SetTitle("Run2")
    hr2.SetName(hr2.GetName())
    hr2.SetLineColor(2)
    hr2.SetLineStyle(7)
    hr2.SetLineWidth(2)
    hr3.SetTitle("Run3")
    hr3.SetName(hr3.GetName())
    return hr2, hr3


def draw(obj, opt=""):
    obj.Draw(opt)
    gPad.Update()
