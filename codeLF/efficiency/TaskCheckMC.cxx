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
#include <TPDGCode.h>

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

bool isphys(TParticle* particle)
{
#if 0
  if (!p)
    return kFALSE;
  Int_t ist = p->GetStatusCode();
  Int_t pdg = TMath::Abs(p->GetPdgCode());
  //
  // Initial state particle
  // Solution for K0L decayed by Pythia6
  // ->
  if ((ist > 1) && (pdg != 130) && index < GetNprimary())
    return kFALSE;
  if ((ist > 1) && index >= GetNprimary())
    return kFALSE;
  // <-

  if (!IsStable(pdg))
    return kFALSE;
  if (index < GetNprimary()) {
    //
    // Particle produced by generator
    // Solution for K0L decayed by Pythia6
    // ->
    Int_t ipm = p->GetFirstMother();
    if (ipm > -1) {
      TParticle* ppm = Particle(ipm, useInEmbedding);
      if (TMath::Abs(ppm->GetPdgCode()) == 130)
        return kFALSE;
    }
    // <-
    // check for direct photon in parton shower
    // ->
    Int_t ipd = p->GetFirstDaughter();
    if (pdg == 22 && ipd > -1) {
      TParticle* ppd = Particle(ipd, useInEmbedding);
      if (ppd->GetPdgCode() == 22)
        return kFALSE;
    }
    // <-
    return kTRUE;
  } else {
    //
    // Particle produced during transport
    //

    Int_t imo = p->GetFirstMother();
    TParticle* pm = Particle(imo, useInEmbedding);
    Int_t mpdg = TMath::Abs(pm->GetPdgCode());
    // Check for Sigma0
    if ((mpdg == 3212) && (imo < GetNprimary()))
      return kTRUE;
    //
    // Check if it comes from a pi0 decay
    //
    if ((mpdg == kPi0) && (imo < GetNprimary()))
      return kTRUE;

    // Check if this is a heavy flavor decay product
    Int_t mfl = Int_t(mpdg / TMath::Power(10, Int_t(TMath::Log10(mpdg))));
    //
    // Light hadron
    if (mfl < 4)
      return kFALSE;

    //
    // Heavy flavor hadron produced by generator
    if (imo < GetNprimary()) {
      return kTRUE;
    }

    // To be sure that heavy flavor has not been produced in a secondary interaction
    // Loop back to the generated mother
    while (imo >= GetNprimary()) {
      imo = pm->GetFirstMother();
      pm = Particle(imo, useInEmbedding);
    }
    mpdg = TMath::Abs(pm->GetPdgCode());
    mfl = Int_t(mpdg / TMath::Power(10, Int_t(TMath::Log10(mpdg))));

    if (mfl < 4) {
      return kFALSE;
    } else {
      return kTRUE;
    }
  } // produced by generator ?
#endif
  return kFALSE;
}

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
  events++;

  /*** MONTECARLO PARTICLES ***/

  /* loop over MC stack */
  for (Int_t ipart = 0; ipart < mcStack->GetNtrack(); ipart++) {
    /* get particle */
    TParticle* particle = mcStack->Particle(ipart);
    if (!particle)
      continue;
    if (isphys(particle))
      Printf("its a prim!");

    /* check physical primary */
    if (!mcStack->IsPhysicalPrimary(ipart))
      continue;
    primaryparticles++;
  }
}

void TaskCheckMC::Terminate(Option_t*)
{
  // terminate
  // called at the END of the analysis (when all events are processed)
  Printf("Events %i", events);
  Printf("Primaries %i", primaryparticles);
}
