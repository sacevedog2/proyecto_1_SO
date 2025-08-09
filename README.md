**Descripción**
Este proyecto implementa un simulador de planificación de procesos con el algoritmo Round Robin, utilizando un quantum configurable para cada proceso.
Los procesos se cargan desde un archivo de texto (procesos.txt), y sus instrucciones se leen desde archivos separados (PID.txt).

El simulador:

	•	Lee y parsea procesos con sus registros iniciales (AX, BX, CX) y quantum.
 
	•	Carga las instrucciones asociadas a cada proceso.
 
	•	Ejecuta las instrucciones interpretando operaciones como ADD, SUB, MUL, INC, JMP y NOP.
 
	•	Maneja cambios de contexto, estados de proceso y detección de bucles infinitos.


 **Formato de Archivos**
 Cada línea define un proceso con el siguiente formato:
PID: <número>, AX=<valor>, BX=<valor>, CX=<valor>, Quantum=<valor>

**Ejecución**
Compilar:

	•	gcc main.c -o simulador

Ejecutar:

	•	./simulador -f procesos.txt
