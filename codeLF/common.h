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

// Run3 includes
#include "convertAO2D.C"

void SaveList(TList* l, TString fname, TString dir)
{

  TFile fout(fname, "RECREATE");
  fout.mkdir(dir);
  fout.cd(dir);
  l->Write();
  fout.Close();
}

#define DOTH1F(OBJ, ...)                   \
  TH1F* OBJ = new TH1F(#OBJ, __VA_ARGS__); \
  lh->Add(OBJ);

#define DOTH2F(OBJ, ...)                   \
  TH2F* OBJ = new TH2F(#OBJ, __VA_ARGS__); \
  lh->Add(OBJ);