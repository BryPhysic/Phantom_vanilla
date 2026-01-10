// ============================================================================
// DetectorConstruction.hh - Definicion de la Geometria
// Aqui declaramos la clase que construye el mundo, phantom y blackhole
// ============================================================================
// NOTA: Los .hh son solo declaraciones, la implementacion va en el .cc
// ============================================================================

#ifndef DETECTOR_CONSTRUCTION_HH // evita que se incluya dos veces
#define DETECTOR_CONSTRUCTION_HH

// ===== SECCION 1: Headers necesarios =====
#include "G4LogicalVolume.hh"             // para el volumen logico
#include "G4VUserDetectorConstruction.hh" // clase base de Geant4

// ============================================================================
// CLASE DetectorConstruction
// Hereda de G4VUserDetectorConstruction que es la clase base de Geant4
// para definir geometrias
// ============================================================================
class DetectorConstruction : public G4VUserDetectorConstruction {
public:
  // Constructor y destructor
  DetectorConstruction();
  virtual ~DetectorConstruction();

  // ===== METODO OBLIGATORIO =====
  // Construct() es el metodo que Geant4 llama para construir la geometria
  // Retorna un puntero al volumen fisico del World (el universo)
  virtual G4VPhysicalVolume *Construct();

  // ===== GETTER para el volumen logico del phantom =====
  // Lo necesitamos para saber en que volumne registrar la dosis
  G4LogicalVolume *GetPhantomLogical() const { return fPhantomLogical; }

private:
  // puntero al volumen logico del phantom (lo usamos en el getter)
  G4LogicalVolume *fPhantomLogical;
};

#endif // DETECTOR_CONSTRUCTION_HH
