// ============================================================================
// PrimaryGeneratorAction.hh - Generador de Particulas Primarias
// ============================================================================
// SWITCH entre GPS y ParticleGun:
//   #define USE_GPS 1   -> Usa GPS (configurable desde .mac)
//   #define USE_GPS 0   -> Usa ParticleGun (energia fija en codigo)
// ============================================================================

#ifndef PRIMARY_GENERATOR_ACTION_HH
#define PRIMARY_GENERATOR_ACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"

// ============================================================================
// SWITCH: Cambiar entre GPS y ParticleGun
// ============================================================================
#define USE_GPS 1 // 0 = ParticleGun (simple), 1 = GPS (flexible)
// ============================================================================

#if USE_GPS
class G4GeneralParticleSource;
#else
class G4ParticleGun;
#endif

class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  PrimaryGeneratorAction();
  virtual ~PrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event *anEvent);

#if USE_GPS
  const G4GeneralParticleSource *GetGPS() const { return fGPS; }

private:
  G4GeneralParticleSource *fGPS;
#else
  const G4ParticleGun *GetParticleGun() const { return fParticleGun; }

private:
  G4ParticleGun *fParticleGun;
#endif
};

#endif
