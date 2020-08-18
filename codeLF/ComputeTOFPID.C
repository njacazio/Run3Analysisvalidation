#include "common.h"

// O2 includes
#include "PID/TOFReso.h"
#include "PID/PIDTOF.h"

using namespace o2::pid;

Bool_t ComputeTOFPID(TString esdfile = "esdLHC15o.txt", bool applyeventcut = 0, bool ismc = kFALSE)
{
  // Defining response
  auto resp = tof::Response();
  // Downloading param from ccdb
  gSystem->Exec("o2-ccdb-downloadccdbfile -p Analysis/PID/TOF/TOFReso -d /tmp/ -t -1");
  // Loading param
  resp.LoadParamFromFile("/tmp/Analysis/PID/TOF/TOFReso/snapshot.root", "ccdb_object", DetectorResponse::kSigma);

  // Defining input
  TChain* chain = CreateLocalChain(esdfile, "ESD");
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
  DOTH1F(hvtxz, ";Vertex Z position;Events", 300, -15, 15);
  DOTH1F(hevtime, ";Event time (ns);Tracks", 100, -2, 2);
  DOTH1F(heta, ";#eta;Tracks", 100, -1, 1);
  DOTH1F(hp, ";#it{p} (GeV/#it{c});Tracks", 100, 0, 20);
  DOTH1F(hpt, ";#it{p}_{T} (GeV/#it{c});Tracks", 100, 0, 20);
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

  AliPIDResponse* pidr = new AliPIDResponse(ismc);

  for (Int_t iEvent = 0; iEvent < chain->GetEntries(); iEvent++) { // Loop on events
    if (!SetEvent(chain, iEvent, esd))
      return kFALSE;

    // pidr->SetTOFResponse(esd, AliPIDResponse::kBest_T0);
    // pidr->SetTOFResponse(esd, AliPIDResponse::kT0_T0);
    pidr->SetTOFResponse(esd, AliPIDResponse::kTOF_T0);

    if (applyeventcut && !VertexOK(esd, hvtxz))
      continue;

    Float_t eventTime[10];
    Float_t eventTimeRes[10];
    Double_t eventTimeWeight[10];
    AliTOFPIDResponse TOFresp = pidr->GetTOFResponse();

    for (Int_t i = 0; i < TOFresp.GetNmomBins(); i++) {
      Float_t mom = (TOFresp.GetMinMom(i) + TOFresp.GetMaxMom(i)) / 2.f;
      // Printf("Mom [%.2f, %.2f] = %.2f", TOFresp.GetMinMom(i), TOFresp.GetMaxMom(i), mom);
      eventTime[i] = TOFresp.GetStartTime(mom);
      // Printf("T0=%f", eventTime[i]);
      eventTimeRes[i] = TOFresp.GetStartTimeRes(mom);
      eventTimeWeight[i] = 1. / (eventTimeRes[i] * eventTimeRes[i]);
    }

    // Recalculate unique event time and its resolution
    const float fEventTime = TMath::Mean(10, eventTime, eventTimeWeight);              // Weighted mean of times per momentum interval
    const float fEventTimeRes = TMath::Sqrt(9. / 10.) * TMath::Mean(10, eventTimeRes); // PH bad approximation
    for (Int_t itrk = 0; itrk < esd->GetNumberOfTracks(); itrk++) {
      AliESDtrack* trk = esd->GetTrack(itrk);
      if (!TrackOK(trk, kTRUE))
        continue;
      // Printf("Track accepted!");
      const float Mom = trk->P();
      const float pt = trk->Pt();
      const float length = trk->GetIntegratedLength();
      const float time = trk->GetTOFsignal();

      // const float EVTIME = TOFresp.GetStartTime(Mom);
      const float EVTIME = fEventTime;
      // const float EVTIME = eventTime[0];

      // Speed of ligth in TOF units
      const Float_t cspeed = 0.029979246f;
      // PID hypothesis for the momentum extraction
      const AliPID::EParticleType tof_pid = AliPID::kPion;
      // Expected beta for such hypothesis
      const Float_t exp_beta = (length / TOFresp.GetExpectedSignal(trk, tof_pid) / cspeed);
      const Float_t tofexpmom = AliPID::ParticleMass(tof_pid) * exp_beta * cspeed / TMath::Sqrt(1. - (exp_beta * exp_beta));
      resp.UpdateTrack(Mom, tofexpmom / cspeed, length, time);

      heta->Fill(trk->Eta());
      hp->Fill(Mom);
      hpt->Fill(pt);
      hlength->Fill(length);
      htime->Fill(time / 1000);
      hevtime->Fill(EVTIME / 1000);
      const float TOF = time - EVTIME;
      hp_beta->Fill(Mom, resp.GetBeta(length, time, EVTIME));
      //
      int counter = 0;
// #define EXP_SIGNAL resp.GetExpectedSignal(counter++)
#define EXP_SIGNAL TOFresp.GetExpectedSignal(trk, pidarray[counter++])

      htimediffEl->Fill(Mom, (TOF - EXP_SIGNAL));
      htimediffMu->Fill(Mom, (TOF - EXP_SIGNAL));
      htimediffPi->Fill(Mom, (TOF - EXP_SIGNAL));
      htimediffKa->Fill(Mom, (TOF - EXP_SIGNAL));
      htimediffPr->Fill(Mom, (TOF - EXP_SIGNAL));
      htimediffDe->Fill(Mom, (TOF - EXP_SIGNAL));
      htimediffTr->Fill(Mom, (TOF - EXP_SIGNAL));
      htimediffHe->Fill(Mom, (TOF - EXP_SIGNAL));
      htimediffAl->Fill(Mom, (TOF - EXP_SIGNAL));
      //
      counter = 0;
// #define EXP_SIGMA resp.GetSeparation(counter++)
// #define EXP_SIGMA TOFresp.GetExpectedSignal(trk, AliPID::kElectron + counter++)
#define EXP_SIGMA (TOF - TOFresp.GetExpectedSignal(trk, pidarray[counter])) / TOFresp.GetExpectedSigma(Mom, time, pidarray[counter++])
      hnsigmaEl->Fill(Mom, EXP_SIGMA);
      hnsigmaMu->Fill(Mom, EXP_SIGMA);
      hnsigmaPi->Fill(Mom, EXP_SIGMA);
      hnsigmaKa->Fill(Mom, EXP_SIGMA);
      hnsigmaPr->Fill(Mom, EXP_SIGMA);
      hnsigmaDe->Fill(Mom, EXP_SIGMA);
      hnsigmaTr->Fill(Mom, EXP_SIGMA);
      hnsigmaHe->Fill(Mom, EXP_SIGMA);
      hnsigmaAl->Fill(Mom, EXP_SIGMA);
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
