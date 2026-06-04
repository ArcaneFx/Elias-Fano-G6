# INFO145 — Proyecto Semestral: Técnicas de Representación y Compresión en Arreglos Ordenados

Algoritmo de compresión asignado al grupo: **Elias-Fano** (Caso 3).

El programa implementa y compara tres representaciones de un arreglo ordenado:

- **Caso 1 — Representación explícita** (`Caso1.hpp`): arreglo de enteros + búsqueda binaria. Línea base.
- **Caso 2 — Gap-Coding** (`Caso2.hpp`): diferencias entre elementos consecutivos + índice de muestreo (*sample*) para acotar el rango de búsqueda.
- **Caso 3 — Elias-Fano** (`Caso3.hpp`): codificación cuasi-sucinta sobre los **valores** del arreglo. La parte baja (k bits) se empaqueta de forma contigua y la parte alta se codifica en **unario**. Usa el *sample* del Caso 2 para acotar `[L, R]`.

## Compilación

```bash
make
```

Compila con `g++ -O3 -Wall -std=c++17` y genera el ejecutable `main`. Para limpiar:

```bash
make clean
```

## Modos de ejecución

### 1. Benchmark automático

```bash
./main --benchmark
```

Genera los arreglos (distribución **lineal** y **normal** con distintas desviaciones estándar), construye las tres estructuras y mide, para cada caso y tamaño:

- tiempo de construcción (ms),
- tiempo de búsqueda promedio en µs (sobre `10000` búsquedas, no una sola),
- espacio utilizado (bytes) y bits por elemento.

**Salida:** archivo `metricas_resultados.csv` con las columnas:

```
Distribucion,Parametro,Tamano,Caso,TiempoConstruccion_ms,TiempoBusqueda_us,Espacio_bytes,BitsPorElemento
```

Estos datos son la base de los gráficos del Hito 2.

> Las escalas (`10^6, 10^7, 10^8`), el `epsilon` de la distribución lineal, las `sigmas` de la normal y el salto del *sample* (`salto_b`) están parametrizados al inicio de `ejecutarBenchmark()` en `main.cpp`. La escala `10^8` usa varios GB de RAM; coméntela si su equipo no la soporta.

### 2. Modo archivo (interactivo)

```bash
./main -i ruta/del/archivo.csv
```

Lee un archivo `.csv` con enteros (separados por comas y/o saltos de línea), ordena los datos y construye las tres estructuras. Luego, de forma interactiva, el usuario elige **en qué estructura** buscar (`1` = explícito, `2` = Gap-Coding, `3` = Elias-Fano) y el valor a buscar. El programa indica si se encontró, en qué posición y el tiempo de búsqueda. Se ingresa `-1` como estructura para salir.

## Tipo y rango de datos

El programa trabaja con **`int32_t`** (enteros de 32 bits). Los valores deben ser **no negativos** (rango `0` … `2147483647`), ya que Gap-Coding y Elias-Fano asumen un arreglo ordenado ascendente de valores `>= 0`. La lectura de enteros se hace con `std::stol` (equivalente a `atol`).

## Nota sobre Elias-Fano

La parte baja se empaqueta **a nivel de bits** (exactamente `k = floor(log2(n))` bits por elemento) sobre palabras `uint64_t`, y la parte alta se almacena en un bitvector unario. El acceso aleatorio al i-ésimo valor usa `select_1` sobre la parte alta, acelerado con un muestreo. Gracias a esto, el espacio por elemento (~`k + 2` bits) es estrictamente menor que los 32 bits de la representación explícita.

## Verificación de memoria

El código no presenta fugas de memoria. Puede verificarse con:

```bash
make
printf "3\n12345\n-1\n" | valgrind --leak-check=full ./main -i ruta/archivo.csv
```
