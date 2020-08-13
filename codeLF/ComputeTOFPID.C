#include "common.h"

// O2 includes
#include "PID/TOFReso.h"
#include "PID/PIDTOF.h"

using namespace o2::pid;

Bool_t ComputeTOFPID(TString esdfile = "esdLHC15o.txt",
                     bool applyeventcut = 0)
{
  // Defining response
  auto resp = tof::Response();
  // Downloading param from ccdb
  gSystem->Exec("o2-ccdb-downloadccdbfile -p Analysis/PID/TOF/TOFReso -d /tmp/ -t -1");
  // Loading param
  resp.LoadParamFromFile("/tmp/Analysis/PID/TOF/TOFReso/snapshot.root", "ccdb_object", DetectorResponse::kSigma);

  // Defining input
  TChain* chain = CreateLocalChain(esdfile, "ESD", 10);
  Printf("Computing TOF Pid Spectra");
  if (!chain) {
    printf("Error: no ESD chain found");
    return kFALSE;
  }
  AliESDEvent* esd = new AliESDEvent;
  Printf("Reading TTree with %lli events", chain->GetEntries());
  esd->ReadFromTree(chain);

  TList* lh = nullptr;
  lh = MakeList("tofqa-task");
  DOTH1F(hevtime, ";Event time (ns);Tracks", 100, -2, 2);
  DOTH1F(hp, ";#it{p} (GeV/#it{c});Tracks", 100, 0, 20);
  DOTH1F(hlength, ";Track Length (cm);Tracks", 100, 0, 1000);
  DOTH1F(htime, ";TOF Time (ns);Tracks", 1000, 0, 600);
  DOTH2F(hp_beta, ";#it{p} (GeV/#it{c});TOF #beta;Tracks", 100, 0, 20, 100, 0, 2);
#define TIT(part) Form(";#it{p} (GeV/#it{c});(t-t_{evt}-t_{exp %s});Tracks", part)
  lh = MakeList("tofexptime-task");
  DOTH2F(htimediffEl, TIT("e"), 100, 0, 5, 100, -1000, 1000);
  DOTH2F(htimediffMu, TIT("#mu"), 100, 0, 5, 100, -1000, 1000);
  DOTH2F(htimediffPi, TIT("#pi"), 100, 0, 5, 100, -1000, 1000);
  DOTH2F(htimediffKa, TIT("K"), 100, 0, 5, 100, -1000, 1000);
  DOTH2F(htimediffPr, TIT("p"), 100, 0, 5, 100, -1000, 1000);
  DOTH2F(htimediffDe, TIT("d"), 100, 0, 5, 100, -1000, 1000);
  DOTH2F(htimediffTr, TIT("t"), 100, 0, 5, 100, -1000, 1000);
  DOTH2F(htimediffHe, TIT("^{3}He"), 100, 0, 5, 100, -1000, 1000);
  DOTH2F(htimediffAl, TIT("#alpha"), 100, 0, 5, 100, -1000, 1000);
#undef TIT
#define TIT(part) Form(";#it{p} (GeV/#it{c});(t-t_{evt}-t_{exp %s})/N_{sigma %s};Tracks", part, part)
  lh = MakeList("tofnsigma-task");
  DOTH2F(hnsigmaEl, TIT("e"), 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaMu, TIT("#mu"), 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaPi, TIT("#pi"), 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaKa, TIT("K"), 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaPr, TIT("p"), 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaDe, TIT("d"), 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaTr, TIT("t"), 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaHe, TIT("^{3}He"), 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaAl, TIT("#alpha"), 100, 0, 5, 100, -10, 10);
#undef TIT
#define TIT ";#it{p}_{T} (GeV/#it{c});Tracks"
  lh = MakeList("tofspectra-task");
  DOTH1F(hpt_El, TIT, 100, 0, 20);
  DOTH1F(hpt_Pi, TIT, 100, 0, 20);
  DOTH1F(hpt_Ka, TIT, 100, 0, 20);
  DOTH1F(hpt_Pr, TIT, 100, 0, 20);
#undef TIT
#define TIT ";#it{p} (GeV/#it{c});Tracks"
  DOTH1F(hp_El, TIT, 100, 0, 20);
  DOTH1F(hp_Pi, TIT, 100, 0, 20);
  DOTH1F(hp_Ka, TIT, 100, 0, 20);
  DOTH1F(hp_Pr, TIT, 100, 0, 20);
#undef TIT
  DOTH1F(hlength_El, ";Track Length (cm);Tracks", 100, 0, 1000);
  DOTH1F(htime_El, ";TOF Time (ns);Tracks", 1000, 0, 600);
  DOTH2F(hp_beta_El, ";#it{p} (GeV/#it{c});#beta - #beta_{el};Tracks", 100, 0, 20, 100, -0.01, 0.01);
  DOTH2F(hp_betasigma_El, ";#it{p} (GeV/#it{c});(#beta - #beta_{el})/#sigma;Tracks", 100, 0, 20, 100, -5, 5);

  AliPIDResponse* pidr = new AliPIDResponse(kTRUE);

  for (Int_t iEvent = 0; iEvent < chain->GetEntries(); iEvent++) { // Loop on events
    if (!SetEvent(chain, iEvent, esd))
      return kFALSE;

    // pidr->SetTOFResponse(esd, AliPIDResponse::kBest_T0);
    // pidr->SetTOFResponse(esd, AliPIDResponse::kT0_T0);
    pidr->SetTOFResponse(esd, AliPIDResponse::kTOF_T0);

    if (applyeventcut && AcceptVertex(esd))
      continue;

    Float_t eventTime[10];
    Float_t eventTimeRes[10];
    Double_t eventTimeWeight[10];

    for (Int_t i = 0; i < pidr->GetTOFResponse().GetNmomBins(); i++) {
      Float_t mom = (pidr->GetTOFResponse().GetMinMom(i) + pidr->GetTOFResponse().GetMaxMom(i)) / 2.f;
      // Printf("Mom [%.2f, %.2f] = %.2f", pidr->GetTOFResponse().GetMinMom(i), pidr->GetTOFResponse().GetMaxMom(i), mom);
      eventTime[i] = pidr->GetTOFResponse().GetStartTime(mom);
      // Printf("T0=%f", eventTime[i]);
      eventTimeRes[i] = pidr->GetTOFResponse().GetStartTimeRes(mom);
      eventTimeWeight[i] = 1. / (eventTimeRes[i] * eventTimeRes[i]);
    }

    // Recalculate unique event time and its resolution
    float fEventTime = TMath::Mean(10, eventTime, eventTimeWeight); // Weighted mean of times per momentum interval

    float fEventTimeRes = TMath::Sqrt(9. / 10.) * TMath::Mean(10, eventTimeRes); // PH bad approximation

    // float EVTIME = pidr->GetTOFResponse().GetStartTime(Mom);
    // const float EVTIME = fEventTime;
    const float EVTIME = eventTime[0];

    for (Int_t itrk = 0; itrk < esd->GetNumberOfTracks(); itrk++) {
      AliESDtrack* trk = esd->GetTrack(itrk);
      const float Mom = trk->P();
      const float pt = trk->Pt();
      const float length = trk->GetIntegratedLength();
      const float time = trk->GetTOFsignal();

      // Speed of ligth in TOF units
      const Float_t cspeed = 0.029979246f;
      // PID hypothesis for the momentum extraction
      const AliPID::EParticleType tof_pid = AliPID::kPion;
      // Expected beta for such hypothesis
      const Float_t exp_beta = (length / pidr->GetTOFResponse().GetExpectedSignal(trk, tof_pid) / cspeed);
      const Float_t tofexpmom = AliPID::ParticleMass(tof_pid) * exp_beta * cspeed / TMath::Sqrt(1. - (exp_beta * exp_beta));
      resp.UpdateTrack(Mom, tofexpmom, length, time);

      if (!AcceptTrack(trk, kTRUE))
        continue;
      hp->Fill(Mom);
      hlength->Fill(length);
      htime->Fill(time / 1000);
      hevtime->Fill(EVTIME / 1000);
      //
      const AliTOFPIDResponse tofresp = pidr->GetTOFResponse();
      htimediffEl->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kElectron)));
      htimediffMu->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kMuon)));
      htimediffPi->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kPion)));
      htimediffKa->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kKaon)));
      htimediffPr->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kProton)));
      htimediffDe->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kDeuteron)));
      htimediffTr->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kTriton)));
      htimediffHe->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kHe3)));
      htimediffAl->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kAlpha)));
      //
      hnsigmaEl->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kElectron)) / tofresp.GetExpectedSigma(Mom, time, AliPID::kElectron));
      hnsigmaMu->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kMuon)) / tofresp.GetExpectedSigma(Mom, time, AliPID::kMuon));
      hnsigmaPi->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kPion)) / tofresp.GetExpectedSigma(Mom, time, AliPID::kPion));
      hnsigmaKa->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kKaon)) / tofresp.GetExpectedSigma(Mom, time, AliPID::kKaon));
      hnsigmaPr->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kProton)) / tofresp.GetExpectedSigma(Mom, time, AliPID::kProton));
      hnsigmaDe->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kDeuteron)) / tofresp.GetExpectedSigma(Mom, time, AliPID::kDeuteron));
      hnsigmaTr->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kTriton)) / tofresp.GetExpectedSigma(Mom, time, AliPID::kTriton));
      hnsigmaHe->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kHe3)) / tofresp.GetExpectedSigma(Mom, time, AliPID::kHe3));
      hnsigmaAl->Fill(Mom, (time - tofresp.GetExpectedSignal(trk, AliPID::kAlpha)) / tofresp.GetExpectedSigma(Mom, time, AliPID::kAlpha));
      //

      if (pidr->NumberOfSigmasTOF(trk, AliPID::kElectron) < 3) {
        hp_El->Fill(Mom);
        hpt_El->Fill(pt);
      } else if (pidr->NumberOfSigmasTOF(trk, AliPID::kKaon) < 3) {
        hp_Ka->Fill(Mom);
        hpt_Ka->Fill(pt);
      } else if (pidr->NumberOfSigmasTOF(trk, AliPID::kProton) < 3) {
        hp_Pr->Fill(Mom);
        hpt_Pr->Fill(pt);
      }

      // Electron id via TOF beta
      const float BETA = resp.GetBeta();
      const float BETAERROR = resp.GetBetaExpectedSigma();
      const float EXPBETAEL = resp.GetExpectedBeta(o2::track::PID::Electron);
      const float EXPBETAELERROR = 0;
      const float betadiff = BETA - EXPBETAEL;
      if (abs(betadiff / BETAERROR) < 1) {
        hp_El->Fill(Mom);
        hpt_El->Fill(pt);
        //
        hlength_El->Fill(length);
        htime_El->Fill(time / 1000);
        //
        hp_beta_El->Fill(Mom, betadiff);
        hp_betasigma_El->Fill(Mom, betadiff / BETAERROR);
      }
    }
    esd->ResetStdContent();
  }
  Save("TOFPid.root");
  return true;
}
