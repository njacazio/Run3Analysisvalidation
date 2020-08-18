#!/bin/bash

TODOWNLOAD="AnalysisResults_TOF.root AnalysisResults_TPC.root TOFPid.root TPCPid.root"

for i in $TODOWNLOAD; do
  env -i scp aliceml:Analysis/Run3Analysisvalidation/codeLF/$i .
done

