#ifndef TASKCHECKMC_H
#define TASKCHECKMC_H

#include "THashList.h"

#include "AliAnalysisTaskSE.h"
#include "AliESDtrackCuts.h"
#include "AliEventCuts.h"
#include "AliPID.h"
#include "TEfficiency.h"

/// Task to build the material for the tracking and TOF matching efficiencies
#include "AliEventCuts.h"

class TaskCheckMC : public AliAnalysisTaskSE {

  public:
  // default constructor
  TaskCheckMC();
  // param. constructor
  TaskCheckMC(const TString partName);
  // default destructor
  ~TaskCheckMC();
  AliEventCuts fEventCuts; //Event cuts

  void UserCreateOutputObjects();  // user create output objects
  void UserExec(Option_t* option); // user exec

  protected:
  // copy constructor
  TaskCheckMC(const TaskCheckMC&);
  // operator=
  TaskCheckMC& operator=(const TaskCheckMC&);

  ClassDef(TaskCheckMC, 1);
};

#endif /* TASKCHECKMC_H */
