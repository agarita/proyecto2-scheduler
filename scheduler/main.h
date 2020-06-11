#ifndef MAIN_H_
#define MAIN_H_

#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>
#include <mpfr.h>
#include <gmp.h>
#include <string.h>

#define MIN_PROCESS 5
#define MAX_PROCESS 25
#define WORK_FOR_CICLE 50
#define NULL 0

enum scheduling_algorithms_t { FCFS, SJF, RR, PS, PSRR, MQS, MFQS };
enum scheduler_type_t { PREEMPTIVE, NONPREEMPTIVE };

int load_configuration_and_process(struct scheduler * scheduler, struct process_list * process_list, char * file); //Carga la informacion del Scheduler. Algoritmo de calendarizacion, tipo, quamtum/cantidad de trabajo, lista de procesos de 5 a 25. Devuelve 0 si se carga la configuracion y los procesos correctamente, sino devuelve 1.


struct process { //Guarda la informacion de un proceso
    int id,arrival_time,work_load,priority,work_done; //# de proceso, tiempo de llegada, cantidad de trabajo asignado, prioridad,cantidad de trabajo realizado
    mpfr_t *state; //Resultado actual del proceso
};

struct process* initialize_process(int id, int arrival_time, int work_load, int priority);
void save_process_state(struct process* process, mpfr_t state, int work_progress); //Guarda el estado del proceso cuando se retira de ejecucion
void load_process_state(struct process* process, mpfr_t* state); //Carga el estado del proceso cuando se le asigna tiempo de ejecucion
int is_finished(struct process* process); //Dice si un proceso ha terminado
void calculate_process_work_load(struct process* process, int* work_load); //Cualcula la cantidad de trabajo a realizar, cuando se le asigna un tiempo de ejecucion 

struct node { //Nodos de las listas
    struct process* process; //Guarda la direccion de un proceso
    struct node* next; //Guarda la direccion del nodo siguiente en la lista
};
struct node* initialize_node (struct process* process); //Inicializa un nodo
struct process* get_process (struct node* node); //Recupera el proceso de un nodo
int is_finished(struct process* process); //Dice si el proceso ya completo su trabajo


struct process_list { //Lista con procesos
    struct node* first_process;
};
struct process_list* initialize_process_list();
int is_list_empty(struct process_list* process_list); //Dice si la lista es vacia
void add_process(struct process_list* process_list, struct process* process); //Agrega un proceso a la lista
void process_arrival(struct process_list* process_list, struct scheduler* scheduler, int time); //Agrega los procesos al scheduler en su tiempo de llegada

struct scheduler { //Cola de processos en ready
    struct node* first_process; //Guarda la direccion del primer nodo de la lista
    // struct node* first_process; //Guarda la direccion del primer nodo de la lista //deberian de haber dos listas mas.
    // struct node* first_process; //Guarda la direccion del primer nodo de la lista
    enum scheduling_algorithms_t algorithm;
    enum scheduler_type_t type; //Algoritmo de la cola y tipo
    int quantum;
};

struct scheduler* initialize_scheduler();
void add_process_to_scheduler(struct scheduler * scheduler,struct process * process); //Determina que algoritmo utilizar para agregar un proceso a la cola del scheduler
struct process* next_process(struct scheduler * scheduler); //Saca el siguiente processo que entra a ejecucion de la cola del scheduler
int is_scheduler_empty(struct scheduler * scheduler); //Dice si la cola del scheduler esta vacia


enum scheduling_algorithms_t get_scheduling_algorithm (char* algorithm);
enum scheduler_type_t get_scheduler_type (char * type_s);

static void activate(GtkApplication* app, gpointer user_data);

void arcsin(unsigned int start, unsigned int finish);

#endif

