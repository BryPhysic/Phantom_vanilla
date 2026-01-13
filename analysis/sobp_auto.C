// ============================================================================
// sobp_auto.C - Analisis SOBP clinico (128-150 MeV, 23 capas)
// ============================================================================
// Lee automaticamente archivos de ../build/output/
// Usa pesos calculados por sobp_solver.C
// ============================================================================

#include <TCanvas.h>
#include <TColor.h>
#include <TFile.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TList.h>
#include <TStyle.h>
#include <TSystem.h>
#include <TSystemDirectory.h>
#include <TTree.h>
#include <algorithm>
#include <iostream>
#include <vector>

void sobp_auto() {
  gStyle->SetOptStat(0);
  gStyle->SetCanvasColor(kWhite);
  gStyle->SetFrameLineWidth(2);
  gStyle->SetPalette(kRainBow);

  // ===== LEER ARCHIVOS =====
  TString dirPath = "../build/output/";
  TSystemDirectory dir(dirPath, dirPath);
  TList *files = dir.GetListOfFiles();

  std::vector<TString> rootFiles;
  std::vector<double> energies;

  if (files) {
    TSystemFile *file;
    TString fname;
    TIter next(files);
    while ((file = (TSystemFile *)next())) {
      fname = file->GetName();
      if (!file->IsDirectory() && fname.EndsWith(".root") &&
          fname.Contains("raw_")) {
        rootFiles.push_back(dirPath + fname);
        TString energyStr = fname;
        energyStr.ReplaceAll("raw_", "");
        if (energyStr.Index("MeV") != -1)
          energyStr = energyStr(0, energyStr.Index("MeV"));
        energies.push_back(energyStr.Atof());
      }
    }
  }

  int nFiles = rootFiles.size();
  if (nFiles == 0) {
    std::cout << "ERROR: No .root files found in " << dirPath << std::endl;
    return;
  }
  std::cout << "Found " << nFiles << " ROOT files" << std::endl;

  // Ordenar por energia (menor a mayor: 128, 129, 130, ..., 150)
  std::vector<size_t> indices(nFiles);
  for (size_t i = 0; i < nFiles; i++)
    indices[i] = i;
  std::sort(indices.begin(), indices.end(), [&energies](size_t a, size_t b) {
    return energies[a] < energies[b];
  });

  // =========================================================
  // PESOS OPTIMIZADOS - Generados por sobp_solver.C
  // =========================================================
  // COPIA AQUI el vector que te da sobp_solver.C
  // El tamaño debe coincidir con nFiles
  std::vector<double> weights = {0.1716, 0.1412, 0.1806,
                                 0.2083, 0.2215, 1.0000};

  // ===== CREAR HISTOGRAMAS =====
  std::vector<TH1D *> hDose(nFiles);
  TH1D *hSOBP = new TH1D("hSOBP", "SOBP", 500, -15, 35);
  hSOBP->SetLineColor(kBlack);
  hSOBP->SetLineWidth(4);
  hSOBP->SetFillStyle(0);

  // Verificar si tenemos pesos para todos los archivos
  bool useWeights = (weights.size() == nFiles);
  if (!useWeights) {
    std::cout << "WARNING: weights.size()=" << weights.size()
              << " != nFiles=" << nFiles << " -> Using linear fallback"
              << std::endl;
  }

  // ===== BUCLE DE SUMA =====
  for (int i = 0; i < nFiles; i++) {
    int idx = indices[i];
    TFile *f = TFile::Open(rootFiles[idx]);
    TTree *tree = (TTree *)f->Get("raw_data");

    hDose[i] = new TH1D(Form("hDose_%d", i), "", 500, -15, 35);
    int color = TColor::GetColorPalette(int((float)i / (nFiles - 1) * 99));
    hDose[i]->SetLineColor(color);
    hDose[i]->SetLineWidth(2);
    hDose[i]->SetFillColorAlpha(color, 0.1);

    tree->Draw(Form("x_pre>>hDose_%d", i),
               "edep*(volumeName==\"Phantom_phys\")", "goff");
    hDose[i]->Smooth(2);

    // Usar peso del vector o fallback lineal
    double weight =
        useWeights ? weights[i] : (0.3 + 0.7 * ((double)i / (nFiles - 1)));

    TH1D *temp = (TH1D *)hDose[i]->Clone();
    temp->Scale(weight);
    hSOBP->Add(temp);

    std::cout << "Energy: " << energies[idx] << " MeV -> Weight: " << weight
              << std::endl;
  }

  hSOBP->Smooth(2);

  // ===== PLOT =====
  TCanvas *c1 = new TCanvas("c1", "SOBP Clinical", 1200, 700);
  gPad->SetGrid();

  hSOBP->SetTitle("Clinical SOBP (4 cm Tumor);Depth (cm);Dose (a.u.)");
  hSOBP->GetXaxis()->SetRangeUser(-15, 25);
  hSOBP->SetMinimum(0);

  hSOBP->Draw("HIST C");
  for (int i = 0; i < nFiles; i++) {
    TH1D *clone = (TH1D *)hDose[i]->Clone();
    double w =
        useWeights ? weights[i] : (0.3 + 0.7 * ((double)i / (nFiles - 1)));
    clone->Scale(w);
    clone->Draw("HIST C SAME");
  }
  hSOBP->Draw("HIST C SAME");

  // Leyenda
  TLegend *leg = new TLegend(0.7, 0.6, 0.88, 0.88);
  leg->SetBorderSize(0);
  leg->SetFillStyle(0);
  leg->AddEntry(hSOBP, "SOBP Total", "l");
  leg->AddEntry((TObject *)0, Form("%d layers", nFiles), "");
  leg->AddEntry((TObject *)0, "128-150 MeV", "");
  leg->Draw();

  c1->SaveAs("sobp_clinical.png");
  std::cout << "Saved: sobp_clinical.png" << std::endl;
}