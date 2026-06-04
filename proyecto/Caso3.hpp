#ifndef CASO3_ELIASFANO_HPP
#define CASO3_ELIASFANO_HPP

#include <vector>
#include <cmath>
#include <cstdint>
#include <algorithm>

/*
 * Caso 3: Elias-Fano sobre los VALORES del arreglo ordenado.
 *
 * A diferencia de los demas algoritmos del proyecto, Elias-Fano opera sobre los
 * valores originales (monotonos crecientes) y no sobre los gaps. Cada valor v se
 * separa en:
 *   - parte baja: los k bits menos significativos, empaquetados de forma contigua
 *                 a exactamente k bits por elemento (sobre palabras uint64_t).
 *   - parte alta: el cociente (v >> k), codificado en UNARIO en un bitvector.
 *
 * Con esto, el espacio por elemento es ~ k + 2 bits (mucho menor que 32 bits).
 * El acceso aleatorio al i-esimo valor usa una operacion select_1 sobre el
 * bitvector de la parte alta (con muestreo para que sea rapida).
 */
class Caso3EliasFano {
private:
    std::vector<uint64_t> low_words;       // parte baja empaquetada (k bits por elemento)
    std::vector<uint64_t> high_bits;       // bitvector unario de la parte alta
    std::vector<uint64_t> select_samples;  // posicion del 1-bit cada SAMPLE para acelerar select
    size_t k;                              // bits de la parte baja
    size_t n;                              // cantidad de elementos
    size_t high_len;                       // largo del bitvector alto (en bits)

    static const size_t SAMPLE = 256;      // se guarda 1 muestra cada 256 unos

    // Lee los k bits de la parte baja del elemento idx.
    inline uint64_t read_low(size_t idx) const {
        size_t pos  = idx * k;
        size_t word = pos >> 6;
        size_t off  = pos & 63;
        uint64_t res = low_words[word] >> off;
        if (off + k > 64) {                       // el codigo cruza el limite de palabra
            res |= low_words[word + 1] << (64 - off);
        }
        return res & ((uint64_t(1) << k) - 1);
    }

    // Escribe los k bits de la parte baja del elemento idx.
    inline void write_low(size_t idx, uint64_t val) {
        size_t pos  = idx * k;
        size_t word = pos >> 6;
        size_t off  = pos & 63;
        low_words[word] |= (val << off);
        if (off + k > 64) {
            low_words[word + 1] |= (val >> (64 - off));
        }
    }

    inline void set_high(size_t pos) {
        high_bits[pos >> 6] |= (uint64_t(1) << (pos & 63));
    }

    // Posicion (0-indexada) del i-esimo bit en 1 del bitvector alto.
    inline size_t select1(size_t i) const {
        size_t s   = i / SAMPLE;
        size_t pos = select_samples[s];   // posicion del (s*SAMPLE)-esimo uno
        size_t rem = i - s * SAMPLE;      // unos que faltan saltar desde pos
        while (true) {
            if (high_bits[pos >> 6] & (uint64_t(1) << (pos & 63))) {
                if (rem == 0) return pos;
                --rem;
            }
            ++pos;
        }
    }

public:
    Caso3EliasFano() : k(0), n(0), high_len(0) {}

    // Construye la estructura a partir del arreglo ORDENADO de valores (>= 0).
    void build(const std::vector<int32_t>& A) {
        low_words.clear();
        high_bits.clear();
        select_samples.clear();
        n = A.size();
        if (n == 0) { k = 0; high_len = 0; return; }

        // k = floor(log2(n)) segun el enunciado.
        k = static_cast<size_t>(std::floor(std::log2(static_cast<double>(n))));
        if (k == 0) k = 1;

        // Universo: el mayor valor (A esta ordenado ascendente y se asume >= 0).
        uint64_t U = static_cast<uint64_t>(A[n - 1] > 0 ? A[n - 1] : 0);
        size_t num_buckets = static_cast<size_t>(U >> k) + 1;
        high_len = n + num_buckets;             // n unos + (num_buckets) ceros aprox.

        low_words.assign((n * k + 63) / 64 + 1, 0ULL);
        high_bits.assign((high_len + 63) / 64 + 1, 0ULL);

        uint64_t low_mask = (uint64_t(1) << k) - 1;
        for (size_t i = 0; i < n; ++i) {
            uint64_t v  = static_cast<uint64_t>(A[i] > 0 ? A[i] : 0);
            uint64_t lo = v & low_mask;
            uint64_t hi = v >> k;
            write_low(i, lo);
            set_high(hi + i);                   // el i-esimo uno cae en la posicion hi+i
        }

        // Muestreo para select: posicion de cada SAMPLE-esimo uno.
        size_t ones = 0;
        for (size_t pos = 0; pos < high_len; ++pos) {
            if (high_bits[pos >> 6] & (uint64_t(1) << (pos & 63))) {
                if (ones % SAMPLE == 0) select_samples.push_back(pos);
                ++ones;
            }
        }
    }

    // Reconstruye el valor original en la posicion i.
    int32_t decompress_value(size_t i) const {
        uint64_t lo = read_low(i);
        uint64_t hi = select1(i) - i;           // valor de la parte alta
        return static_cast<int32_t>((hi << k) | lo);
    }

    // Busca target decodificando secuencialmente el rango [L, R] (acotado por el sample).
    int64_t searchInRange(size_t L, size_t R, int32_t target) const {
        if (n == 0) return -1;
        if (R >= n) R = n - 1;
        for (size_t i = L; i <= R; ++i) {
            int32_t value = decompress_value(i);
            if (value == target) return static_cast<int64_t>(i);
            if (value > target) break;          // ordenado: ya nos pasamos
        }
        return -1;
    }

    // Espacio total ocupado por la estructura comprimida (en bytes).
    size_t sizeInBytes() const {
        return low_words.size()      * sizeof(uint64_t)
             + high_bits.size()      * sizeof(uint64_t)
             + select_samples.size() * sizeof(uint64_t);
    }

    size_t getN() const { return n; }
    size_t getK() const { return k; }
};

#endif
