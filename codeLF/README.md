# Prepared LF tasks and tests

## PID in O2
Tables for PID values in O2 are defined in './Analysis/DataModel/include/PID/PIDResponse.h'
You can include it in you task with:
``` c++
#include "PID/PIDResponse.h"
```
In the process functions you can join the table for the TOF PID as:
``` c++
void process(aod::Collision const& collision, soa::Join<aod::Tracks, aod::TracksExtra, aod::pidRespTOF> const& tracks)
```
or for TPC PID as:
``` c++
void process(aod::Collision const& collision, soa::Join<aod::Tracks, aod::TracksExtra, aod::pidRespTPC> const& tracks)
```

## Task for PID spectra (and to fill PID tables!)
O2 tasks dedicated to the filling of the PID tables are called with  
* TOF PID Table
``` bash
o2-analysis-pid-tof
```
* TPC PID Table
``` bash
o2-analysis-pid-tpc
```

### TOF spectra task
You can run the TOF spectra task with:

``` bash
o2-analysis-spectra-tof --aod-file AO2D.root -b | o2-analysis-pid-tof -b
```
as in [`runTOFO2.sh`](https://github.com/AliceO2Group/Run3Analysisvalidation/tree/master/codeHF/runTOFO2.sh)

### TPC spectra task
You can run the TPC spectra task with:

``` bash
o2-analysis-spectra-tpc --aod-file AO2D.root -b | o2-analysis-pid-tpc -b
```
as in [`runTPCO2.sh`](https://github.com/AliceO2Group/Run3Analysisvalidation/tree/master/codeHF/runTPCO2.sh)
