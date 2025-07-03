#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <memory>
#include <numeric>
#include <iomanip> 
#include "IEstrategiaBusqueda.h"
#include "EstrategiasAlgoritmicas.h"
#include "EstrategiasEstructuras.h"

// --- CARGAR TEXTO ---
std::string cargar_texto_desde_archivos(int num_documentos) {
    std::string texto_completo;
    const std::string separador = "$"; 
    for (int i = 0; i < num_documentos; ++i) {
        std::ifstream archivo("dna_" + std::to_string(i));
        if (!archivo.is_open()) {
            std::cerr << "Error al abrir dna_" << i << "\n";
            continue;
        }
        std::string contenido((std::istreambuf_iterator<char>(archivo)), std::istreambuf_iterator<char>());
        texto_completo += contenido + separador;
        archivo.close();
    }
    return texto_completo;
}

// --- CARGAR PATRONES ---
std::vector<std::string> cargar_patrones(const std::string& nombre_archivo_patrones) {
    std::vector<std::string> patrones;
    std::ifstream archivo(nombre_archivo_patrones);
    if (!archivo.is_open()) {
        std::cerr << "Error abriendo el archivo de patrones.\n";
        return patrones;
    }
    std::string patron;
    while (std::getline(archivo, patron)) {
        patrones.push_back(patron);
    }
    archivo.close();
    return patrones;
}

int main() {
    // --- PARAMETROS ---
    const int NUM_REPETICIONES = 20; 
    const int DOCS_A_PROBAR = 40;    
    const std::string ARCHIVO_PATRONES = "pattern.txt";
    
    // Estrategias Habilitadas
    std::vector<bool> estrategias_habilitadas = {
        true,   // Boyer-Moore (0)
        false,   // Knuth-Morris-Pratt (1)
        false,   // Rabin-Karp (2)
        false,   // FM-Index (3)
        true    // Suffix Array (4)
    };

    // --- Preparacion del archivo de resultados CSV ---
    std::ofstream archivo_resultados("resultados_experimento.csv");
    archivo_resultados << "Estrategia,Num_Documentos,Tamano_Texto_Caracteres,Num_Patrones,Tiempo_Construccion_Promedio_ms,Tiempo_Busqueda_Promedio_ms,Memoria_Bytes\n";
   
    // --- 1. Carga de Datos ---
    std::cout << "Cargando " << DOCS_A_PROBAR << " documentos..." << std::endl;
    std::string texto_completo = cargar_texto_desde_archivos(DOCS_A_PROBAR);
    std::vector<std::string> patrones = cargar_patrones(ARCHIVO_PATRONES);
    std::cout << "Texto cargado (" << texto_completo.length() << " caracteres), "
              << patrones.size() << " patrones cargados." << std::endl;

    // --- 2. Creacion de Estrategias habilitadas ---
    std::vector<std::unique_ptr<IEstrategiaBusqueda>> estrategias;
    
    // Solo creamos las estrategias que estan habilitadas
    if (estrategias_habilitadas[0]) estrategias.push_back(std::make_unique<BusquedaBoyerMoore>());
    if (estrategias_habilitadas[1]) estrategias.push_back(std::make_unique<BusquedaKMP>());
    if (estrategias_habilitadas[2]) estrategias.push_back(std::make_unique<BusquedaRabinKarp>());
    if (estrategias_habilitadas[3]) estrategias.push_back(std::make_unique<BusquedaFMIndex>());
    if (estrategias_habilitadas[4]) estrategias.push_back(std::make_unique<BusquedaSuffixArray>());

    // --- 3. Ejecucion del Experimento ---
    for (const auto& estrategia : estrategias) {
        std::cout << "\n--- Evaluando Estrategia: " << estrategia->nombre() << " ---" << std::endl;
        std::vector<double> tiempos_construccion;
        std::vector<double> tiempos_busqueda;
        size_t memoria_utilizada = 0;

        for (int i = 0; i < NUM_REPETICIONES; ++i) {
            auto inicio_construccion = std::chrono::high_resolution_clock::now();
            estrategia->inicializar(texto_completo);
            auto fin_construccion = std::chrono::high_resolution_clock::now();
            
            
            std::chrono::duration<double> duracion_construccion = fin_construccion - inicio_construccion;
            tiempos_construccion.push_back(duracion_construccion.count());

            auto inicio_busqueda = std::chrono::high_resolution_clock::now();
            for (const auto& pat : patrones) {
                estrategia->contarOcurrencias(pat);
            }
            auto fin_busqueda = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duracion_busqueda = fin_busqueda - inicio_busqueda;
            tiempos_busqueda.push_back(duracion_busqueda.count());
        }

        // Calculo de promedios
        double prom_construccion = std::accumulate(tiempos_construccion.begin(), tiempos_construccion.end(), 0.0) / NUM_REPETICIONES;
        double prom_busqueda = std::accumulate(tiempos_busqueda.begin(), tiempos_busqueda.end(), 0.0) / NUM_REPETICIONES;

        // Reporte a la consola
        std::cout << "Tiempo de Construccion/Inicializacion Promedio: " 
                  << std::fixed << std::setprecision(2) << prom_construccion * 1000.0 << " ms" << std::endl;
        std::cout << "Tiempo de Busqueda Promedio (para " << patrones.size() << " patrones): " 
                  << std::fixed << std::setprecision(2) << prom_busqueda * 1000.0 << " ms" << std::endl;

        // Reporte al archivo CSV
        archivo_resultados << estrategia->nombre() << ","
                           << DOCS_A_PROBAR << ","
                           << texto_completo.length() << ","
                           << patrones.size() << ","
                           << prom_construccion * 1000.0 << ","
                           << prom_busqueda * 1000.0 << ","
                           << memoria_utilizada << "\n";
    }
    
    archivo_resultados.close();
    std::cout << "\nResultados del experimento guardados en 'resultados_experimento.csv'" << std::endl;

    return 0;
}