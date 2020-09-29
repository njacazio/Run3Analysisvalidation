#!/bin/bash

reset

function runit() {
    cd $1
    echo "PWD: ${@}"
    pwd
    CMD="o2-analysis-mc-validation --aod-file AO2D.root -b | o2-analysis-trackselection -b"
    CMD="/home/njacazio/Analysis/Run3Analysisvalidation/codeLF/runTPCO2.sh"
    echo $CMD
    eval $CMD > run.log 2>&1
    cd -
}

Path="${1}"
if [[ -z "$Path" ]]; then
    echo "Please provide data path"
    exit 1
fi

Dirs=$(ls -d "$Path"/*)

for i in $Dirs; do
    echo $i
    runit "${i}" &
    # sleep 0.5
done

wait

OUT_FILE="AnalysisResults_TPC.root"
Files=$(find ${Path} | grep ${OUT_FILE} | xargs)
echo "hadd /tmp/${OUT_FILE} $Files"


