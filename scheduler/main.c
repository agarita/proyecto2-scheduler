#include "main.h"

/*--------
  GLOBAL
--------*/
struct process_list* process_list; //Lista con los procesos
struct scheduler* scheduler; //Cola de procesos para el uso del cpu
struct process* actual_process;
mpfr_t state;
int time; //Lleva la cuenta de los ciclos realizados

/*--------
 INTERFAZ
--------*/
static void activate (GtkApplication* app, gpointer user_data){
  GtkWidget *window;

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);
  gtk_widget_show_all (window);
}

/*-------------
 TAYLOR SERIES
-------------*/
void arcsin(unsigned int start, unsigned int finish){
  if(start == 0 || finish == 0){
    printf("Los argumentos de esta función deben ser enteros mayores a 0\n");
    return;
  }
  if(finish < start){
    printf("El argumento finish debe ser mayor al argumento start.\n");
    return;
  }

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

  for (unsigned int n = start; n <= finish; n++){
    for(unsigned int i = 1; i <= 2*n; i++){
      mpfr_mul_ui (u, u, i, MPFR_RNDU); // (2n)!
    }
    //----------------------------------
    mpfr_mul_ui (a, a, 4, MPFR_RNDD);
    mpfr_pow_ui (a, a, n, MPFR_RNDD);   // 4^n
    //**********************************
    for(unsigned int i = 1; i <= n; i++){
      mpfr_mul_ui (b, b, i, MPFR_RNDU); // n!
    }
    mpfr_pow_ui (b, b, 2, MPFR_RNDD);   // b^2
    //**********************************
    mpfr_mul_ui (c, c, 2*n, MPFR_RNDD);
    mpfr_add_ui (c, c, 1, MPFR_RNDD);   // 2n+1
    //==================================
    mpfr_mul (d, a, b, MPFR_RNDD);
    mpfr_mul (d, d, c, MPFR_RNDD);
    ////////////////////////////////////
    mpfr_div (t, u, d, MPFR_RNDD);

    mpfr_add (res, res, t, MPFR_RNDD);
    mpfr_set (state, res, MPFR_RNDD);
    mpfr_mul_ui (pi, res, 2, MPFR_RNDD);

    printf("Suma %u de 2arcsin(1) ~ ", n);
    mpfr_out_str (stdout, 10, 0, pi, MPFR_RNDD);
    putchar('\n');

    mpfr_set_d (a, 1.0, MPFR_RNDD); mpfr_set_d (b, 1.0, MPFR_RNDD);
    mpfr_set_d (c, 1.0, MPFR_RNDD); mpfr_set_d (d, 1.0, MPFR_RNDD);
    mpfr_set_d (u, 1.0, MPFR_RNDD); mpfr_set_d (t, 1.0, MPFR_RNDD);
  }
  mpfr_clears (res, pi, a, b, c, u, d, t, (mpfr_ptr) 0);
  mpfr_free_cache ();
}

/*-------------------------------------
        MANEJO DE ESTRUCTURAS
-------------------------------------*/
struct process* initialize_process(int id, int arrival_time, int work_load, int priority) {
  struct process * new_process;
  new_process = malloc(sizeof(struct process));
  if (new_process == NULL) {
    printf("Error allocating memory for process.");
    exit(1);
  }
  new_process->id = id;
  new_process->arrival_time = arrival_time;
  new_process->work_load = work_load;
  new_process->priority = priority;
  new_process->work_done = 0;
  mpfr_t state;
  mpfr_init2(state, 256);
  mpfr_set_d(state, 1.0, MPFR_RNDD);
  new_process->state = &state;
  return new_process;
};

struct node * initialize_node (struct process* process) {
  struct node* new_node;
  new_node = malloc(sizeof(struct node));
  if (new_node == NULL) {
    printf("Error allocating memory for node.");
    exit(1);
  }
  new_node->process = process;
  new_node->next = NULL;
  return new_node;
};

struct process_list* initialize_process_list() {
  struct process_list* new_process_list;
  new_process_list = malloc(sizeof(struct process_list));
  if (new_process_list == NULL) {
    printf("Error allocating memory for process list.");
    exit(1);
  }
  new_process_list->first_process = NULL;
  return new_process_list;
};

struct scheduler* initialize_scheduler() {
  struct scheduler* new_scheduler;
  new_scheduler = malloc(sizeof(struct scheduler));
  if (new_scheduler == NULL) {
    printf("Error allocating memory for process scheduler.");
    exit(1);
  }
  new_scheduler->algorithm = NULL;
  new_scheduler->first_process = NULL;
  new_scheduler->type = NULL;
  return new_scheduler;
};

int is_list_empty(struct process_list* process_list) {
  if( process_list == NULL )
    return 1;
  return 0;
};

int is_scheduler_empty(struct scheduler * scheduler) {
  if (scheduler->first_process == NULL)
    return 1;
  return 0;
};

void add_process(struct process_list* process_list,struct process* process) {
  struct node * new_node, * tmp_node;
  new_node = initialize_node(process);

  if (is_list_empty(process_list)){
    process_list->first_process = new_node;
  };
  tmp_node = process_list->first_process;
  while(tmp_node->next != NULL) {
    tmp_node = tmp_node->next;
  }
  tmp_node->next = new_node;
};
/*
struct process* get_process(struct node* process_list,int index) {
  
  if (is_list_empty(process_list)){
    return NULL;
  };

  int process_number = 0;
  struct node* tmp_node = process_list->first_process;

}*/
void save_process_state(struct process* process, mpfr_t state, int work_progress) {
  process->state = state;
  process->work_done = process->work_done + work_progress;
};

void load_process_state(struct process* process, mpfr_t* state) {
  state = process->state;
};

int is_finished(struct process* process) {
  return (process->work_load == process->work_done);
};

void process_arrival(struct process_list* process_list, struct scheduler* scheduler, int time) {
  
  if (is_list_empty(process_list))
    return;
  
  struct node* tmp_node,* next_node;
  struct process* tmp_process;
  tmp_node = process_list->first_process;
  
  while(tmp_node!=NULL) {
    if(tmp_node->process->arrival_time == time) {
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
    if(tmp_node->next->process->arrival_time == time){
      tmp_process = tmp_node->next->process;
      add_process_to_scheduler(scheduler,tmp_process);
      next_node = tmp_node->next->next;
      free(tmp_node->next);
      tmp_node->next = next_node;
    };
  };
};

struct process* next_process(struct scheduler * scheduler) {
  struct process* tmp_process;
  struct node* tmp_node;
  tmp_node = scheduler->first_process;
  tmp_process = tmp_node->process;
  scheduler->first_process = tmp_node->next;
  free(tmp_node);
  return tmp_process;
};

void add_process_to_scheduler(struct scheduler * scheduler,struct process * process) { //Esto es provicional, aqui decide que algoritmo usar para agregar a la cola
  struct node * new_node, * tmp_node;
  new_node = initialize_node(process);

  if (is_scheduler_empty(scheduler)){
    scheduler->first_process = new_node;
  };
  tmp_node = scheduler->first_process;
  while(tmp_node->next != NULL) {
    tmp_node = tmp_node->next;
  }
  tmp_node->next = new_node;
};

int load_configuration_and_process(struct scheduler * scheduler, struct process_list * process_list, char * file) { //Lee el archivo de configuracion y carga la configuracion y los procesos correspondientes.
  //Verifica que el archivo sea correcto. Si todo se ejecuta correctamente devuelve 0.
  //Sino, devuelve 1 si hay un error abriendo el archivo.
  //2 si el archivo no tiene el formato correcto

  FILE *configuration_file;
  char buffer[32];
  enum scheduling_algorithms_t algorithm;
  enum scheduler_type_t type_s;
  int number_of_process,process_parameters,quantum,arrival_time,work_load,priority;

  if ((configuration_file = fopen(file,'r')) == NULL) {
    printf("Error! opening file.");
    return 1;
  }
  
  if (fgets(buffer,sizeof(buffer),configuration_file) == NULL) {
    printf("Error! file incomplete missing algorithm.");
    return 2;
  };
  //proceso algoritmo
  if (buffer[strlen(buffer)-1] == '\n') {
    buffer[strlen(buffer)-1] == 0;
  };
  
  algorithm = get_scheduling_algorithm(buffer);
  if (algorithm == NULL) {
    print("Error! incorrect algorithm");
    return 2;
  };

  if (fgets(buffer,sizeof(buffer),configuration_file) == NULL) {
    printf("Error! file incomplete missing type.");
    return 2;
  };
  //proceso tipo
  if (buffer[strlen(buffer)-1] == '\n') {
    buffer[strlen(buffer)-1] == 0;
  };

  type_s = get_scheduler_type(buffer);
  if (type_s == NULL) {
    print("Error! incorrect scheduler type");
    return 2;
  };
  
  if (fgets(buffer,sizeof(buffer),configuration_file) == NULL) {
    printf("Error! file incomplete missing quantum.");
    return 2;
  };

  //proceso quantum/cantidad de trabajo

  if (fgets(buffer,sizeof(buffer),configuration_file) == NULL) {
    printf("Error! file incomplete missing the process.");
    return 2;
  };

  if (strcmp(buffer,'\n')) {
    printf("Error! incorrect file format.");
    return 2;
  };

  if ( not (1 == sscanf(buffer, "%*[^0123456789]%d", &quantum))) {
        print("Error! incorrect quantum");
        return 2;
  };

  scheduler->algorithm = algorithm;
  scheduler->type = type_s;
  scheduler->quantum = quantum;

  number_of_process = 0;
  if (type_s == PS || type_s == PSRR) {
    process_parameters = 3;
  } else {
    process_parameters = 2;
  };

  while( fgets(buffer,sizeof(buffer),configuration_file) == NULL ) {
    int n;
    int arrival_time,work_load,priority;

    if (process_parameters == 2) {
      if ( not (process_parameters == sscanf(buffer, "%*[^0123456789]%d%*[^0123456789]%d", &arrival_time, &work_load))) {
        print("Error! incorrect number of process parameters");
        return 2;
      };

    } else {
      if ( not ( process_parameters == sscanf(buffer, "%*[^0123456789]%d%*[^0123456789]%d%*[^0123456789]%d", &arrival_time, &work_load, &priority))) {
         print("Error! incorrect number of process parameters");
        return 2;
      };
    };

    struct process* new_process;
    new_process = initialize_process(number_of_process,arrival_time,work_load,priority);
    add_process(process_list,new_process);
    number_of_process++;

  };
  
  if (number_of_process < MIN_PROCESS || number_of_process > MAX_PROCESS) {
    print("Error! not enough process");
    return 2;
  }


  fclose(configuration_file); 
  return 0;
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
    return NULL;
  };
};

enum scheduler_type_t get_scheduler_type (char * type_s) {
  if (strcmp(type_s,"PREEMPTIVE")) {
    return PREEMPTIVE;
  } else if (strcmp(type_s,"NONPREEMPTIVE")) {
    return NONPREEMPTIVE;
  } else {
    return NULL;
  };
};
/*-------------
     MAIN
-------------*/
int main(int argc, char *argv[]) {
  initialize_process_list(process_list); //Se inicializa la lista de procesos
  initialize_scheduler(scheduler); //Se inicializa la cola del scheduler
  time = 0; // inicia en el ciclo 0

  load_configuration_and_process(scheduler,process_list,"path"); //Se carga la configuracion y los procesos
  mpfr_t pi;
  
  while (1) {
    process_arrival(process_list,scheduler,time);
    if ( is_list_empty(process_list) && is_scheduler_empty(scheduler) ) {
      //salir
      //limpiar la memoria reservada
      return 0;
    }

    if (is_finished(actual_process)) {
      mpfr_init2(pi, 256);
      mpfr_mul_ui(pi, state, 2, MPFR_RNDD);
    }
    //falta
  };

  mpfr_init2(pi, 256);
  mpfr_init2(state, 256);

  mpfr_set_d(state, 1.0, MPFR_RNDD);

  arcsin(1, 500);
  printf("APROXIMACION 1 = ");
  mpfr_mul_ui(pi, state, 2, MPFR_RNDD);
  mpfr_out_str (stdout, 10, 0, pi, MPFR_RNDD);
  putchar('\n');

  arcsin(501, 511);
  printf("APROXIMACION 2 = ");
  mpfr_mul_ui(pi, state, 2, MPFR_RNDD);
  mpfr_out_str (stdout, 10, 0, pi, MPFR_RNDD);
  putchar('\n');

  mpfr_clear(pi);
  mpfr_clear(state);
  return 0;

  /*GtkApplication *app;
  int status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
  status = g_application_run(G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return status;*/
}
