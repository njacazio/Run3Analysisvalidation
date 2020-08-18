#include "common.h"

// O2 includes
#include "PID/BetheBloch.h"
#include "PID/TPCReso.h"
#include "PID/PIDTPC.h"

using namespace o2::pid;

Bool_t ComputeTPCPID(TString esdfile = "esdLHC15o.txt",
                     bool applyeventcut = 0,
                     bool ismc = kFALSE)
{
  // Defining response
  auto resp = tpc::Response();
  // Downloading param from ccdb
  gSystem->Exec("o2-ccdb-downloadccdbfile -p Analysis/PID/TPC/BetheBloch -d /tmp/ -t -1");
  gSystem->Exec("o2-ccdb-downloadccdbfile -p Analysis/PID/TPC/TPCReso -d /tmp/ -t -1");
  // Loading param
  resp.LoadParamFromFile("/tmp/Analysis/PID/TPC/BetheBloch/snapshot.root", "ccdb_object", DetectorResponse::kSignal);
  resp.LoadParamFromFile("/tmp/Analysis/PID/TPC/TPCReso/snapshot.root", "ccdb_object", DetectorResponse::kSigma);

  // Defining input
  TChain* chain = CreateLocalChain(esdfile, "ESD");
  Printf("Computing TPC Pid Spectra");
  if (!chain) {
    printf("Error: no ESD chain found");
    return kFALSE;
  }
  AliESDEvent* esd = new AliESDEvent;
  Printf("Reading TTree with %lli events", chain->GetEntries());
  esd->ReadFromTree(chain);

  TList* lh = MakeList("TPCpidqa-expsignal-task");

#define BIN_AXIS 1000, 0, 5, 1000, 0, 1000

  DOTH2F(htpcsignal, ";#it{p} (GeV/#it{c});TPC Signal;Tracks", BIN_AXIS);
  DOTH2F(hexpEl, ";#it{p} (GeV/#it{c});TPC expected signal e;Tracks", BIN_AXIS);
  DOTH2F(hexpMu, ";#it{p} (GeV/#it{c});TPC expected signal #mu;Tracks", BIN_AXIS);
  DOTH2F(hexpPi, ";#it{p} (GeV/#it{c});TPC expected signal #pi;Tracks", BIN_AXIS);
  DOTH2F(hexpKa, ";#it{p} (GeV/#it{c});TPC expected signal K;Tracks", BIN_AXIS);
  DOTH2F(hexpPr, ";#it{p} (GeV/#it{c});TPC expected signal p;Tracks", BIN_AXIS);
  DOTH2F(hexpDe, ";#it{p} (GeV/#it{c});TPC expected signal d;Tracks", BIN_AXIS);
  DOTH2F(hexpTr, ";#it{p} (GeV/#it{c});TPC expected signal t;Tracks", BIN_AXIS);
  DOTH2F(hexpHe, ";#it{p} (GeV/#it{c});TPC expected signal ^{3}He;Tracks", BIN_AXIS);
  DOTH2F(hexpAl, ";#it{p} (GeV/#it{c});TPC expected signal #alpha;Tracks", BIN_AXIS);

  lh = MakeList("TPCpidqa-signalwTOF-task");
  DOTH2F(htpcsignalPi, ";#it{p} (GeV/#it{c});TPC Signal;Tracks", BIN_AXIS);
  DOTH2F(htpcsignalKa, ";#it{p} (GeV/#it{c});TPC Signal;Tracks", BIN_AXIS);
  DOTH2F(htpcsignalPr, ";#it{p} (GeV/#it{c});TPC Signal;Tracks", BIN_AXIS);
  DOTH2F(htpcsignalDe, ";#it{p} (GeV/#it{c});TPC Signal;Tracks", BIN_AXIS);
#undef BIN_AXIS

  // Log binning for p
  const Int_t nbins = 1000;
  Double_t binp[nbins + 1] = {0};
  Double_t max = 20;
  Double_t min = 0.01;
  Double_t lmin = TMath::Log10(min);
  Double_t ldelta = (TMath::Log10(max) - lmin) / ((Double_t)nbins);
  for (int i = 0; i < nbins; i++) {
    binp[i] = TMath::Exp(TMath::Log(10) * (lmin + i * ldelta));
  }
  binp[nbins] = max + 1;
  htpcsignal->GetXaxis()->Set(nbins, binp);
  hexpEl->GetXaxis()->Set(nbins, binp);
  hexpMu->GetXaxis()->Set(nbins, binp);
  hexpPi->GetXaxis()->Set(nbins, binp);
  hexpKa->GetXaxis()->Set(nbins, binp);
  hexpPr->GetXaxis()->Set(nbins, binp);
  hexpDe->GetXaxis()->Set(nbins, binp);
  hexpTr->GetXaxis()->Set(nbins, binp);
  hexpHe->GetXaxis()->Set(nbins, binp);
  hexpAl->GetXaxis()->Set(nbins, binp);
  //
  htpcsignalPi->GetXaxis()->Set(nbins, binp);
  htpcsignalKa->GetXaxis()->Set(nbins, binp);
  htpcsignalPr->GetXaxis()->Set(nbins, binp);
  htpcsignalDe->GetXaxis()->Set(nbins, binp);

  lh = MakeList("TPCpidqa-nsigma-task");

  DOTH2F(hnsigmaEl, ";#it{p} (GeV/#it{c});TPC N_{sigma e};Tracks", 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaMu, ";#it{p} (GeV/#it{c});TPC N_{sigma #mu};Tracks", 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaPi, ";#it{p} (GeV/#it{c});TPC N_{sigma #pi};Tracks", 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaKa, ";#it{p} (GeV/#it{c});TPC N_{sigma K};Tracks", 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaPr, ";#it{p} (GeV/#it{c});TPC N_{sigma p};Tracks", 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaDe, ";#it{p} (GeV/#it{c});TPC N_{sigma d};Tracks", 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaTr, ";#it{p} (GeV/#it{c});TPC N_{sigma t};Tracks", 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaHe, ";#it{p} (GeV/#it{c});TPC N_{sigma ^{3}He};Tracks", 100, 0, 5, 100, -10, 10);
  DOTH2F(hnsigmaAl, ";#it{p} (GeV/#it{c});TPC N_{sigma #alpha};Tracks", 100, 0, 5, 100, -10, 10);

  // Aliroot response
  AliPIDResponse* pidr = new AliPIDResponse(ismc);

  for (Int_t iEvent = 0; iEvent < chain->GetEntries(); iEvent++) { // Loop on events
    if (!SetEvent(chain, iEvent, esd))
      return kFALSE;
    //
    if (applyeventcut && !VertexOK(esd))
      continue;
    //
    AliTPCPIDResponse TPCresp = pidr->GetTPCResponse();

    for (Int_t itrk = 0; itrk < esd->GetNumberOfTracks(); itrk++) {
      AliESDtrack* trk = esd->GetTrack(itrk);
      if (!TrackOK(trk))
        continue;
      //
      const AliExternalTrackParam* intp = trk->GetTPCInnerParam();
      const float mom = (intp ? intp->GetP() : 0);
      resp.UpdateTrack(mom, trk->GetTPCsignal(), (trk->GetTPCSharedMap()).CountBits());
      htpcsignal->Fill(mom, trk->GetTPCsignal());
      int counter = 0;
// Exp values
// #define EXP_SIGNAL resp.GetExpectedSignal(counter++)
#define EXP_SIGNAL TPCresp.GetExpectedSignal(trk, pidarray[counter++])
      hexpEl->Fill(mom, EXP_SIGNAL);
      hexpMu->Fill(mom, EXP_SIGNAL);
      hexpPi->Fill(mom, EXP_SIGNAL);
      hexpKa->Fill(mom, EXP_SIGNAL);
      hexpPr->Fill(mom, EXP_SIGNAL);
      hexpDe->Fill(mom, EXP_SIGNAL);
      hexpTr->Fill(mom, EXP_SIGNAL);
      hexpHe->Fill(mom, EXP_SIGNAL);
      hexpAl->Fill(mom, EXP_SIGNAL);
      // Nsigma
      counter = 0;
// #define EXP_SIGMA resp.GetSeparation(counter++)
#define EXP_SIGMA TPCresp.GetNumberOfSigmas(trk, pidarray[counter++])
      hnsigmaEl->Fill(mom, EXP_SIGMA);
      hnsigmaMu->Fill(mom, EXP_SIGMA);
      hnsigmaPi->Fill(mom, EXP_SIGMA);
      hnsigmaKa->Fill(mom, EXP_SIGMA);
      hnsigmaPr->Fill(mom, EXP_SIGMA);
      hnsigmaDe->Fill(mom, EXP_SIGMA);
      hnsigmaTr->Fill(mom, EXP_SIGMA);
      hnsigmaHe->Fill(mom, EXP_SIGMA);
      hnsigmaAl->Fill(mom, EXP_SIGMA);
    }
    esd->ResetStdContent();
  }
  Save("TPCPid.root");
  return true;
}
