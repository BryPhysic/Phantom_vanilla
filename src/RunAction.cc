// ============================================================================
// RunAction.cc - Genera archivos ROOT con nombres dinamicos
// ============================================================================
// Actualizado para funcionar con GPS (General Particle Source)
// El nombre del archivo incluye energia, eventos y runID
// ============================================================================

#include "RunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"
#include "G4SystemOfUnits.hh"

// ============================================================================
// CODIGO ANTERIOR (ParticleGun) - Headers comentados
// ============================================================================
// #include "G4ParticleGun.hh"
// #include "PrimaryGeneratorAction.hh"  // para obtener el gun
// ============================================================================

// ============================================================================
// CODIGO NUEVO (GPS) - Headers activos
// ============================================================================
#include "G4GeneralParticleSource.hh"
#include "PrimaryGeneratorAction.hh"
// ============================================================================

#include <cstring>
#include <iomanip>
#include <sstream>

// ===== Constructor =====
RunAction::RunAction()
    : fRootFile(nullptr), fTree(nullptr), fEventID(0), fTrackID(0),
      fParentID(0), fPdgCode(0), fX_pre(0), fY_pre(0), fZ_pre(0), fX_post(0),
      fY_post(0), fZ_post(0), fEdep(0), fKinE_pre(0), fKinE_post(0),
      fStepLength(0), fBeamEnergy(0) {
  fParticleName[0] = '\0';
  fProcessName[0] = '\0';
  fVolumeName[0] = '\0';
}

// ===== Destructor =====
RunAction::~RunAction() {}

// ============================================================================
// BeginOfRunAction() - Crea archivo ROOT con nombre dinamico
// ============================================================================
void RunAction::BeginOfRunAction(const G4Run *run) {
  // ===== Obtener la energia del beam =====
  // ========================================================================
  // CODIGO ANTERIOR (ParticleGun):
  // ========================================================================
  /*
  const PrimaryGeneratorAction* primaryGen =
      dynamic_cast<const PrimaryGeneratorAction*>(
          G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction()
      );
  if (primaryGen && primaryGen->GetParticleGun()) {
      fBeamEnergy = primaryGen->GetParticleGun()->GetParticleEnergy() / MeV;
  }
  */
  // ========================================================================

  // ========================================================================
  // CODIGO NUEVO (GPS):
  // ========================================================================
  const PrimaryGeneratorAction *primaryGen =
      dynamic_cast<const PrimaryGeneratorAction *>(
          G4RunManager::GetRunManager()->GetUserPrimaryGeneratorAction());

  if (primaryGen && primaryGen->GetGPS()) {
    // GPS puede tener distribuciones de energia, obtenemos la energia "mono"
    fBeamEnergy = primaryGen->GetGPS()
                      ->GetCurrentSource()
                      ->GetEneDist()
                      ->GetMonoEnergy() /
                  MeV;
  } else {
    fBeamEnergy = 0.0;
  }
  // ========================================================================

  // ===== Obtener numero de eventos programados =====
  G4int nEvents = run->GetNumberOfEventToBeProcessed();
  G4int runID = run->GetRunID();

  // ===== Generar nombre del archivo =====
  std::ostringstream filename;
  filename << "raw_" << std::fixed << std::setprecision(0) << fBeamEnergy
           << "MeV_" << nEvents << "evts_run" << runID << ".root";

  G4cout << "========================================" << G4endl;
  G4cout << " Iniciando Run #" << runID << G4endl;
  G4cout << " Energia del beam: " << fBeamEnergy << " MeV" << G4endl;
  G4cout << " Eventos programados: " << nEvents << G4endl;
  G4cout << " Archivo de salida: " << filename.str() << G4endl;
  G4cout << "========================================" << G4endl;

  // ===== Crear archivo ROOT =====
  fRootFile = new TFile(filename.str().c_str(), "RECREATE");

  // ===== TTree con datos RAW =====
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

  // Rama de energia del beam (para referencia)
  fTree->Branch("beamEnergy", &fBeamEnergy, "beamEnergy/D");

  // Ramas de step
  fTree->Branch("stepLength", &fStepLength, "stepLength/D");
  fTree->Branch("processName", fProcessName, "processName/C");
  fTree->Branch("volumeName", fVolumeName, "volumeName/C");
}

// ============================================================================
// EndOfRunAction() - Guarda y cierra archivo ROOT
// ============================================================================
void RunAction::EndOfRunAction(const G4Run *run) {
  G4cout << "========================================" << G4endl;
  G4cout << " Run #" << run->GetRunID() << " completado" << G4endl;
  G4cout << " Eventos procesados: " << run->GetNumberOfEvent() << G4endl;

  if (fRootFile && fTree) {
    G4cout << " Entries en TTree: " << fTree->GetEntries() << G4endl;

    fRootFile->Write();
    fRootFile->Close();

    G4cout << " Archivo guardado exitosamente!" << G4endl;
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
  fEventID = eventID;
  fTrackID = trackID;
  fParentID = parentID;
  fPdgCode = pdgCode;

  strncpy(fParticleName, particleName.c_str(), 31);
  fParticleName[31] = '\0';
  strncpy(fProcessName, processName.c_str(), 31);
  fProcessName[31] = '\0';
  strncpy(fVolumeName, volumeName.c_str(), 31);
  fVolumeName[31] = '\0';

  fX_pre = x_pre / cm;
  fY_pre = y_pre / cm;
  fZ_pre = z_pre / cm;
  fX_post = x_post / cm;
  fY_post = y_post / cm;
  fZ_post = z_post / cm;

  fEdep = edep / MeV;
  fKinE_pre = kinE_pre / MeV;
  fKinE_post = kinE_post / MeV;

  fStepLength = stepLength / mm;

  if (fTree) {
    fTree->Fill();
  }
}
