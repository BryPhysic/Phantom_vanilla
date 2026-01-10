# 🔬 Phantom Hadron Therapy Simulation

Simulación Monte Carlo de protonterapia utilizando **Geant4** para calcular distribuciones de dosis en un phantom de agua.

---

## 📋 Descripción

Este proyecto simula un haz de protones de **150 MeV** impactando un **phantom de agua**, típico de estudios de hadronterapia/protonterapia. La simulación registra datos crudos a nivel de *step* para análisis posterior en ROOT.

### Objetivos
- Calcular la **Curva de Bragg** (distribución longitudinal de dosis)
- Obtener **perfiles transversales** de dosis (Y, Z)
- Estudiar **partículas secundarias** y fragmentación nuclear
- Analizar **procesos físicos** de interacción

---

## 🏗️ Geometría

```
                           ← 100 cm →
    ┌────────────────────────────────────────────────────────┐
    │                        WORLD                            │ ↕ 60 cm
    │     ████           ┌──────────────────────┐            │
    │     ████ ──────────│                      │            │
    │     ████   Beam    │      PHANTOM         │            │
    │    Source  150 MeV │      Agua            │            │
    │                    │    40×20×20 cm³      │            │
    │                    └──────────────────────┘            │
    │          ─────────────────────────────────────> +X     │
    └────────────────────────────────────────────────────────┘
```

| Componente | Material | Dimensiones | Posición (X,Y,Z) |
|------------|----------|-------------|------------------|
| World | Aire (G4_AIR) | 100×60×60 cm³ | Centro |
| Source | Aluminio | Cilindro Ø4×2 cm | (-41.5, 0, 0) cm |
| Phantom | Agua (G4_WATER) | 40×20×20 cm³ | (+10, 0, 0) cm |
| Haz | Protones 150 MeV | Puntual | (-40, 0, 0) cm → +X |

---

## 📁 Estructura del Proyecto

```
Phantom_01/
├── phantom_sim.cc           # Main: inicializa Geant4 y conecta componentes
├── CMakeLists.txt           # Configuración de compilación
│
├── include/
│   ├── DetectorConstruction.hh
│   ├── PrimaryGeneratorAction.hh
│   ├── RunAction.hh
│   └── SteppingAction.hh
│
├── src/
│   ├── DetectorConstruction.cc    # Geometría: World + Source + Phantom
│   ├── PrimaryGeneratorAction.cc  # Haz de protones 150 MeV
│   ├── RunAction.cc               # Crea/cierra archivo ROOT
│   └── SteppingAction.cc          # Registra cada step → TTree
│
├── macros/
│   ├── run.mac             # Modo batch (sin GUI)
│   └── vis.mac             # Modo interactivo (con GUI)
│
└── build/
    ├── phantom_sim         # Ejecutable
    ├── raw_output.root     # Datos de salida
    └── *.png               # Gráficos generados
```

---

## ⚙️ Compilación y Ejecución

### Requisitos
- Geant4 (>= 10.7) con visualización
- ROOT (>= 6.0)
- CMake (>= 3.10)

### Compilar

```bash
cd Phantom_01
mkdir -p build && cd build
cmake ..
make -j4
```

### Ejecutar

#### Modo Interactivo (GUI)
```bash
./phantom_sim
```
Luego en la consola de Geant4:
```
/run/beamOn 1000
```

#### Modo Batch (sin GUI)
```bash
./phantom_sim run.mac
```

---

## 📊 Datos de Salida

### Archivo ROOT: `raw_output.root`

Contiene un TTree llamado `raw_data` con información completa de cada *step*:

| Branch | Tipo | Descripción | Unidades |
|--------|------|-------------|----------|
| `eventID` | Int | ID del evento | - |
| `trackID` | Int | ID del track | - |
| `parentID` | Int | ID del padre (0 = primaria) | - |
| `particleName` | Char[32] | Nombre de la partícula | - |
| `pdgCode` | Int | Código PDG | - |
| `x_pre`, `y_pre`, `z_pre` | Double | Posición inicial del step | cm |
| `x_post`, `y_post`, `z_post` | Double | Posición final del step | cm |
| `edep` | Double | Energía depositada | MeV |
| `kinE_pre`, `kinE_post` | Double | Energía cinética antes/después | MeV |
| `stepLength` | Double | Longitud del step | mm |
| `processName` | Char[32] | Proceso físico | - |
| `volumeName` | Char[32] | Volumen donde ocurrió | - |

---

## 🔬 Análisis Posibles

Con los datos crudos puedes realizar:

### 1. Distribución de Dosis

| Análisis | Descripción | Variables a usar |
|----------|-------------|------------------|
| **Curva de Bragg** | Dosis vs profundidad | `x_pre`, `edep` |
| **Perfil Y** | Dosis transversal | `y_pre`, `edep` (en X del pico) |
| **Perfil Z** | Dosis transversal | `z_pre`, `edep` (en X del pico) |
| **Mapa 2D XY** | Distribución espacial | `x_pre`, `y_pre`, `edep` |
| **Mapa 2D XZ** | Distribución espacial | `x_pre`, `z_pre`, `edep` |

### 2. Análisis de Partículas

| Análisis | Descripción | Variables a usar |
|----------|-------------|------------------|
| **Espectro de secundarias** | Tipos de partículas | `particleName`, `pdgCode` |
| **Fragmentación nuclear** | Productos de reacción | `parentID`, `particleName` |
| **Tracks de primarias** | Trayectoria del protón | `trackID=1`, posiciones |

### 3. Análisis de Procesos

| Análisis | Descripción | Variables a usar |
|----------|-------------|------------------|
| **Procesos principales** | Qué física domina | `processName` |
| **Ionización vs Nuclear** | Contribución a dosis | `processName`, `edep` |
| **Scattering** | Dispersión lateral | `processName`, posiciones |

### 4. Análisis Avanzados

| Análisis | Descripción |
|----------|-------------|
| **LET** | dE/dx = `edep` / `stepLength` |
| **Range** | Posición máxima de protones primarios |
| **Straggling** | Distribución estadística del range |
| **Penumbra** | Bordes de la distribución transversal |

---

## 📈 Ejemplo Rápido en ROOT

```cpp
// Abrir archivo
TFile *f = new TFile("raw_output.root");
TTree *t = (TTree*)f->Get("raw_data");

// Curva de Bragg (solo en phantom, solo protones)
t->Draw("x_pre:edep>>h_bragg(100,-10,30)", 
        "volumeName==\"Phantom_phys\" && particleName==\"proton\"",
        "prof");
```

---

## 🎯 Física Utilizada

**QGSP_BIC_HP** - Optimizada para hadronterapia:
- **QGSP**: Quark Gluon String Precompound (alta energía)
- **BIC**: Binary Cascade (protones < 200 MeV)
- **HP**: High Precision (neutrones de baja energía)

---

## 📚 Referencias

- [Geant4 Physics Reference Manual](https://geant4.web.cern.ch/collaboration/working_groups/physics/manual)
- [QGSP_BIC_HP Documentation](https://geant4.kek.jp/Reference/PhysicsReferenceManual/physicsLists.html)
- Particle Therapy Physics, edited by Harald Paganetti

---

## 👨‍🔬 Autor

Proyecto de Simulación - Laboratorio de Física Nuclear II

---

## 📄 Licencia

**© 2026 BryPhysic - Todos los derechos reservados**

Uso personal y académico permitido. Uso comercial prohibido. Ver [LICENSE](LICENSE) para detalles.
