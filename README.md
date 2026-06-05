# 📊 Proyecto Semestral: Representación y Compresión en Arreglos Ordenados

## 👥 Grupo 6 — Algoritmo Asignado: Elias-Fano
* **Benjamín Parra**
* **Jason Cárdenas**
* **Hans Benicke**

---

## 🎯 ¿De qué trata este proyecto?
El objetivo del programa es crear una lista gigante de números ordenados y comparar tres formas distintas de guardarla en la computadora para ver cuál ahorra más memoria y cuál busca más rápido:

1. **Caso 1 (Línea Base):** Guarda los números normales (sin comprimir) y busca usando la clásica búsqueda binaria.
2. **Caso 2 (Gap-Coding):** Guarda solamente las diferencias (restas) entre un número y el anterior para gastar menos espacio. Usa una pequeña guía de muestras (*Sample*) para saber por dónde buscar.
3. **Caso 3 (Elias-Fano):** Nuestro algoritmo especial. Separa los números en bits "bajos" (que empaqueta de forma ultra apretada) y bits "altos" (en código unario). ¡Ahorra muchísima memoria!

---

## 🛠️ Compilación y Ejecución (Cómo usar el programa)

1. **Compilar:** Se tiene que escribir `make` mientras se está parado en la carpeta del proyecto. Esto creará el archivo ejecutable llamado `main`.
2. **Ejecutar:** Se debe ingresar el comando `./main --benchmark`. 

### 🖥️ ¿Qué verás en la terminal al ejecutarlo?
Mientras el programa corre, verás texto en tiempo real indicando el avance del experimento paso a paso:
* El mensaje de inicio: `Iniciando modo Benchmark Automatizado...`.
* El progreso de los tamaños procesados (1, 10 y 100 millones de datos).
* La distribución actual en evaluación (Lineal o Normal) junto con un código de verificación (`checksum`).
* El mensaje final de éxito: `[OK] Resultados consolidados en 'metricas_resultados.csv'.`

### 📊 ¿Dónde quedan los resultados?
Al finalizar, todos los tiempos de construcción (en milisegundos), tiempos de búsqueda (en microsegundos), el espacio en bytes y los bits netos por elemento quedarán guardados de forma automática en el archivo **`metricas_resultados.csv`** dentro de la misma carpeta. 

