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

int load_configuration_and_process(struct scheduler_t* scheduler, struct process_list_t * process_list, char * file); //Carga la informacion del Scheduler. Algoritmo de calendarizacion, tipo, quamtum/cantidad de trabajo, lista de procesos de 5 a 25. Devuelve 0 si se carga la configuracion y los procesos correctamente, sino devuelve 1.
int load_scheduler_MFQS_queues(struct scheduler_t* scheduler,FILE* configuration_file); //Carga los algoritmos de las colas de un scheduler utilizando MFQS del archivo de configuracion.

struct process_t { //Guarda la informacion de un proceso
    int id,arrival_time,work_load,priority,work_done,work_progress,last_queue; //# de proceso, tiempo de llegada, cantidad de trabajo asignado, prioridad,cantidad de trabajo realizado
    mpfr_t *state; //Estado actual del proceso
    mpfr_t *pi; //Resultado actual del proceso
};

struct process_t* initialize_process(int id, int arrival_time, int work_load, int priority);
void save_process_state(struct process_t* process, mpfr_t state, int work_done); //Guarda el estado del proceso cuando se retira de ejecucion
void load_process_state(struct process_t* process, mpfr_t * state,int * work_done); //Carga el estado del proceso cuando se le asigna tiempo de ejecucion
int is_finished(struct process_t* process); //Dice si un proceso ha terminado
void calculate_process_work_load(struct process_t* process, int* work_load); //Cualcula la cantidad de trabajo a realizar, cuando se le asigna un tiempo de ejecucion 

struct node_t { //Nodos de las listas
    struct process_t* process; //Guarda la direccion de un proceso
    struct node_t* next; //Guarda la direccion del nodo siguiente en la lista
};
struct node_t* initialize_node (struct process_t* process); //Inicializa un nodo
struct process_t* get_process (struct node_t* node_t); //Recupera el proceso de un nodo
int is_finished(struct process_t* process); //Dice si el proceso ya completo su trabajo


struct process_list_t { //Lista con procesos
    struct node_t* first_process;
};
struct process_list_t* initialize_process_list();
int is_list_empty(struct process_list_t* process_list); //Dice si la lista es vacia
void add_process(struct process_list_t* process_list, struct process_t* process); //Agrega un proceso a la lista
void process_arrival(struct process_list_t* process_list, struct scheduler_t* scheduler, int time); //Agrega los procesos al scheduler en su tiempo de llegada

struct scheduler_t { //Cola de processos en ready
    struct queue_list_t* queue_list; //Lista de colas del scheduler
    struct process_list_t* process_list; //Guarda la direccion del primer nodo de la lista
    // struct node* first_process; //Guarda la direccion del primer nodo de la lista //deberian de haber dos listas mas.
    // struct node* first_process; //Guarda la direccion del primer nodo de la lista
    enum scheduling_algorithms_t algorithm;
    enum scheduler_type_t type; //Algoritmo de la cola y tipo
    int number_of_queues,quantum;
};

struct scheduler_t* initialize_scheduler(); //Reserva memoria para el scheduler
void add_process_to_scheduler(struct scheduler_t * scheduler,struct process_t * process); //Determina que algoritmo utilizar para agregar un proceso a la cola del scheduler
struct process_t* next_process(struct process_list_t * process_list); //Saca el siguiente processo que entra a ejecucion de la cola del scheduler
int is_scheduler_empty(struct scheduler_t * scheduler); //Dice si la cola del scheduler esta vacia

struct queue_node_t { //Nodo de las listas de colas
    int id; //Guarda el numero de la cola
    struct scheduler_t * scheduler; //Guarda la direccion de la lista
    struct queue_node_t * next; //Guarda la direccion del siguiente nodo de la lista de colas
};
struct queue_node_t* initialize_queue_node (); //Reserva memoria para un nodo de la lista de colas

struct queue_list_t { //Lista con listas de procesos
    struct queue_node_t * first_queue; //Guarda la direccion del primer nodo de la lista de colas
};
struct queue_list_t* initialize_queue_list (); //Reserva memoria para una lista de colas
void add_queue_list (struct queue_list_t* queue_list, struct scheduler_t* new_scheduler, int id); //Agrega una cola a la lista de colas
int is_queue_list_empty (struct queue_list_t* queue_list); //Dice si la lista de colas esta vacia

enum scheduling_algorithms_t get_scheduling_algorithm (char* algorithm);
enum scheduler_type_t get_scheduler_type (char * type_s);

static void activate(GtkApplication* app, gpointer user_data);

//Funcion que realiza el cpu de arcsin
void arcsin(unsigned int start, unsigned int finish);

//Fuciones con los algoritmos de las colas
void FCFS(struct process_list_t* process_list, struct process_t * process); //First Come First Served. El primer proceso en entrar a la cola es el primero en salir.
void SJF(struct process_list_t* process_list, struct process_t * process); //Shortest Job First. El proceso entra detras de todos los procesos con menor carga de trabajo pendiente
void RR(struct process_list_t* process_list, struct process_t * process); //Round Robin. Expropiativo. Cada proceso tiene un quantum asignado para ejecutarse.
void PS(struct process_list_t* process_list, struct process_t * process); //Priority Scheduling. El proceso entra a la cola delante de los que tengan menor prioridad, en caso de tener la misma prioridad es FCFS.
void PSRR(struct process_list_t* process_list, struct process_t * process); //Priority Scheduling with Round Robin. El proceso entra a la cola delante de los que tengan menor prioridad, en caso de tener la misma prioridad usan round robin.
void MQS(struct process_list_t* process_list, struct process_t * process); //Multilevel Queue. El proceso entra a la cola que corresponda de su prioridad
void MFQS(struct queue_list_t* queue_list, struct process_t * process); //Multilevel Feedback Queue. El proceso entra a la primera cola, si no termina pasa a la segunda, y sigue. Cuando sale de la ultima y no ha terminado trabajo, regresa a la primera.

#endif

