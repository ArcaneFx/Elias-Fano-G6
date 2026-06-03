#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <fstream>
#include <sstream>
#include <algorithm>
//casos
#include "Caso1.hpp"
#include "Caso2.hpp"
#include "Caso3.hpp"

void ejecutarBenchmark() {
    std::cout << "Iniciando modo Benchmark Automatizado..." << std::endl;
    std::ofstream archivoCsv("metricas_resultados.csv");
    
    // Encabezado del archivo CSV 
    archivoCsv << "Tamano,Caso,TiempoConstruccion_ms,TiempoBusqueda_us\n";

    // Escalas: 10^6, 10^7 y 10^8
    std::vector<size_t> tamanos_prueba = {1000000, 10000000, 100000000}; 
    int32_t epsilon = 5;
    size_t salto_b = 4; 

    long long dummy_sum = 0;

    for (size_t tamano : tamanos_prueba) {
        std::cout << "\nProcesando tamano n = " << tamano << std::endl;

        Caso1Explicito caso1;
        Caso2GapCoding caso2;
        Caso3EliasFano caso3;

        std::vector<int32_t> A_lineal;
        
        // Caso1 //
        std::cout << " -> Corriendo Caso 1..." << std::endl;
        auto t0 = std::chrono::high_resolution_clock::now();
        caso1.generarLineal(A_lineal, tamano, epsilon);
        auto t1 = std::chrono::high_resolution_clock::now();
        double t_const_c1 = std::chrono::duration<double, std::milli>(t1 - t0).count();

        std::vector<int32_t> targets(10000);
        for(size_t i = 0; i < 10000; ++i) {
            targets[i] = A_lineal[rand() % tamano];
        }
        t0 = std::chrono::high_resolution_clock::now();
        for(int32_t t : targets) {
            dummy_sum += caso1.busquedaBinaria(A_lineal, t);
        }
        t1 = std::chrono::high_resolution_clock::now();
        auto microsegundos_totales_c1 = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        double t_busq_c1 = static_cast<double>(microsegundos_totales_c1) / 10000.0;
        archivoCsv << tamano << ",Caso1," << t_const_c1 << "," << t_busq_c1 << "\n";


        // Caso2 //
        std::cout << " -> Corriendo Caso 2..." << std::endl;
        std::vector<int32_t> A_copia;
        caso1.generarLineal(A_copia, tamano, epsilon);
        std::vector<int32_t> A_respaldo = A_copia; 

        t0 = std::chrono::high_resolution_clock::now();
        caso2.build(A_copia, salto_b); 
        t1 = std::chrono::high_resolution_clock::now();
        double t_const_c2 = std::chrono::duration<double, std::milli>(t1 - t0).count();

        t0 = std::chrono::high_resolution_clock::now();
        for(int32_t t : targets) {
            dummy_sum += caso2.search(t);
        }
        t1 = std::chrono::high_resolution_clock::now();
        auto microsegundos_totales_c2 = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        double t_busq_c2 = static_cast<double>(microsegundos_totales_c2) / 10000.0;
        archivoCsv << tamano << ",Caso2," << t_const_c2 << "," << t_busq_c2 << "\n";


        // Caso3 //
        std::cout << " -> Corriendo Caso 3..." << std::endl;
        t0 = std::chrono::high_resolution_clock::now();
        caso3.build(A_respaldo); 
        t1 = std::chrono::high_resolution_clock::now();
        double t_const_c3 = std::chrono::duration<double, std::milli>(t1 - t0).count();

        t0 = std::chrono::high_resolution_clock::now();
        for(int32_t t : targets) {
            const auto& sample = caso2.getSample();
            auto it = std::upper_bound(sample.begin(), sample.end(), t);
            size_t idx = std::distance(sample.begin(), it);
            size_t sample_start = (idx > 0) ? (idx - 1) : 0;
            
            dummy_sum += caso3.searchInRange(caso2.getL(sample_start), caso2.getR(sample_start), t);
        }
        t1 = std::chrono::high_resolution_clock::now();
        auto microsegundos_totales_c3 = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();
        double t_busq_c3 = static_cast<double>(microsegundos_totales_c3) / 10000.0;
        archivoCsv << tamano << ",Caso3," << t_const_c3 << "," << t_busq_c3 << "\n";
    }

    std::cout << "\n[OK] Todas las escalas procesadas correctamente. (Checksum: " << dummy_sum << ")" << std::endl;
    std::cout << "Resultados consolidados en 'metricas_resultados.csv'." << std::endl;
    archivoCsv.close();
}

void ejecutarArchivo(const std::string& ruta) {
    std::ifstream archivo(ruta);
    if (!archivo.is_open()) {
        std::cerr << "Error: No se pudo abrir el archivo CSV especificado." << std::endl;
        return;
    }

    std::vector<int32_t> datos;
    std::string linea, valor;
    if (std::getline(archivo, linea)) {
        std::stringstream ss(linea);
        while (std::getline(ss, valor, ',')) {
            datos.push_back(std::stoi(valor));
        }
    }
    std::sort(datos.begin(), datos.end()); 

    Caso3EliasFano eF;
    eF.build(datos);

    std::cout << "Estructura interactiva cargada correctamente." << std::endl;
    std::cout << "Ingrese el entero a buscar (o -1 para salir):" << std::endl;
    
    int32_t target;
    while (std::cin >> target && target != -1) {
        auto t0 = std::chrono::high_resolution_clock::now();
        int64_t pos = eF.searchInRange(0, datos.size() - 1, target);
        auto t1 = std::chrono::high_resolution_clock::now();
        auto duracion = std::chrono::duration_cast<std::chrono::microseconds>(t1 - t0).count();

        if (pos != -1) {
            std::cout << " -> Encontrado en el índice: " << pos << " | Tiempo de respuesta: " << duracion << " us" << std::endl;
        } else {
            std::cout << " -> Elemento no encontrado en la estructura." << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Error de argumentos. Modos válidos:\n  ./main --benchmark\n  ./main -i <ruta_archivo.csv>" << std::endl;
        return 1;
    }

    std::string modo = argv[1];
    if (modo == "--benchmark") {
        ejecutarBenchmark();
    } else if (modo == "-i" && argc == 3) {
        ejecutarArchivo(argv[2]);
    } else {
        std::cerr << "Modo inválido o faltan argumentos en la línea de comando." << std::endl;
        return 1;
    }
    return 0;
}
