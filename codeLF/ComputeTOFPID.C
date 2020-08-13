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

  TList* lh = new TList();
  lh->SetOwner();

  // Standard pT distributions
  DOTH1F(hp_NoCut, ";#it{p} (GeV/#it{c});Tracks", 100, 0, 20);
  DOTH1F(hp_TrkCut, ";#it{p} (GeV/#it{c});Tracks", 100, 0, 20);
  DOTH1F(hp_TOFCut, ";#it{p} (GeV/#it{c});Tracks", 100, 0, 20);
  // Standard TOF distributions
  DOTH1F(hlength_NoCut, ";Track Length (cm);Tracks", 100, 0, 1000);
  DOTH1F(htime_NoCut, ";TOF Time (ns);Tracks", 1000, 0, 600);
  DOTH1F(hevtime_NoCut, ";Event time (ns);Tracks", 100, -2, 2);
  DOTH2F(hnsigmaPi_NoCut, ";#it{p} (GeV/#it{c});Tracks", 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaKa_NoCut, ";#it{p} (GeV/#it{c});Tracks", 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaPr_NoCut, ";#it{p} (GeV/#it{c});Tracks", 100, 0, 5, 100, -10, 10);
  // Distributions with PID
  DOTH1F(hp_El, ";#it{p} (GeV/#it{c});Tracks", 100, 0, 20);
  DOTH1F(hpt_El, ";#it{p}_{T} (GeV/#it{c});Tracks", 100, 0, 20);
  //
  DOTH1F(hlength_El, ";Track Length (cm);Tracks", 100, 0, 1000);
  DOTH1F(htime_El, ";TOF Time (ns);Tracks", 1000, 0, 600);
  DOTH1F(hevtime_El, ";Event time (ns);Tracks", 100, -2, 2);
  //
  DOTH2F(hp_beta, ";#it{p} (GeV/#it{c});TOF #beta;Tracks", 100, 0, 20, 100, 0, 2);
  DOTH2F(hp_beta_El, ";#it{p} (GeV/#it{c});#beta - #beta_{el};Tracks", 100, 0, 20, 100, -0.01, 0.01);
  DOTH2F(hp_betasigma_El, ";#it{p} (GeV/#it{c});(#beta - #beta_{el})/#sigma;Tracks", 100, 0, 20, 100, -5, 5);
  //
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
      Printf("Mom [%.2f, %.2f] = %.2f", pidr->GetTOFResponse().GetMinMom(i), pidr->GetTOFResponse().GetMaxMom(i), mom);
      eventTime[i] = pidr->GetTOFResponse().GetStartTime(mom);
      Printf("T0=%f", eventTime[i]);
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

      float Mom = p(trk->Eta(), trk->GetSigned1Pt());
      hp_NoCut->Fill(Mom);
      hlength_NoCut->Fill(trk->GetIntegratedLength());
      htime_NoCut->Fill(trk->GetTOFsignal() / 1000);
      hevtime_NoCut->Fill(EVTIME / 1000);

      if (!AcceptTrack(trk))
        continue;
      hp_TrkCut->Fill(Mom);
      if (!AcceptTrack(trk, kTRUE))
        continue;


      hnsigmaPi_NoCut->Fill(Mom, (trk->GetTOFsignal() - pidr->GetTOFResponse().GetExpectedSignal(trk, AliPID::kPion)) / pidr->GetTOFResponse().GetExpectedSigma(Mom, trk->GetTOFsignal(), AliPID::kPion));
      hnsigmaKa_NoCut->Fill(Mom, (trk->GetTOFsignal() - pidr->GetTOFResponse().GetExpectedSignal(trk, AliPID::kKaon)) / pidr->GetTOFResponse().GetExpectedSigma(Mom, trk->GetTOFsignal(), AliPID::kKaon));
      hnsigmaPr_NoCut->Fill(Mom, (trk->GetTOFsignal() - pidr->GetTOFResponse().GetExpectedSignal(trk, AliPID::kProton)) / pidr->GetTOFResponse().GetExpectedSigma(Mom, trk->GetTOFsignal(), AliPID::kProton));
      //
      hp_TOFCut->Fill(Mom);
      if (!trk->GetESDEvent())
        Printf("For track %i I cannot get ESD event from track!!!", itrk);
      // Electron id via TOF beta
      float BETA = beta(trk->GetIntegratedLength(), trk->GetTOFsignal(), EVTIME);
      float BETAERROR = betaerror(trk->GetIntegratedLength(), trk->GetTOFsignal(), EVTIME);
      float EXPBETAEL = expbeta(Mom, kElectronMass);
      float EXPBETAELERROR = 0;
      float betadiff = BETA - EXPBETAEL;
      if (abs(betadiff / BETAERROR) < 1) {
        hp_El->Fill(trk->P());
        hpt_El->Fill(trk->Pt());
        //
        hlength_El->Fill(trk->GetIntegratedLength());
        htime_El->Fill(trk->GetTOFsignal() / 1000);
        hevtime_El->Fill(EVTIME / 1000);
        //
        hp_beta->Fill(trk->Pt(), BETA);
        hp_beta_El->Fill(trk->Pt(), betadiff);
        hp_betasigma_El->Fill(trk->Pt(), betadiff / BETAERROR);
      }
    }
    esd->ResetStdContent();
  }
  SaveList(lh, "TOFPid.root", "filterEl-task");
  return true;
}
