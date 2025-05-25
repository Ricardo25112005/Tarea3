GraphQuest
=====
---
Para esta Programa de base de datos de canciones se implementaran un tipo grafo guardado en un **Mapa** tener todas las salas guardadas con su id, ademas de implementar **Listas**  para organizar cada item de cada escenario, item del inventario, entre otros, esto utilizando las siguiente estructura:
````c
typedef struct Node Node;
typedef struct List List;

struct Node{
    void * ticket; 
    Node * next;
    Node * prev;
};

struct List{
    Node * head;
    Node * tail;
    Node * current;
};
````
````c
typedef struct HashMap HashMap;
typedef struct Pair Pair;

typedef struct Pair {
     char * key;
     void * value;
} Pair;

struct HashMap {
    Pair ** buckets;
    long size;
    long capacity;
    long current;
};
````
para cada escenario se utiliza el siguiente struct:
````c
typedef struct {
  char id[10];           
  char room_name[501];
  char description[501];
  char abajo[10];
  char arriba[10];
  char izquierda[10];
  char derecha[10];       
  List *items; 
  List *itemProcesados;
  char is_final[10]; 
} tipoGuardado;
````
Para cada item del escenario se tiene el siguiente struct:
````c
typedef struct {
  char nombre[101];
  int valor;
  int peso;
} tipoItem;
````
Para el jugador se utiliza el siguiente struct:
````c
typedef struct { 
  int tiempo_restante;
  List *inventario;
  int peso_total;
  int puntaje_acumulado;
  tipoGuardado *escenario_actual;
  List *escenariosVisitados;
} tipoJugador;
````
---
Este programa tiene como finalidad crear un juego en el cual tenemor que ir avanzando entre salas, recoger o descartar items para generar una mayor puntacion, teniendo en cuenta el tiempo, el cual si se acaba se reinicia la partida automaticamente. El programa se divide en 2 secciones:

1. Cargado de escenarios: En esta parte se llama a la funcion `void leer_escenarios(Map *escenarios, tipoJugador *escenario_actual)`, funcion la cual recive el mapa y el estado actual el cual es tipo jugador, el programa abre el archivo csv llamado graphquest.csv, lee cada linea del csv y almacena las variables dentro de un auxiliar, luego almacena el escenario dentro del mapa de escenarios por ID utilizando la funcion `map_insert`, luego inicializa el tipoJugador escenario actual, con un tiempo de 30, peso y puntaje acumulado igual a 0, crea la lista de inventario, para finalmente mostrar un mensaje de que se cargaron los escenarios. 

2. Inicializar Partida: En esta seccion la funcion a llamar es `void iniciar_partida(Map *escenarios, tipoJugador *actual, List *escenarios_list)` la cual nos mueve a un nuevo menu con varias opciones, ademas de mostrar el estado actual del jugador, esto principalmente para saber donde estamos ubicados y cuantos objetos tenemos y el puntaje total del jugador, el menu esta compuesto por 4 secciones:

    1. Recoger Item: en esta parte se llama a la funcion `void recoger_items(tipoJugador *estado_actual, Map *escenarios)` la cual muestra todos los objetos que contiene el escenario actual, luego se pide que se ingrese el numero del item que se quiere recoger (Al mostrar los Items se les coloca un numero del uno en adelante), si se encuentra el item este es agregado al inventario del jugador y se elimina de la lista de itemsProcesados del escenario, se descuenta 1 del tiempo restante.
    
    2. Descartar Item: se llama a la funcion `void descartar_items(tipoJugador *estado_actual, Map *escenarios)` la cual muestra todos los item que estan almacenados en el inventario del jugador, luego se pide al jugador que ingrese el numero del item que quiere descartar (Al mostrar los Items se les coloca un numero del uno en adelante), si se encuentra el item este es eliminado del inventario del jugador, sin agregarlo nuevamente al escenario, se descuenta 1 del tiempo restante.
    
    3. Avanzar Escenario: en esta parte se llama a la funcion `void avanzar_escenario(tipoJugador *estado_actual, Map *escenarios)` muestra el id de los escenarios a los que se puede acceder desde el escenario actual, se le pide al usuario que ingrese el id de la sala a la cual se quiere mover, se verifica que sea una direccion valida, si la direccion es valida se mueve el escenario actual del jugador al nuevo escenario escogido con el jugador, disminuyendo su tiempo con un redondeo al superior del peso del inventario mas 1 dividido en 10.
   
    4. Reiniciar Escenario: es esta seccion se llama a la funcion `void reiniciar(Map *escenarios, tipoJugador *estado_actual)` la cual reinicia las listas de items de cada escenario, limpia el mapa de escenario y el estado actual del jugador, esto para volver a leer los escenarios para que todos queden en su estado inicial.
       
Luego para compilar y ejecutar:

    gcc -o tarea3 main.c TDAS/List.c TDAS/Map.c TDAS/Extra.c
    ./tarea3 
