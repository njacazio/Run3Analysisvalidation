#!/bin/bash

TODOWNLOAD="AnalysisResults.root TOFPid.root TPCPid.root"

for i in $TODOWNLOAD; do
  scp aliceml:Analysis/Run3Analysisvalidation/codeLF/$i .
done

