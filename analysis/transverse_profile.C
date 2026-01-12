// ============================================================================
// transverse_profile.C - Transverse Dose Distribution Analysis
// ============================================================================
// Analyzes Y-Z dose distribution at different depths
// Uses y_pre and z_pre from raw data
// ============================================================================

#include <TCanvas.h>
#include <TColor.h>
#include <TFile.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TLegend.h>
#include <TList.h>
#include <TStyle.h>
#include <TSystemDirectory.h>
#include <TTree.h>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

void transverse_profile() {
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow);

  // --- 1. LOAD ALL FILES ---
  TString dirPath = "../build/output/";
  TSystemDirectory dir(dirPath, dirPath);
  TList *files = dir.GetListOfFiles();

  // Chain to combine all files
  TChain *chain = new TChain("raw_data");
  int nFiles = 0;

  if (files) {
    TIter next(files);
    TSystemFile *file;
    while ((file = (TSystemFile *)next())) {
      TString fname = file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(".root") &&
          fname.Contains("raw_")) {
        chain->Add(dirPath + fname);
        nFiles++;
      }
    }
  }

  std::cout << "=== TRANSVERSE PROFILE ANALYSIS ===" << std::endl;
  std::cout << "Files loaded: " << nFiles << std::endl;
  std::cout << "Total entries: " << chain->GetEntries() << std::endl;

  if (nFiles == 0)
    return;

  // --- 2. CREATE HISTOGRAMS ---

  // Y profile at different X depths
  TH1D *hY_entrance = new TH1D(
      "hY_entrance", "Y Profile at Entrance (X=-8 cm);Y (cm);Dose (a.u.)", 100,
      -15, 15);
  TH1D *hY_middle =
      new TH1D("hY_middle", "Y Profile at Middle (X=0 cm);Y (cm);Dose (a.u.)",
               100, -15, 15);
  TH1D *hY_peak =
      new TH1D("hY_peak", "Y Profile at Bragg Peak (X=5 cm);Y (cm);Dose (a.u.)",
               100, -15, 15);

  // Z profile at different X depths
  TH1D *hZ_entrance = new TH1D(
      "hZ_entrance", "Z Profile at Entrance (X=-8 cm);Z (cm);Dose (a.u.)", 100,
      -15, 15);
  TH1D *hZ_middle =
      new TH1D("hZ_middle", "Z Profile at Middle (X=0 cm);Z (cm);Dose (a.u.)",
               100, -15, 15);
  TH1D *hZ_peak =
      new TH1D("hZ_peak", "Z Profile at Bragg Peak (X=5 cm);Z (cm);Dose (a.u.)",
               100, -15, 15);

  // 2D Y-Z maps at different depths
  TH2D *hYZ_entrance = new TH2D(
      "hYZ_entrance", "Y-Z Dose Map at Entrance (X=-8 cm);Y (cm);Z (cm)", 50,
      -10, 10, 50, -10, 10);
  TH2D *hYZ_middle =
      new TH2D("hYZ_middle", "Y-Z Dose Map at Middle (X=0 cm);Y (cm);Z (cm)",
               50, -10, 10, 50, -10, 10);
  TH2D *hYZ_peak =
      new TH2D("hYZ_peak", "Y-Z Dose Map at Bragg Peak (X=5 cm);Y (cm);Z (cm)",
               50, -10, 10, 50, -10, 10);

  // --- 3. FILL HISTOGRAMS ---
  std::cout << "Filling histograms..." << std::endl;

  // Y profiles (1 cm slices)
  chain->Draw("y_pre>>hY_entrance",
              "edep*(volumeName==\"Phantom_phys\" && x_pre>-9 && x_pre<-7)",
              "goff");
  chain->Draw("y_pre>>hY_middle",
              "edep*(volumeName==\"Phantom_phys\" && x_pre>-1 && x_pre<1)",
              "goff");
  chain->Draw("y_pre>>hY_peak",
              "edep*(volumeName==\"Phantom_phys\" && x_pre>4 && x_pre<6)",
              "goff");

  // Z profiles (1 cm slices)
  chain->Draw("z_pre>>hZ_entrance",
              "edep*(volumeName==\"Phantom_phys\" && x_pre>-9 && x_pre<-7)",
              "goff");
  chain->Draw("z_pre>>hZ_middle",
              "edep*(volumeName==\"Phantom_phys\" && x_pre>-1 && x_pre<1)",
              "goff");
  chain->Draw("z_pre>>hZ_peak",
              "edep*(volumeName==\"Phantom_phys\" && x_pre>4 && x_pre<6)",
              "goff");

  // 2D Y-Z maps (2 cm slices)
  chain->Draw("z_pre:y_pre>>hYZ_entrance",
              "edep*(volumeName==\"Phantom_phys\" && x_pre>-9 && x_pre<-7)",
              "goff");
  chain->Draw("z_pre:y_pre>>hYZ_middle",
              "edep*(volumeName==\"Phantom_phys\" && x_pre>-1 && x_pre<1)",
              "goff");
  chain->Draw("z_pre:y_pre>>hYZ_peak",
              "edep*(volumeName==\"Phantom_phys\" && x_pre>4 && x_pre<6)",
              "goff");

  // --- 4. STYLE ---
  hY_entrance->SetLineColor(kBlue);
  hY_middle->SetLineColor(kGreen + 2);
  hY_peak->SetLineColor(kRed);
  hZ_entrance->SetLineColor(kBlue);
  hZ_middle->SetLineColor(kGreen + 2);
  hZ_peak->SetLineColor(kRed);

  hY_entrance->SetLineWidth(2);
  hY_middle->SetLineWidth(2);
  hY_peak->SetLineWidth(2);
  hZ_entrance->SetLineWidth(2);
  hZ_middle->SetLineWidth(2);
  hZ_peak->SetLineWidth(2);

  // --- 5. PLOT ---
  TCanvas *c1 = new TCanvas("c1", "Transverse Profiles", 1400, 900);
  c1->Divide(3, 2);

  // Row 1: Y profiles
  c1->cd(1);
  gPad->SetGrid();
  hY_entrance->Draw("HIST");

  c1->cd(2);
  gPad->SetGrid();
  hY_middle->Draw("HIST");

  c1->cd(3);
  gPad->SetGrid();
  hY_peak->Draw("HIST");

  // Row 2: 2D maps
  c1->cd(4);
  gPad->SetRightMargin(0.15);
  hYZ_entrance->Draw("COLZ");

  c1->cd(5);
  gPad->SetRightMargin(0.15);
  hYZ_middle->Draw("COLZ");

  c1->cd(6);
  gPad->SetRightMargin(0.15);
  hYZ_peak->Draw("COLZ");

  c1->SaveAs("transverse_profiles.png");

  // --- 6. COMPARISON PLOT ---
  TCanvas *c2 = new TCanvas("c2", "Y Profile Comparison", 1000, 600);
  gPad->SetGrid();

  // Normalize to max
  hY_entrance->Scale(1.0 / hY_entrance->GetMaximum());
  hY_middle->Scale(1.0 / hY_middle->GetMaximum());
  hY_peak->Scale(1.0 / hY_peak->GetMaximum());

  hY_entrance->SetTitle(
      "Transverse Y Profile at Different Depths;Y (cm);Normalized Dose");
  hY_entrance->GetYaxis()->SetRangeUser(0, 1.2);
  hY_entrance->Draw("HIST");
  hY_middle->Draw("HIST SAME");
  hY_peak->Draw("HIST SAME");

  TLegend *leg = new TLegend(0.65, 0.7, 0.88, 0.88);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(hY_entrance, "Entrance (X=-8 cm)", "l");
  leg->AddEntry(hY_middle, "Middle (X=0 cm)", "l");
  leg->AddEntry(hY_peak, "Bragg Peak (X=5 cm)", "l");
  leg->Draw();

  c2->SaveAs("transverse_comparison.png");

  std::cout << "\nSaved: transverse_profiles.png" << std::endl;
  std::cout << "Saved: transverse_comparison.png" << std::endl;
}
