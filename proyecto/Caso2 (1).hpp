#ifndef CASO2_GAPCODING_HPP
#define CASO2_GAPCODING_HPP

#include <vector>
#include <algorithm>
#include <cstdint>

class Caso2GapCoding {
private:
    std::vector<int32_t> GC;     // gaps: GC[0]=A[0], GC[i]=A[i]-A[i-1]
    std::vector<int32_t> Sample; // muestreo de valores cada b posiciones
    size_t b;                    // salto del muestreo
    size_t total_elements;

public:
    void build(std::vector<int32_t>& A, size_t salto_b) {
        b = salto_b;
        total_elements = A.size();
        if (total_elements == 0) return;
        GC.resize(total_elements);
        GC[0] = A[0];
        for (size_t i = 1; i < total_elements; ++i) {
            GC[i] = A[i] - A[i - 1];
        }

        for (size_t i = 0; i < total_elements; i += b) {
            Sample.push_back(A[i]);
        }

        // El arreglo original se vuelve inaccesible.
        A.clear();
        A.shrink_to_fit();
    }

    int64_t search(int32_t target) {
        if (Sample.empty()) return -1;
        auto it = std::upper_bound(Sample.begin(), Sample.end(), target);
        size_t sample_idx = std::distance(Sample.begin(), it);

        size_t sample_start = (sample_idx > 0) ? (sample_idx - 1) : 0;
        size_t L = sample_start * b;
        size_t R = std::min(L + b, total_elements - 1);

        int32_t current_value = Sample[sample_start];
        if (current_value == target) return L;
        for (size_t i = L + 1; i <= R; ++i) {
            current_value += GC[i];
            if (current_value == target) return i;
            if (current_value > target) break;
        }
        return -1;
    }

    size_t getL(size_t sample_start) const { return sample_start * b; }
    size_t getR(size_t sample_start) const {
        return std::min((sample_start + 1) * b, total_elements - 1);
    }
    const std::vector<int32_t>& getSample() const { return Sample; }

    // Espacio del arreglo de gaps + el sample (en bytes).
    size_t sizeInBytes() const {
        return GC.size() * sizeof(int32_t) + Sample.size() * sizeof(int32_t);
    }
};

#endif
