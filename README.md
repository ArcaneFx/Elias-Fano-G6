# INFO145: Diseño y Análisis de Algoritmos
## Proyecto Semestral: Técnicas de Representación y Compresión en Arreglos Ordenados 

**Grupo 6: Codificación Elias-Fano** 

### 👥 Integrantes:
* Benjamín Parra
* Jason Cárdenas
* Hans Benicke

---

Este repositorio contiene la implementación y el marco de experimentación para evaluar el trade-off de espacio-tiempo al representar arreglos ordenados de gran magnitud utilizando tres enfoques distintos: Representación Explícita (Línea Base), Gap-Coding (con Índices de Muestreo) y Compresión Cuasi-Sucinta mediante Codificación Elias-Fano

---



### Instrucciones de Compilación 
- Estando dentro de la carpeta ELIAS-FANO-G6 se debe ejecutar el comando 'make' para compilar.
- Se debe ejecutar el comando './main --benchmark' para que se cree el archivo 'metricas_resultados.csv'.
- Que hace el comando?
    * Genera los datos: Crea arreglos gigantes de 1, 10 y 100 millones de enteros aleatorios ordenados.
    * Construye las estructuras: Procesa los datos en memoria para la Línea Base (Caso 1), Gap-Coding con muestreo (Caso 2) y la compresión por división de bits de Elias-Fano (Caso 3).
    * Mide el rendimiento: Realiza 10,000 búsquedas automáticas en cada caso para registrar promedios exactos de tiempo y el espacio utilizado.
    * Exporta los resultados: Guarda de forma automática todas las métricas de construcción y búsqueda en el archivo '.csv' final.
-Para poder ver los datos generados se debe ocupar el comando 'cat metricas_resultados.csv'


