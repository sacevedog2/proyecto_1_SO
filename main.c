#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_PROCESOS 100
#define MAX_INSTRUCCIONES 100

typedef struct {
    int pid;
    int pc; // Contador de programa
    int ax, bx, cx; 
    int quantum;
    char estado[15];
    char instrucciones[MAX_INSTRUCCIONES][50]; 
    int num_instrucciones;
    int codigo_salida;
} Proceso;

// Función para saltarse espacios en blanco
void saltarEspacios(char** ptr) {
    while (**ptr == ' ' || **ptr == '\t') {
        (*ptr)++;
    }
}

// Función para leer un número
int leerNumero(char** ptr) {
    int numero = 0;
    while (isdigit(**ptr)) {
        numero = numero * 10 + (**ptr - '0');
        (*ptr)++;
    }
    return numero;
}

// Función para leer un registro (AX, BX, CX)
void leerRegistro(char** ptr, char* registro) {
    int i = 0;
    while (isalpha(**ptr) && i < 2) {
        registro[i] = **ptr;
        (*ptr)++;
        i++;
    }
    registro[i] = '\0';
}

int* obtenerRegistro(Proceso* p, const char* nombre) {
    if (strcmp(nombre, "AX") == 0) return &p->ax;
    if (strcmp(nombre, "BX") == 0) return &p->bx;
    if (strcmp(nombre, "CX") == 0) return &p->cx;
    return NULL;
}

// Función para cargar instrucciones desde archivo
int cargarInstruccionesDesdeArchivo(Proceso* p) {
    char nombreArchivo[20];
    sprintf(nombreArchivo, "%d.txt", p->pid);
    
    FILE* file = fopen(nombreArchivo, "r");
    if (!file) {
        printf("Error: No se pudo abrir el archivo de instrucciones %s para el proceso %d\n", 
               nombreArchivo, p->pid);
        return 1;
    }
    
    char linea[100];
    p->num_instrucciones = 0;
    
    while (fgets(linea, sizeof(linea), file) && p->num_instrucciones < MAX_INSTRUCCIONES) {
        // Eliminar salto de línea
        linea[strcspn(linea, "\n")] = 0;
        
        // Copiar la instrucción (sin procesarla aquí, se procesará en ejecución)
        strcpy(p->instrucciones[p->num_instrucciones], linea);
        p->num_instrucciones++;
    }
    
    fclose(file);
    return 0;
}

// Función para parsear una línea del archivo procesos.txt
int parsearProceso(char* linea, Proceso* proceso) {
    char* ptr = linea;
    
    // Saltear espacios iniciales
    saltarEspacios(&ptr);
    
    // Verificar "PID:"
    if (strncmp(ptr, "PID:", 4) != 0) {
        return 0; // No es una línea válida de proceso
    }
    ptr += 4;
    
    // Saltear espacios después de PID:
    saltarEspacios(&ptr);
    
    // Leer el número del PID
    if (!isdigit(*ptr)) {
        return 0;
    }
    proceso->pid = leerNumero(&ptr);
    
    // Inicializar registros con valores por defecto
    proceso->ax = 0;
    proceso->bx = 0;
    proceso->cx = 0;
    proceso->quantum = 1; // Valor por defecto
    
    // Procesar el complemento
    while (*ptr != '\0' && *ptr != '\n') {
        saltarEspacios(&ptr);
        
        if (*ptr == ',') {
            ptr++; // Saltar la coma
            saltarEspacios(&ptr);
            
            // Verificar si es Quantum
            if (strncmp(ptr, "Quantum", 7) == 0) {
                ptr += 7;
                saltarEspacios(&ptr);
                if (*ptr == '=') {
                    ptr++;
                    saltarEspacios(&ptr);
                    proceso->quantum = leerNumero(&ptr);
                }
            } 
            // O si es un registro (AX, BX, CX)
            else {
                char registro[3];
                leerRegistro(&ptr, registro);
                
                saltarEspacios(&ptr);
                if (*ptr == '=') {
                    ptr++;
                    saltarEspacios(&ptr);
                    int valor = leerNumero(&ptr);
                    
                    if (strcmp(registro, "AX") == 0) {
                        proceso->ax = valor;
                    } else if (strcmp(registro, "BX") == 0) {
                        proceso->bx = valor;
                    } else if (strcmp(registro, "CX") == 0) {
                        proceso->cx = valor;
                    }
                }
            }
        } else {
            break;
        }
    }
    
    // Inicializar otros campos
    proceso->pc = 0;
    strcpy(proceso->estado, "Listo");
    
    return 1; // Proceso parseado correctamente
}

int cargarProcesos(const char* archivo, Proceso procesos[], int* total) {
    FILE* file = fopen(archivo, "r");
    if (!file) {
        perror("Error al abrir el archivo");
        return 1;
    }

    char linea[200];
    *total = 0;

    while (fgets(linea, sizeof(linea), file)) {
        // Eliminar espacios en blanco al final
        char* end = linea + strlen(linea) - 1;
        while (end > linea && (*end == '\n' || *end == '\r' || *end == ' ' || *end == '\t')) {
            *end = '\0';
            end--;
        }
        
        // Saltar líneas vacías
        if (strlen(linea) == 0) {
            continue;
        }
        
        if (parsearProceso(linea, &procesos[*total])) {
            // Cargar instrucciones para este proceso
            if (cargarInstruccionesDesdeArchivo(&procesos[*total]) == 0) {
                (*total)++;
            } else {
                printf("Error cargando instrucciones para proceso %d\n", 
                       procesos[*total].pid);
            }
        }
    }

    fclose(file);
    return 0;
}

void imprimirProcesos(Proceso procesos[], int total) {
    printf("Lista de procesos cargados:\n");
    for (int i = 0; i < total; i++) {
        printf("PID=%d | AX=%d | BX=%d | CX=%d | Quantum=%d | Estado=%s | Instrucciones=%d\n",
            procesos[i].pid, procesos[i].ax, procesos[i].bx,
            procesos[i].cx, procesos[i].quantum, procesos[i].estado,
            procesos[i].num_instrucciones);
    }
}
int ejecutarInstruccion(Proceso* p) {
    if (p->pc >= p->num_instrucciones) return 1;

    char* instr = p->instrucciones[p->pc];
    printf("Proceso %d ejecutando: %s\n", p->pid, instr);

    char* ptr = instr;
    char comando[10], reg1[3], reg2[3];
    int valor;

    // Leer el comando
    int i = 0;
    while (*ptr && !isspace(*ptr) && i < 9) {
        comando[i++] = *ptr++;
    }
    comando[i] = '\0';

    if (strcmp(comando, "NOP") == 0) {
        // No hace nada
    }
    else if (strcmp(comando, "INC") == 0) {
        saltarEspacios(&ptr);
        leerRegistro(&ptr, reg1);
        int* reg = obtenerRegistro(p, reg1);
        if (reg) (*reg)++;
    }
    else if (strcmp(comando, "JMP") == 0) {
        saltarEspacios(&ptr);
        int destino = leerNumero(&ptr);
        if (destino < 1 || destino > p->num_instrucciones) {
            printf("ERROR: Proceso %d intentó hacer JMP a instrucción inválida (%d)\n", p->pid, destino);
            return 1;
        }
        p->pc = destino - 1;
        return 0;
    }
    else if (
        strcmp(comando, "ADD") == 0 ||
        strcmp(comando, "SUB") == 0 ||
        strcmp(comando, "MUL") == 0) {
        
        saltarEspacios(&ptr);
        leerRegistro(&ptr, reg1);

        saltarEspacios(&ptr);
        if (*ptr == ',') ptr++;
        saltarEspacios(&ptr);

        int esNumero = isdigit(*ptr);
        if (esNumero) {
            valor = leerNumero(&ptr);
        } else {
            leerRegistro(&ptr, reg2);
        }

        int* destino = obtenerRegistro(p, reg1);
        if (!destino) return 1;

        int operando = 0;
        if (esNumero) {
            operando = valor;
        } else {
            int* origen = obtenerRegistro(p, reg2);
            if (!origen) return 1;
            operando = *origen;
        }

        if (strcmp(comando, "ADD") == 0) *destino += operando;
        else if (strcmp(comando, "SUB") == 0) *destino -= operando;
        else if (strcmp(comando, "MUL") == 0) *destino *= operando;
    }

    p->pc += 1;
    return 0;
}

void ejecutarRoundRobin(Proceso procesos[], int total) {
    int procesosTerminados = 0;
    int ciclosMaximos = 1000; // Límite para evitar bucles infinitos
    int cicloActual = 0;

    while (procesosTerminados < total && cicloActual < ciclosMaximos) {
        for (int i = 0; i < total; i++) {
            Proceso* p = &procesos[i];

            if (strcmp(p->estado, "Terminado") == 0)
                continue;

            printf("\n[Cambio de contexto - Ciclo %d]\n", cicloActual + 1);
            printf("Cargando proceso %d: PC=%d, AX=%d, BX=%d, CX=%d\n",
                   p->pid, p->pc, p->ax, p->bx, p->cx);

            strcpy(p->estado, "Ejecutando");

            int instruccionesEjecutadas = 0;
            int instruccionesTotales = 0;

            int error = 0;

            while (instruccionesEjecutadas < p->quantum && 
                p->pc < p->num_instrucciones && 
                instruccionesTotales < 100) {

                if (ejecutarInstruccion(p) != 0) {
                    error = 1;
                    break;
                }

                instruccionesEjecutadas++;
                instruccionesTotales++;
            }   

                if (error) {
                printf("Proceso %d finalizado con error en instrucción.\n", p->pid);
                strcpy(p->estado, "Terminado");
                p->codigo_salida = 1;
                procesosTerminados++;
                }
                else if (p->pc >= p->num_instrucciones) {
                    strcpy(p->estado, "Terminado");
                    p->codigo_salida = 0;
                    procesosTerminados++;
                    printf("Proceso %d terminado.\n", p->pid);
                }
                else if (instruccionesTotales >= 100) {
                    printf("Proceso %d detenido por exceso de instrucciones (bucle infinito)\n", p->pid);
                    strcpy(p->estado, "Terminado");
                    p->codigo_salida = 1;
                    procesosTerminados++;
                }
                else {
                    strcpy(p->estado, "Listo");
                }

            printf("Guardando proceso %d: PC=%d, AX=%d, BX=%d, CX=%d\n",
                   p->pid, p->pc, p->ax, p->bx, p->cx);
        }
        cicloActual++;
    }

    if (cicloActual >= ciclosMaximos) {
        printf("\nPROGRAMA DETENIDO: Se alcanzó el límite máximo de ciclos (%d)\n", ciclosMaximos);
        printf("Posible bucle infinito detectado.\n");
    } else {
        printf("\nTodos los procesos han terminado.\n");
    }

    // Mostrar códigos de salida al final
    printf("\nResumen de estado de salida por proceso:\n");
    for (int i = 0; i < total; i++) {
        printf("Proceso %d → Código de salida: %d\n", procesos[i].pid, procesos[i].codigo_salida);
    }
}

int main(int argc, char* argv[]) {
    Proceso procesos[MAX_PROCESOS];
    int total = 0;

    const char* archivo = "procesos.txt";

    if (argc == 3 && strcmp(argv[1], "-f") == 0) {
        archivo = argv[2];
    }

    if (cargarProcesos(archivo, procesos, &total) != 0) {
        return 1;
    }

    if (total == 0) {
        printf("No se cargaron procesos válidos.\n");
        return 1;
    }

    imprimirProcesos(procesos, total);

    ejecutarRoundRobin(procesos, total);

    return 0;
}