#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdint>
// Casos
#include "Caso1.hpp"
#include "Caso2.hpp"
#include "Caso3.hpp"

using Reloj = std::chrono::high_resolution_clock;
static inline double ms(Reloj::time_point a, Reloj::time_point b) {
    return std::chrono::duration<double, std::milli>(b - a).count();
}
static inline double us(Reloj::time_point a, Reloj::time_point b) {
    return std::chrono::duration<double, std::micro>(b - a).count();
}

// Cantidad de busquedas por medicion (no basta con una sola).
static const size_t NUM_QUERIES = 10000;

// Procesa una escala: genera el arreglo segun la distribucion, construye los tres
// casos y mide tiempo de construccion, tiempo de busqueda y espacio de cada uno.
void procesarEscala(std::ofstream& csv, const std::string& dist, double parametro,
                    size_t n, size_t salto_b) {
    Caso1Explicito caso1;
    Caso2GapCoding  caso2;
    Caso3EliasFano  caso3;

    // --- Construccion Caso 1 (la generacion ES la construccion explicita) ---
    std::vector<int32_t> A;
    auto t0 = Reloj::now();
    if (dist == "lineal")
        caso1.generarLineal(A, n, static_cast<int32_t>(parametro));
    else
        caso1.generarNormal(A, n, parametro * 3.0, parametro); // media = 3*sigma
    auto t1 = Reloj::now();
    double tc1 = ms(t0, t1);

    // Targets: valores que SI existen, tomados del mismo arreglo. Iguales para
    // los tres casos (comparacion justa).
    std::vector<int32_t> targets(NUM_QUERIES);
    for (size_t i = 0; i < NUM_QUERIES; ++i) targets[i] = A[rand() % n];

    long long checksum = 0;

    // --- Busqueda Caso 1 ---
    t0 = Reloj::now();
    for (int32_t t : targets) checksum += caso1.busquedaBinaria(A, t);
    t1 = Reloj::now();
    double tb1 = us(t0, t1) / NUM_QUERIES;
    size_t esp1 = n * sizeof(int32_t);
    csv << dist << "," << parametro << "," << n << ",Caso1,"
        << tc1 << "," << tb1 << "," << esp1 << "," << (esp1 * 8.0 / n) << "\n";

    // --- Caso 2 (consume una copia; A sigue intacto) ---
    std::vector<int32_t> A2 = A;
    t0 = Reloj::now();
    caso2.build(A2, salto_b);
    t1 = Reloj::now();
    double tc2 = ms(t0, t1);

    t0 = Reloj::now();
    for (int32_t t : targets) checksum += caso2.search(t);
    t1 = Reloj::now();
    double tb2 = us(t0, t1) / NUM_QUERIES;
    size_t esp2 = caso2.sizeInBytes();
    csv << dist << "," << parametro << "," << n << ",Caso2,"
        << tc2 << "," << tb2 << "," << esp2 << "," << (esp2 * 8.0 / n) << "\n";

    // --- Caso 3 (Elias-Fano global; usa el sample del Caso 2 para acotar [L,R]) ---
    std::vector<int32_t> A3 = A;
    t0 = Reloj::now();
    caso3.build(A3);
    t1 = Reloj::now();
    double tc3 = ms(t0, t1);
    A3.clear(); A3.shrink_to_fit();

    const auto& sample = caso2.getSample();
    t0 = Reloj::now();
    for (int32_t t : targets) {
        auto it = std::upper_bound(sample.begin(), sample.end(), t);
        size_t idx = std::distance(sample.begin(), it);
        size_t ss = (idx > 0) ? (idx - 1) : 0;
        checksum += caso3.searchInRange(caso2.getL(ss), caso2.getR(ss), t);
    }
    t1 = Reloj::now();
    double tb3 = us(t0, t1) / NUM_QUERIES;
    size_t esp3 = caso3.sizeInBytes();
    csv << dist << "," << parametro << "," << n << ",Caso3,"
        << tc3 << "," << tb3 << "," << esp3 << "," << (esp3 * 8.0 / n) << "\n";

    std::cout << "    [n=" << n << " " << dist << " param=" << parametro
              << "] checksum=" << checksum << "\n";
}

void ejecutarBenchmark() {
    std::cout << "Iniciando modo Benchmark Automatizado..." << std::endl;
    std::ofstream csv("metricas_resultados.csv");
    csv << "Distribucion,Parametro,Tamano,Caso,"
           "TiempoConstruccion_ms,TiempoBusqueda_us,Espacio_bytes,BitsPorElemento\n";

    // ---- Configuracion del experimento (editable) ----
    // OJO: 10^8 usa varios GB y tarda. Comente la ultima escala si su equipo no la soporta.
    std::vector<size_t> tamanos = {1000000, 10000000, 100000000};
    int32_t epsilon = 5;                              // parametro de la distribucion lineal
    std::vector<double> sigmas = {100000.0, 10000000.0}; // desviaciones estandar para la normal
    size_t salto_b = 64;                              // salto del muestreo (sample)

    for (size_t n : tamanos) {
        std::cout << "\nProcesando tamano n = " << n << std::endl;
        std::cout << " -> Distribucion lineal" << std::endl;
        procesarEscala(csv, "lineal", epsilon, n, salto_b);
        for (double s : sigmas) {
            std::cout << " -> Distribucion normal (sigma=" << s << ")" << std::endl;
            procesarEscala(csv, "normal", s, n, salto_b);
        }
    }

    csv.close();
    std::cout << "\n[OK] Resultados consolidados en 'metricas_resultados.csv'." << std::endl;
}

// Busqueda en Caso 3 reutilizando el sample del Caso 2 para acotar [L,R].
static int64_t buscarCaso3(Caso2GapCoding& c2, Caso3EliasFano& c3, int32_t target) {
    const auto& sample = c2.getSample();
    auto it = std::upper_bound(sample.begin(), sample.end(), target);
    size_t idx = std::distance(sample.begin(), it);
    size_t ss = (idx > 0) ? (idx - 1) : 0;
    return c3.searchInRange(c2.getL(ss), c2.getR(ss), target);
}

void ejecutarArchivo(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cerr << "Error: no se pudo abrir el archivo CSV especificado." << std::endl;
        return;
    }

    // Lee enteros separados por comas y/o saltos de linea (rango: int32 no negativo).
    std::vector<int32_t> datos;
    std::string token;
    std::string contenido((std::istreambuf_iterator<char>(archivo)),
                           std::istreambuf_iterator<char>());
    std::stringstream ss(contenido);
    while (std::getline(ss, token, ',')) {
        std::stringstream linea(token);
        std::string num;
        while (linea >> num) {
            try { datos.push_back(static_cast<int32_t>(std::stol(num))); }
            catch (...) { /* ignora tokens no numericos */ }
        }
    }
    if (datos.empty()) {
        std::cerr << "Error: el archivo no contiene enteros validos." << std::endl;
        return;
    }
    std::sort(datos.begin(), datos.end());

    // Construye las tres estructuras sobre los mismos datos.
    std::vector<int32_t> base = datos;   // Caso 1 (explicito)
    std::vector<int32_t> d2   = datos;   // Caso 2
    std::vector<int32_t> d3   = datos;   // Caso 3

    Caso1Explicito caso1;
    Caso2GapCoding  caso2;
    Caso3EliasFano  caso3;
    caso2.build(d2, 64);
    caso3.build(d3);

    std::cout << "Estructuras cargadas (" << base.size() << " elementos).\n";
    std::cout << "Estructuras disponibles: 1=Explicito  2=Gap-Coding  3=Elias-Fano\n";

    while (true) {
        std::cout << "\nElija estructura (1/2/3) o -1 para salir: ";
        int estructura;
        if (!(std::cin >> estructura)) break;
        if (estructura == -1) break;
        if (estructura < 1 || estructura > 3) {
            std::cout << "Estructura invalida.\n";
            continue;
        }

        std::cout << "Ingrese el entero a buscar: ";
        int32_t target;
        if (!(std::cin >> target)) break;

        auto t0 = Reloj::now();
        int64_t pos = -1;
        if (estructura == 1)      pos = caso1.busquedaBinaria(base, target);
        else if (estructura == 2) pos = caso2.search(target);
        else                      pos = buscarCaso3(caso2, caso3, target);
        auto t1 = Reloj::now();
        double dur = us(t0, t1);

        if (pos != -1)
            std::cout << " -> Encontrado en el indice " << pos
                      << " | Tiempo: " << dur << " us\n";
        else
            std::cout << " -> No encontrado | Tiempo: " << dur << " us\n";
    }
    std::cout << "Saliendo del modo interactivo.\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error de argumentos. Modos validos:\n"
                  << "  ./main --benchmark\n"
                  << "  ./main -i <ruta_archivo.csv>" << std::endl;
        return 1;
    }

    std::string modo = argv[1];
    if (modo == "--benchmark") {
        ejecutarBenchmark();
    } else if (modo == "-i" && argc == 3) {
        ejecutarArchivo(argv[2]);
    } else {
        std::cerr << "Modo invalido o faltan argumentos." << std::endl;
        return 1;
    }
    return 0;
}
