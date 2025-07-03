#include "EstrategiasAlgoritmicas.h"
#include <algorithm>
#include <vector>

// ============== IMPLEMENTACION DE BOYER-MOORE ==============
void BusquedaBoyerMoore::inicializar(const std::string& texto) {
    this->texto_a_buscar = texto;
}

std::string BusquedaBoyerMoore::nombre() const {
    return "Boyer-Moore";
}

void BusquedaBoyerMoore::heuristicaCaracterErroneo(const std::string& patron, int tamano, int tablaCaracterErroneo[NUM_CARACTERES]) {
    for (int i = 0; i < NUM_CARACTERES; i++)
        tablaCaracterErroneo[i] = -1;
    for (int i = 0; i < tamano; i++)
        tablaCaracterErroneo[(int)patron[i]] = i;
}

int BusquedaBoyerMoore::contarOcurrencias(const std::string& patron) {
    int m = patron.size();
    int n = this->texto_a_buscar.size();
    if (m == 0 || m > n) return 0;

    int n_coincidencias_totales = 0;
    int tablaCaracterErroneo[NUM_CARACTERES];
    heuristicaCaracterErroneo(patron, m, tablaCaracterErroneo);

    int s = 0; // s es el desplazamiento
    while (s <= (n - m)) {
        int j = m - 1;
        while (j >= 0 && patron[j] == this->texto_a_buscar[s + j])
            j--;

        if (j < 0) { // Coincidencia total
            n_coincidencias_totales++;
            
            // En lugar de una regla de desplazamiento compleja tras una coincidencia,
            // usamos un incremento para garantizar el avance y evitar bucles, esto se hizo por prblemas de ejecucion y rendimiento del algoritmo, bajo harto los tiempos de ejecucion en mi pc xd
            s += 1;
        } else { 
            s += std::max(1, j - tablaCaracterErroneo[this->texto_a_buscar[s + j]]);
        }
    }
    return n_coincidencias_totales;
}


// ============== IMPLEMENTACIÓN DE KMP ==============
void BusquedaKMP::inicializar(const std::string& texto) {
    this->texto_a_buscar = texto;
}

std::string BusquedaKMP::nombre() const {
    return "Knuth-Morris-Pratt";
}

void BusquedaKMP::construirTablaLPS(const std::string& patron, std::vector<int>& lps) {
    int longitud = 0;
    lps[0] = 0;
    int i = 1;
    while (i < patron.length()) {
        if (patron[i] == patron[longitud]) {
            longitud++;
            lps[i] = longitud;
            i++;
        } else {
            if (longitud != 0) {
                longitud = lps[longitud - 1];
            } else {
                lps[i] = 0;
                i++;
            }
        }
    }
}

int BusquedaKMP::contarOcurrencias(const std::string& patron) {
    int n = this->texto_a_buscar.length();
    int m = patron.length();
    if (m == 0 || m > n) return 0;
    
    std::vector<int> lps(m);
    int n_coincidencias_totales = 0;
    construirTablaLPS(patron, lps);
    
    int i = 0; //Texto
    int j = 0; //Patron
    while (i < n) {
        if (this->texto_a_buscar[i] == patron[j]) {
            i++;
            j++;
            if (j == m) {
                n_coincidencias_totales++;
                j = lps[j - 1];
            }
        } else {
            if (j != 0)
                j = lps[j - 1];
            else
                i++;
        }
    }
    return n_coincidencias_totales;
}

// ============== IMPLEMENTACION DE RABIN-KARP ==============
void BusquedaRabinKarp::inicializar(const std::string& texto) {
    this->texto_a_buscar = texto;
}

std::string BusquedaRabinKarp::nombre() const {
    return "Rabin-Karp";
}

int BusquedaRabinKarp::contarOcurrencias(const std::string& patron) {
    int q = 1e9 + 7; // Un numero primo random grande para evitar colisiones
    int M = patron.length();
    int N = this->texto_a_buscar.length();
    if (M == 0 || M > N) return 0;

    int p = 0; // Valor hash para el patrón
    int t = 0; // Valor hash para la ventana de texto
    int h = 1;
    int n_coincidencias_totales = 0;

    for (int i = 0; i < M - 1; i++)
        h = (h * NUM_CARACTERES) % q;
    for (int i = 0; i < M; i++) {
        p = (NUM_CARACTERES * p + patron[i]) % q;
        t = (NUM_CARACTERES * t + this->texto_a_buscar[i]) % q;
    }

    for (int i = 0; i <= N - M; i++) {
        if (p == t) {
            bool coincidencia = true;
            for (int j = 0; j < M; j++) {
                if (this->texto_a_buscar[i + j] != patron[j]) {
                    coincidencia = false;
                    break;
                }
            }
            if (coincidencia)
                n_coincidencias_totales++;
        }
        if (i < N - M) {
            t = (NUM_CARACTERES * (t - this->texto_a_buscar[i] * h) + this->texto_a_buscar[i + M]) % q;
            if (t < 0)
                t += q;
        }
    }
    return n_coincidencias_totales;
}
