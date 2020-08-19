#include "TF1.h"
#include "TH1.h"
#include "TColor.h"

Double_t BetheBlochAleph(Double_t* x, Double_t* par)
{
  Double_t bg = x[0];
  Double_t kp1 = par[0];
  Double_t kp2 = par[1];
  Double_t kp3 = par[2];
  Double_t kp4 = par[3];
  Double_t kp5 = par[4];
  Double_t beta = bg / std::sqrt(static_cast<Double_t>(1.) + bg * bg);

  Double_t aa = std::pow(beta, kp4);
  Double_t bb = std::pow(static_cast<Double_t>(1.) / bg, kp5);
  bb = std::log(kp3 + bb);

  return (kp2 - aa - bb) * kp1 / aa;
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
  f->SetParameters(0.0320981, 19.9768, 2.52666e-16, 2.72123, 6.08092);
  f->SetLineColor(TColor::GetColor("#e41a1c"));
  return f;
}

TF1* BB2(TString n = "BB2")
{
  TF1* f = new TF1(n, BetheBlochAleph2, 0, 20, 7);
  f->SetParameters(0.0320981, 19.9768, 2.52666e-16, 2.72123, 6.08092, 50.f, 2.3);
  f->SetLineColor(TColor::GetColor("#4daf4a"));
  return f;
}
