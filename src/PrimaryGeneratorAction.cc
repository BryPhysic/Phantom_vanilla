// ============================================================================
// PrimaryGeneratorAction.cc - Generador de Protones
// El haz sale desde la posicion de la Source y viaja en +X
// ============================================================================

#include "PrimaryGeneratorAction.hh"

#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#include "G4SystemOfUnits.hh"
#include "G4ThreeVector.hh"

// ===== Constructor =====
PrimaryGeneratorAction::PrimaryGeneratorAction() {
  G4int nParticles = 1;
  fParticleGun = new G4ParticleGun(nParticles);

  // Configurar proton
  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition *proton = particleTable->FindParticle("proton");
  fParticleGun->SetParticleDefinition(proton);

  // Energia: 150 MeV
  fParticleGun->SetParticleEnergy(150.0 * MeV);

  // Posicion: sale desde la Source (X=-40cm, justo enfrente del cilindro)
  // La source esta en X=-41.5cm, el haz sale desde su cara frontal
  fParticleGun->SetParticlePosition(G4ThreeVector(-40.0 * cm, 0, 0));

  // Direccion: +X (hacia el phantom)
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1, 0, 0));

  G4cout << "========================================" << G4endl;
  G4cout << " Beam: proton 150 MeV" << G4endl;
  G4cout << " Posicion: (-40, 0, 0) cm" << G4endl;
  G4cout << " Direccion: +X" << G4endl;
  G4cout << "========================================" << G4endl;
}

// ===== Destructor =====
PrimaryGeneratorAction::~PrimaryGeneratorAction() { delete fParticleGun; }

// ===== GeneratePrimaries =====
void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {
  fParticleGun->GeneratePrimaryVertex(anEvent);
}
