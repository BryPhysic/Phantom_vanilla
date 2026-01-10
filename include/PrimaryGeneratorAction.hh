// ============================================================================
// PrimaryGeneratorAction.hh - Generador de Particulas Primarias
// Aqui declaramos la clase que dispara los protones de 150 MeV
// ============================================================================

#ifndef PRIMARY_GENERATOR_ACTION_HH
#define PRIMARY_GENERATOR_ACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh" // clase base de Geant4

// forward declaration (para no incluir todo el header)
class G4ParticleGun;
class G4Event;

// ============================================================================
// CLASE PrimaryGeneratorAction
// Esta clase genera las particulas primarias (los protones que disparamos)
// ============================================================================
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  PrimaryGeneratorAction();
  virtual ~PrimaryGeneratorAction();

  // ===== METODO OBLIGATORIO =====
  // GeneratePrimaries() se llama al inicio de cada evento
  // Aqui disparamos un proton hacia el phantom
  virtual void GeneratePrimaries(G4Event *anEvent);

private:
  // ParticleGun es como una "pistola" que dispara particulas
  G4ParticleGun *fParticleGun;
};

#endif // PRIMARY_GENERATOR_ACTION_HH
