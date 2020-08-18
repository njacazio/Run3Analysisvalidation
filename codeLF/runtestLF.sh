#!/bin/bash

reset

LISTNAME="$1"

if [[ -z $LISTNAME ]]; then
    echo "Please provide an input list of ESDs"
    exit
fi

DOCONVERT=0
DORUN1=1
DORUN3=0
DOCOMPARE=0

APPLYEVTSELRUN1=1

if [[ $DOCONVERT -eq 1 ]]; then
    root -q -l "convertAO2D.C(\"$LISTNAME\", 0)"
fi

if [[ $DORUN1 -eq 1 ]]; then
#    root -q -l -b "ComputeTOFPID.C+g(\"$LISTNAME\", $APPLYEVTSELRUN1)"
    root -q -l -b "ComputeTPCPID.C+g(\"$LISTNAME\", $APPLYEVTSELRUN1)"
fi

if [[ $DORUN3 -eq 1 ]]; then
    ./runTOFO2.sh
    mv AnalysisResults.root AnalysisResults_TOF.root
    ./runTPCO2.sh
    mv AnalysisResults.root AnalysisResults_TPC.root
fi

if [[ $DOCOMPARE -eq 1 ]]; then
    ./compareO2_ESD.py -b
fi

if [[ $DORUN3ONAOD -eq 1 ]]; then
    o2-analysis-spectra --aod-file /data/Run3data/5_20200131-0902/0001/AO2D.root --readers 80 -b
    echo "Enter to continue"
    read INP
fi

rm localhost*_*
