# ============================================================================
# CMakeLists.txt para Simulacion de Phantom de Agua con Protones
# Proyecto: Hadronterapia - Laboratorio de Fisica Nuclear II
# Autor: Yo xd briancito ...
# Fecha: Enero 2026
# ============================================================================
# Nota: Este proyecto simula la irradaicion de un phantom de agua con 
#       protones de 150 MeV para estudiar la distribucion de dosis (Bragg)
# ============================================================================

cmake_minimum_required(VERSION 3.16...3.27)  # version minima de CMake

project(phantom_sim)  # nombre del projecto 

# ===== SECCION 1: Busqueda de paquetes necesarios =====
# Geant4 es el motor de simulacion de Monte Carlo
# ui_all: todas las interfaces de usuario
# vis_all: todas las herramientas de visualizacion 
find_package(Geant4 REQUIRED ui_all vis_all)

# ROOT es para el analisis y guardar histogramas de dosis
# Buscamos ROOT con los componentes que necesitamos
find_package(ROOT REQUIRED COMPONENTS Core RIO Hist Tree)

# ===== SECCION 2: Configuracion de Geant4 =====
# Esto carga todas las variables y configuraciones de Geant4
include(${Geant4_USE_FILE})

# ===== SECCION 3: Directorios de headers =====
# le decimos al compilador donde buscar nuestros archivos .hh
include_directories(${PROJECT_SOURCE_DIR}/include)
# tambien los headers de ROOT
include_directories(${ROOT_INCLUDE_DIRS})

# ===== SECCION 4: Archivos fuente =====
# buscamos todos los .cc en src/ y todos los .hh en include/
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

# ===== SECCION 5: Macros de ejecucion =====
# copiamos los archivos .mac a la carpeta build para ejecutar
file(GLOB macros ${PROJECT_SOURCE_DIR}/macros/*.mac)
foreach(macro ${macros})
    get_filename_component(macro_name ${macro} NAME)
    configure_file(${macro} ${PROJECT_BINARY_DIR}/${macro_name} COPYONLY)
endforeach()

# ===== SECCION 6: Creacion del ejecutable =====
# phantom_sim sera nuestro ejecutable final
add_executable(phantom_sim phantom_sim.cc ${sources} ${headers})

# ===== SECCION 7: Librerias a enlazar =====
# enlazamos con Geant4 y ROOT
target_link_libraries(phantom_sim ${Geant4_LIBRARIES} ${ROOT_LIBRARIES})
