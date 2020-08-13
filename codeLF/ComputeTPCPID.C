#include "common.h"

// O2 includes
#include "PID/BetheBloch.h"
#include "PID/TPCReso.h"
#include "PID/PIDTPC.h"

using namespace o2::pid;

Bool_t ComputeTPCPID(TString esdfile = "esdLHC15o.txt",
                     bool applyeventcut = 0)
{
  // Defining response
  auto resp = tpc::Response();
  // Downloading param from ccdb
  gSystem->Exec("o2-ccdb-downloadccdbfile -p Analysis/PID/TPC/BetheBloch -d /tmp/ -t -1");
  gSystem->Exec("o2-ccdb-downloadccdbfile -p Analysis/PID/TPC/TPCReso -d /tmp/ -t -1");
  // Loading param
  resp.LoadParamFromFile("/tmp/Analysis/PID/TPC/BetheBloch/snapshot.root", "ccdb_object", DetectorResponse::kSignal);
  resp.LoadParamFromFile("/tmp/Analysis/PID/TPC/TPCReso/snapshot.root", "ccdb_object", DetectorResponse::kSigma);

  TChain* chain = CreateLocalChain(esdfile, "ESD", 10);
  Printf("Computing TPC Pid Spectra");
  if (!chain) {
    printf("Error: no ESD chain found");
    return kFALSE;
  }
  AliESDEvent* esd = new AliESDEvent;
  Printf("Reading TTree with %lli events", chain->GetEntries());
  esd->ReadFromTree(chain);

  TList* lh = new TList();
  lh->SetOwner();

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

  for (Int_t iEvent = 0; iEvent < chain->GetEntries(); iEvent++) {
    chain->GetEvent(iEvent);
    if (!esd) {
      printf("Error: no ESD object found for event %d", iEvent);
      return kFALSE;
    }
    esd->ConnectTracks(); // Deve essere sempre chiamato dopo aver letto
                          // l'evento (non troverebbe l'ESDevent). Scrivo in
                          // tutte le tracce l origine dell evento così poi da
                          // arrivare ovunque(tipo al cluster e al tempo
                          // quindi).
    Printf("Event %i has %i tracks", iEvent, esd->GetNumberOfTracks());
    if (!esd->AreTracksConnected() && esd->GetNumberOfTracks() > 0)
      Printf("!!!Tracks are not connected, %i tracks are affected !!!!", esd->GetNumberOfTracks());

    AliESDVertex* primvtx = (AliESDVertex*)esd->GetPrimaryVertex();
    if (applyeventcut == 1) {
      Printf("Applying event selection");
      if (!primvtx)
        return kFALSE;
      if (primvtx->IsFromVertexer3D() || primvtx->IsFromVertexerZ())
        continue;
      if (primvtx->GetNContributors() < 2)
        continue;
    }

    for (Int_t itrk = 0; itrk < esd->GetNumberOfTracks(); itrk++) {
      AliESDtrack* trk = esd->GetTrack(itrk);
      Int_t status = trk->GetStatus();

      bool sel = status & AliESDtrack::kITSrefit &&
                 (trk->HasPointOnITSLayer(0) || trk->HasPointOnITSLayer(1)) &&
                 trk->GetNcls(1) > 70;
      if (!sel)
        continue;

      if (!trk->GetESDEvent())
        Printf("For track %i I cannot get ESD event from track!!!", itrk);
      //
      const AliExternalTrackParam* intp = trk->GetTPCInnerParam();
      const float mom = (intp ? intp->GetP() : 0);
      resp.UpdateTrack(mom, trk->GetTPCsignal(), (trk->GetTPCSharedMap()).CountBits());
      htpcsignal->Fill(mom, trk->GetTPCsignal());
      int counter = 0;
      hexpEl->Fill(mom, resp.GetExpectedSignal(counter++));
      hexpMu->Fill(mom, resp.GetExpectedSignal(counter++));
      hexpPi->Fill(mom, resp.GetExpectedSignal(counter++));
      hexpKa->Fill(mom, resp.GetExpectedSignal(counter++));
      hexpPr->Fill(mom, resp.GetExpectedSignal(counter++));
      hexpDe->Fill(mom, resp.GetExpectedSignal(counter++));
      hexpTr->Fill(mom, resp.GetExpectedSignal(counter++));
      hexpHe->Fill(mom, resp.GetExpectedSignal(counter++));
      hexpAl->Fill(mom, resp.GetExpectedSignal(counter++));
    }
    esd->ResetStdContent();
  }
  SaveList(lh, "TPCPid.root", "TPCpidqa-expsignal-task");
  return true;
}
