#include "TF1.h"
#include "TH1.h"
#include "TColor.h"
#include "TSystem.h"
#include "TFile.h"
#include "TPCSimulation/Detector.h"
#include "PID/BetheBloch.h"

Double_t BetheBlochAleph(Double_t* x, Double_t* par)
{
  Double_t bg = x[0];
  Double_t kp1 = par[0];
  Double_t kp2 = par[1];
  Double_t kp3 = par[2];
  Double_t kp4 = par[3];
  Double_t kp5 = par[4];
  return o2::tpc::Detector::BetheBlochAleph(bg, kp1, kp2, kp3, kp4, kp5);
}

Double_t BetheBlochAleph2(Double_t* x, Double_t* par)
{
  //   Double_t charge = x[1];
  Double_t charge = 1;
  return par[5] * BetheBlochAleph(x, par) * std::pow(charge, par[6]);
}

TF1* BB(TString n = "BB")
{
  TF1* f = new TF1(n, BetheBlochAleph, 0, 20, 5);
  gSystem->Exec("o2-ccdb-downloadccdbfile -p Analysis/PID/TPC/BetheBloch -d /tmp/ -t -1");
  TFile fin("/tmp/Analysis/PID/TPC/BetheBloch/snapshot.root", "READ");
  o2::pid::tpc::BetheBloch* bb = nullptr;
  fin.GetObject("ccdb_object", bb);
  for (Int_t i = 0; i < f->GetNpar(); i++) {
    f->SetParameter(i, bb->GetParameters()[i]);
  }
  f->SetLineColor(TColor::GetColor("#e41a1c"));
  return f;
}

TF1* BB2(TString n = "BB2")
{
  TF1* f = new TF1(n, BetheBlochAleph2, 0, 20, 7);
  gSystem->Exec("o2-ccdb-downloadccdbfile -p Analysis/PID/TPC/BetheBloch -d /tmp/ -t -1");
  TFile fin("/tmp/Analysis/PID/TPC/BetheBloch/snapshot.root", "READ");
  o2::pid::tpc::BetheBloch* bb = nullptr;
  fin.GetObject("ccdb_object", bb);
  for (Int_t i = 0; i < f->GetNpar(); i++) {
    f->SetParameter(i, bb->GetParameters()[i]);
  }
  f->SetLineColor(TColor::GetColor("#4daf4a"));
  return f;
}

Double_t BBReso(Double_t* x, Double_t* par)
{
  Double_t signal = x[0];
  // Double_t points = x[1];
  Double_t points = 90;
  return signal * par[0] * (points > 0 ? sqrt(1. + par[1] / points) : 1.f);
}

TF1* BBReso(TString n = "BBReso")
{
  TF1* f = new TF1(n, BBReso, 0, 20, 2);
  f->SetParameters(0.07, 0.0);
  f->SetLineColor(TColor::GetColor("#4daf4a"));
  return f;
}