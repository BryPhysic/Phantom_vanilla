// ============================================================================
// PrimaryGeneratorAction.cc - Generador con GPS (General Particle Source)
// ============================================================================
// MIGRACION DIDACTICA: De G4ParticleGun a G4GeneralParticleSource
//
// DIFERENCIA CLAVE:
// - G4ParticleGun: Configuracion en CODIGO (hay que recompilar para cambiar)
// - GPS: Configuracion en MACRO (.mac) - mas flexible, no hay que recompilar
//
// Con GPS, la energia, posicion y direccion se controlan desde el .mac:
//   /gps/particle proton
//   /gps/energy 150 MeV
//   /gps/ene/type Gauss        <-- DISTRIBUCION de energia (realista!)
//   /gps/ene/sigma 1.5 MeV     <-- Dispersion del acelerador
//   /gps/pos/type Point
//   /gps/pos/centre -40 0 0 cm
//   /gps/direction 1 0 0
// ============================================================================

#include "PrimaryGeneratorAction.hh"

// ============================================================================
// HEADERS ANTERIORES (ParticleGun) - COMENTADOS
// ============================================================================
// #include "G4ParticleGun.hh"          // la "pistola" de particulas
// #include "G4ParticleTable.hh"        // tabla de todas las particulas
// #include "G4ParticleDefinition.hh"   // definicion de particulas
// ============================================================================

// ============================================================================
// HEADERS NUEVOS (GPS) - ACTIVOS
// ============================================================================
#include "G4GeneralParticleSource.hh" // el generador profesional
// ============================================================================

#include "G4SystemOfUnits.hh"

// ============================================================================
// CONSTRUCTOR
// ============================================================================
PrimaryGeneratorAction::PrimaryGeneratorAction() {
  // ========================================================================
  // CODIGO ANTERIOR (ParticleGun) - COMENTADO PARA REFERENCIA
  // ========================================================================
  /*
  // Con ParticleGun, habia que configurar TODO aqui en el codigo:

  G4int nParticles = 1;
  fParticleGun = new G4ParticleGun(nParticles);

  // Tipo de particula - FIJO en el codigo
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4ParticleDefinition* proton = particleTable->FindParticle("proton");
  fParticleGun->SetParticleDefinition(proton);

  // Energia - FIJA en el codigo (150.0000 MeV exactos)
  fParticleGun->SetParticleEnergy(150.0*MeV);

  // Posicion - FIJA en el codigo
  fParticleGun->SetParticlePosition(G4ThreeVector(-40.0*cm, 0, 0));

  // Direccion - FIJA en el codigo
  fParticleGun->SetParticleMomentumDirection(G4ThreeVector(1, 0, 0));

  // PROBLEMA: Para cambiar CUALQUIER cosa, hay que recompilar!
  */
  // ========================================================================

  // ========================================================================
  // CODIGO NUEVO (GPS) - ACTIVO
  // ========================================================================
  // Con GPS, solo creamos el generador aqui.
  // TODA la configuracion se hace desde el archivo .mac!
  // Esto permite cambiar energia, posicion, distribuciones sin recompilar.

  fGPS = new G4GeneralParticleSource();

  // GPS viene con valores por defecto, pero los sobreescribimos en el .mac
  // ========================================================================

  G4cout << "========================================" << G4endl;
  G4cout << " GPS (GeneralParticleSource) inicializado" << G4endl;
  G4cout << " Configure el haz desde el archivo .mac:" << G4endl;
  G4cout << "   /gps/particle proton" << G4endl;
  G4cout << "   /gps/energy 150 MeV" << G4endl;
  G4cout << "   /gps/pos/centre -40 0 0 cm" << G4endl;
  G4cout << "   /gps/direction 1 0 0" << G4endl;
  G4cout << "========================================" << G4endl;
}

// ============================================================================
// DESTRUCTOR
// ============================================================================
PrimaryGeneratorAction::~PrimaryGeneratorAction() {
  // ========================================================================
  // CODIGO ANTERIOR
  // ========================================================================
  // delete fParticleGun;
  // ========================================================================

  // ========================================================================
  // CODIGO NUEVO
  // ========================================================================
  delete fGPS;
  // ========================================================================
}

// ============================================================================
// GeneratePrimaries() - Se llama al inicio de cada evento
// ============================================================================
void PrimaryGeneratorAction::GeneratePrimaries(G4Event *anEvent) {
  // ========================================================================
  // CODIGO ANTERIOR
  // ========================================================================
  // fParticleGun->GeneratePrimaryVertex(anEvent);
  // ========================================================================

  // ========================================================================
  // CODIGO NUEVO - Funciona igual, solo cambia el nombre
  // ========================================================================
  fGPS->GeneratePrimaryVertex(anEvent);
  // ========================================================================
}
