TaskCheckMC* AddTaskCheckMC(const TString partName)
{

  /* check analysis manager */
  AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    Error("AddTaskCheckMC", "cannot get analysis manager");
    return NULL;
  }

  /* check input event handler */
  if (!mgr->GetInputEventHandler()) {
    Error("AddTaskCheckMC", "cannot get input event handler");
    return NULL;
  }

  /* check input data type */
  TString str = mgr->GetInputEventHandler()->GetDataType();
  if (str.CompareTo("ESD")) {
    Error("AddTaskCheckMC", "input data type is not \"ESD\"");
    return NULL;
  }

  /* check MC truth event handler */
  if (!mgr->GetMCtruthEventHandler()) {
    Error("AddTaskCheckMC", "cannot get MC truth event handler");
    return NULL;
  }

  /* get common input data container */
  AliAnalysisDataContainer* inputc = mgr->GetCommonInputContainer();
  if (!inputc) {
    Error("AddTaskCheckMC", "cannot get common input container");
    return NULL;
  }

  /* create output data container */
  AliAnalysisDataContainer* outputc = mgr->CreateContainer("generator-histogram", TList::Class(), AliAnalysisManager::kOutputContainer, "AnalysisResults.root");
  if (!outputc) {
    Error("", "cannot create output container \"Histos\"");
    return NULL;
  }

  /*  create task and connect input/output */
  TaskCheckMC* task = new TaskCheckMC(partName);
  mgr->ConnectInput(task, 0, inputc);
  mgr->ConnectOutput(task, 1, outputc);

  /* return task */
  return task;
}