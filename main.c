#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include "TDAS/List.h"
#include "TDAS/Map.h"
#include "TDAS/Extra.h"

//codigo para ejecutar el programa.    gcc -o tarea2 main.c TDAS/List.c TDAS/Map.c TDAS/Extra.c 
typedef struct {
  char id[50];           
  List *artists;
  char album_name[201];  
  char track_name[201];  
  int tempo;           
  char track_genre[101];  
} tipoCancion;

void limpiarPantalla() { system("clear"); }

void presioneTeclaParaContinuar() {
    puts("Presione una tecla para continuar...");
    getchar(); // Consume el '\n' del buffer de entrada
    getchar(); // Espera a que el usuario presione una tecla
}

// Función para mostrar el menú principal
void mostrarMenuPrincipal() {
  limpiarPantalla();
  puts("========================================");
  puts("     Base de Datos de Canciones");
  puts("========================================");

  puts("1) Cargar Canciones");
  puts("2) Buscar por id");
  puts("3) Buscar por artista");
  puts("4) Buscar por género");
  puts("5) Buscar por Tempo");
  puts("6) Salir");
}

// Función para cargar canciones desde un archivo CSV
void cargar_Canciones(HashMap *canciones_id, HashMap *canciones_genres, HashMap *canciones_artist, List *lista_lentas, List *lista_moderadas, List *lista_rapidas) {
  limpiarPantalla();
  // Abre el archivo CSV en modo lectura
  FILE *archivo = fopen("Data/song_dataset_.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }
  char **campos;
  // Leer y parsear una línea del archivo CSV. La función devuelve un arreglo de
  // strings, donde cada elemento del arreglo repesenta un campo de la linea csv.
  campos = leer_linea_csv(archivo, ',');
  // Lee cada línea del archivo CSV hasta el final y guarda los datos en las variables del struct
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    tipoCancion *Cancion = (tipoCancion *)malloc(sizeof(tipoCancion)); // Reserva memoria para una nueva canción
    strcpy(Cancion->id, campos[1]); // Almacena el id de la funckion utilizando la funcion strcpy para copiar el string       
    strcpy(Cancion->album_name, campos[3]); // Almacena el nombre del album utilizando la funcion strcpy para copiar el string     
    strcpy(Cancion->track_name, campos[4]); // Almacena el nombre de la cancion utilizando la funcion strcpy para copiar el string
    strcpy(Cancion->track_genre, campos[20]); // Almacena el genero de la cancion utilizando la funcion strcpy para copiar el string 
    Cancion->artists = split_string(campos[2], ";");  // Divide la cadena de artistas en una lista y se guarda la lista en el campo artists     
    Cancion->tempo = atoi(campos[18]); //se almacena el tempo de la cancion utilizando la funcion atoi para convertir el string a un entero
    // Agrega la cancion al mapa de canciones por ID
    insertMap(canciones_id, Cancion->id, Cancion);

    
    // Obtiene el genero de la cancion y verifica si este ya existe en el mapa de canciones por genero
    Pair *genre_pair = searchMap(canciones_genres, Cancion->track_genre);
    // Si el genero no existe en el mapa, crea una nueva lista para este genero, agrega la cancion a la lista e inserta la lista en el mapa por genero
    if (genre_pair == NULL) {
      List *new_list = list_create();
      list_pushBack(new_list, Cancion);
      insertMap(canciones_genres, Cancion->track_genre, new_list);
    }
    // Si el genero ya eciste en el mapa, se obtiene la lista y se agrega la cancion a la lista de este genero
    else {
      List *genre_list = (List *)genre_pair->value;
      list_pushBack(genre_list, Cancion);
    }
    // Obtiene el primer artista de la cancion
    char *artist = list_first(Cancion->artists);
    // Recorre la lista de artistas y verifica para cada artista si este ya existe en el mapa de canciones por artista
    // Si el artista no existe en el mapa, crea una nueva lista para este artista, agrega la cancion a la lista e inserta la lista en el mapa por artista
    // Si el artista ya existe en el mapa, se obtiene la lista y se agrega la cancion a la lista de este artista
    while (artist != NULL) {
        Pair *artist_pair = searchMap(canciones_artist, artist);
        if (artist_pair == NULL) {
            List *nueva_list = list_create();
            list_pushBack(nueva_list, Cancion);
            insertMap(canciones_artist, strdup(artist), nueva_list);
        } else {
            List *artist_list = (List *)artist_pair->value;
            list_pushBack(artist_list, Cancion);
        }
        artist = list_next(Cancion->artists);
    }
    // Clasifica la cancion en una de los tres tipos de tempo, si el tempo es menor a 60 se clasifica como lenta, si el tempo es mayor o igual a 60 y menor o igual a 120 se clasifica
    // como moderada, si el tempo es mayor a 120 se clasifica como rapida
    if (Cancion->tempo < 60) {
      list_pushBack(lista_lentas, Cancion); // Agrega a la lista de lentas
    } else if (Cancion->tempo >= 60 && Cancion->tempo <= 120) {
      list_pushBack(lista_moderadas, Cancion); // Agrega a la lista de moderadas
    } else {
      list_pushBack(lista_rapidas, Cancion); // Agrega a la lista de rápidas
    }
  }
  // Se muestra un mensaje indicando que las canciones fueron cargadas correctamente y cierra el archivo despues de leer todas las lineas
  puts("========================================");
  puts("Las canciones han sido cargadas correctamente.");
  fclose(archivo);
}

// Funcion para buscar una cancion por id
void buscar_id(HashMap *canciones_id) {
  limpiarPantalla();
  puts("========================================");
  // Se declara una variable de tipo char para almacenar el id buscado
  // Se le pide al usuario que ingrese el id de la cancion que desea buscar y se almacena en la variable id
  char id[50];
  printf("Ingrese el ID de la canción: ");
  scanf("%s", id);
  // Se busca la cancion en el mapa de canciones por id utilizando la funcion searchMap
  Pair *pair = searchMap(canciones_id, id);
  // Si la cancion existe en el mapa, se obtiene la cancion y se imprimen sus datos
  if (pair != NULL) {
    tipoCancion *cancion = pair->value;
    printf("ID: %s, Álbum: %s, Titulo: %s, Género: %s, Tempo: %d\n", cancion->id, cancion->album_name, cancion->track_name, cancion->track_genre ,cancion->tempo);
    // Se recorre la lista de artistas de la cancion imprimiendo el nombre de cada artista
    printf("Artista(s): ");
    for(char *artista = (char *) list_first(cancion->artists); artista != NULL; artista = list_next(cancion->artists)){
      printf("%s    ", artista);
    }
    printf("\n");
  }
  // Si la cancion no existe en el mapa, se imprime un mensaje indicando que no se encontro alguna cancion con ese id 
  else {
    puts("No se encontró ninguna canción con ese ID.");
  }
}

// Función para buscar una cancion por genero
void buscar_genero(HashMap *canciones_genres) {
  limpiarPantalla();
  puts("========================================");
  // Se declara una variable de tipo char para almacenar el genero buscado
  // Se le pide al usuario que ingrese el genero buscado y se almacena en la variable genero
  char genero[50];
  printf("Ingrese el género de la canción: ");
  scanf("%s", genero);
  puts("Canciones encontradas:");
  // Se busca la lista de canciones del genero en el mapa de canciones por genero utilizando la funcion searchmap
  Pair *pair = searchMap(canciones_genres, genero);
  // Si la lista de canciones del genero existe en el mapa, se obtiene la lista y se recorre la lista imprimiendo los datos de cada cancion
  if (pair != NULL) {
    List *lista = (List *)pair->value;
    tipoCancion *cancion = list_first(lista);
    while (cancion != NULL) {
      printf("ID: %s, Álbum: %s, Titulo: %s, Género: %s, Tempo: %d\n", cancion->id, cancion->album_name, cancion->track_name, cancion->track_genre ,cancion->tempo);
      // Se recorre la lista de artistas de la cancion imprimiendo el nombre de cada artista
      printf("Artista(s): ");
      for(char *artista = (char *) list_first(cancion->artists); artista != NULL; artista = list_next(cancion->artists)){
        printf("%s    ", artista);
      }
      printf("\n");
      cancion = list_next(lista); // Avanza al siguiente elemento en la lista
    }
  } else {
    puts("No se encontró ninguna canción con ese género.");
  }
}

// Función para buscar canciones por artista
void buscar_artista(HashMap *canciones_artist) {
  limpiarPantalla();
  puts("========================================");
  // Se declara una variable de tipo char para almacenar el artista buscado
  // Se le pide al usuario que ingrese el artista buscado y se almacena en la variable artista
  char artista[50];
  printf("Ingrese el artista: ");
  scanf(" %[^\n]50s", artista);
  puts("Canciones encontradas:");
  // Se busca la lista de canciones del artista en el mapa de canciones por artista utilizando la funcion searchMap
  Pair *pair = searchMap(canciones_artist, strdup(artista));
  // Si la lista de canciones del artista existe en el mapa, se obtiene la lista y se recorre la lista imprimiendo los datos de cada cancion del artista
  if (pair != NULL) {
    List *lista = (List *)pair->value;
    tipoCancion *cancion = list_first(lista);
    while (cancion != NULL) {
      printf("ID: %s, Álbum: %s, Titulo: %s, Género: %s, Tempo: %d\n", cancion->id, cancion->album_name, cancion->track_name, cancion->track_genre ,cancion->tempo);
      printf("Artista(s): ");
      for(char *artista = (char *)list_first(cancion->artists); artista != NULL; artista = list_next(cancion->artists)){
        printf("%s   ", artista);
      }
      printf("\n");
      cancion = list_next(lista); // Avanza al siguiente elemento en la lista
    }
    printf("\n");
  } 
  // Si la lista de canciones del artista no existe en el mapa, se imprime un mensaje indicando que no se encontro ninguna cancion con ese artista
  else {
    puts("No se encontró ninguna canción de ese artista.");
  }
}

// Función para buscar canciones por tempo
void buscar_tempo(List *lista_lentas, List *lista_moderadas, List *lista_rapidas) {
  limpiarPantalla();
  // Se declara una variable de tipo int para almacenar el tempo deseado
  // Se le pide al usuario que ingrese el tempo deseado y se almacena en la variable tempo_deseado
  int tempo_deseado;
  puts("========================================");
  printf("Velocidad de tempo deseada(1 = lenta, 2 = moderada, 3 = rapida): ");
  scanf("%d", &tempo_deseado);
  // Se verifica si el tempo deseado es valido, si no es valido se imprime un mensaje indicando que la opcion no es valida
  // y se retorna de la funcion sin hacer nada
  if (tempo_deseado < 1 || tempo_deseado > 3) {
    puts("Opción no válida. Por favor, elija entre 1 y 3.");
    return;
  }
  // Si el tempo deseado es 1, se imprime la lista de canciones lentas, mostrando los datos de cada cancion
  if (tempo_deseado == 1) {
    puts("Canciones lentas:");
    tipoCancion *cancion = list_first(lista_lentas);
    while (cancion != NULL) {
      printf("ID: %s, Álbum: %s, Titulo: %s, Género: %s, Tempo: %d\n", cancion->id, cancion->album_name, cancion->track_name, cancion->track_genre ,cancion->tempo);
      printf("Artista(s): ");
      for(char *artista = (char *) list_first(cancion->artists); artista != NULL; artista = list_next(cancion->artists)){
        printf("%s    ", artista);
      }
      printf("\n");
      cancion = list_next(lista_lentas); // Avanza al siguiente elemento en la lista
    }
  }
  // Si el tempo deseado es 2, se imprime la lista de canciones moderadas, mostrando los datos de cada cancion
  else if (tempo_deseado == 2) {
    puts("Canciones moderadas:");
    tipoCancion *cancion = list_first(lista_moderadas);
    while (cancion != NULL) {
      printf("ID: %s, Álbum: %s, Titulo: %s, Género: %s, Tempo: %d\n", cancion->id, cancion->album_name, cancion->track_name, cancion->track_genre ,cancion->tempo);
      printf("Artista(s): ");
      for(char *artista = (char *) list_first(cancion->artists); artista != NULL; artista = list_next(cancion->artists)){
        printf("%s    ", artista);
      }
      printf("\n");
      cancion = list_next(lista_moderadas); // Avanza al siguiente elemento en la lista
    }

  } 
  // Si el tempo deseado es 3, se imprime la lista de canciones rapidas, mostrando los datos de cada cancion
  else {
    puts("Canciones rápidas:");
    tipoCancion *cancion = list_first(lista_rapidas);
    while (cancion != NULL) {
      printf("ID: %s, Álbum: %s, Titulo: %s, Género: %s, Tempo: %d\n", cancion->id, cancion->album_name, cancion->track_name, cancion->track_genre ,cancion->tempo);
      printf("Artista(s): ");
      for(char *artista = (char *) list_first(cancion->artists); artista != NULL; artista = list_next(cancion->artists)){
        printf("%s    ", artista);
      }
      printf("\n");
      cancion = list_next(lista_rapidas); // Avanza al siguiente elemento en la lista
    }
  }
}

int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario

  // se crean los mapas y listas necesarias para almacenar las canciones
  HashMap *canciones_id = createMap(160000); // Mapa para almacenar canciones por ID
  HashMap *canciones_genres = createMap(160000); // Mapa para almacenar canciones por género
  HashMap *canciones_artist = createMap(160000); // Mapa para almacenar canciones por artista
  List *lista_lentas = list_create(); // Lista para almacenar canciones lentas
  List *lista_moderadas = list_create(); // Lista para almacenar canciones moderadas
  List *lista_rapidas = list_create(); // Lista para almacenar canciones rápidas

  //se muestra el menu principal y se le pide al usuario que ingrese una opcion
  // Se utiliza un bucle do-while para mostrar el menú y ejecutar la opción seleccionada por el usuario hasta que el usario elija salir
  do {  
    mostrarMenuPrincipal();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      // Carga las canciones desde el archivo CSV y las almacena en los mapas y listas
      cargar_Canciones(canciones_id, canciones_genres, canciones_artist, lista_lentas, lista_moderadas, lista_rapidas);
      break;
    case '2':
      // Busca una canción por ID
      buscar_id(canciones_id);
      break;
    case '3':
      // Busca canciones por artista
      buscar_artista(canciones_artist);
      break;
    case '4':
      // Busca canciones por género
      buscar_genero(canciones_genres);
      break;
    case '5':
      // Busca canciones por tempo
      buscar_tempo(lista_lentas, lista_moderadas, lista_rapidas);
      break;
    }
  presioneTeclaParaContinuar();
  } while (opcion != '6');
  
  // Libera la memoria utilizada
  list_clean(lista_lentas);
  list_clean(lista_moderadas);
  list_clean(lista_rapidas);
  limpiarPantalla();
  // Se imprime un mensaje de despedida
  puts("========================================");
  puts("Gracias por usar el programa. ¡Hasta luego!");
  return 0;
}
  