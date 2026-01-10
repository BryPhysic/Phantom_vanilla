// ============================================================================
// SteppingAction.hh - Acciones en Cada Paso de Particula
// Aqui registramos la deposicion de energia en el phantom
// ============================================================================

#ifndef STEPPING_ACTION_HH
#define STEPPING_ACTION_HH

#include "G4UserSteppingAction.hh" // clase base

// forward declaration
class RunAction;

// ============================================================================
// CLASE SteppingAction
// Se ejecuta en CADA paso de CADA particula durante la simulacion
// Aqui es donde capturamos la fisica: cuanta energia se deposita y donde
// ============================================================================
class SteppingAction : public G4UserSteppingAction {
public:
  // Constructor recibe puntero a RunAction para llenar histogramas
  SteppingAction(RunAction *runAction);
  virtual ~SteppingAction();

  // ===== METODO OBLIGATORIO =====
  // UserSteppingAction() se llama en cada paso de cada particula
  virtual void UserSteppingAction(const G4Step *step);

private:
  // puntero a RunAction para poder llenar los histogramas
  RunAction *fRunAction;
};

#endif // STEPPING_ACTION_HH
