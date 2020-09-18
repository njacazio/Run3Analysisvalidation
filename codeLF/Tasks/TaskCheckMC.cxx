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
  DefineOutput(1, TList::Class());
}

TaskCheckMC::~TaskCheckMC()
{
  Printf("Called destructor");
}
#define PDGBINNING 100, 0, 100

void TaskCheckMC::UserCreateOutputObjects()
{
  lOut = new TList();
  lOut->SetOwner(kTRUE);

  ptH = new TH2F("pt", "pt;#it{p}_{T} (GeV/#it{c});PDG code", 500, 0, 20, PDGBINNING);
  lOut->Add(ptH);
  pdgH = new TH1F("pdg", "pdg;PDG code", PDGBINNING);
  lOut->Add(pdgH);
  PostData(1, lOut);
}
#undef PDGBINNING

Bool_t IsStable(Int_t pdg)
{
  //
  // Decide whether particle (pdg) is stable
  //

  // All ions/nucleons are considered as stable
  // Nuclear code is 10LZZZAAAI
  if (pdg > 1000000000)
    return kTRUE;

  const Int_t kNstable = 18;
  Int_t i;

  Int_t pdgStable[kNstable] = {
    kGamma,      // Photon
    kElectron,   // Electron
    kMuonPlus,   // Muon
    kPiPlus,     // Pion
    kKPlus,      // Kaon
    kK0Short,    // K0s
    kK0Long,     // K0l
    kProton,     // Proton
    kNeutron,    // Neutron
    kLambda0,    // Lambda_0
    kSigmaMinus, // Sigma Minus
    kSigmaPlus,  // Sigma Plus
    3312,        // Xsi Minus
    3322,        // Xsi
    3334,        // Omega
    kNuE,        // Electron Neutrino
    kNuMu,       // Muon Neutrino
    kNuTau       // Tau Neutrino
  };

  Bool_t isStable = kFALSE;
  for (i = 0; i < kNstable; i++) {
    if (pdg == TMath::Abs(pdgStable[i])) {
      isStable = kTRUE;
      break;
    }
  }

  return isStable;
}

bool isphys(Int_t index, AliStack* stack)
{
  TParticle* p = stack->Particle(index);
  if (!p)
    return kFALSE;
  Int_t ist = p->GetStatusCode();
  Int_t pdg = TMath::Abs(p->GetPdgCode());
  //
  // Initial state particle
  // Solution for K0L decayed by Pythia6
  // ->
  if ((ist > 1) && (pdg != 130) && index < stack->GetNprimary())
    return kFALSE;
  if ((ist > 1) && index >= stack->GetNprimary())
    return kFALSE;
  // <-

  if (!IsStable(pdg))
    return kFALSE;
  if (index < stack->GetNprimary()) {
    //
    // Particle produced by generator
    // Solution for K0L decayed by Pythia6
    // ->
    Int_t ipm = p->GetFirstMother();
    if (ipm > -1) {
      TParticle* ppm = stack->Particle(ipm);
      if (TMath::Abs(ppm->GetPdgCode()) == 130)
        return kFALSE;
    }
    // <-
    // check for direct photon in parton shower
    // ->
    Int_t ipd = p->GetFirstDaughter();
    if (pdg == 22 && ipd > -1) {
      TParticle* ppd = stack->Particle(ipd);
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
    TParticle* pm = stack->Particle(imo);
    Int_t mpdg = TMath::Abs(pm->GetPdgCode());
    // Check for Sigma0
    if ((mpdg == 3212) && (imo < stack->GetNprimary()))
      return kTRUE;
    //
    // Check if it comes from a pi0 decay
    //
    if ((mpdg == kPi0) && (imo < stack->GetNprimary()))
      return kTRUE;

    // Check if this is a heavy flavor decay product
    Int_t mfl = Int_t(mpdg / TMath::Power(10, Int_t(TMath::Log10(mpdg))));
    //
    // Light hadron
    if (mfl < 4)
      return kFALSE;

    //
    // Heavy flavor hadron produced by generator
    if (imo < stack->GetNprimary()) {
      return kTRUE;
    }

    // To be sure that heavy flavor has not been produced in a secondary interaction
    // Loop back to the generated mother
    while (imo >= stack->GetNprimary()) {
      imo = pm->GetFirstMother();
      pm = stack->Particle(imo);
    }
    mpdg = TMath::Abs(pm->GetPdgCode());
    mfl = Int_t(mpdg / TMath::Power(10, Int_t(TMath::Log10(mpdg))));

    if (mfl < 4) {
      return kFALSE;
    } else {
      return kTRUE;
    }
  } // produced by generator ?
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
    PostData(1, lOut);

    return;
  }
  events++;

  /*** MONTECARLO PARTICLES ***/

  /* loop over MC stack */

  TParticle* particle;
#if 0
  for (Int_t ipart = 0; ipart < mcStack->GetNtrack(); ipart++) {
    /* get particle */
    particle = mcStack->Particle(ipart);
#else
  for (Int_t ipart = 0; ipart < mcEvent->GetNumberOfTracks(); ++ipart) {
    AliVParticle* vpt = mcEvent->GetTrack(ipart);
    particle = vpt->Particle();
#endif
    if (!particle)
      continue;
    if (TMath::Abs(particle->Eta()) > 0.8)
      continue;
    if (isphys(ipart, mcStack))
      myprimaryparticles++;

    particles++;
    /* check physical primary */
    if (!mcStack->IsPhysicalPrimary(ipart))
      continue;
    primaryparticles++;

    const auto pdg = Form("%i", particle->GetPdgCode());
    pdgH->Fill(pdg, 1);
    const float pdgbin = pdgH->GetXaxis()->GetBinCenter(pdgH->GetXaxis()->FindBin(pdg));

    ptH->Fill(particle->Pt(), pdgbin);
  }
  PostData(1, lOut);
}

void TaskCheckMC::Terminate(Option_t*)
{
  // terminate
  // called at the END of the analysis (when all events are processed)
  Printf("Events %i", events);
  Printf("Particles %i", particles);
  Printf("Primaries %i", primaryparticles);
  Printf("My Primaries %i", myprimaryparticles);
}
