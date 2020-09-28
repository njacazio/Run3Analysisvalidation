#!/bin/bash

reset

#o2-analysis-pid-tpc --aod-file AO2D.root -b
#o2-analysis-spectra-tpc --aod-file AO2D.root -b | o2-analysis-pid-tpc --aod-file AO2D.root -b
#o2-analysis-pid-tpc --aod-file AO2D.root -b | o2-analysis-spectra-tpc -b 
#o2-analysis-spectra-tpc --aod-file AO2D.root -b | o2-analysis-pid-tpc -b --param-file /tmp/param.root
# o2-analysis-spectra-tpc --aod-file AO2D.root -b | o2-analysis-pid-tpc -b

o2-analysis-spectra-tpc --aod-file AO2D.root -b | o2-analysis-pid-tpc -b | o2-analysis-pid-tof -b

#o2-analysis-pid-tpc --aod-file AO2D.root -b 

mv AnalysisResults.root AnalysisResults_TPC.root

rm localhost*

