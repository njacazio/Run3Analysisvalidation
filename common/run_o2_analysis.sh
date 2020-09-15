#!/bin/bash

reset

function runit() {
  cd $1
  echo "PWD: ${@}"
  pwd 
  CMD="o2-analysis-mc-validation --aod-file AO2D.root -b | o2-analysis-trackselection -b"
  echo $CMD
  eval $CMD
  cd -
}

Path="${1}"
if [[ -z "$Path" ]]; then
  echo "Please provide data path"
  exit 1
fi

Dirs=$(ls -d "$Path"/*)

for i in $Dirs; do
  runit "${i}" & 
done

wait

Files=$(find $1 | grep AnalysisResults.root)
echo "Files:"
echo $Files | xargs


