**Informe**
https://eafit-my.sharepoint.com/:w:/r/personal/sacevedog2_eafit_edu_co/_layouts/15/doc2.aspx?sourcedoc=%7BD6562D01-836C-4A21-9FE9-E64284CDFAF1%7D&file=Document.docx&action=editNew&mobileredirect=true&wdOrigin=APPHOME-WEB.DIRECT%2CAPPHOME-WEB.UNAUTH%2CAPPHOME-WEB.BANNER.NEWBLANK&wdPreviousSession=2b62a9eb-830f-4d35-b8f2-a2f6a13c6d16&wdPreviousSessionSrc=AppHomeWeb&ct=1754690829643

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

- `Quantum` por defecto es `1` si no se especifica.
- Los registros por defecto inician en `0`.
- Campos sensibles a mayúsculas: `PID`, `Quantum`, `AX`, `BX`, `CX`.

### Archivos de instrucciones `PID.txt`
Por cada proceso del `procesos.txt` debe existir un archivo `PID.txt` en el mismo directorio.
Ejemplo:
- `1.txt`:
- 
## Conjunto de instrucciones soportadas

- `NOP`: no hace nada.
- `INC REG`: incrementa en 1 el registro (`AX|BX|CX`).
- `JMP n`: salta a la **instrucción n (1-based)**. Valida rango.
- `ADD REG, X` / `SUB REG, X` / `MUL REG, X`  
Donde `X` puede ser **inmediato positivo** (ej. `5`) o **registro** (`AX|BX|CX`).  
La coma es opcional: `ADD AX 5` también funciona.

**Importante**:
- Registros deben ir en **mayúsculas** (`AX`, `BX`, `CX`).
- Inmediatos negativos **no** están soportados en la versión actual.
- Si se usa un comando desconocido, se comportará como un **NOP silencioso**.

## Compilación

Requiere GCC (Linux / Ubuntu). Desde el directorio del proyecto:
```bash
# Compilación recomendada (con warnings)
gcc main.c -o rr -Wall -Wextra -O2

# Alternativa mínima
gcc main.c -o rr

**Ejecución**
Compilar:
	•	gcc main.c -o simulador

Ejecutar:
	•	./simulador -f procesos.txt
