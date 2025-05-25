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

    3. Mostrar canciones del artista: en esta parte se llama a la funcion `void buscar_artista(HashMap *canciones_artist)` la cual lee el nombre del artista al cual le queremos buscar sus canciones, busca en el mapa de artista si se encuentra el artista, en caso de encontrar el artista, se muestra en pantalla la lista con todas las canciones del artista y las canciones con sus respectivas caracteristicas, si no se encuentra el artista se muestra un mensaje indicando que no hay canciones de ese artista.
    
    4. Mostrar canciones del genero: se llama a la funcion `void buscar_genero(HashMap *canciones_genres)` la cual lee el genero el cual queremos buscar, buscando si se encuentra el genero en el mapa de canciones por genero, en caso de encontrar el genro se mustra la lista de las canciones del genero con sus respectivas caracteristicas, si no se encuentra el genero se muestra en pantalla un mensaje indicando que no se encontraron canciones con ese genero.
    
    5. Mostrar canciones por tempo: en esta parte se llama a la funcion `void buscar_tempo(List *lista_lentas, List *lista_moderadas, List *lista_rapidas)` la cual muestra lee el tempo deseado en formato de numero (1 para lentas, 2 para moderadas y 3 para rapidas) y muestra la lista de canciones con ese tempo, mostrando las caracteristicas de cada cancion, en caso de ingresar un numero no relacionado relacionados a las listas se muestra un mensaje indicando que la opcion no es valida y regresa al menu principal.
       
Luego para compilar y ejecutar:

    gcc -o tarea2 main.c TDAS/List.c TDAS/Map.c TDAS/Extra.c
    ./tarea2 

En caso de que el programa al mostrar canciones por tempo de segmentation fault reejecutar el programa(el error ocurre muy pocas veces, casi nunca).
