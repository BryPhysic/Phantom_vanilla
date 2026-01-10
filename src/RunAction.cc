// ============================================================================
// RunAction.cc - Crea archivo ROOT con TTree de datos RAW completos
// ============================================================================
// Este archivo guarda TODOS los datos crudos de cada step para que puedas
// hacer tu propio analisis en ROOT de forma artesanal
// ============================================================================
// TTree "raw_data" contiene:
//   - eventID, trackID, parentID
//   - particleName, pdgCode
//   - x_pre, y_pre, z_pre, x_post, y_post, z_post
//   - edep, kinE_pre, kinE_post
//   - stepLength, processName, volumeName
// ============================================================================

#include "RunAction.hh"

#include "G4Run.hh"
#include "G4SystemOfUnits.hh"

#include <cstring> // para strncpy

// ===== Constructor =====
RunAction::RunAction()
    : fRootFile(nullptr), fTree(nullptr), fEventID(0), fTrackID(0),
      fParentID(0), fPdgCode(0), fX_pre(0), fY_pre(0), fZ_pre(0), fX_post(0),
      fY_post(0), fZ_post(0), fEdep(0), fKinE_pre(0), fKinE_post(0),
      fStepLength(0) {
  fParticleName[0] = '\0';
  fProcessName[0] = '\0';
  fVolumeName[0] = '\0';
}

// ===== Destructor =====
RunAction::~RunAction() {}

// ============================================================================
// BeginOfRunAction() - Crea archivo ROOT y TTree
// ============================================================================
void RunAction::BeginOfRunAction(const G4Run *run) {
  G4cout << "========================================" << G4endl;
  G4cout << " Iniciando Run #" << run->GetRunID() << G4endl;
  G4cout << " Creando raw_output.root (datos crudos)" << G4endl;
  G4cout << "========================================" << G4endl;

  // Crear archivo ROOT
  fRootFile = new TFile("raw_output.root", "RECREATE");

  // ===== TTree con TODOS los datos RAW =====
  fTree = new TTree("raw_data", "Raw step data for manual analysis");

  // Ramas de identificacion
  fTree->Branch("eventID", &fEventID, "eventID/I");
  fTree->Branch("trackID", &fTrackID, "trackID/I");
  fTree->Branch("parentID", &fParentID, "parentID/I");

  // Ramas de particula
  fTree->Branch("particleName", fParticleName, "particleName/C");
  fTree->Branch("pdgCode", &fPdgCode, "pdgCode/I");

  // Ramas de posicion (en cm)
  fTree->Branch("x_pre", &fX_pre, "x_pre/D");
  fTree->Branch("y_pre", &fY_pre, "y_pre/D");
  fTree->Branch("z_pre", &fZ_pre, "z_pre/D");
  fTree->Branch("x_post", &fX_post, "x_post/D");
  fTree->Branch("y_post", &fY_post, "y_post/D");
  fTree->Branch("z_post", &fZ_post, "z_post/D");

  // Ramas de energia (en MeV)
  fTree->Branch("edep", &fEdep, "edep/D");
  fTree->Branch("kinE_pre", &fKinE_pre, "kinE_pre/D");
  fTree->Branch("kinE_post", &fKinE_post, "kinE_post/D");

  // Ramas de step
  fTree->Branch("stepLength", &fStepLength, "stepLength/D");
  fTree->Branch("processName", fProcessName, "processName/C");
  fTree->Branch("volumeName", fVolumeName, "volumeName/C");

  G4cout << " TTree 'raw_data' creado con 16 ramas" << G4endl;
}

// ============================================================================
// EndOfRunAction() - Guarda y cierra archivo ROOT
// ============================================================================
void RunAction::EndOfRunAction(const G4Run *run) {
  G4cout << "========================================" << G4endl;
  G4cout << " Run #" << run->GetRunID() << " completado" << G4endl;
  G4cout << " Eventos: " << run->GetNumberOfEvent() << G4endl;

  if (fRootFile && fTree) {
    G4cout << " Entries en TTree: " << fTree->GetEntries() << G4endl;

    fRootFile->Write();
    fRootFile->Close();

    G4cout << "========================================" << G4endl;
    G4cout << " Archivo guardado: raw_output.root" << G4endl;
    G4cout << " ----------------------------------------" << G4endl;
    G4cout << " Ramas disponibles para analisis:" << G4endl;
    G4cout << "   eventID, trackID, parentID" << G4endl;
    G4cout << "   particleName, pdgCode" << G4endl;
    G4cout << "   x_pre, y_pre, z_pre [cm]" << G4endl;
    G4cout << "   x_post, y_post, z_post [cm]" << G4endl;
    G4cout << "   edep, kinE_pre, kinE_post [MeV]" << G4endl;
    G4cout << "   stepLength [mm], processName, volumeName" << G4endl;
    G4cout << "========================================" << G4endl;

    delete fRootFile;
    fRootFile = nullptr;
  }
}

// ============================================================================
// FillRawData() - Llena el TTree con todos los datos de un step
// ============================================================================
void RunAction::FillRawData(G4int eventID, G4int trackID, G4int parentID,
                            const G4String &particleName, G4int pdgCode,
                            G4double x_pre, G4double y_pre, G4double z_pre,
                            G4double x_post, G4double y_post, G4double z_post,
                            G4double edep, G4double kinE_pre,
                            G4double kinE_post, G4double stepLength,
                            const G4String &processName,
                            const G4String &volumeName) {
  // Llenar variables
  fEventID = eventID;
  fTrackID = trackID;
  fParentID = parentID;
  fPdgCode = pdgCode;

  // Copiar strings (maximo 31 caracteres + null)
  strncpy(fParticleName, particleName.c_str(), 31);
  fParticleName[31] = '\0';
  strncpy(fProcessName, processName.c_str(), 31);
  fProcessName[31] = '\0';
  strncpy(fVolumeName, volumeName.c_str(), 31);
  fVolumeName[31] = '\0';

  // Posiciones en cm
  fX_pre = x_pre / cm;
  fY_pre = y_pre / cm;
  fZ_pre = z_pre / cm;
  fX_post = x_post / cm;
  fY_post = y_post / cm;
  fZ_post = z_post / cm;

  // Energias en MeV
  fEdep = edep / MeV;
  fKinE_pre = kinE_pre / MeV;
  fKinE_post = kinE_post / MeV;

  // Step length en mm
  fStepLength = stepLength / mm;

  // Agregar entrada al TTree
  if (fTree) {
    fTree->Fill();
  }
}
