#include <string>

#include <AliAnalysisTaskAO2Dconverter.h>
#include <AliAnalysisTaskSpectraTPCRun3.h>
#include <AliAnalysisTaskWeakDecayVertexer.h>
#include <AliPhysicsSelectionTask.h>
#include <TROOT.h>

void ConfigureTrain()
{
  // Add mult selection Task
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C");
  gROOT->ProcessLine("AddTaskMultSelection()");

  // PhysicsSelection Configuration
  // gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
  // Use a reinterpreted cast to get the task for further configurations in this task
  gROOT->ProcessLine("AddTaskPhysicsSelection(false, true)");

  if (1) { // PID task
    // PID
    gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
    gROOT->ProcessLine("AddTaskPIDResponse()");
  }
  if (0) { // Conversion for run 3
    // Weak decays
    gROOT->LoadMacro("$ALICE_PHYSICS/PWGLF/STRANGENESS/Cascades/Run2/macros/AddTaskWeakDecayVertexer.C");
    AliAnalysisTaskWeakDecayVertexer* decays = reinterpret_cast<AliAnalysisTaskWeakDecayVertexer*>(gROOT->ProcessLine("AddTaskWeakDecayVertexer()"));
    decays->SetUseImprovedFinding();
    decays->SetupLooseVertexing();
    decays->SetRevertexAllEvents(kTRUE);

    // Converter
    gROOT->LoadMacro("$ALICE_PHYSICS/RUN3/AddTaskAO2Dconverter.C");
    AliAnalysisTaskAO2Dconverter* converter = reinterpret_cast<AliAnalysisTaskAO2Dconverter*>(gROOT->ProcessLine("AddTaskAO2Dconverter()"));
    converter->SetUseEventCuts(true);
    converter->SetTruncation(true);
  }
  if (1) { // Spectra task
    gROOT->LoadMacro("$ALICE_PHYSICS/PWGLF/SPECTRA/PiKaPr/TPC/Run3/AddTaskSpectraTPCRun3.C");
    // AliAnalysisTaskSpectraTPCRun3* AddTaskSpectraTPCRun3(Bool_t mc = kFALSE, Bool_t aod = kTRUE, Bool_t o2pid = kTRUE)
    AliAnalysisTaskSpectraTPCRun3* bc = reinterpret_cast<AliAnalysisTaskSpectraTPCRun3*>(gROOT->ProcessLine("AddTaskSpectraTPCRun3(kFALSE, kTRUE, kFALSE)"));
    bc->fUseEventSelection = kFALSE;
  }
}
