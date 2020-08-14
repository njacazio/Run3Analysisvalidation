#!/bin/bash

reset

o2-analysis-spectra-tof --aod-file AO2D.root -b | o2-analysis-pid-tof -b

rm localhost*

