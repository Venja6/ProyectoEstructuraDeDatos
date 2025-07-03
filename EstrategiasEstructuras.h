#pragma once

#include "IEstrategiaBusqueda.h"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>

class BusquedaFMIndex : public IEstrategiaBusqueda {
private:
    std::string texto_original;
    std::string bwt;
    
    // Se utilizan tablas para almacenar la información del FM-Index, antes el codigo no podia realizar nada y se quedaba en un bucle infinito, ahora se corrige con estas tablas
    std::vector<int> C_table;
    std::vector<std::vector<int>> Occ_table;
    
    static const int ALPHABET_SIZE = 256;
    int checkpoint_interval = 64;

    void construir(const std::string& texto);
    int get_occ(int c, int index) const;

    void mostrarProgreso(double porcentaje, const std::string& etapa);  // Se agrego la capacidad de mostrar progreso para saber si había algun problema en la ejecucion 

public:
    void inicializar(const std::string& texto_entrada) override;
    int contarOcurrencias(const std::string& patron) override;
    std::string nombre() const override;
};



class BusquedaSuffixArray : public IEstrategiaBusqueda {
private:
    std::string texto;
    std::vector<int> sa; // Arreglo de sufijos
    std::vector<int> rank; // Array de rangos para construcción
    
    void mostrarProgreso(double porcentaje, const std::string& etapa);
    
public:
    void inicializar(const std::string& texto_entrada) override;
    int contarOcurrencias(const std::string& patron) override;
    std::string nombre() const override;
};