// --- NOMBRE DE ARCHIVO: EstrategiasAlgoritmicas.h ---
#pragma once

#include "IEstrategiaBusqueda.h"


// --- Boyer-Moore ---
class BusquedaBoyerMoore : public IEstrategiaBusqueda {
private:
    std::string texto_a_buscar;
    static const int NUM_CARACTERES = 256;
    void heuristicaCaracterErroneo(const std::string& patron, int tamano, int tablaCaracterErroneo[NUM_CARACTERES]);

public:
    void inicializar(const std::string& texto) override;
    int contarOcurrencias(const std::string& patron) override;
    std::string nombre() const override;
};



// --- Knuth-Morris-Pratt ---
class BusquedaKMP : public IEstrategiaBusqueda {
private:
    std::string texto_a_buscar;
    void construirTablaLPS(const std::string& patron, std::vector<int>& lps);

public:
    void inicializar(const std::string& texto) override;
    int contarOcurrencias(const std::string& patron) override;
    std::string nombre() const override;
};



// --- Rabin-Karp ---
class BusquedaRabinKarp : public IEstrategiaBusqueda {
private:
    std::string texto_a_buscar;
    static const int NUM_CARACTERES = 256;

public:
    void inicializar(const std::string& texto) override;
    int contarOcurrencias(const std::string& patron) override;
    std::string nombre() const override;
};