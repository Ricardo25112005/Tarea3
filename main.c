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
  char id[10];           
  char room_name[501];   
  char description[501]; 
  char abajo[10];
  char arriba[10];
  char izquierda[10];
  char derecha[10];           
  List *items; // Lista de items
  List *itemProcesados;
  char is_final[10]; // Indica si es un escenario final
} tipoGuardado;


typedef struct {
  char id[10];           
  char room_name[501];   
  char description[501]; 
  struct tipoEscenario *conexiones[4];          
  List *items; // Lista de items
  char is_final[10]; // Indica si es un escenario final
} tipoEscenario;

typedef struct {
  char nombre[101];         // Nombre del item
  int valor;              // Valor del item
  int peso;               // Peso del item
} tipoItem;

typedef struct { 
  int tiempo_restante;        // Tiempo restante en el escenario
  List *inventario;           // Inventario del jugador (ítems recogidos)
  int peso_total;             // Peso total de los ítems en el inventario
  int puntaje_acumulado;      // Puntaje acumulado del jugador
  tipoEscenario *escenario_actual; // Escenario actual del jugador
} tipoJugador;

void limpiarPantalla() { system("clear"); }

void presioneTeclaParaContinuar() {
    puts("Presione una tecla para continuar...");
    getchar(); // Consume el '\n' del buffer de entrada
    getchar(); // Espera a que el usuario presione una tecla
}

// Función para mostrar el menú principal
void mostrarMenuPrincipal(tipoJugador *estado_actual) {
  limpiarPantalla();
  puts("========================================");
  printf("Estado actual:\n");
  printf("Tiempo restante: %d\n", estado_actual->tiempo_restante);
  printf("Peso total: %d\n", estado_actual->peso_total);
  printf("Puntaje acumulado: %d\n", estado_actual->puntaje_acumulado);
  printf("Escenario actual: %s\n", estado_actual->escenario_actual->room_name);
  printf("Descripción: %s\n", estado_actual->escenario_actual->description);
  printf("Items disponibles:\n");
  for (tipoItem *item = list_first(estado_actual->escenario_actual->items); item != NULL; item = list_next(estado_actual->escenario_actual->items)) {
    printf("  - %s (%d pts, %d kg)\n", item->nombre, item->valor, item->peso);
  }
  if (list_first(estado_actual->inventario) != NULL) {
    printf("Items en inventario:\n");
    for (tipoItem *item = list_first(estado_actual->inventario); item != NULL; item = list_next(estado_actual->inventario)) {
      printf("  - %s (%d pts, %d kg)\n", item->nombre, item->valor, item->peso);
    }
  }
  else {
    printf("No hay items en el inventario.\n");
  }
  puts("========================================");
  puts("     Opciones del jugador");
  puts("1) Recoger item");
  puts("2) Descartar item");
  puts("3) Avanzar en escenario");
  puts("4) Reiniciar partida");
  puts("5) Salir");
  puts("========================================");
}

void mostrarMenuInicial() {
  limpiarPantalla();
  puts("========================================");
  puts("     Opciones del juego");
  puts("========================================");
  puts("1) Leer escenarios");
  puts("2) Iniciar partida");
  puts("3) Salir");
}

void generar_conexiones_desde_lista(HashMap *escenarios,List *escenarios_list, tipoEscenario *escenario_actual) {
  for (tipoEscenario *escenario = list_first(escenarios_list); escenario != NULL; escenario = list_next(escenarios_list)) {
    if (strcmp(escenario->id, escenario_actual->id) == 0) {
      Pair *pair = searchMap(escenarios, escenario->id);
      tipoGuardado *guardado = (tipoGuardado *)pair->value;
      if (strcmp(guardado->arriba, "-1") != 0) {
          for (tipoEscenario *otro = list_first(escenarios_list); otro != NULL; otro = list_next(escenarios_list)) {
              if (strcmp(guardado->arriba, otro->id) == 0) {
                  escenario->conexiones[0] = (struct tipoEscenario *)otro;
                  break;
              }
          }
      } else {
          escenario->conexiones[0] = NULL;
      }
      if (strcmp(guardado->abajo, "-1") != 0) {
          for (tipoEscenario *otro = list_first(escenarios_list); otro != NULL; otro = list_next(escenarios_list)) {
              if (strcmp(guardado->abajo, otro->id) == 0) {
                  escenario->conexiones[1] = (struct tipoEscenario *)otro;
                  break;
              }
          }
      } else {
          escenario->conexiones[1] = NULL;
      }
      if (strcmp(guardado->izquierda, "-1") != 0) {
          for (tipoEscenario *otro = list_first(escenarios_list); otro != NULL; otro = list_next(escenarios_list)) {
              if (strcmp(guardado->izquierda, otro->id) == 0) {
                  escenario->conexiones[2] = (struct tipoEscenario *)otro;
                  break;
              }
          }
      } else {
          escenario->conexiones[2] = NULL;
      }
      if (strcmp(guardado->derecha, "-1") != 0) {
          for (tipoEscenario *otro = list_first(escenarios_list); otro != NULL; otro = list_next(escenarios_list)) {
              if (strcmp(guardado->derecha, otro->id) == 0) {
                  escenario->conexiones[3] = (struct tipoEscenario *)otro;
                  break;
              }
          }
      } else {
          escenario->conexiones[3] = NULL;
      }
    }
  }
}

// Función para cargar canciones desde un archivo CSV
void leer_escenarios(HashMap *escenarios, tipoJugador *escenario_actual, List *escenarios_list) {
  // Intenta abrir el archivo CSV que contiene datos de películas
  FILE *archivo = fopen("Data/graphquest.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo"); // Informa si el archivo no puede abrirse
    return;
  }

  char **campos;
  // Leer y parsear una línea del archivo CSV. La función devuelve un array de
  // strings, donde cada elemento representa un campo de la línea CSV procesada.
  campos = leer_linea_csv(archivo, ','); // Lee los encabezados del CSV
  int contador = 1;

  // Lee cada línea del archivo CSV hasta el final
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    tipoGuardado *escenario = malloc(sizeof(tipoGuardado));
    if (escenario == NULL) {
      perror("Error al asignar memoria para el escenario");
      fclose(archivo);
      return;
    }
    strcpy(escenario->id, campos[0]);
    strcpy(escenario->room_name, campos[1]);
    strcpy(escenario->description, campos[2]);
    escenario->items = split_string(campos[3], ";");
    strcpy(escenario->arriba, campos[4]);
    strcpy(escenario->abajo, campos[5]);
    strcpy(escenario->izquierda, campos[6]);
    strcpy(escenario->derecha, campos[7]);
    strcpy(escenario->is_final, campos[8]);
    printf("ID: %s\n", escenario->id);
    printf("Nombre: %s\n", escenario->room_name);
    printf("Descripción: %s\n", escenario->description);

    List *listaProcesada = NULL;
    if (list_first(escenario->items) != NULL) {
      printf("Items: \n");
      listaProcesada = list_create();
      for(char *item = list_first(escenario->items); item != NULL; item = list_next(escenario->items)){
        tipoItem *itemProcesado = malloc(sizeof(tipoItem));
        List* values = split_string(item, ",");
        char* item_name = list_first(values);
        strcpy(itemProcesado->nombre, item_name);
        int item_value = atoi(list_next(values));
        itemProcesado->valor = item_value;
        int item_weight = atoi(list_next(values));
        itemProcesado->peso = item_weight;
        list_pushBack(listaProcesada, itemProcesado);
        printf("  - %s (%d pts, %d kg)\n", item_name, item_value, item_weight);
    }
    } else {
      printf("No hay items disponibles.\n");
    }
    escenario->itemProcesados = listaProcesada;
    if (strcmp(escenario->arriba, "-1") != 0) printf("Arriba: %s\n", escenario->arriba);
    if (strcmp(escenario->abajo, "-1") != 0) printf("Abajo: %s\n", escenario->abajo);
    if (strcmp(escenario->izquierda, "-1") != 0) printf("Izquierda: %s\n", escenario->izquierda);
    if (strcmp(escenario->derecha, "-1") != 0) printf("Derecha: %s\n", escenario->derecha);
    if (strcmp(escenario->is_final, "Si") == 0) printf("Es final\n");
    else printf("No es final\n");
    printf("========================================\n");
    insertMap(escenarios, escenario->id, escenario);

  }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas
  Pair *pair = firstMap(escenarios);
  while (pair != NULL) {
    // Crear un nuevo tipoEscenario
    tipoGuardado *actual = (tipoGuardado *)pair->value;
    tipoEscenario *nuevo = malloc(sizeof(tipoEscenario));
    strcpy(nuevo->id, actual->id);
    printf("ID: %s\n", nuevo->id);
    strcpy(nuevo->room_name, actual->room_name);
    strcpy(nuevo->description, actual->description);
    strcpy(nuevo->is_final, actual->is_final);

    if (actual->itemProcesados != NULL) {
        nuevo->items = actual->itemProcesados;
        printf("Items: \n");
        for(char *item = list_first(actual->itemProcesados); item != NULL; item = list_next(actual->itemProcesados)){
          tipoItem *itemProcesado = (tipoItem *)item;
          printf("  - %s (%d pts, %d kg)\n", itemProcesado->nombre, itemProcesado->valor, itemProcesado->peso);
        }
    } else {
        nuevo->items = NULL;
    }
    for (int i = 0; i < 4; i++) {
        nuevo->conexiones[i] = NULL;
    }
    list_pushBack(escenarios_list, nuevo);
    pair = nextMap(escenarios);
  }
  for (tipoEscenario *escenario = list_first(escenarios_list); escenario != NULL; escenario = list_next(escenarios_list)) {
    generar_conexiones_desde_lista(escenarios, escenarios_list, escenario);
  }
  escenario_actual->tiempo_restante = 100; // Reinicia el tiempo restante
  escenario_actual->peso_total = 0; // Reinicia el peso total
  escenario_actual->puntaje_acumulado = 0; // Reinicia el puntaje acumulado
  escenario_actual->inventario = list_create(); // Reinicia el inventario
  for (tipoEscenario *escenario = list_first(escenarios_list); escenario != NULL; escenario = list_next(escenarios_list)) {
    if (strcmp(escenario->id, "1") == 0) {
      escenario_actual->escenario_actual = escenario;
      break;
    }
  }
  presioneTeclaParaContinuar();
}



void reiniciar_partida(tipoJugador *estado_actual, List *escenarios_list) {
  estado_actual->tiempo_restante = 100; // Reinicia el tiempo restante
  estado_actual->peso_total = 0; // Reinicia el peso total
  estado_actual->puntaje_acumulado = 0; // Reinicia el puntaje acumulado
  estado_actual->inventario = list_create(); // Reinicia el inventario
  estado_actual->escenario_actual = NULL; // Reinicia el escenario actual
}

void recoger_item(tipoJugador *estado_actual) {
  estado_actual->tiempo_restante -= 10; // Reduce el tiempo restante al recoger un item
  /*if (estado_actual->escenario_actual->items != NULL) {
    tipoItem *item = list_first(estado_actual->escenario_actual->items);
    if (item != NULL) {
      estado_actual->peso_total += item->peso;
      estado_actual->puntaje_acumulado += item->valor;
      list_pushBack(estado_actual->inventario, item);
      list_remove(estado_actual->escenario_actual->items, item);
      printf("Has recogido el item: %s\n", item->nombre);
    } else {
      printf("No hay items disponibles para recoger.\n");
    }
  } else {
    printf("No hay items disponibles en este escenario.\n");
  }*/
}

void iniciar_partida(HashMap *escenarios, tipoJugador *actual, List *escenarios_list) {
  char opcion;
  do {
    if (strcmp(actual->escenario_actual->is_final, "Si") == 0) {
      puts("se ha llegado al final del juego");
      printf("Puntaje total: %d\n", actual->puntaje_acumulado);
      printf("Tiempo restante: %d\n", actual->tiempo_restante);
      printf("Peso total: %d\n", actual->peso_total);
      printf("Items en inventario:\n");
      for (tipoItem *item = list_first(actual->inventario); item != NULL; item = list_next(actual->inventario)) {
        printf("  - %s (%d pts, %d kg)\n", item->nombre, item->valor, item->peso);
      }
      puts("Se Reiniciara la partida");
      presioneTeclaParaContinuar();
      reiniciar_partida(actual, escenarios_list);
      return;
    }
    mostrarMenuPrincipal(actual);
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      recoger_item(actual);
      break;
    case '2':
      //descartar_item(actual);
      break;
    case '3':
      //avanzar_escenario(actual);
      break;
    case '4':
      leer_escenarios(escenarios, actual, escenarios_list);
      //reiniciar_partida(actual, escenarios_list);
      puts("Partida reiniciada.");
      break;
    }
    presioneTeclaParaContinuar();
  } while (opcion != '5');
}

int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario
  HashMap *escenarios = createMap(20); // Lista para almacenar el escenario actual
  tipoJugador *estado_actual = malloc(sizeof(tipoJugador));
  List *escenarios_list = list_create(); // Lista para almacenar los escenarios
  do{
    mostrarMenuInicial();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);
    switch (opcion) {
      case '1':
        leer_escenarios(escenarios, estado_actual, escenarios_list); // Llama a la función para leer los escenarios desde el archivo CSV
        break;
      case '2':
        iniciar_partida(escenarios, estado_actual, escenarios_list); // Llama a la función para iniciar la partida
        break;
    }
  }while (opcion != '3');
  
  // Libera la memoria utilizada
  /*list_clean(lista_lentas);
  list_clean(lista_moderadas);
  list_clean(lista_rapidas);
  limpiarPantalla();
  */
  // Se imprime un mensaje de despedida
  limpiarPantalla();
  puts("========================================");
  puts("Gracias por usar el programa. ¡Hasta luego!");
  return 0;
}