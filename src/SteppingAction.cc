// ============================================================================
// SteppingAction.cc - Extrae TODOS los datos de cada step
// ============================================================================
// Recolecta datos RAW para analisis manual en ROOT:
//   - Identificacion (eventID, trackID, parentID)
//   - Particula (nombre, PDG code)
//   - Posicion (pre y post step)
//   - Energia (depositada, cinetica pre/post)
//   - Step (longitud, proceso fisico, volumen)
// ============================================================================

#include "SteppingAction.hh"
#include "RunAction.hh"

#include "G4Event.hh"
#include "G4LogicalVolume.hh"
#include "G4RunManager.hh"
#include "G4Step.hh"
#include "G4StepPoint.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4VProcess.hh"

// ===== Constructor =====
SteppingAction::SteppingAction(RunAction *runAction) : fRunAction(runAction) {}

// ===== Destructor =====
SteppingAction::~SteppingAction() {}

// ============================================================================
// UserSteppingAction() - Se llama en CADA paso de CADA particula
// ============================================================================
void SteppingAction::UserSteppingAction(const G4Step *step) {
  // ===== 1. Obtener el track (particula) =====
  G4Track *track = step->GetTrack();

  // ===== 2. Identificadores =====
  G4int eventID =
      G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
  G4int trackID = track->GetTrackID();
  G4int parentID = track->GetParentID(); // 0 = particula primaria

  // ===== 3. Informacion de la particula =====
  G4String particleName = track->GetDefinition()->GetParticleName();
  G4int pdgCode = track->GetDefinition()->GetPDGEncoding();

  // ===== 4. Puntos pre y post step =====
  G4StepPoint *prePoint = step->GetPreStepPoint();
  G4StepPoint *postPoint = step->GetPostStepPoint();

  // Posiciones
  G4ThreeVector prePos = prePoint->GetPosition();
  G4ThreeVector postPos = postPoint->GetPosition();

  // ===== 5. Energia =====
  G4double edep = step->GetTotalEnergyDeposit();
  G4double kinE_pre = prePoint->GetKineticEnergy();
  G4double kinE_post = postPoint->GetKineticEnergy();

  // ===== 6. Informacion del step =====
  G4double stepLength = step->GetStepLength();

  // Proceso fisico que limito el step
  G4String processName = "undefined";
  if (postPoint->GetProcessDefinedStep()) {
    processName = postPoint->GetProcessDefinedStep()->GetProcessName();
  }

  // Volumen donde ocurrio
  G4String volumeName = "undefined";
  if (prePoint->GetTouchable() && prePoint->GetTouchable()->GetVolume()) {
    volumeName = prePoint->GetTouchable()->GetVolume()->GetName();
  }

  // ===== 7. Llenar el TTree =====
  if (fRunAction) {
    fRunAction->FillRawData(eventID, trackID, parentID, particleName, pdgCode,
                            prePos.x(), prePos.y(), prePos.z(), postPos.x(),
                            postPos.y(), postPos.z(), edep, kinE_pre, kinE_post,
                            stepLength, processName, volumeName);
  }
}
