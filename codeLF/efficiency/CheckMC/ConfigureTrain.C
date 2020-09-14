#include <string>

#include <AliAnalysisTaskAO2Dconverter.h>
#include <AliAnalysisTaskPIDResponse.h>
#include <AliAnalysisTaskWeakDecayVertexer.h>
#include <AliExternalTrackParam.h>
#include <AliPhysicsSelectionTask.h>
#include <TROOT.h>

void ConfigureTrain()
{
  // Add mult selection Task
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/COMMON/MULTIPLICITY/macros/AddTaskMultSelection.C");
  gROOT->ProcessLine("AddTaskMultSelection()");

  // PhysicsSelection Configuration
  gROOT->LoadMacro("$ALICE_PHYSICS/OADB/macros/AddTaskPhysicsSelection.C");
  gROOT->ProcessLine("AddTaskPhysicsSelection(kTRUE)");

  // PID
  gROOT->LoadMacro("$ALICE_ROOT/ANALYSIS/macros/AddTaskPIDResponse.C");
  gROOT->ProcessLine("AddTaskPIDResponse()");

  // Weak decays
  gROOT->LoadMacro("$ALICE_ROOT/PWGLF/STRANGENESS/Cascades/Run2/macros/AddTaskWeakDecayVertexer.C");
  AliAnalysisTaskWeakDecayVertexer* decays = reinterpret_cast<AliAnalysisTaskWeakDecayVertexer*>(gROOT->ProcessLine("AddTaskPIDResponse()"));
  decays->SetUseImprovedFinding();
  decays->SetupLooseVertexing();
  decays->SetRevertexAllEvents(kTRUE);

  // Converter
  gROOT->LoadMacro("$ALICE_PHYSICS/RUN3/AddTaskAO2Dconverter.C");
  AliAnalysisTaskAO2Dconverter* converter = reinterpret_cast<AliAnalysisTaskAO2Dconverter*>(gROOT->ProcessLine("AddTaskAO2Dconverter()"));
  converter->SetMCMode();
  converter->SetTruncation(true);

  gROOT->LoadMacro("TaskCheckMC.cxx+g");
  gROOT->LoadMacro("AddTaskCheckMC.C");
  gROOT->ProcessLine("AddTaskCheckMC(\"check\");");
}
