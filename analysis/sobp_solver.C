// ============================================================================
// sobp_solver.C - Solucionador SOBP CORREGIDO
// ============================================================================
// Normaliza curvas antes de optimizar para evitar escalas enormes
// ============================================================================

#include <TCanvas.h>
#include <TColor.h>
#include <TFile.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TList.h>
#include <TStyle.h>
#include <TSystemDirectory.h>
#include <TTree.h>
#include <algorithm>
#include <iostream>
#include <numeric>
#include <vector>

void sobp_solver() {
  gStyle->SetOptStat(0);
  gStyle->SetPalette(kRainBow);

  // --- 1. CARGAR ARCHIVOS ---
  TString dirPath = "../build/output/";
  TSystemDirectory dir(dirPath, dirPath);
  TList *files = dir.GetListOfFiles();
  std::vector<TString> filenames;
  std::vector<double> energies;

  if (!files) {
    std::cout << "Error: No files" << std::endl;
    return;
  }

  TIter next(files);
  TSystemFile *file;
  while ((file = (TSystemFile *)next())) {
    TString fname = file->GetName();
    if (!file->IsDirectory() && fname.EndsWith(".root") &&
        fname.Contains("raw_")) {
      filenames.push_back(dirPath + fname);
      TString eStr = fname;
      eStr.ReplaceAll("raw_", "");
      if (eStr.Index("MeV") != -1)
        eStr = eStr(0, eStr.Index("MeV"));
      energies.push_back(eStr.Atof());
    }
  }

  int n = filenames.size();
  std::cout << "=== SOBP SOLVER ===" << std::endl;
  std::cout << "Files: " << n << std::endl;

  if (n == 0)
    return;

  // Ordenar por energia
  std::vector<int> idx(n);
  std::iota(idx.begin(), idx.end(), 0);
  std::sort(idx.begin(), idx.end(),
            [&](int i, int j) { return energies[i] < energies[j]; });

  // --- 2. CARGAR Y NORMALIZAR HISTOGRAMAS ---
  std::vector<TH1D *> hDose(n);
  std::vector<double> weights(n, 1.0);
  std::vector<int> peakBin(n);
  std::vector<double> peakHeight(n);

  for (int i = 0; i < n; i++) {
    TFile *f = TFile::Open(filenames[idx[i]]);
    TTree *t = (TTree *)f->Get("raw_data");
    hDose[i] = new TH1D(Form("h_%d", i), "", 500, -15, 35);
    t->Draw(Form("x_pre>>h_%d", i), "edep*(volumeName==\"Phantom_phys\")",
            "goff");
    hDose[i]->SetDirectory(0);
    f->Close();

    hDose[i]->Smooth(2);

    // Buscar pico en la region del tumor (X > -5)
    hDose[i]->GetXaxis()->SetRangeUser(-5, 15);
    peakBin[i] = hDose[i]->GetMaximumBin();
    peakHeight[i] = hDose[i]->GetBinContent(peakBin[i]);
    hDose[i]->GetXaxis()->SetRangeUser(-15, 35);

    // NORMALIZAR: cada curva tiene pico = 1.0
    if (peakHeight[i] > 0) {
      hDose[i]->Scale(1.0 / peakHeight[i]);
    }

    std::cout << "  " << i + 1 << ". " << energies[idx[i]]
              << " MeV, peak bin: " << peakBin[i] << ", normalized"
              << std::endl;
  }

  // --- 3. ALGORITMO ITERATIVO ---
  TH1D *hTotal = new TH1D("hTotal", "SOBP", 500, -15, 35);

  // Meta: dosis uniforme de 1.0 en toda la region del SOBP
  double targetDose = 1.0;

  // El ultimo pico (150 MeV, el mas profundo) tiene peso 1.0
  weights[n - 1] = 1.0;

  std::cout << "\nOptimizing..." << std::endl;

  for (int iter = 0; iter < 200; iter++) {
    hTotal->Reset();
    for (int i = 0; i < n; i++)
      hTotal->Add(hDose[i], weights[i]);

    // Ajustar pesos para igualar la dosis en cada pico
    for (int i = 0; i < n - 1; i++) { // No tocamos el ultimo
      double currentDose = hTotal->GetBinContent(peakBin[i]);
      if (currentDose > 0) {
        double correction = targetDose / currentDose;
        // Factor de aprendizaje suave
        weights[i] = weights[i] * (0.8 + 0.2 * correction);
        // Limitar pesos negativos o muy grandes
        if (weights[i] < 0.01)
          weights[i] = 0.01;
        if (weights[i] > 2.0)
          weights[i] = 2.0;
      }
    }
  }

  // --- 4. IMPRIMIR RESULTADO ---
  std::cout << "\n========================================" << std::endl;
  std::cout << "OPTIMIZED WEIGHTS" << std::endl;
  std::cout << "========================================" << std::endl;
  std::cout << "std::vector<double> weights" << n << " = {" << std::endl
            << "    ";
  for (int i = 0; i < n; i++) {
    std::cout << Form("%.4f", weights[i]);
    if (i < n - 1)
      std::cout << ", ";
    if ((i + 1) % 6 == 0 && i < n - 1)
      std::cout << std::endl << "    ";
  }
  std::cout << std::endl << "};" << std::endl;

  // --- 5. GRAFICAR ---
  hTotal->Reset();
  for (int i = 0; i < n; i++)
    hTotal->Add(hDose[i], weights[i]);

  TCanvas *c1 = new TCanvas("c1", "SOBP Solver", 1600, 600);
  c1->Divide(2, 1);

  // Panel 1: Curvas individuales
  c1->cd(1);
  gPad->SetGrid();

  TH1D *hFrame1 =
      new TH1D("hFrame1", "Normalized Bragg Peaks;Depth (cm);Normalized Dose",
               500, -15, 35);
  hFrame1->SetMaximum(1.5);
  hFrame1->SetMinimum(0);
  hFrame1->GetXaxis()->SetRangeUser(-15, 15);
  hFrame1->Draw();

  for (int i = 0; i < n; i++) {
    TH1D *hClone = (TH1D *)hDose[i]->Clone();
    hClone->SetLineColor(TColor::GetColorPalette(i * 99 / n));
    hClone->SetLineWidth(1);
    hClone->Draw("HIST C SAME");
  }

  // Panel 2: SOBP final
  c1->cd(2);
  gPad->SetGrid();

  hTotal->SetLineWidth(3);
  hTotal->SetLineColor(kBlack);
  hTotal->SetTitle("Optimized SOBP;Depth (cm);Dose (a.u.)");
  hTotal->GetXaxis()->SetRangeUser(-15, 15);
  hTotal->SetMinimum(0);
  hTotal->Draw("HIST C");

  // Dibujar curvas escaladas
  for (int i = 0; i < n; i++) {
    TH1D *hClone = (TH1D *)hDose[i]->Clone();
    hClone->Scale(weights[i]);
    hClone->SetLineColor(TColor::GetColorPalette(i * 99 / n));
    hClone->SetLineWidth(1);
    hClone->SetFillColorAlpha(hClone->GetLineColor(), 0.1);
    hClone->Draw("HIST C SAME");
  }
  hTotal->Draw("HIST C SAME");

  TLegend *leg = new TLegend(0.65, 0.7, 0.88, 0.88);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(hTotal, "SOBP Total", "l");
  leg->AddEntry((TObject *)0, Form("%d layers", n), "");
  leg->Draw();

  c1->SaveAs("sobp_solved.png");
  std::cout << "\nSaved: sobp_solved.png" << std::endl;
}