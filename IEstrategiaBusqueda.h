#pragma once // once para evitar inclusiones multiples, que quiere decir que este archivo solo se incluira una vez por compilacion  

#include <string>
#include <vector>

class IEstrategiaBusqueda {
public:
    virtual ~IEstrategiaBusqueda() = default;

    virtual void inicializar(const std::string& texto) = 0;

    virtual int contarOcurrencias(const std::string& patron) = 0;

    virtual std::string nombre() const = 0;
};