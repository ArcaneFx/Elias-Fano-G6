#ifndef CASO3_ELIASFANO_HPP
#define CASO3_ELIASFANO_HPP

#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>

class Caso3EliasFano {
private:
    std::vector<uint8_t> bits_bajos;  
    std::vector<uint32_t> bits_altos; 
    size_t k;                         
    size_t total_elements;

public:
    // Constructor 
    Caso3EliasFano() : k(0), total_elements(0) {}

    void build(const std::vector<int32_t>& A) {
        total_elements = A.size();
        if (total_elements == 0) return;
        k = static_cast<size_t>(std::floor(std::log2(total_elements)));
        if (k == 0) k = 1; 

        bits_bajos.resize(total_elements);
        bits_altos.resize(total_elements);

        uint32_t mascara_baja = (1 << k) - 1;

        for (size_t i = 0; i < total_elements; ++i) {
            bits_bajos[i] = A[i] & mascara_baja; 
            bits_altos[i] = A[i] >> k;           
        }
    }
    
    int32_t decompress_value(size_t i) const {
        uint32_t alto = bits_altos[i];
        uint8_t bajo = bits_bajos[i];
        return (alto << k) | bajo;
    }

    int64_t searchInRange(size_t L, size_t R, int32_t target) {
        for (size_t i = L; i <= R; ++i) {
            int32_t value = decompress_value(i);
            if (value == target) return i;
            if (value > target) break; 
        }
        return -1;
    }
}; 

#endif 
