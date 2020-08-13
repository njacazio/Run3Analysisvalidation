#include "AliAnalysisManager.h"
#include "AliESDEvent.h"
#include "AliESDtrack.h"
#include "AliGenEventHeader.h"
#include "AliHeader.h"
#include "AliInputEventHandler.h"
#include "AliPID.h"
#include "AliPIDResponse.h"
#include "AliTOFGeometry.h"
#include "AliTOFPIDResponse.h"
//#include "TAlienCollection.h"
#include "TFile.h"
#include "TGrid.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TLeaf.h"
#include "TList.h"
#include "TMath.h"
#include "TRandom.h"
#include "TTree.h"
#include "TSystem.h"

// Run3 includes
#include "convertAO2D.C"

bool AcceptVertex(AliESDEvent* esd)
{
  AliESDVertex* primvtx = (AliESDVertex*)esd->GetPrimaryVertex();
  Printf("Applying event selection");
  if (!primvtx)
    return kFALSE;
  if (primvtx->IsFromVertexer3D() || primvtx->IsFromVertexerZ())
    return kFALSE;
  if (primvtx->GetNContributors() < 2)
    return kFALSE;
  return kTRUE;
}

bool AcceptTrack(AliESDtrack* trk, bool requireTOF = kFALSE)
{
  Int_t status = trk->GetStatus();

  bool sel = status & AliESDtrack::kITSrefit &&
             (trk->HasPointOnITSLayer(0) || trk->HasPointOnITSLayer(1)) &&
             trk->GetNcls(1) > 70;
  if (requireTOF) {
    sel = sel && (status & AliESDtrack::kTOFout) && (status & AliESDtrack::kTIME);
  }
  if (!trk->GetESDEvent())
    Printf("Cannot get ESD event from track!!!");

  return sel;
}

bool SetEvent(TChain* chain, int iEvent, AliESDEvent* esd)
{
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
  return kTRUE;
}

void SaveList(TList* l, TString fname)
{

  TFile fout(fname, "RECREATE");
  const TString dir = l->GetName();
  fout.mkdir(dir);
  fout.cd(dir);
  l->Write();
  fout.Close();
}

TList* MakeList(TString name)
{
  TList* l = new TList();
  l->SetName(name)
    l->SetOwner();
  return l;
}

#define DOTH1F(OBJ, ...)                   \
  TH1F* OBJ = new TH1F(#OBJ, __VA_ARGS__); \
  lh->Add(OBJ);

#define DOTH2F(OBJ, ...)                   \
  TH2F* OBJ = new TH2F(#OBJ, __VA_ARGS__); \
  lh->Add(OBJ);