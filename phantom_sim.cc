// ============================================================================
// phantom_sim.cc - Programa Principal
// Simulacion de Hadronterapia: Phantom de Agua con Protones de 150 MeV
// Laboratorio de Fisica Nuclear II
// ============================================================================
// Este es el archivo principal (main) de la simulacion. Aqui se configura
// todo el "cerebro" de Geant4 y se conectan todas las piezas del rompecabezas
// ============================================================================

// ===== SECCION 1: Headers de Geant4 (el motor de simulacion) =====
#include "G4RunManager.hh"   // el cerebro de Geant4, controla todo
#include "G4UIExecutive.hh"  // para la interfaz interactiva
#include "G4UImanager.hh"    // maneja los comandos de la linea de comandos
#include "G4VisExecutive.hh" // para la visualizacion grafica

// ===== SECCION 2: Lista de Fisica =====
// QGSP_BIC es buena para hadronterapia:
// - QGSP: Quark Gluon String Precompound (para particulas de alta energia)
// - BIC: Binary Cascade (cascada binaria, buena para protones < 200 MeV)
// NOTA: Usamos QGSP_BIC en vez de QGSP_BIC_HP para evitar errores de mutex en
// macOS
#include "QGSP_BIC.hh"

// ===== SECCION 3: Nuestras clases (las que vamos a crear nosotros) =====
#include "DetectorConstruction.hh"   // geometria: el phantom y el mundo
#include "PrimaryGeneratorAction.hh" // fuente: el haz de protones
#include "RunAction.hh"              // acciones al inicio/fin del run
#include "SteppingAction.hh"         // registra cada paso de particula

// ============================================================================
// FUNCION PRINCIPAL - Aqui empieza todo
// ============================================================================
int main(int argc, char **argv) {
  // ===== SECCION 4: Detectar modo de ejecucion =====
  // argc = argument count (cuantos argumentos hay)
  // argv = argument vector (los argumentos como tal)
  // Si argc == 1, no hay argumentos -> modo interactivo con GUI
  // Si argc > 1, hay un archivo .mac -> modo batch sin GUI

  G4UIExecutive *ui = nullptr;
  if (argc == 1) {
    // modo interactivo - creamos la interfaz grafica
    ui = new G4UIExecutive(argc, argv);
  }

  // ===== SECCION 5: Crear el RunManager (el cerebro) =====
  // El RunManager es como el director de una pelicula
  // Controla toda la simulacion y coordina las demas clases
  G4RunManager *runManager = new G4RunManager();

  // ===== SECCION 6: Registrar las piezas obligatorias =====
  // Geant4 REQUIERE estas 3 cosas minimo:
  // 1. Geometria (detectores, mundo, phantom)
  // 2. Fisica (que interacciones simulamos)
  // 3. Generador de particulas (que disparamos)

  // Geometria - nuestro phantom de agua y el mundo de aire
  runManager->SetUserInitialization(new DetectorConstruction());

  // Fisica - QGSP_BIC para hadronterapia (sin HP para evitar errores mutex)
  runManager->SetUserInitialization(new QGSP_BIC());

  // ===== SECCION 7: Acciones de usuario (opcionales pero utiles) =====
  // Estas clases nos permiten "enganchar" codigo en distintos momentos

  // RunAction - se ejecuta al inicio y final de cada run
  // Aqui creamos y cerramos el archivo ROOT
  RunAction *runAction = new RunAction();
  runManager->SetUserAction(runAction);

  // PrimaryGenerator - genera los protones de 150 MeV
  runManager->SetUserAction(new PrimaryGeneratorAction());

  // SteppingAction - se ejecuta en cada paso de cada particula
  // Aqui registramos la deposicion de energia para la dosis
  runManager->SetUserAction(new SteppingAction(runAction));

  // ===== SECCION 8: Inicializar Geant4 =====
  // Esto construye la geometria y prepara todo
  runManager->Initialize();

  // ===== SECCION 9: Configurar visualizacion =====
  G4VisManager *visManager = new G4VisExecutive();
  visManager->Initialize();

  // ===== SECCION 10: Obtener el UIManager para comandos =====
  G4UImanager *UImanager = G4UImanager::GetUIpointer();

  // ===== SECCION 11: Ejecutar segun el modo =====
  if (ui) {
    // Modo interactivo - abrimos la GUI
    UImanager->ApplyCommand("/control/execute vis.mac");
    ui->SessionStart();
    delete ui;
  } else {
    // Modo batch - ejecutamos el archivo .mac pasado como argumento
    // argv[1] es el primer argumento despues del nombre del programa
    G4String command = "/control/execute ";
    G4String fileName = argv[1];
    UImanager->ApplyCommand(command + fileName);
  }

  // ===== SECCION 12: Limpieza =====
  // Liberamos memoria antes de salir
  delete visManager;
  delete runManager;

  G4cout << "======================================" << G4endl;
  G4cout << "  Simulacion completada con exito!   " << G4endl;
  G4cout << "  Revisa el archivo ROOT de salida   " << G4endl;
  G4cout << "======================================" << G4endl;

  return 0; // 0 significa que todo salio bien
}
