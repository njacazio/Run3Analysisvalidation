#!/usr/bin/python3

"""
Set of utilities for analysis
"""

from ROOT import TFile


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
