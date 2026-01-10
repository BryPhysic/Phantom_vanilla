// ============================================================================
// DetectorConstruction.cc - Implementacion de la Geometria
// Aqui construimos: World (aire), Source (visualizacion), Phantom (agua)
// ============================================================================
// Configuracion estetica para visualizacion clara de hadronterapia
// ============================================================================

#include "DetectorConstruction.hh"

// ===== SECCION 1: Headers de Geant4 para geometria =====
#include "G4Box.hh"           // para crear cajas
#include "G4Colour.hh"        // colores
#include "G4LogicalVolume.hh" // volumen logico
#include "G4NistManager.hh"   // materiales NIST
#include "G4PVPlacement.hh"   // posicionar volumenes
#include "G4SystemOfUnits.hh" // unidades
#include "G4Tubs.hh"          // para crear cilindros (la source)
#include "G4VisAttributes.hh" // visualizacion

// ===== SECCION 2: Constructor =====
DetectorConstruction::DetectorConstruction() : fPhantomLogical(nullptr) {}

// ===== SECCION 3: Destructor =====
DetectorConstruction::~DetectorConstruction() {}

// ============================================================================
// METODO Construct() - Creamos la geometria completa
// ============================================================================
G4VPhysicalVolume *DetectorConstruction::Construct() {
  // ===== SECCION 4: Materiales =====
  G4NistManager *nist = G4NistManager::Instance();

  G4Material *aire = nist->FindOrBuildMaterial("G4_AIR");
  G4Material *agua = nist->FindOrBuildMaterial("G4_WATER");
  // Usamos aluminio para la "carcasa" de la fuente (solo estetico)
  G4Material *aluminio = nist->FindOrBuildMaterial("G4_Al");

  // ===== SECCION 5: WORLD =====
  // World grande para ver todo el setup: 100x60x60 cm^3
  // Semi-dimensiones: 50x30x30 cm

  G4double world_hx = 50.0 * cm; // largo en X (direccion del haz)
  G4double world_hy = 30.0 * cm; // ancho en Y
  G4double world_hz = 30.0 * cm; // alto en Z

  G4Box *solidWorld = new G4Box("World_solid", world_hx, world_hy, world_hz);

  G4LogicalVolume *logicWorld =
      new G4LogicalVolume(solidWorld, aire, "World_log");

  G4VPhysicalVolume *physWorld = new G4PVPlacement(
      0, G4ThreeVector(), logicWorld, "World_phys", 0, false, 0);

  // ===== SECCION 6: SOURCE BEAM (representacion visual) =====
  // Un cilindro verde que representa la fuente de protones
  // No afecta la fisica, es solo para visualizacion
  // Posicion: en el lado izquierdo del World (X negativo)

  G4double source_rmin = 0.0 * cm; // radio interno
  G4double source_rmax = 2.0 * cm; // radio externo
  G4double source_hz = 1.0 * cm;   // semi-altura (8cm total)

  // Cilindro orientado en X (el haz va en +X)
  G4Tubs *solidSource = new G4Tubs("Source_solid", source_rmin, source_rmax,
                                   source_hz, 0.0 * deg, 360.0 * deg);

  G4LogicalVolume *logicSource =
      new G4LogicalVolume(solidSource, aluminio, "Source_log");

  // Rotacion: el cilindro por defecto esta en Z, lo rotamos 90 grados en Y
  // para que apunte en direccion X
  G4RotationMatrix *rotSource = new G4RotationMatrix();
  rotSource->rotateY(90.0 * deg);

  // Posicion: a la izquierda, cerca del borde del World
  new G4PVPlacement(rotSource,
                    G4ThreeVector(-41.5 * cm, 0, 0), // posicion de la fuente
                    logicSource, "Source_phys", logicWorld, false, 0);

  // ===== SECCION 7: PHANTOM DE AGUA =====
  // Dimensiones: 20 x 20 x 40 cm^3 (pero orientado en X)
  // El haz viaja en +X, asi que el phantom es largo en X

  G4double phantom_hx =
      20.0 * cm; // 40 cm en X (profundidad, direccion del haz)
  G4double phantom_hy = 10.0 * cm; // 20 cm en Y
  G4double phantom_hz = 10.0 * cm; // 20 cm en Z

  G4Box *solidPhantom =
      new G4Box("Phantom_solid", phantom_hx, phantom_hy, phantom_hz);

  fPhantomLogical = new G4LogicalVolume(solidPhantom, agua, "Phantom_log");

  // Phantom centrado, ligeramente hacia la derecha para dar espacio al haz
  new G4PVPlacement(0,
                    G4ThreeVector(10.0 * cm, 0, 0), // desplazado a la derecha
                    fPhantomLogical, "Phantom_phys", logicWorld, false, 0);

  // ===== SECCION 8: Atributos de Visualizacion =====

  // World: wireframe gris para ver el contorno
  G4VisAttributes *worldVis = new G4VisAttributes(G4Colour(0.5, 0.5, 0.5, 0.3));
  worldVis->SetForceWireframe(true);
  logicWorld->SetVisAttributes(worldVis);

  // Source: verde solido (como tu dibujo)
  G4VisAttributes *sourceVis =
      new G4VisAttributes(G4Colour(0.0, 0.8, 0.0, 0.9));
  sourceVis->SetForceSolid(true);
  logicSource->SetVisAttributes(sourceVis);

  // Phantom: azul/cyan translucido (como tu dibujo)
  G4VisAttributes *phantomVis =
      new G4VisAttributes(G4Colour(0.3, 0.7, 1.0, 0.4));
  phantomVis->SetForceSolid(true);
  fPhantomLogical->SetVisAttributes(phantomVis);

  // ===== SECCION 9: Mensaje informativo =====
  G4cout << "================================================" << G4endl;
  G4cout << " PHANTOM HADRON THERAPY - Geometria:" << G4endl;
  G4cout << " - World: 100x60x60 cm^3 (aire)" << G4endl;
  G4cout << " - Source: cilindro verde (visual)" << G4endl;
  G4cout << " - Phantom: 40x20x20 cm^3 (agua)" << G4endl;
  G4cout << " - Haz: protones 150 MeV en +X" << G4endl;
  G4cout << "================================================" << G4endl;

  return physWorld;
}
