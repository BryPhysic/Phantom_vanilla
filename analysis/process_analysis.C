// ============================================================================
// process_analysis.C - Analyze physics processes in simulation
// ============================================================================
// Shows which physics processes are occurring
// Usage: root -l process_analysis.C
// ============================================================================

#include <TCanvas.h>
#include <TChain.h>
#include <TFile.h>
#include <TH1D.h>
#include <TStyle.h>
#include <TTree.h>
#include <iostream>
#include <map>

void process_analysis() {
  gStyle->SetOptStat(0);

  // Load one file
  TString filename = "../build/output/raw_150MeV_1000000evts_run0.root";
  TFile *f = TFile::Open(filename);
  if (!f) {
    std::cout << "Cannot open file" << std::endl;
    return;
  }

  TTree *tree = (TTree *)f->Get("raw_data");
  std::cout << "=== PHYSICS PROCESS ANALYSIS ===" << std::endl;
  std::cout << "File: " << filename << std::endl;
  std::cout << "Total entries: " << tree->GetEntries() << std::endl;

  // Setup branches
  char processName[32];
  char particleName[32];
  double edep;
  int parentID;

  tree->SetBranchAddress("processName", processName);
  tree->SetBranchAddress("particleName", particleName);
  tree->SetBranchAddress("edep", &edep);
  tree->SetBranchAddress("parentID", &parentID);

  // Count processes
  std::map<std::string, int> processCount;
  std::map<std::string, double> processEdep;
  std::map<std::string, int> particleCount;

  Long64_t nEntries = tree->GetEntries();
  for (Long64_t i = 0; i < nEntries; i++) {
    tree->GetEntry(i);
    processCount[processName]++;
    processEdep[processName] += edep;
    particleCount[particleName]++;
  }

  // Print process summary
  std::cout << "\n--- PHYSICS PROCESSES ---" << std::endl;
  std::cout << "Process Name          | Count      | Total Edep (MeV)"
            << std::endl;
  std::cout << "----------------------|------------|------------------"
            << std::endl;
  for (auto &p : processCount) {
    printf("%-21s | %10d | %12.3f\n", p.first.c_str(), p.second,
           processEdep[p.first]);
  }

  // Print particle summary
  std::cout << "\n--- PARTICLE TYPES ---" << std::endl;
  std::cout << "Particle Name         | Count" << std::endl;
  std::cout << "----------------------|------------" << std::endl;
  for (auto &p : particleCount) {
    printf("%-21s | %10d\n", p.first.c_str(), p.second);
  }

  // Create histogram
  TCanvas *c1 = new TCanvas("c1", "Process Analysis", 1400, 500);
  c1->Divide(2, 1);

  // Panel 1: Process histogram
  c1->cd(1);
  gPad->SetBottomMargin(0.25);
  TH1D *hProc = new TH1D("hProc", "Physics Processes;Process;Count",
                         processCount.size(), 0, processCount.size());
  int bin = 1;
  for (auto &p : processCount) {
    hProc->SetBinContent(bin, p.second);
    hProc->GetXaxis()->SetBinLabel(bin, p.first.c_str());
    bin++;
  }
  hProc->SetFillColor(kBlue);
  hProc->GetXaxis()->LabelsOption("v");
  hProc->Draw("BAR");

  // Panel 2: Particle histogram
  c1->cd(2);
  gPad->SetBottomMargin(0.25);
  TH1D *hPart = new TH1D("hPart", "Particle Types;Particle;Count",
                         particleCount.size(), 0, particleCount.size());
  bin = 1;
  for (auto &p : particleCount) {
    hPart->SetBinContent(bin, p.second);
    hPart->GetXaxis()->SetBinLabel(bin, p.first.c_str());
    bin++;
  }
  hPart->SetFillColor(kRed);
  hPart->GetXaxis()->LabelsOption("v");
  hPart->Draw("BAR");

  c1->SaveAs("process_analysis.png");
  std::cout << "\nSaved: process_analysis.png" << std::endl;
}
