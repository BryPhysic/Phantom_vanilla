// ============================================================================
// PrimaryGeneratorAction.cc - Generador con switch GPS/ParticleGun
// ============================================================================
// SWITCH: USE_GPS (definido en .hh)
//   USE_GPS = 1 -> GPS (energia configurable desde .mac)
//   USE_GPS = 0 -> ParticleGun (150 MeV fijos, simple, sin dispersion)
// ============================================================================

#include "PrimaryGeneratorAction.hh"
#include "G4SystemOfUnits.hh"

#if USE_GPS
#include "G4GeneralParticleSource.hh"
#else
#include "G4ParticleDefinition.hh"
#include "G4ParticleGun.hh"
#include "G4ParticleTable.hh"
#endif

// ============================================================================
// CONSTRUCTOR
// ============================================================================
PrimaryGeneratorAction::PrimaryGeneratorAction() {
#if USE_GPS
  // GPS - Configuracion desde .mac
  fGPS = new G4GeneralParticleSource();
  G4cout << "========================================" << G4endl;
  G4cout << " USANDO: G4GeneralParticleSource (GPS)" << G4endl;
  G4cout << " Configure desde el .mac:" << G4endl;
  G4cout << "   /gps/particle proton" << G4endl;
  G4cout << "   /gps/ene/mono 150 MeV" << G4endl;
  G4cout << "========================================" << G4endl;
#else
  // ParticleGun - Configuracion fija en codigo
  G4int nParticles = 1;
  fParticleGun = new G4ParticleGun(nParticles);

  // Particula: proton
  G4ParticleTable *particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition *proton = particleTable->FindParticle("proton");
  fParticleGun->SetParticleDefinition(proton);

  // Energia: 150 MeV FIJA (sin dispersion Gaussiana)
  fParticleGun->SetParticleEnergy(150.0 * MeV);

  // Posicion: fuera del phantom
  fParticleGun->SetParticlePosition(G4ThreeVector(-40.0 * cm, 0, 0));

  // Direccion: +X
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1, 0, 0));

  G4cout << "========================================" << G4endl;
  G4cout << " USANDO: G4ParticleGun (simple)" << G4endl;
  G4cout << " Configuracion FIJA:" << G4endl;
  G4cout << "   - Proton 150 MeV" << G4endl;
  G4cout << "   - Posicion: (-40, 0, 0) cm" << G4endl;
  G4cout << "   - Direccion: +X" << G4endl;
  G4cout << "========================================" << G4endl;
#endif
}

// ============================================================================
// DESTRUCTOR
// ============================================================================
PrimaryGeneratorAction::~PrimaryGeneratorAction() {
#if USE_GPS
  delete fGPS;
#else
  delete fParticleGun;
#endif
}

// ============================================================================
// GeneratePrimaries()
// ============================================================================
void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {
#if USE_GPS
  fGPS->GeneratePrimaryVertex(anEvent);
#else
  fParticleGun->GeneratePrimaryVertex(anEvent);
#endif
}
