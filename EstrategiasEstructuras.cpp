#include "EstrategiasEstructuras.h"
#include <numeric>
#include <algorithm>
#include <iomanip>

std::string BusquedaFMIndex::nombre() const {
    return "FM-Index";
}

void BusquedaFMIndex::mostrarProgreso(double porcentaje, const std::string& etapa) {
    int barWidth = 50;
    std::cout << etapa << " [";
    int pos = barWidth * porcentaje;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << porcentaje * 100.0 << "%\r";
    std::cout.flush();
}

void BusquedaFMIndex::inicializar(const std::string& texto_entrada) {
    bwt.clear();
    C_table.clear();
    Occ_table.clear();
    this->texto_original = texto_entrada + "$";
    construir(this->texto_original);
}

void BusquedaFMIndex::construir(const std::string& texto) {
    int n = texto.length();
    std::cout << "Inicializando FM-Index para texto de " << n << " caracteres..." << std::endl;

    //Suffix Array (Manber-Myers)
    std::vector<int> sa(n);
    for (int i = 0; i < n; i++) sa[i] = i;
    
    std::vector<int> rank(texto.begin(), texto.end());
    
    // Calcular número de iteraciones para mostrar progreso
    int num_iterations = 0;
    for (int k = 1; k < n; k *= 2) num_iterations++;
    
    int current_iteration = 0;
    for (int k = 1; k < n; k *= 2, current_iteration++) {
        // Mostrar progreso de SA
        double progress = static_cast<double>(current_iteration) / num_iterations;
        mostrarProgreso(progress, "Construyendo Suffix Array");
        
        auto cmp = [&](int i, int j) {
            if (rank[i] != rank[j]) return rank[i] < rank[j];
            int ri = (i + k < n) ? rank[i + k] : -1;
            int rj = (j + k < n) ? rank[j + k] : -1;
            return ri < rj;
        };
        std::sort(sa.begin(), sa.end(), cmp);
        
        std::vector<int> new_rank(n);
        int r = 0;
        new_rank[sa[0]] = r;
        for (int i = 1; i < n; i++) {
            r += cmp(sa[i-1], sa[i]) ? 1 : 0;
            new_rank[sa[i]] = r;
        }
        rank = new_rank;
    }
    mostrarProgreso(1.0, "Construyendo Suffix Array");
    std::cout << std::endl;

    // Construir BWT
    std::cout << "Generando BWT..." << std::endl;
    bwt.resize(n);
    for (int i = 0; i < n; i++) {
        bwt[i] = (sa[i] == 0) ? texto[n - 1] : texto[sa[i] - 1];
    }

    // Corregido : Tabla C optimizada porque se estaba calculando mal y se demoraba mucho
    std::cout << "Calculando tabla C..." << std::endl;
    std::vector<int> freq(ALPHABET_SIZE, 0);
    for (char c : bwt) {
        freq[(unsigned char)c]++;
    }
    
    C_table.resize(ALPHABET_SIZE);
    C_table[0] = 0;
    for (int i = 1; i < ALPHABET_SIZE; i++) {
        C_table[i] = C_table[i-1] + freq[i-1];
    }

    // Tabla Occ optimizada con progreso
    std::cout << "Construyendo tabla Occ..." << std::endl;
    int num_checkpoints = (n + checkpoint_interval - 1) / checkpoint_interval;
    Occ_table = std::vector<std::vector<int>>(ALPHABET_SIZE, std::vector<int>(num_checkpoints, 0));
    
    std::vector<int> current_count(ALPHABET_SIZE, 0);
    for (int i = 0; i < n; i++) {
        // Mostrar progreso cada 0.5% de avance
        if (i % (n / 200) == 0) {
            double progress = static_cast<double>(i) / n;
            mostrarProgreso(progress, "Construyendo tabla Occ");
        }
        
        unsigned char c = bwt[i];
        current_count[c]++;
        if ((i + 1) % checkpoint_interval == 0) {
            for (int j = 0; j < ALPHABET_SIZE; j++) {
                Occ_table[j][i / checkpoint_interval] = current_count[j];
            }
        }
    }
    mostrarProgreso(1.0, "Construyendo tabla Occ");
    std::cout << "\nFM-Index construido exitosamente!" << std::endl;
}

// get_occ optimizada para evitar bucles infinitos y mejorar rendimiento, esta parte la tuve que gepetiar porque el codigo no me funcionaba y era por esto
int BusquedaFMIndex::get_occ(int c, int index) const {
    if (index < 0) return 0;
    
    int checkpoint_idx = index / checkpoint_interval;
    int count = (checkpoint_idx > 0) ? Occ_table[c][checkpoint_idx - 1] : 0;
    int start = checkpoint_idx * checkpoint_interval;
    
    for (int i = start; i <= index; i++) {
        if ((unsigned char)bwt[i] == c) {
            count++;
        }
    }
    return count;
}

int BusquedaFMIndex::contarOcurrencias(const std::string& patron) {

    if (patron.empty()) return 0;
    
    int low = 0;
    int high = bwt.size();
    
    for (int i = patron.size() - 1; i >= 0; i--) {
        unsigned char c = patron[i];
        if (c >= ALPHABET_SIZE) return 0;  
        
        low = C_table[c] + get_occ(c, low - 1);
        high = C_table[c] + get_occ(c, high - 1);
        
        if (low >= high) return 0;
    }
    
    return high - low;
}



// --- SUFFIX ARRAY ---- 
void BusquedaSuffixArray::mostrarProgreso(double porcentaje, const std::string& etapa) {
    int barWidth = 50;
    std::cout << etapa << " [";
    int pos = barWidth * porcentaje;
    for (int i = 0; i < barWidth; ++i) {
        if (i < pos) std::cout << "=";
        else if (i == pos) std::cout << ">";
        else std::cout << " ";
    }
    std::cout << "] " << std::fixed << std::setprecision(1) << porcentaje * 100.0 << "%\r";
    std::cout.flush();
}

std::string BusquedaSuffixArray::nombre() const {
    return "Suffix-Array";
}

void BusquedaSuffixArray::inicializar(const std::string& texto_entrada) {
    texto = texto_entrada + "$";
    int n = texto.length();
    std::cout << "Inicializando Suffix Array para texto de " << n << " caracteres..." << std::endl;
    
    // Inicializar SA y rank
    sa.resize(n);
    rank.resize(n);
    for (int i = 0; i < n; i++) {
        sa[i] = i;
        rank[i] = texto[i];
    }
    
    // Calcular iteraciones para mostrar progreso
    int num_iterations = 0;
    for (int k = 1; k < n; k *= 2) num_iterations++;
    
    int current_iteration = 0;
    for (int k = 1; k < n; k *= 2, current_iteration++) {
        // Mostrar progreso
        double progress = static_cast<double>(current_iteration) / num_iterations;
        mostrarProgreso(progress, "Construyendo Suffix Array");
        
        // Función de comparacion para ordenar los sufijos
        auto cmp = [&](int i, int j) {
            if (rank[i] != rank[j]) return rank[i] < rank[j];
            int ri = (i + k < n) ? rank[i + k] : -1;
            int rj = (j + k < n) ? rank[j + k] : -1;
            return ri < rj;
        };
        
        // Ordenar SA
        std::sort(sa.begin(), sa.end(), cmp);
        
        // Actualizar rank
        std::vector<int> new_rank(n);
        int r = 0;
        new_rank[sa[0]] = r;
        for (int i = 1; i < n; i++) {
            r += cmp(sa[i-1], sa[i]) ? 1 : 0;
            new_rank[sa[i]] = r;
        }
        rank = new_rank;
    }
    mostrarProgreso(1.0, "Construyendo Suffix Array");
    std::cout << "\nSuffix Array construido exitosamente!" << std::endl;
}

int BusquedaSuffixArray::contarOcurrencias(const std::string& patron) {

    if (patron.empty()) return 0;
    
    int n = texto.length();
    int m = patron.length();
    
    // Función para comparar un sufijo con el patrón
    auto compare = [&](int idx) -> int {
        for (int j = 0; j < m; j++) {
            if (idx + j >= n) return -1; // Sufijo más corto
            if (texto[idx + j] < patron[j]) return -1;
            if (texto[idx + j] > patron[j]) return 1;
        }
        return 0;
    };
    
    // Encontrar primera ocurrencia
    int low = 0, high = n - 1;
    int first = n; // indice de la primera ocurrencia
    
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int res = compare(sa[mid]);
        
        if (res >= 0) {
            first = mid;
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }
    
    // Si no pillo ninguna ocurrencia
    if (first == n || compare(sa[first]) != 0) {
        return 0;
    }
    
    // Encontrar ultima ocurrencia
    low = 0;
    high = n - 1;
    int last = first;
    
    while (low <= high) {
        int mid = low + (high - low) / 2;
        int res = compare(sa[mid]);
        
        if (res <= 0) {
            last = mid;
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }    
    return last - first + 1;
}

