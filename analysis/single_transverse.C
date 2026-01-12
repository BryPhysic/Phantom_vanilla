// ============================================================================
// single_transverse.C - Single Energy Transverse Profile Analysis
// ============================================================================
// Analyzes Y-Z profile from ONE .root file at a time
// Usage: root -l
// 'single_transverse.C("../build/output/raw_150MeV_1000000evts_run0.root")'
// ============================================================================

#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TTree.h>
#include <iostream>

void single_transverse(
    TString filename = "../build/output/raw_150MeV_1000000evts_run0.root") {
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow);

  // Extract energy from filename
  TString energyStr = filename;
  if (energyStr.Contains("raw_")) {
    energyStr = energyStr(energyStr.Index("raw_") + 4, 10);
    energyStr = energyStr(0, energyStr.Index("MeV"));
  }
  double energy = energyStr.Atof();

  std::cout << "=== SINGLE TRANSVERSE PROFILE ===" << std::endl;
  std::cout << "File: " << filename << std::endl;
  std::cout << "Energy: " << energy << " MeV" << std::endl;

  // Open file
  TFile *f = TFile::Open(filename);
  if (!f || f->IsZombie()) {
    std::cout << "ERROR: Cannot open file " << filename << std::endl;
    return;
  }

  TTree *tree = (TTree *)f->Get("raw_data");
  std::cout << "Entries: " << tree->GetEntries() << std::endl;

  // First find the Bragg peak position
  TH1D *hTemp = new TH1D("hTemp", "", 200, -15, 35);
  tree->Draw("x_pre>>hTemp", "edep*(volumeName==\"Phantom_phys\")", "goff");
  hTemp->GetXaxis()->SetRangeUser(-5, 20);
  int peakBin = hTemp->GetMaximumBin();
  double peakX = hTemp->GetBinCenter(peakBin);
  std::cout << "Bragg peak at X = " << peakX << " cm" << std::endl;

  // Create histograms
  TH1D *hY = new TH1D(
      "hY",
      Form("Y Profile at Bragg Peak (%.0f MeV);Y (cm);Dose (a.u.)", energy),
      100, -10, 10);
  TH1D *hZ = new TH1D(
      "hZ",
      Form("Z Profile at Bragg Peak (%.0f MeV);Z (cm);Dose (a.u.)", energy),
      100, -10, 10);
  TH2D *hYZ = new TH2D(
      "hYZ",
      Form("Y-Z Dose Map at Bragg Peak (%.0f MeV);Y (cm);Z (cm)", energy), 50,
      -8, 8, 50, -8, 8);

  // Fill - use 2 cm slice around peak
  TString cutPeak =
      Form("edep*(volumeName==\"Phantom_phys\" && x_pre>%.1f && x_pre<%.1f)",
           peakX - 1, peakX + 1);

  tree->Draw("y_pre>>hY", cutPeak, "goff");
  tree->Draw("z_pre>>hZ", cutPeak, "goff");
  tree->Draw("z_pre:y_pre>>hYZ", cutPeak, "goff");

  // Style
  hY->SetLineColor(kBlue);
  hY->SetLineWidth(2);
  hY->SetFillColorAlpha(kBlue, 0.2);
  hZ->SetLineColor(kRed);
  hZ->SetLineWidth(2);
  hZ->SetFillColorAlpha(kRed, 0.2);

  // Calculate beam width (FWHM)
  double maxY = hY->GetMaximum();
  int binLeft = hY->FindFirstBinAbove(maxY / 2);
  int binRight = hY->FindLastBinAbove(maxY / 2);
  double fwhm = hY->GetBinCenter(binRight) - hY->GetBinCenter(binLeft);
  std::cout << "Beam FWHM (Y): " << fwhm << " cm" << std::endl;

  // Plot
  TCanvas *c1 = new TCanvas("c1", "Transverse Profile", 1200, 500);
  c1->Divide(3, 1);

  c1->cd(1);
  gPad->SetGrid();
  hY->Draw("HIST");

  c1->cd(2);
  gPad->SetGrid();
  hZ->Draw("HIST");

  c1->cd(3);
  gPad->SetRightMargin(0.15);
  hYZ->Draw("COLZ");

  TString outName = Form("transverse_%.0fMeV.png", energy);
  c1->SaveAs(outName);

  // Summary
  std::cout << "\n--- RESULTS ---" << std::endl;
  std::cout << "Peak X: " << peakX << " cm" << std::endl;
  std::cout << "Beam FWHM (Y): " << fwhm << " cm" << std::endl;
  std::cout << "Saved: " << outName << std::endl;
}
