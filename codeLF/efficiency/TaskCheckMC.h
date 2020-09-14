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
  TaskCheckMC() = default;
  // param. constructor
  TaskCheckMC(const TString partName);
  // default destructor
  ~TaskCheckMC();
  AliEventCuts fEventCuts; //Event cuts

  void UserCreateOutputObjects();  // user create output objects
  void UserExec(Option_t* option); // user exec
  virtual void Terminate(Option_t* option);

  protected:
  // copy constructor
  TaskCheckMC(const TaskCheckMC&);
  // operator=
  TaskCheckMC& operator=(const TaskCheckMC&);

  int events = 0;
  int primaryparticles = 0;
  int myprimaryparticles = 0;

  ClassDef(TaskCheckMC, 1);
};

#endif /* TASKCHECKMC_H */
