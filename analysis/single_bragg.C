// ============================================================================
// single_bragg.C - Single Energy Bragg Peak Analysis
// ============================================================================
// Analyzes ONE .root file with optional filtering
// Usage:
//   root -l 'single_bragg.C("file.root", true)'   <- Primary protons only
//   root -l 'single_bragg.C("file.root", false)'  <- All particles
//   root -l single_bragg.C                         <- Default: All particles
// ============================================================================

#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TLine.h>
#include <TStyle.h>
#include <TTree.h>
#include <iostream>

void single_bragg(
    TString filename = "../build/output/raw_150MeV_1000000evts_run0.root",
    bool primaryOnly = false) // <-- SWITCH: true = solo primarios, false = todo
{
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
  std::cout << "Filter: "
            << (primaryOnly ? "PRIMARY PROTONS ONLY" : "ALL PARTICLES")
            << std::endl;

  // Open file
  TFile *f = TFile::Open(filename);
  if (!f || f->IsZombie()) {
    std::cout << "ERROR: Cannot open file " << filename << std::endl;
    return;
  }

  TTree *tree = (TTree *)f->Get("raw_data");
  std::cout << "Entries: " << tree->GetEntries() << std::endl;

  // ===== BUILD THE CUT STRING =====
  TString baseCut = "volumeName==\"Phantom_phys\"";
  TString primaryCut = "particleName==\"proton\" && parentID==0";

  TString finalCut;
  if (primaryOnly) {
    finalCut = Form("edep*(%s && %s)", baseCut.Data(), primaryCut.Data());
  } else {
    finalCut = Form("edep*(%s)", baseCut.Data());
  }

  std::cout << "Cut: " << finalCut << std::endl;

  // Create histograms
  TString title =
      primaryOnly
          ? Form("Bragg Peak - %.0f MeV (Primary Protons);Depth (cm);Dose "
                 "(a.u.)",
                 energy)
          : Form("Bragg Peak - %.0f MeV (All Particles);Depth (cm);Dose (a.u.)",
                 energy);

  TH1D *hDose = new TH1D("hDose", title, 300, -15, 35);
  hDose->SetLineColor(primaryOnly ? kRed : kBlue);
  hDose->SetLineWidth(2);
  hDose->SetFillColorAlpha(primaryOnly ? kRed : kBlue, 0.2);

  // Fill
  tree->Draw("x_pre>>hDose", finalCut, "goff");
  hDose->Smooth(1);

  // Find peak
  hDose->GetXaxis()->SetRangeUser(-5, 20);
  int peakBin = hDose->GetMaximumBin();
  double peakX = hDose->GetBinCenter(peakBin);
  double peakDose = hDose->GetBinContent(peakBin);
  hDose->GetXaxis()->SetRangeUser(-15, 20);

  std::cout << "\n--- RESULTS ---" << std::endl;
  std::cout << "Peak position: " << peakX << " cm" << std::endl;
  std::cout << "Peak dose: " << peakDose << std::endl;

  // Plot
  TCanvas *c1 = new TCanvas("c1", "Single Bragg Peak", 900, 600);
  gPad->SetGrid();

  hDose->GetXaxis()->SetRangeUser(-15, 20);
  hDose->SetMinimum(0);
  hDose->Draw("HIST");

  // Mark peak
  TLine *peakLine = new TLine(peakX, 0, peakX, peakDose);
  peakLine->SetLineColor(kBlack);
  peakLine->SetLineStyle(2);
  peakLine->SetLineWidth(2);
  peakLine->Draw();

  TLegend *leg = new TLegend(0.55, 0.7, 0.88, 0.88);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(hDose, Form("%.0f MeV protons", energy), "f");
  leg->AddEntry(peakLine, Form("Peak: %.1f cm", peakX), "l");
  leg->AddEntry((TObject *)0, primaryOnly ? "Primary only" : "All particles",
                "");
  leg->Draw();

  TString suffix = primaryOnly ? "_primary" : "_all";
  TString outName = Form("bragg_%.0fMeV%s.png", energy, suffix.Data());
  c1->SaveAs(outName);
  std::cout << "\nSaved: " << outName << std::endl;
}
