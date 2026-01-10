// ============================================================================
// RunAction.hh - Acciones del Run con datos RAW completos
// ============================================================================

#ifndef RUN_ACTION_HH
#define RUN_ACTION_HH

#include "G4UserRunAction.hh"
#include "globals.hh"

// Headers de ROOT
#include "TFile.h"
#include "TTree.h"

#include <string>

// ============================================================================
class RunAction : public G4UserRunAction {
public:
  RunAction();
  virtual ~RunAction();

  virtual void BeginOfRunAction(const G4Run *run);
  virtual void EndOfRunAction(const G4Run *run);

  // ===== Metodo para llenar el TTree con datos RAW =====
  void FillRawData(G4int eventID, G4int trackID, G4int parentID,
                   const G4String &particleName, G4int pdgCode, G4double x_pre,
                   G4double y_pre, G4double z_pre, G4double x_post,
                   G4double y_post, G4double z_post, G4double edep,
                   G4double kinE_pre, G4double kinE_post, G4double stepLength,
                   const G4String &processName, const G4String &volumeName);

private:
  TFile *fRootFile;
  TTree *fTree;

  // Variables para el TTree (todas las ramas)
  G4int fEventID;
  G4int fTrackID;
  G4int fParentID;
  char fParticleName[32]; // string como char array para ROOT
  G4int fPdgCode;
  G4double fX_pre, fY_pre, fZ_pre;
  G4double fX_post, fY_post, fZ_post;
  G4double fEdep;
  G4double fKinE_pre, fKinE_post;
  G4double fStepLength;
  char fProcessName[32];
  char fVolumeName[32];
};

#endif
