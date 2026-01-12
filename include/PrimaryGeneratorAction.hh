// ============================================================================
// PrimaryGeneratorAction.hh - Generador de Particulas Primarias
// ============================================================================
// MIGRACION DIDACTICA: De G4ParticleGun a G4GeneralParticleSource (GPS)
//
// G4ParticleGun: Dispara particulas con parametros FIJOS (poco realista)
// GPS: Permite DISTRIBUCIONES de energia, posicion y direccion (realista)
//
// Con GPS, todo se controla desde el archivo .mac, sin recompilar!
// ============================================================================

#ifndef PRIMARY_GENERATOR_ACTION_HH
#define PRIMARY_GENERATOR_ACTION_HH

#include "G4VUserPrimaryGeneratorAction.hh"

// ============================================================================
// CODIGO ANTERIOR (ParticleGun) - COMENTADO PARA REFERENCIA DIDACTICA
// ============================================================================
// class G4ParticleGun;  // <-- Antes usabamos esto
// ============================================================================

// ============================================================================
// CODIGO NUEVO (GPS) - ACTIVO
// ============================================================================
class G4GeneralParticleSource; // <-- Ahora usamos GPS
// ============================================================================

class G4Event;

// ============================================================================
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction {
public:
  PrimaryGeneratorAction();
  virtual ~PrimaryGeneratorAction();

  virtual void GeneratePrimaries(G4Event *anEvent);

  // ========================================================================
  // CODIGO ANTERIOR (ParticleGun) - COMENTADO
  // ========================================================================
  // const G4ParticleGun* GetParticleGun() const { return fParticleGun; }
  // ========================================================================

  // ========================================================================
  // CODIGO NUEVO (GPS) - ACTIVO
  // ========================================================================
  const G4GeneralParticleSource *GetGPS() const { return fGPS; }
  // ========================================================================

private:
  // ========================================================================
  // CODIGO ANTERIOR (ParticleGun) - COMENTADO
  // ========================================================================
  // G4ParticleGun* fParticleGun;  // <-- Puntero al ParticleGun
  // ========================================================================

  // ========================================================================
  // CODIGO NUEVO (GPS) - ACTIVO
  // ========================================================================
  G4GeneralParticleSource *fGPS; // <-- Puntero al GPS
  // ========================================================================
};

#endif
