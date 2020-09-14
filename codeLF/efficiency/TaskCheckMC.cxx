#include "TaskCheckMC.h"

#include "AliAnalysisManager.h"
#include "AliCentrality.h"
#include "AliESDEvent.h"
#include "AliESDVertex.h"
#include "AliESDtrack.h"
#include "AliESDtrackCuts.h"
#include "AliInputEventHandler.h"
#include "AliMCEvent.h"
#include "AliPIDResponse.h"
#include "AliStack.h"
#include "TDatabasePDG.h"
#include "TH3F.h"
#include "TObjArray.h"
#include "TParticle.h"
#include "TParticlePDG.h"

ClassImp(TaskCheckMC);

TaskCheckMC::TaskCheckMC(const TString partName)
    : AliAnalysisTaskSE("TaskCheckMC" + partName)
    , fEventCuts()
{
}

TaskCheckMC::~TaskCheckMC()
{
  Printf("Called destructor");
}

void TaskCheckMC::UserCreateOutputObjects() {}

void TaskCheckMC::UserExec(Option_t*)
{

  /* get ESD event */
  AliESDEvent* esdEvent = dynamic_cast<AliESDEvent*>(InputEvent());
  if (!esdEvent)
    return;
  /* get MC event */
  AliMCEvent* mcEvent = dynamic_cast<AliMCEvent*>(MCEvent());
  if (!mcEvent)
    return;
  /* get stack */
  AliStack* mcStack = mcEvent->Stack();
  if (!mcStack)
    return;

  /*** EVENT SELECTION ***/

  /* collision candidate */
  if (!fEventCuts.AcceptEvent(esdEvent)) {
    return;
  }

  /*** MONTECARLO PARTICLES ***/

  /* loop over MC stack */
  for (Int_t ipart = 0; ipart < mcStack->GetNtrack(); ipart++) {
    /* get particle */
    TParticle* particle = mcStack->Particle(ipart);
    if (!particle)
      continue;

    /* check physical primary */
    if (!mcStack->IsPhysicalPrimary(ipart))
      continue;
  }
}
