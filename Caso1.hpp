#ifndef CASO1_EXPLICITO_HPP
#define CASO1_EXPLICITO_HPP

#include <vector>
#include <random>
#include <algorithm>
#include <cstdint>
#include <cstdlib>

class Caso1Explicito {
public:

    // Distribucion lineal: A[0] aleatorio, A[i] = A[i-1] + rand()%epsilon.
    void generarLineal(std::vector<int32_t>& A, size_t n, int32_t epsilon) {
        if (epsilon < 1) epsilon = 1;           
        A.resize(n);
        if (n == 0) return;
        A[0] = rand() % 100;
        for (size_t i = 1; i < n; ++i) {
            A[i] = A[i - 1] + (rand() % epsilon);
        }
    }

    void generarNormal(std::vector<int32_t>& A, size_t n, double media, double desv_estandar) {
        A.resize(n);
        if (n == 0) return;
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> d(media, desv_estandar);

        for (size_t i = 0; i < n; ++i) {
            A[i] = static_cast<int32_t>(d(gen));
        }
        std::sort(A.begin(), A.end());

        // Desplaza para que el minimo sea 0 (sin romper el orden).
        if (A[0] < 0) {
            int32_t offset = -A[0];
            for (size_t i = 0; i < n; ++i) A[i] += offset;
        }
    }

    // Busqueda binaria 
    int64_t busquedaBinaria(const std::vector<int32_t>& A, int32_t target) {
        auto it = std::lower_bound(A.begin(), A.end(), target);
        if (it != A.end() && *it == target) {
            return std::distance(A.begin(), it);
        }
        return -1;
    }
};

#endif
