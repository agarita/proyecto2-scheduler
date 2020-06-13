#include "main.h"

/*--------
  GLOBAL
--------*/
struct process_list_t* process_list,* finished_process; //Lista con los procesos
struct scheduler_t* scheduler,* actual_scheduler; //Cola de procesos para el uso del cpu
struct process_t* actual_process;
mpfr_t state;
int time_g,work_done,priority_required; //Lleva la cuenta de los ciclos realizados
clock_t cpu_timer,cpu_start;

GtkWidget *window; //Toda la ventana
GtkWidget *txtAlgoritmo, *txtModo, *txtQuantum_Trabajo; //Informacion algoritmo
GtkWidget *lblQuantum_Trabajo; //Label que cambia dependiendo del modo
GtkWidget *txtPID, *txtAvance; //Informacion proceso activo
GtkWidget *tvReady, *tvFinished; //Informacion proceos ready y finished
GtkWidget *pgProceso; //Barra de progreso del proceso activo
/*-------------
 TAYLOR SERIES
-------------*/
void arcsin(){ //Calcula un termino. Utiliza las variables globales state y workdone. Para obtener el resultado de la serie se debe multiplicar *2.
  /*
  if(start == 0 || finish == 0){
    printf("Los argumentos de esta función deben ser enteros mayores a 0\n");
    return;
  }
  if(finish < start){
    printf("El argumento finish debe ser mayor al argumento start.\n");
    return;
  }*/

  mpfr_t res; // arcsin(1)
  mpfr_t pi;  // 2*res

  mpfr_t u;   // (2n)!

  mpfr_t a;   // 4^n
  mpfr_t b;   // (n!)^2
  mpfr_t c;   // 2n+1
  mpfr_t d;   // a*b*c

  mpfr_t t;   // t = u/d

  mpfr_inits2 (256, res, pi, u, a, b, c, d, t, (mpfr_ptr) 0);

  mpfr_set (res, state, MPFR_RNDD); mpfr_set_d (pi, 1.0, MPFR_RNDD);

  mpfr_set_d (a, 1.0, MPFR_RNDD); mpfr_set_d (b, 1.0, MPFR_RNDD);
  mpfr_set_d (c, 1.0, MPFR_RNDD); mpfr_set_d (d, 1.0, MPFR_RNDD);
  mpfr_set_d (u, 1.0, MPFR_RNDD); mpfr_set_d (t, 1.0, MPFR_RNDD);

  //work_done = n
  for(unsigned int i = 1; i <= 2*work_done; i++){
    mpfr_mul_ui (u, u, i, MPFR_RNDU); // (2n)!
  }
  //----------------------------------
  mpfr_mul_ui (a, a, 4, MPFR_RNDD);
  mpfr_pow_ui (a, a, work_done, MPFR_RNDD);   // 4^n
  //**********************************
  for(unsigned int i = 1; i <= work_done; i++){
    mpfr_mul_ui (b, b, i, MPFR_RNDU); // n!
  }
  mpfr_pow_ui (b, b, 2, MPFR_RNDD);   // b^2
  //**********************************
  mpfr_mul_ui (c, c, 2*work_done, MPFR_RNDD);
  mpfr_add_ui (c, c, 1, MPFR_RNDD);   // 2n+1
  //==================================
  mpfr_mul (d, a, b, MPFR_RNDD);
  mpfr_mul (d, d, c, MPFR_RNDD);
  ////////////////////////////////////
  mpfr_div (t, u, d, MPFR_RNDD);

  mpfr_add (res, res, t, MPFR_RNDD);
  mpfr_set (state, res, MPFR_RNDD);
  /*
  mpfr_mul_ui (pi, res, 2, MPFR_RNDD); //Se podria comentar, ya que la suma final se realiza cuando se guarda el estado del proceso

  printf("Suma %u de 2arcsin(1) ~ ", work_done);
  mpfr_out_str (stdout, 10, 0, pi, MPFR_RNDD);
  putchar('\n');

  mpfr_set_d (a, 1.0, MPFR_RNDD); mpfr_set_d (b, 1.0, MPFR_RNDD);
  mpfr_set_d (c, 1.0, MPFR_RNDD); mpfr_set_d (d, 1.0, MPFR_RNDD);
  mpfr_set_d (u, 1.0, MPFR_RNDD); mpfr_set_d (t, 1.0, MPFR_RNDD);
  */
  mpfr_clears (res, pi, a, b, c, u, d, t, (mpfr_ptr) 0);
  mpfr_free_cache ();
}

/*-------------------------------------
        MANEJO DE ESTRUCTURAS
-------------------------------------*/
struct process_t* initialize_process(int id, int arrival_time, int work_load, int priority) { //Reserva memoria e inicializa un proceso

  mpfr_t state,pi;
  struct process_t * new_process;
  new_process = malloc(sizeof(struct process_t));
  if (new_process == NULL) {
    printf("Error allocating memory for process.");
    exit(1);
  }
  mpfr_init2(state, 256);
  mpfr_init2(pi, 256);
  mpfr_set_d(state, 1.0, MPFR_RNDD);
  mpfr_set_d(pi, 1.0, MPFR_RNDD);
  new_process->id = id;
  new_process->arrival_time = cpu_start + (double) arrival_time;
  new_process->work_load = work_load * (int) WORK_FOR_UNIT;
  new_process->priority = priority;
  new_process->work_done = 0;
  new_process->work_progress = 0;
  new_process->last_queue = 0;
  new_process->state = &state;
  new_process->pi = &pi;
  return new_process;
};

struct node_t * initialize_node (struct process_t* process) {
  struct node_t* new_node;
  new_node = malloc(sizeof(struct node_t));
  if (new_node == NULL) {
    printf("Error allocating memory for node.");
    exit(1);
  }
  new_node->process = process;
  new_node->next = NULL;
  return new_node;
};

struct process_list_t* initialize_process_list() {
  struct process_list_t* new_process_list;
  new_process_list = malloc(sizeof(struct process_list_t));
  if (new_process_list == NULL) {
    printf("Error allocating memory for process list.");
    exit(1);
  }
  new_process_list->first_process = NULL;
  return new_process_list;
};

struct scheduler_t* initialize_scheduler() {
  struct scheduler_t* new_scheduler;
  new_scheduler = malloc(sizeof(struct scheduler_t));
  if (new_scheduler == NULL) {
    printf("Error allocating memory for process scheduler.");
    exit(1);
  }
  new_scheduler->algorithm = -1;
  new_scheduler->process_list = initialize_process_list();
  new_scheduler->type = -1;
  new_scheduler->queue_list = initialize_queue_list();
  new_scheduler->quantum = 0;
  return new_scheduler;
};

struct queue_list_t* initialize_queue_list () {
  struct queue_list_t* new_queue_list;
  new_queue_list = malloc(sizeof(struct queue_list_t));
  if (new_queue_list == NULL) {
    printf("Error allocating memory for queue list.");
    exit(1);
  }
  new_queue_list->first_queue = NULL;
  return new_queue_list;
};

struct queue_node_t* initialize_queue_node (int id) {
  struct queue_node_t* new_queue_node;
  new_queue_node = malloc(sizeof(struct queue_node_t));
  if (new_queue_node == NULL) {
    printf("Error allocating memory for queue node.");
    exit(1);
  }
  new_queue_node->id = id;
  new_queue_node->next = NULL;
  new_queue_node->scheduler = NULL;
  return new_queue_node;
};

void add_queue_list (struct queue_list_t* queue_list, struct scheduler_t* new_scheduler, int id) {
  struct queue_node_t* new_node, * tmp_node;
  new_node = initialize_queue_node(id);
  new_node->id = id;
  new_node->next = NULL;
  new_node->scheduler = new_scheduler;
  if (is_queue_list_empty(queue_list)) {
    queue_list->first_queue = new_node;
  };
  tmp_node = queue_list->first_queue;
  while(tmp_node->next != NULL) {
    tmp_node = tmp_node->next;
  }
  tmp_node->next = new_node;
};

int is_queue_list_empty (struct queue_list_t* queue_list) {
  if (queue_list->first_queue == NULL)
    return 1;
  return 0;
}; //Dice si la lista de colas esta vacia

int is_list_empty(struct process_list_t* process_list) {
  if( process_list->first_process == NULL )
    return 1;
  return 0;
};

int is_scheduler_empty(struct scheduler_t * scheduler) {
  struct queue_node_t* tmp_queue_node;
  if (scheduler->algorithm == MQS) {
    tmp_queue_node = scheduler->queue_list->first_queue;

    while (tmp_queue_node == NULL) {
      if (!is_list_empty(tmp_queue_node->scheduler->process_list)) { //No esta vacio
        return 0;
      }
      tmp_queue_node = tmp_queue_node->next;
    };
    return 1;

  } else if (scheduler->algorithm == MFQS) {
    tmp_queue_node = scheduler->queue_list->first_queue;

    while (tmp_queue_node == NULL) {
      if ((tmp_queue_node->scheduler->algorithm == MQS) || (tmp_queue_node->scheduler->algorithm == MFQS)) {
        if (!is_scheduler_empty(tmp_queue_node->scheduler)) {
          return 0;
        }
      } else {
        if (!is_list_empty(tmp_queue_node->scheduler->process_list)) {
          return 0;
        }
      }
      tmp_queue_node = tmp_queue_node->next;
    };
    return 1;

  } else {
    return is_list_empty(scheduler->process_list);
  };
};

void add_process(struct process_list_t* process_list,struct process_t* process) {
  struct node_t * new_node, * tmp_node;
  new_node = initialize_node(process);

  if (is_list_empty(process_list)){
    process_list->first_process = new_node;
    return;
  };
  tmp_node = process_list->first_process;
  while(tmp_node->next != NULL) {
    tmp_node = tmp_node->next;
  }
  tmp_node->next = new_node;
};

void save_process_state(struct process_t* process, mpfr_ptr state, int work_done) {
  //process->state = state;

  mpfr_set((mpfr_ptr) *(process->state), (mpfr_ptr) state, MPFR_RNDD);
  mpfr_mul_ui((mpfr_ptr) *(process->pi), (mpfr_ptr) state, 2, MPFR_RNDD);
  //mpfr_set((mpfr_ptr) *(process->state), (mpfr_ptr) pi, MPFR_RNDD);
  process->work_done = work_done;
};

void load_process_state(struct process_t* process, mpfr_t state, int * work_done) {
  mpfr_set(state, (mpfr_ptr) * (process->state), MPFR_RNDD);
  work_done = &process->work_done;
};

int is_finished(struct process_t* process) {
  return (process->work_load == process->work_done);
};

void process_arrival(struct process_list_t* process_list, struct scheduler_t* scheduler) {

  if (is_list_empty(process_list))
    return;

  struct node_t* tmp_node,* next_node;
  struct process_t* tmp_process;
  tmp_node = process_list->first_process;

  while(tmp_node!=NULL) {
    if (tmp_node->process->arrival_time <= clock()) {
      tmp_process = tmp_node->process;
      add_process_to_scheduler(scheduler,tmp_process);
      next_node = tmp_node->next;
      free(tmp_node);
      tmp_node = next_node;
    } else break;
  };
  if (is_list_empty(process_list))
    return;

  while( tmp_node->next != NULL) {
    if (tmp_node->next->process->arrival_time <= clock()) {
      tmp_process = tmp_node->next->process;
      add_process_to_scheduler(scheduler,tmp_process);
      next_node = tmp_node->next->next;
      free(tmp_node->next);
      tmp_node->next = next_node;
    };
  };
};

struct process_t* next_process(struct scheduler_t * scheduler) {
  struct process_t* tmp_process;
  struct node_t* tmp_node;
  struct queue_node_t* tmp_queue_node;

  if (scheduler->algorithm == MQS) {
    tmp_queue_node = scheduler->queue_list->first_queue;
    while(tmp_queue_node != NULL){
      if (is_list_empty(tmp_queue_node->scheduler->process_list)) {
        tmp_queue_node = tmp_queue_node->next;
        continue;
      };
      tmp_node = tmp_queue_node->scheduler->process_list->first_process;
      tmp_process = tmp_node->process;
      tmp_queue_node->scheduler->process_list->first_process = tmp_node->next;
      free(tmp_node);
      return tmp_process;
    };

  } else if (scheduler->algorithm == MFQS) {
    tmp_queue_node = scheduler->queue_list->first_queue;
    while(tmp_queue_node != NULL){
      if (is_scheduler_empty(tmp_queue_node->scheduler)) {
        tmp_queue_node = tmp_queue_node->next;
        continue;
      };
      actual_scheduler = tmp_queue_node->scheduler;
      return next_process(actual_scheduler);
    };

  } else {
    tmp_node = scheduler->process_list->first_process;
    tmp_process = tmp_node->process;
    scheduler->process_list->first_process = tmp_node->next;
    free(tmp_node);
    return tmp_process;
  };
};

void add_process_to_scheduler(struct scheduler_t * scheduler,struct process_t * process) { //Esto es provicional, aqui decide que algoritmo usar para agregar a la cola
  struct node_t * new_node, * tmp_node;
  new_node = initialize_node(process);
  if (scheduler->type == NONPREEMPTIVE) {
    new_node->process->work_progress = scheduler->quantum;
  };
  // Seleccionar el algoritmo y llamar la funcion
  if ((scheduler->algorithm == FCFS) || (scheduler->algorithm == RR)) {
    add_process(scheduler->process_list, process);
  } else if (scheduler->algorithm == SJF) {
    SJF_A(scheduler->process_list,process);
  } else if ((scheduler->algorithm == PS) || (scheduler->algorithm == PSRR)) {
    PS_A(scheduler->process_list,process);
  } else if (scheduler->algorithm == MQS) {
    MQS_A(scheduler->queue_list,process);
  } else if (scheduler->algorithm == MFQS) {
    MFQS_A(scheduler->queue_list,process);
  };

};

/* esto casi que hay que cambiarlo, porque segun lo que dijo el profe es necesario hacer el MFQS dinamicamente.
   Una solucion sencilla que encontre es por linea del archivo de texto escribir
   "algoritmo modo quantom/carga" //inf del scheduler
   "0 1 2" // inf del proceso
   Entonces procesar por linea, se lee una linea del archivo y se utiliza sscanf() para sacar los terminos,
   al sscanf() le puedo decir que agarre 3 (o los que quiera) valores del string del formato que le diga.
   Entonces es facil solo agarrar el string "algoritmo modo quantom/carga" y decirle sscanf %s,%s,%s con 3 variables y lo jala.
   EN config_template.txt hay una aproximacion del formato del txt con la configuracion
   Si la cola es MFQS, espera un N, con la cantidad de colas del algoritmo.
   SERIA BUENO PONER LA CANTIDAD DE PROCESOS QUE ENTRAN? creo que es indiferente.*/
int load_configuration_and_process(struct scheduler_t * scheduler, struct process_list_t * process_list, char * file) { //Lee el archivo de configuracion y carga la configuracion y los procesos correspondientes.
  //Verifica que el archivo sea correcto. Si todo se ejecuta correctamente devuelve 0.
  //Sino, devuelve 1 si hay un error abriendo el archivo.
  //2 si el archivo no tiene el formato correcto

  FILE *configuration_file;
  char buffer[64];
  char algoritm_entry[20], type_s_entry[20];
  enum scheduling_algorithms_t algorithm;
  enum scheduler_type_t type_s;
  int number_of_process,process_parameters,arrival_time,work_load,priority;
  double quantum;

  quantum = arrival_time = work_load = priority = priority = priority_required = 0;
  //printf("load config\n");
  if ((configuration_file = fopen(file,"r")) == NULL) {
    //printf("Error! opening file.\n");
    return 1;
  };
  //printf("load config. open file\n");
  if (fgets(buffer,sizeof(buffer),configuration_file) == NULL) {
    //printf("Error! file incomplete missing algorithm.");
    return 2;
  };
  //printf("load config read first line\n");
  if ( !(3 == sscanf(buffer, "%s %s %lf", algoritm_entry,type_s_entry, &quantum))) {
    printf("Error! incorrect quantum");
    return 2;
  };
  //printf("load config store variables\n");
  algorithm = get_scheduling_algorithm(algoritm_entry);
  type_s = get_scheduler_type(type_s_entry);
  scheduler->algorithm = algorithm;
  scheduler->type = type_s;
  scheduler->quantum = quantum;

  printf("Algoritmo: %d, tipo: %d, quantim: %f\n",(int) algorithm, (int) type_s, quantum);

  if (algorithm == MFQS) {
    printf("load config MFQS\n");
    load_scheduler_MFQS_queues(scheduler,configuration_file);
  } else if (algorithm == MQS) {
    printf("load config MQS\n");
    initialize_scheduler_MQS_queue(scheduler);
  };

  if (fgets(buffer,sizeof(buffer),configuration_file) == NULL) {
    printf("Error! file incomplete.\n");
    return 2;
  };
  if (strcmp(buffer,"\n") != 0) {
    printf("Error! incorrect entry.\n");
    return 2;
  };
  printf("load config pasa cambio de linea\n");
  number_of_process = 0;
  if (type_s == PS || type_s == PSRR || type_s == MQS || priority_required) {
    process_parameters = 3;
  } else {
    process_parameters = 2;
  };

  while( fgets(buffer,sizeof(buffer),configuration_file) != NULL ) {
    int n;
    int arrival_time,work_load,priority;
    printf("load config process\n");
    if (process_parameters == 2) {
      if ( !(process_parameters == sscanf(buffer, "%d %d", &arrival_time, &work_load))) {
        printf("Error! incorrect entry\n");
        return 2;
      };
      printf("load config process store 2 variables\n");
    } else {
      if ( !(process_parameters == sscanf(buffer, "%d %d %d", &arrival_time, &work_load, &priority))) {
        printf("Error! incorrect entry\n");
        return 2;
      };
      printf("load config process store 3 variables\n");
    };
    printf("new process added %d\n", number_of_process);
    struct process_t* new_process;
    new_process = initialize_process(number_of_process,arrival_time,work_load,priority);
    add_process(process_list,new_process);
    number_of_process++;
  };
  
  if (number_of_process < MIN_PROCESS || number_of_process > MAX_PROCESS) {
    printf("Error! not enough process %d\n", number_of_process);
    return 2;
  }
  printf("load config ends\n");
  fclose(configuration_file);
  return 0;
};

int load_scheduler_MFQS_queues(struct scheduler_t* scheduler,FILE* configuration_file) {
  struct queue_list_t* new_queue_list;
  new_queue_list = initialize_queue_list();

  char buffer[64];
  char algoritm_entry[20], type_s_entry[20];
  enum scheduling_algorithms_t algorithm;
  enum scheduler_type_t type_s;
  int quantum,number_of_queue,total_queues;
  quantum = total_queues = 0;
  number_of_queue = 1;

  if (fgets(buffer,sizeof(buffer),configuration_file) == NULL) {
    printf("Error! file incomplete.");
    return 2;
  };

  if ( !(1 == sscanf(buffer, "%d", &total_queues))) {
    printf("Error! incorrect entry.");
    return 2;
  };

  while( (number_of_queue <= total_queues) && (fgets(buffer,sizeof(buffer),configuration_file) == NULL )) {
    if ( !(3 == sscanf(buffer, "%s %s %d", algoritm_entry,type_s_entry, &quantum))) {
      printf("Error! incorrect entry.");
      return 2;
    };

    algorithm = get_scheduling_algorithm(algoritm_entry);
    type_s = get_scheduler_type(type_s_entry);

    struct scheduler_t* new_scheduler;
    new_scheduler = initialize_scheduler();
    if (algorithm == MFQS) {
      load_scheduler_MFQS_queues(new_scheduler,configuration_file);
      if (fgets(buffer,sizeof(buffer),configuration_file) == NULL) {
        printf("Error! file incomplete.");
        return 2;
      };
      if ( strcmp(buffer,"\n") != 0) { //WARNING!. Esto puede causar que sea necesario poner doble espacio si se tiene una cola mfqs dentro de una cola mfqs
        printf("Error! incorrect entry.");
        return 2;
      };
    } else if (algorithm == MQS) {
      initialize_scheduler_MQS_queue(new_scheduler);
    } else {
      struct process_list_t* new_process_list;
      new_process_list = initialize_process_list();
      new_scheduler->process_list = new_process_list;
    };
    new_scheduler->algorithm = algorithm;
    new_scheduler->type = type_s;
    new_scheduler->quantum = quantum;
    add_queue_list(new_queue_list,new_scheduler,number_of_queue);
    number_of_queue++;
  };
  scheduler->queue_list = new_queue_list;

};

int initialize_scheduler_MQS_queue(struct scheduler_t* scheduler) {
  struct queue_list_t* new_queue_list;
  new_queue_list = initialize_queue_list();
  for(int i=1;i<6;i++) {
    struct scheduler_t* new_scheduler;
    new_scheduler = initialize_scheduler();

    add_queue_list(new_queue_list,new_scheduler,i);
  };
  scheduler->queue_list = new_queue_list;
};

enum scheduling_algorithms_t get_scheduling_algorithm (char* algorithm) {

  if (strcmp(algorithm,"FCFS") == 0) {
    return FCFS;
  } else if (strcmp(algorithm,"SJF") == 0) {
    return SJF;
  } else if (strcmp(algorithm,"RR") == 0) {
    return RR;
  } else if (strcmp(algorithm,"PS") == 0) {
    return PS;
  } else if (strcmp(algorithm,"PSRR") == 0) {
    return PSRR;
  } else if (strcmp(algorithm,"MQS") == 0) {
    return MQS;
  } else if (strcmp(algorithm,"MFQS") == 0) {
    return MFQS;
  } else {
    return -1;
  };
};

enum scheduler_type_t get_scheduler_type (char * type_s) {
  if (strcmp(type_s,"PREEMPTIVE")) {
    return PREEMPTIVE;
  } else if (strcmp(type_s,"NONPREEMPTIVE")) {
    return NONPREEMPTIVE;
  } else {
    return -1;
  };
};

/* ---------------------------------------
     Algoritmos de calendarizacion
--------------------------------------- */
void SJF_A(struct process_list_t* process_list, struct process_t * process) {
  struct node_t * new_node, * tmp_node;
  new_node = initialize_node(process);

  if (is_list_empty(process_list)){
    process_list->first_process = new_node;
    return;
  };
  tmp_node = process_list->first_process;

  while(tmp_node->next != NULL) {
    if ( ( process->work_load-process->work_done ) < ( tmp_node->process->work_load-tmp_node->process->work_load ) ) {
      new_node->next = tmp_node;
      process_list->first_process = new_node;
      return;
    }
    tmp_node = tmp_node->next;
  }
  tmp_node->next = new_node;
};
// ME PARECE QUE ESTE NO ES EN SI UN ALGORITMO PARA GUARDAR A LA COLA, SINO QUE USAMOS FCFS, y en el ciclo del CPU, cada vez que se complete el quantum, se saca el proceso del cpu y se mete a la cola, y se coloca el siguiente proceso en cpu.

void PS_A(struct process_list_t* process_list, struct process_t * process) {
  struct node_t * new_node, * tmp_node;
  new_node = initialize_node(process);

  if (is_list_empty(process_list)){
    process_list->first_process = new_node;
    return;
  };
  tmp_node = process_list->first_process;

  if ( process->priority < tmp_node->process->priority ) {
    new_node->next = tmp_node;
    process_list->first_process = new_node;
    return;
  }

  while(tmp_node->next != NULL) {
    if ( process->priority < tmp_node->next->process->priority ) {
      new_node->next = tmp_node->next;
      tmp_node->next = new_node;
      return;
    }
    tmp_node = tmp_node->next;
  }
  tmp_node->next = new_node;
};
//Este de aqui me parece que funciona igual que el PS, solo que en el ciclo del CPU, cada vez que se complete el quantum, se compara el el proceso actual del cpu con el primero de la cola, si comparten prioridad, se saca el proceso y se mete a la cola, y se coloca el siguiente proceso en cpu, sino continua otra ronda de quantum.

void MQS_A(struct queue_list_t* queue_list, struct process_t * process) {
  struct queue_node_t * tmp_queue_node;
  struct node_t * new_node;

  tmp_queue_node = queue_list->first_queue;
  new_node = initialize_node(process);

  while (tmp_queue_node != NULL) {
    if (tmp_queue_node->id == process->priority) {
      add_process(tmp_queue_node->scheduler->process_list,process);
    };
    tmp_queue_node = tmp_queue_node->next;
  };
};

void MFQS_A(struct queue_list_t* queue_list, struct process_t * process) {
  struct queue_node_t * tmp_queue_node;
  struct node_t * new_node;
  int next_queue;

  next_queue = (process->last_queue + 1)% 5;
  if (!next_queue) {
    next_queue = 5;
  };
  tmp_queue_node = queue_list->first_queue;
  new_node = initialize_node(process);

  while (tmp_queue_node != NULL) {
    if (tmp_queue_node->id == next_queue) {
      add_process_to_scheduler(tmp_queue_node->scheduler,process);
    };
    tmp_queue_node = tmp_queue_node->next;
  };
};

/* ------------------------------------------------
 Funciones para liberar memoria de las estructuras
------------------------------------------------ */
void free_process(struct process_t* process) {
  mpfr_clear((mpfr_ptr)* (process->pi));
  mpfr_clear((mpfr_ptr) *(process->state));
  free(process);
};

void free_process_list(struct process_list_t * process_list) {
  struct node_t * tmp_node, * actual_node;
  if(process_list == NULL){
    return;
  }
  else{
    actual_node = process_list->first_process;
    while(actual_node != NULL) {
      tmp_node = actual_node->next;
      free_process(actual_node->process);
      free(actual_node);
      actual_node = tmp_node;
    };
    free(process_list);
    //printf("Memoria liberada");
  };
};

void free_scheduler(struct scheduler_t * scheduler) {
  struct queue_node_t * tmp_queue_node, * actual_queue_node;
  if (scheduler->algorithm == MQS) {
    actual_queue_node = scheduler->queue_list->first_queue;
    while (actual_queue_node != NULL) {
      free_process_list(actual_queue_node->scheduler->process_list);
      tmp_queue_node = actual_queue_node->next;
      free(actual_queue_node);
      actual_queue_node = tmp_queue_node;
    };
    free_process_list(scheduler->process_list);
    free(scheduler->queue_list);
    free(scheduler);
    
  } else if (scheduler->algorithm == MFQS) {
      //falta un toque aqui
    free_queue_list(scheduler->queue_list);
    free_process_list(scheduler->process_list);
    free(scheduler);

  } else {
    free_queue_list(scheduler->queue_list);
    free_process_list(scheduler->process_list);
    free(scheduler);
  };
  
};

void free_queue_list(struct queue_list_t* queue_list) {
  struct queue_node_t * tmp_queue_node, * actual_queue_node;
  actual_queue_node = queue_list->first_queue;
  while (actual_queue_node != NULL) {
    free_scheduler(actual_queue_node->scheduler);
    tmp_queue_node = actual_queue_node->next;
    free(actual_queue_node);
    actual_queue_node = tmp_queue_node;
  };
  free(queue_list);
};
/* --------
 INTERFAZ
--------*/
void update_algorithm_GUI (char * algoritmo, char * modo, char * quantum_trabajo, int modeType){
  gtk_label_set_text(GTK_LABEL(txtAlgoritmo), algoritmo);
  gtk_label_set_text(GTK_LABEL(txtModo), modo);
  gtk_label_set_text(GTK_LABEL(txtQuantum_Trabajo), quantum_trabajo);
  if(modeType == NONPREEMPTIVE){
    gtk_label_set_text(GTK_LABEL(lblQuantum_Trabajo), "Cantidad de Trabajo:");
  }
  else{
    gtk_label_set_text(GTK_LABEL(lblQuantum_Trabajo), "Quantum:");
  }

}

void update_active_process_GUI (char * id, char * avance, int work_done, int total_work){
  double fraction;

  gtk_label_set_text(GTK_LABEL(txtPID), id);
  gtk_label_set_text(GTK_LABEL(txtAvance), avance);

  fraction = (double)(1.0/total_work)*(double)work_done;
  printf("%F\n", fraction);
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pgProceso), fraction);
}

void update_process_list_GUI(){
  GtkTextBuffer *bufferReady, *bufferFinished;
  bufferReady = gtk_text_buffer_new(NULL);
  bufferFinished = gtk_text_buffer_new(NULL);
  gtk_text_buffer_insert_at_cursor(bufferReady, "Procesos listos!!!", -1);
  gtk_text_buffer_insert_at_cursor(bufferFinished, "Procesos terminados!!!\n", -1);
  gtk_text_buffer_insert_at_cursor(bufferFinished, "Procesos terminados!!!\n", -1);
  gtk_text_view_set_buffer (GTK_TEXT_VIEW(tvReady), bufferReady);
  gtk_text_view_set_buffer (GTK_TEXT_VIEW(tvFinished), bufferFinished);
}

void on_btnEjecutar_click (GtkButton *button, gpointer user_data){
  printf("Ejecutar\n");
}

void on_btnCargar_click (GtkButton *button, gpointer user_data){
  GtkWidget *dialog;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
  gint res;

  dialog = gtk_file_chooser_dialog_new ("Abrir Archivo", GTK_WINDOW(window), action,
                                        "_Cancel",GTK_RESPONSE_CANCEL,
                                        "_Open", GTK_RESPONSE_ACCEPT,
                                        NULL);
  res = gtk_dialog_run (GTK_DIALOG (dialog));
  if (res == GTK_RESPONSE_ACCEPT){
    char *filename;
    GtkFileChooser *chooser = GTK_FILE_CHOOSER (dialog);

    //Aquí se obtiene el nombre de archivo
    filename = gtk_file_chooser_get_filename (chooser);
    printf("%s\n", filename);
    g_free(filename);
  }

  gtk_widget_destroy(dialog);
}

void on_btnLimpiar_click (GtkButton *button, gpointer user_data){
  update_algorithm_GUI("","","",0);
  update_active_process_GUI("", "", 56, 100);
  update_process_list_GUI();
  printf("Limpiar\n");
}

void on_btnReiniciar_click (GtkButton *button, gpointer user_data){
  printf("Resetear\n");
}

static void activate (GtkApplication* app, gpointer user_data){
  //CONTAINERS
  GtkWidget *grid, *gridlbl, *gridActive;
  GtkWidget *frame1, *frame2, *frame3, *frame4;
  GtkWidget *scrolledWindow, *scrolledWindow2;

  //BUTTONS
  GtkWidget *btnEjecutar, *btnCargar, *btnLimpiar, *btnReiniciar;

  // Create window, and set name, size and margin
  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Scheduler");
  gtk_window_set_default_size(GTK_WINDOW (window), 600, 450);
  gtk_container_set_border_width(GTK_CONTAINER(window), 10);

  // Create the grid to pack everything and pack it on the window
  grid = gtk_grid_new ();
  gtk_container_add(GTK_CONTAINER(window), grid);

  //Create text labels.
  frame1 = gtk_frame_new("Algoritmo de Calendarización");
  gtk_widget_set_hexpand(frame1, TRUE);
  gtk_grid_attach(GTK_GRID(grid), frame1, 0, 0, 8, 3);
  gridlbl = gtk_grid_new();
  gtk_container_add(GTK_CONTAINER (frame1), gridlbl);

  GtkWidget * lblAlgoritmo = gtk_label_new("Algoritmo: ");
  gtk_widget_set_halign(GTK_WIDGET(lblAlgoritmo), GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (gridlbl), lblAlgoritmo, 0, 0, 1, 1);
  GtkWidget * lblModo = gtk_label_new("Modo: ");
  gtk_widget_set_halign(GTK_WIDGET(lblModo), GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (gridlbl), lblModo, 0, 1, 1, 1);
  lblQuantum_Trabajo = gtk_label_new("Quantum: ");
  gtk_widget_set_halign(GTK_WIDGET(lblQuantum_Trabajo), GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (gridlbl), lblQuantum_Trabajo, 0, 2, 1, 1);

  //Create editable lables
  txtAlgoritmo = gtk_label_new("a");
  gtk_widget_set_halign(GTK_WIDGET(txtAlgoritmo), GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (gridlbl), txtAlgoritmo, 1, 0, 1, 1);
  txtModo = gtk_label_new("a");
  gtk_widget_set_halign(GTK_WIDGET(txtModo), GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (gridlbl), txtModo, 1, 1, 1, 1);
  txtQuantum_Trabajo = gtk_label_new("a");
  gtk_widget_set_halign(GTK_WIDGET(txtQuantum_Trabajo), GTK_ALIGN_START);
  gtk_grid_attach (GTK_GRID (gridlbl), txtQuantum_Trabajo, 1, 2, 1, 1);

  //Create buttons
  btnEjecutar = gtk_button_new_with_label("Ejecutar");
  gtk_widget_set_tooltip_text(btnEjecutar, "Ejecutar algoritmos");
  g_signal_connect (GTK_BUTTON (btnEjecutar), "clicked",
                    G_CALLBACK (on_btnEjecutar_click), NULL);
  gtk_grid_attach (GTK_GRID (grid), btnEjecutar, 5,14,1,1);
  btnCargar = gtk_button_new_with_label("Cargar");
  gtk_widget_set_tooltip_text(btnCargar, "Cargar un archivo de configuración");
  g_signal_connect (GTK_BUTTON (btnCargar), "clicked",
                    G_CALLBACK (on_btnCargar_click), window);
  gtk_grid_attach (GTK_GRID (grid), btnCargar, 5,15,1,1);
  btnLimpiar = gtk_button_new_with_label("Limpiar");
  gtk_widget_set_tooltip_text(btnLimpiar, "Limpiar la configuración");
  g_signal_connect (GTK_BUTTON (btnLimpiar), "clicked",
                    G_CALLBACK (on_btnLimpiar_click), NULL);
  gtk_grid_attach (GTK_GRID (grid), btnLimpiar, 6,14,1,1);
  btnReiniciar = gtk_button_new_with_label("Reiniciar");
  gtk_widget_set_tooltip_text(btnReiniciar, "Reinicia el programa con la configuración actual");
  g_signal_connect (GTK_BUTTON (btnReiniciar), "clicked",
                    G_CALLBACK (on_btnReiniciar_click), NULL);
  gtk_grid_attach (GTK_GRID (grid), btnReiniciar, 6,15,1,1);

  //Create textviews for ready and finished proccess
  frame2 = gtk_frame_new("Procesos listos");
  gtk_grid_attach (GTK_GRID (grid), frame2, 0,4,4,4);
  gtk_widget_set_hexpand(frame2, TRUE);
  gtk_widget_set_vexpand(frame2, TRUE);
  scrolledWindow = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add (GTK_CONTAINER(frame2), scrolledWindow);
  tvReady = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(tvReady), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(tvReady), FALSE);
  gtk_container_add (GTK_CONTAINER (scrolledWindow), tvReady);

  frame3 = gtk_frame_new("Procesos terminados");
  gtk_grid_attach(GTK_GRID(grid), frame3, 0,12,4,4);
  gtk_widget_set_hexpand(frame3, TRUE);
  gtk_widget_set_vexpand(frame3, TRUE);
  scrolledWindow2 = gtk_scrolled_window_new(NULL, NULL);
  gtk_container_add (GTK_CONTAINER(frame3), scrolledWindow2);
  tvFinished = gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(tvFinished), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(tvFinished), FALSE);
  gtk_container_add (GTK_CONTAINER(scrolledWindow2), tvFinished);

  //Create active process section
  frame4 = gtk_frame_new("Proceso activo");
  gtk_grid_attach(GTK_GRID(grid), frame4, 5, 6, 2, 1);
  gridActive = gtk_grid_new();
  gtk_container_add (GTK_CONTAINER(frame4), gridActive);

  GtkWidget * lblPID = gtk_label_new("PID: ");
  gtk_widget_set_halign(GTK_WIDGET(lblPID), GTK_ALIGN_START);
  gtk_grid_attach(GTK_GRID(gridActive), lblPID, 0, 0, 1, 1);
  GtkWidget * lblPAvance = gtk_label_new("Avance: \n");
  gtk_widget_set_halign(GTK_WIDGET(lblPAvance), GTK_ALIGN_START);
  gtk_grid_attach(GTK_GRID(gridActive), lblPAvance, 0, 1, 1, 1);
  txtPID = gtk_label_new("a");
  gtk_widget_set_halign(GTK_WIDGET(txtPID), GTK_ALIGN_START);
  gtk_grid_attach(GTK_GRID(gridActive), txtPID, 1, 0, 1, 1);
  txtAvance = gtk_label_new("a\n");
  gtk_widget_set_halign(GTK_WIDGET(txtAvance), GTK_ALIGN_START);
  gtk_grid_attach(GTK_GRID(gridActive), txtAvance, 1, 1, 1, 1);
  pgProceso = gtk_progress_bar_new();
  gtk_progress_bar_set_text(GTK_PROGRESS_BAR(pgProceso), NULL);
  gtk_progress_bar_set_show_text(GTK_PROGRESS_BAR(pgProceso), TRUE);
  gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(pgProceso), 0.3);
  gtk_grid_attach(GTK_GRID(gridActive), pgProceso, 0, 2, 4, 1);

  //Load everyhting
  gtk_widget_show_all (window);
}

/*-------------
     MAIN
-------------*/
int main(int argc, char *argv[]) {

  GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run(G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;
  */
  process_list = initialize_process_list(); //Se inicializa la lista de procesos para los procesos cargados del archivo de configuracion
  finished_process = initialize_process_list(); //Se inicializa la lista para los procesos terminados
  scheduler = initialize_scheduler(); //Se inicializa la cola del scheduler

  work_done = 0;
  actual_process = NULL;

  load_configuration_and_process(scheduler,process_list,"./configuration/config1.txt"); //Se carga la configuracion y los procesos
  mpfr_t pi;
//
  cpu_start = clock() *CLOCKS_PER_SEC/1000;
  while (1) {
    process_arrival(process_list,scheduler);
    if ( is_list_empty(process_list) && is_scheduler_empty(scheduler) ) {
      free_process_list(process_list);
      free_process_list(finished_process);
      free_scheduler(scheduler);
      return 0;
    }
    if (actual_process == NULL) { //si no hay un proceso ejecutandose
      //Cargar siguiente proceso
      actual_process = next_process(scheduler);
      load_process_state(actual_process,state,&work_done);
    }
    if (actual_scheduler->type == PREEMPTIVE) { //Es expropiativo
      //asigna quantum al timer
      cpu_timer = clock()+ actual_scheduler->quantum *CLOCKS_PER_SEC/1000;
      while(1) {
        if (( clock() > cpu_timer ) || (is_finished(actual_process)) ) {//Termino el quantum o termino el proceso
          save_process_state(actual_process,state,work_done);

          if (is_finished(actual_process)) {
            add_process(finished_process,actual_process);
          } else {
            add_process_to_scheduler(scheduler,actual_process);
          }
          actual_process = next_process(scheduler);
          load_process_state(actual_process,state,&work_done);
          break;
        }
        arcsin();
      }
    } else {//Es no expropiativo
      //
      int limit = work_done + actual_process->work_progress;
      for (work_done;(work_done<=limit || !is_finished(actual_process));work_done++) {
        arcsin();
      }
      save_process_state(actual_process,state,work_done);
      actual_process = NULL; //Devuelve el cpu
      if (is_finished(actual_process)) {//Termino el proceso
        add_process(finished_process,actual_process);
      } else { //No ha terminado el proceso, pero termino la carga asignada antes de devolver voluntariamente el cpu
      //Guarda el estado del proceso
        add_process_to_scheduler(scheduler,actual_process);
      }
    }
  }
  return 0;
}
