// ============================================================================
// single_bragg.C - Single Energy Bragg Peak Analysis
// ============================================================================
// Analyzes ONE .root file at a time for comparison
// Usage: root -l
// 'single_bragg.C("../build/output/raw_150MeV_100000evts_run0.root")'
// ============================================================================

#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TStyle.h>
#include <TTree.h>
#include <iostream>

void single_bragg(
    TString filename = "../build/output/raw_150MeV_1000000evts_run0.root") {
  gStyle->SetOptStat(0);

  // Extract energy from filename
  TString energyStr = filename;
  if (energyStr.Contains("raw_")) {
    energyStr = energyStr(energyStr.Index("raw_") + 4, 10);
    energyStr = energyStr(0, energyStr.Index("MeV"));
  }
  double energy = energyStr.Atof();

  std::cout << "=== SINGLE BRAGG PEAK ===" << std::endl;
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

  // Create histograms
  TH1D *hDose = new TH1D(
      "hDose", Form("Bragg Peak - %.0f MeV;Depth (cm);Dose (a.u.)", energy),
      300, -15, 35);
  hDose->SetLineColor(kBlue);
  hDose->SetLineWidth(2);
  hDose->SetFillColorAlpha(kBlue, 0.2);

  // Fill
  tree->Draw("x_pre>>hDose", "edep*(volumeName==\"Phantom_phys\")", "goff");
  hDose->Smooth(1);

  // Find peak
  int peakBin = hDose->GetMaximumBin();
  double peakX = hDose->GetBinCenter(peakBin);
  double peakDose = hDose->GetBinContent(peakBin);

  std::cout << "\n--- RESULTS ---" << std::endl;
  std::cout << "Peak position: " << peakX << " cm" << std::endl;
  std::cout << "Peak dose: " << peakDose << std::endl;

  // Calculate range (where dose drops to 80%)
  double threshold = peakDose * 0.8;
  int rangeBin = peakBin;
  for (int i = peakBin; i < hDose->GetNbinsX(); i++) {
    if (hDose->GetBinContent(i) < threshold) {
      rangeBin = i;
      break;
    }
  }
  double range = hDose->GetBinCenter(rangeBin);
  std::cout << "Range (80%): " << range << " cm" << std::endl;

  // Plot
  TCanvas *c1 = new TCanvas("c1", "Single Bragg Peak", 900, 600);
  gPad->SetGrid();

  hDose->GetXaxis()->SetRangeUser(-15, 20);
  hDose->SetMinimum(0);
  hDose->Draw("HIST");

  // Mark peak
  TLine *peakLine = new TLine(peakX, 0, peakX, peakDose);
  peakLine->SetLineColor(kRed);
  peakLine->SetLineStyle(2);
  peakLine->SetLineWidth(2);
  peakLine->Draw();

  TLegend *leg = new TLegend(0.6, 0.7, 0.88, 0.88);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(hDose, Form("%.0f MeV protons", energy), "f");
  leg->AddEntry(peakLine, Form("Peak: %.1f cm", peakX), "l");
  leg->Draw();

  TString outName = Form("bragg_%.0fMeV.png", energy);
  c1->SaveAs(outName);
  std::cout << "\nSaved: " << outName << std::endl;
}
