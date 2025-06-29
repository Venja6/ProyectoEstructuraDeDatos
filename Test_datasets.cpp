#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <bits/stdc++.h>
#include <ctime>
#include <chrono>
#include <dirent.h>
using namespace std;
#define NO_OF_CHARS 256

// A cada letra posible se le asigna el valor -1, excepto por las presentes en el patron, ya que a estas se les asigna la ultima posicion de cada caracter,
// esto para realizar los saltos/desplazamientos
void badCharHeuristic(string str, int size, int badchar[NO_OF_CHARS]){ // badchar es una arreglo de 256 posiciones, uno para cada caracter ASCII
    int i;

    // Todas las letras a -1
    for (i = 0; i < NO_OF_CHARS; i++)
        badchar[i] = -1;

    // Se llena el valor de los saltos dependiendo de la ultima ocurrencia de la letra
    for (i = 0; i < size; i++)
        badchar[(int)str[i]] = i; // Transforma cada caracter en el patrón a su codigo ASCII y guarda la posición de ese caracter en el patrón (i) en el arreglo badchar en la posición correspondiente a su código ASCII ((int)str[i])
    // si más adelante hay otro carácter igual, su posición se sobreescribe. La idea es conservar la última posicíon de cada caracter en el patrón.
}

/* Algoritmo de Boyer Moore */
int searchBM(string pat, string txt){
    int m = pat.size();
    int n = txt.size();
    int n_coincidencias_totales = 0;
    int badchar[NO_OF_CHARS];

    // Se guardan las ÚLTIMAS posiciones de cada caracter del patrón en el array de BadChar usados para los desplazamientos
    badCharHeuristic(pat, m, badchar);

    int s = 0; // s es el salto o desplazamiento a realizar (posición en el texto en donde va el patrón actualmente), es calculado despues de comparar caracteres
    while (s <= (n - m))
    {                  // El algoritmo finaliza cuando el patrón no puede "desplazarse" más (no necesariamente cuando llega al final del texto)
        int j = m - 1; // última posición del patrón (se reinicia cada vez que hay una coincidencia total o una discordancia)

        // Compara de derecha a izquierda,. Termina si hay una discordancia (pat[j] == txt[s + j] no se cumple) o si revisa todas los carácteres y no encuentra discordancia, en tal caso j queda como -1
        while (j >= 0 && pat[j] == txt[s + j])
            j--;

        if (j < 0)
        { // Coincidencia total
            n_coincidencias_totales++;
            s += (s + m < n) ? m - badchar[txt[s + m]] : 1; // si existe un caracter siguiente en el texto (s + m < n) entonces,
            // se desplaza hasta que coincida ese caracter con el último caracter de ese tipo en el patron o en el caso de que ese caracter no exista en el patrón, se desplaza hasta saltarlo.
            // si no existe un caracter siguiente en el texto, entonces salta 1 para terminar el ciclo.
        }

        else
            // Llega aqui si un caracter no coincide al patron pero si está en él, realiza el desplazamiento para que coincida con el último en el patrón
            s += max(1, j - badchar[txt[s + j]]); // Si el caracter no existe en el patrón (badchar[txt[s + j]] = -1), se desplaza hasta pasar este caracter
    }
    return n_coincidencias_totales;
}

void constructLps(string &pat, vector<int> &lps){
    // len stores the length of longest prefix which
    // is also a suffix for the previous index
    int len = 0;
    // lps[0] siempre es 0
    lps[0] = 0;
    int i = 1;
    while (i < pat.length())
    {
        // Si los caracteres comienzan a formar un patron se aumenta el valor
        if (pat[i] == pat[len])
        {
            len++;
            lps[i] = len;
            i++;
        }
        // Si Se pierde el patron
        else
        {
            if (len != 0)
            {
                // Update len to the previous lps value
                // to avoid reduntant comparisons
                len = lps[len - 1];
            }
            else
            {
                // Como no existe un prefijo, el valor vuelve a 0
                lps[i] = 0;
                i++;
            }
        }
    }
}

int searchKMP(string &pat, string &txt){
    int n = txt.length();
    int m = pat.length();

    vector<int> lps(m);
    int n_coincidencias_totales = 0;

    constructLps(pat, lps);

    // Punteros al texto y al patron para comparar las letras
    int i = 0;
    int j = 0;

    while (i < n)
    {

        // Si la comparacion es correcta, ambos puteros se mueven
        if (txt[i] == pat[j])
        {
            i++;
            j++;

            // Si se encuentra el patron, se guarda el punto donde se encontro
            if (j == m)
            {
                n_coincidencias_totales++;

                // Usa el valor del lps en el indice anterior para mover el inicio y evitar comparaciones innecesarios
                j = lps[j - 1];
            }
        }

        // Si ocurre una comparacion erronea
        else
        {

            //  Usa el valor del lps en el indice anterior para mover el inicio y evitar comparaciones innecesarios
            if (j != 0)
                j = lps[j - 1];
            else
                i++;
        }
    }
    return n_coincidencias_totales;
}


int searchRK(const string &pat, const string &txt){

    // Numero primo para favorecer a la tabla hash
    int q = 1e9 + 7;
    // Largo de los elementos
    int M = pat.length();
    int N = txt.length();
    // Hash para el patron
    int p = 0;
    // Hash para el texto siendo leido
    int t = 0;
    // Multiplicador
    int h = 1;

    int n_coincidencias_totales = 0;

    // Valor de h
    for (int i = 0; i < M - 1; i++)
        h = (h * NO_OF_CHARS) % q;
    // Valores hash iniciales para el patron y el texto
    for (int i = 0; i < M; i++)
    {
        p = (NO_OF_CHARS * p + pat[i]) % q;
        t = (NO_OF_CHARS * t + txt[i]) % q;
    }

    // Movimiento del patron por el texto (Y comparacion)
    for (int i = 0; i <= N - M; i++)
    {
        //Si los valores hash son correctos, revisa letra a letra el texto
        if (p == t)
        {
            bool match = true;
            for (int j = 0; j < M; j++)
            {
                if (txt[i + j] != pat[j])
                {
                    match = false;
                    break;
                }
            }
            if (match)
                n_coincidencias_totales++;
        }

        //Valor delm hash para la siguiente seccion 
        if (i < N - M)
        {
            t = (NO_OF_CHARS * (t - txt[i] * h) + txt[i + M]) % q;
            if (t < 0)
                t += q;
        }
    }
    return n_coincidencias_totales;
}
/*
BoyerMoore: tipo_algoritmo = 0
Knuth-Morris-Pratt: tipo_algoritmo = 1
RabinKarp: tipo_algoritmo = 2
*/
int x_search(string txt, string pat, int tipo_algoritmo){
    if (tipo_algoritmo == 0) // BoyerMoore
    {
        return searchBM(pat, txt);
    }
    if (tipo_algoritmo == 1) // Knuth-Morris-Pratt
    {
        return searchKMP(pat, txt);
    }
    if (tipo_algoritmo == 2) // RabinKarp
    {
        return searchRK(pat, txt);
    }
    else
    {
        std::cout << "Algoritmo no encontrado" << std::endl;
        return -1;
    }
}

const std::vector<std::string> nombres_algoritmos = {
    "BoyerMoore",
    "Knuth-Morris-Pratt", "RabinKarp"
};

int main() {
    vector<double> duracionesBM, duracionesKMP, duracionesRK;
    chrono::high_resolution_clock::time_point start, end;
    chrono::duration<double> duracion;
    double promedio_patrones;
    int cantidad_patrones = 0;
    int toString;
    std::string nombre;

    vector<string> text_files;
    string pattern_file = "pattern.txt";

    vector<string> patterns;
    ifstream patfile(pattern_file);
    if (!patfile.is_open()) {
        cerr << "Error opening pattern file.\n";
        return 1;
    }
    
    string pat;
    while (std::getline(patfile, pat)) {
        patterns.push_back(pat);
        cantidad_patrones++;
    }
    patfile.close();

    string texto_completo;
    for (int i = 0; i <= 39; i++) {
        ifstream file("dna_" + to_string(i));
        if (!file.is_open()) {
            cerr << "Error al abrir dna_" << i << "\n";
            continue;
        }
        string linea;
        while (getline(file, linea)) {  
            texto_completo += linea + "\n";
        }
        file.close();
    }
    
    for (int rep = 0; rep < 20; rep++) {
        for (int algoritmo = 0; algoritmo <= 2; algoritmo++) {
            for (const auto& pat : patterns) {
                start = chrono::high_resolution_clock::now();
                x_search(texto_completo, pat, algoritmo);
                end = chrono::high_resolution_clock::now();
                chrono::duration<double> duracion = end - start;
                
                switch (algoritmo) {
                    case 0: duracionesBM.push_back(duracion.count()); break;
                    case 1: duracionesKMP.push_back(duracion.count()); break;
                    case 2: duracionesRK.push_back(duracion.count()); break;
                }
            }
        }
        
        double totalBM = 0.0, totalKMP = 0.0, totalRK = 0.0;
        for (double t : duracionesBM) totalBM += t;
        for (double t : duracionesKMP) totalKMP += t;
        for (double t : duracionesRK) totalRK += t;
        cout <<"Boyer-Moore;"<< totalBM << endl;
        cout <<"KMP;"<< totalKMP << endl;
        cout <<"RK;"<< totalRK << endl;
        duracionesBM.clear();
        duracionesKMP.clear();
        duracionesRK.clear();
    }

    /*double totalfind;
    for(int i = 0; i < 100; i++){
        start = chrono::high_resolution_clock::now();
        texto_completo.find(patterns[i]);
        end = chrono::high_resolution_clock::now();
        double findD = chrono::duration<double>(end - start).count();
        totalfind += findD;
    }
    cout << "find de c" << totalfind;*/

    return 0;
}
