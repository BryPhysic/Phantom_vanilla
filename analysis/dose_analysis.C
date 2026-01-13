// ============================================================================
// dose_analysis.C - Complete Dose Analysis with Gray conversion
// ============================================================================
// Usage: root -l
// 'dose_analysis.C("../build/output/raw_150MeV_200000evts_run0.root")'
// ============================================================================

#include <TCanvas.h>
#include <TFile.h>
#include <TH1D.h>
#include <TLegend.h>
#include <TLine.h>
#include <TStyle.h>
#include <TTree.h>
#include <iostream>

void dose_analysis(
    TString filename = "../build/output/raw_150MeV_200000evts_run0.root") {
  gStyle->SetOptStat(0);

  // ===== PARAMETROS DEL PHANTOM =====
  double phantom_dx = 40.0; // Longitud X del phantom (cm)
  double phantom_dy = 20.0; // Ancho Y del phantom (cm)
  double phantom_dz = 20.0; // Alto Z del phantom (cm)
  double rho = 1.0;         // Densidad agua (g/cm³)

  // Parametros del histograma
  int nBins = 200;
  double xMin = -15, xMax = 25;
  double binWidth = (xMax - xMin) / nBins; // cm per bin

  // ===== EXTRAER INFO DEL FILENAME =====
  TString energyStr = filename;
  if (energyStr.Contains("raw_")) {
    energyStr = energyStr(energyStr.Index("raw_") + 4, 10);
    energyStr = energyStr(0, energyStr.Index("MeV"));
  }
  double energy = energyStr.Atof();

  // Extraer numero de eventos
  TString evtStr = filename;
  if (evtStr.Contains("MeV_")) {
    evtStr = evtStr(evtStr.Index("MeV_") + 4, 20);
    evtStr = evtStr(0, evtStr.Index("evts"));
  }
  int nProtons = evtStr.Atoi();

  std::cout << "=== DOSE ANALYSIS ===" << std::endl;
  std::cout << "File: " << filename << std::endl;
  std::cout << "Energy: " << energy << " MeV" << std::endl;
  std::cout << "Protons: " << nProtons << std::endl;

  // ===== ABRIR ARCHIVO =====
  TFile *f = TFile::Open(filename);
  if (!f || f->IsZombie()) {
    std::cout << "ERROR: Cannot open file" << std::endl;
    return;
  }
  TTree *t = (TTree *)f->Get("raw_data");

  // ===== CREAR HISTOGRAMAS =====
  // 1. Solo protones primarios
  TH1D *hPrimary = new TH1D("hPrimary", "", nBins, xMin, xMax);
  t->Draw("x_pre>>hPrimary",
          "edep*(volumeName==\"Phantom_phys\" && particleName==\"proton\" && "
          "parentID==0)",
          "goff");

  // 2. Todas las particulas (dosis total)
  TH1D *hTotal = new TH1D("hTotal", "", nBins, xMin, xMax);
  t->Draw("x_pre>>hTotal", "edep*(volumeName==\"Phantom_phys\")", "goff");

  // 3. Solo secundarios
  TH1D *hSecondary = (TH1D *)hTotal->Clone("hSecondary");
  hSecondary->Add(hPrimary, -1);

  // ===== CONVERTIR A GRAY =====
  // Volumen por bin: dx * dy * dz
  double volume_cm3 = binWidth * phantom_dy * phantom_dz;
  double mass_kg = volume_cm3 * rho / 1000.0; // g -> kg

  // Factor conversion: MeV -> J / masa -> Gy
  // 1 MeV = 1.602e-13 J
  double MeV_to_J = 1.602e-13;
  double scale = MeV_to_J / mass_kg;

  TH1D *hPrimaryGy = (TH1D *)hPrimary->Clone("hPrimaryGy");
  TH1D *hTotalGy = (TH1D *)hTotal->Clone("hTotalGy");
  TH1D *hSecondaryGy = (TH1D *)hSecondary->Clone("hSecondaryGy");

  hPrimaryGy->Scale(scale);
  hTotalGy->Scale(scale);
  hSecondaryGy->Scale(scale);

  // ===== SUAVIZAR =====
  hPrimary->Smooth(2);
  hTotal->Smooth(2);
  hSecondary->Smooth(2);
  hPrimaryGy->Smooth(2);
  hTotalGy->Smooth(2);
  hSecondaryGy->Smooth(2);

  // ===== ENCONTRAR PICO =====
  hPrimaryGy->GetXaxis()->SetRangeUser(-5, 15);
  int peakBin = hPrimaryGy->GetMaximumBin();
  double peakX = hPrimaryGy->GetBinCenter(peakBin);
  double peakDose = hPrimaryGy->GetBinContent(peakBin);
  hPrimaryGy->GetXaxis()->SetRangeUser(xMin, xMax);

  std::cout << "\n=== RESULTS ===" << std::endl;
  std::cout << "Peak position: " << peakX << " cm" << std::endl;
  std::cout << "Peak dose (primary): " << peakDose << " Gy" << std::endl;
  std::cout << "Bin volume: " << volume_cm3 << " cm³" << std::endl;
  std::cout << "Bin mass: " << mass_kg * 1000 << " g" << std::endl;

  // ===== PLOT =====
  TCanvas *c1 = new TCanvas("c1", "Dose Analysis", 1400, 600);
  c1->Divide(2, 1);

  // Panel 1: Dosis en unidades arbitrarias
  c1->cd(1);
  gPad->SetGrid();
  hTotal->SetTitle(
      Form("Dose Profile - %.0f MeV (%d protons);Depth (cm);Dose (MeV)", energy,
           nProtons));
  hTotal->SetLineColor(kBlack);
  hTotal->SetLineWidth(2);
  hTotal->GetXaxis()->SetRangeUser(-12, 15);
  hTotal->Draw("HIST");

  hPrimary->SetLineColor(kBlue);
  hPrimary->SetLineWidth(2);
  hPrimary->Draw("HIST SAME");

  hSecondary->SetLineColor(kRed);
  hSecondary->SetLineWidth(2);
  hSecondary->SetLineStyle(2);
  hSecondary->Draw("HIST SAME");

  TLegend *leg1 = new TLegend(0.6, 0.7, 0.88, 0.88);
  leg1->SetBorderSize(0);
  leg1->AddEntry(hTotal, "Total", "l");
  leg1->AddEntry(hPrimary, "Primary protons", "l");
  leg1->AddEntry(hSecondary, "Secondaries", "l");
  leg1->Draw();

  // Panel 2: Dosis en Gray
  c1->cd(2);
  gPad->SetGrid();
  hTotalGy->SetTitle(
      Form("Dose Profile - %.0f MeV;Depth (cm);Dose (Gy)", energy));
  hTotalGy->SetLineColor(kBlack);
  hTotalGy->SetLineWidth(2);
  hTotalGy->GetXaxis()->SetRangeUser(-12, 15);
  hTotalGy->Draw("HIST");

  hPrimaryGy->SetLineColor(kBlue);
  hPrimaryGy->SetLineWidth(2);
  hPrimaryGy->Draw("HIST SAME");

  // Marcar pico
  TLine *peakLine = new TLine(peakX, 0, peakX, peakDose);
  peakLine->SetLineColor(kRed);
  peakLine->SetLineStyle(2);
  peakLine->Draw();

  TLegend *leg2 = new TLegend(0.55, 0.7, 0.88, 0.88);
  leg2->SetBorderSize(0);
  leg2->AddEntry(hTotalGy, "Total dose", "l");
  leg2->AddEntry(hPrimaryGy, "Primary protons", "l");
  leg2->AddEntry(peakLine, Form("Peak: %.1f cm, %.2e Gy", peakX, peakDose),
                 "l");
  leg2->Draw();

  c1->SaveAs("dose_analysis.png");
  std::cout << "\nSaved: dose_analysis.png" << std::endl;
}
