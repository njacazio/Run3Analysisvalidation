TaskEff* AddTaskEff(const TString partName, const TString tag = "")
{

  /* check analysis manager */
  AliAnalysisManager* mgr = AliAnalysisManager::GetAnalysisManager();
  if (!mgr) {
    Error("AddTaskEff", "cannot get analysis manager");
    return NULL;
  }

  /* check input event handler */
  if (!mgr->GetInputEventHandler()) {
    Error("AddTaskEff", "cannot get input event handler");
    return NULL;
  }

  /* check input data type */
  TString str = mgr->GetInputEventHandler()->GetDataType();
  if (str.CompareTo("ESD")) {
    Error("AddTaskEff", "input data type is not \"ESD\"");
    return NULL;
  }

  /* check MC truth event handler */
  if (!mgr->GetMCtruthEventHandler()) {
    Error("AddTaskEff", "cannot get MC truth event handler");
    return NULL;
  }

  /* get common input data container */
  AliAnalysisDataContainer* inputc = mgr->GetCommonInputContainer();
  if (!inputc) {
    Error("AddTaskEff", "cannot get common input container");
    return NULL;
  }

  /* create output data container */
  AliAnalysisDataContainer* outputc1 = mgr->CreateContainer("Eff_" + partName + tag, THashList::Class(), AliAnalysisManager::kOutputContainer, "ParticleEfficiency.root");
  if (!outputc1) {
    Error("", "cannot create output container \"Histos\"");
    return NULL;
  }

  /*  create task and connect input/output */
  TaskEff* task = new TaskEff(partName, tag);
  mgr->ConnectInput(task, 0, inputc);
  mgr->ConnectOutput(task, 1, outputc1);

  /* return task */
  return task;
}