#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>
#include "TDAS/List.h"
#include "TDAS/Map.h"
#include "TDAS/Extra.h"

//codigo para ejecutar el programa.    gcc -o tarea3 main.c TDAS/List.c TDAS/Map.c TDAS/Extra.c 
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
  char nombre[101];         // Nombre del item
  int valor;              // Valor del item
  int peso;               // Peso del item
} tipoItem;

typedef struct { 
  int tiempo_restante;        // Tiempo restante en el escenario
  List *inventario;           // Inventario del jugador (ítems recogidos)
  int peso_total;             // Peso total de los ítems en el inventario
  int puntaje_acumulado;      // Puntaje acumulado del jugador
  tipoGuardado *escenario_actual; // Escenario actual del jugador
} tipoJugador;

void limpiarPantalla() { system("clear"); }

void presioneTeclaParaContinuar() {
    puts("Presione una tecla para continuar...");
    getchar(); // Consume el '\n' del buffer de entrada
    getchar(); // Espera a que el usuario presione una tecla
}

int string_equal(void *key1, void *key2) {
  return strcmp((char *)key1, (char *)key2) == 0;
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
  for (tipoItem *item = list_first(estado_actual->escenario_actual->itemProcesados); item != NULL; item = list_next(estado_actual->escenario_actual->itemProcesados)) {
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

void reiniciar(Map *escenarios, tipoJugador *estado_actual) {
  // Liberar los escenarios y sus recursos
  MapPair *pair = map_first(escenarios);
  while (pair) {
      tipoGuardado *escenario = (tipoGuardado *)pair->value;

      // Liberar los ítems procesados
      if (escenario->itemProcesados) {
          for (tipoItem *item = list_first(escenario->itemProcesados); item != NULL; item = list_next(escenario->itemProcesados)) {
              free(item); // Liberar cada ítem
          }
          list_clean(escenario->itemProcesados); // Limpiar la lista
          free(escenario->itemProcesados);       // Liberar la lista
      }

      // Liberar los ítems originales
      if (escenario->items) {
          list_clean(escenario->items); // Limpiar la lista
          free(escenario->items);       // Liberar la lista
      }

      // Liberar el escenario
      free(escenario);
      pair = map_next(escenarios);
  }
  map_clean(escenarios); // Limpiar el mapa de escenarios

  // Liberar los recursos del jugador
  if (estado_actual) {
      if (estado_actual->inventario) {
          for (tipoItem *item = list_first(estado_actual->inventario); item != NULL; item = list_next(estado_actual->inventario)) {
              free(item); // Liberar cada ítem del inventario
          }
          list_clean(estado_actual->inventario); // Limpiar la lista
          free(estado_actual->inventario);       // Liberar la lista
      }
      free(estado_actual); // Liberar la estructura del jugador
  }
}


void leer_escenarios(Map *escenarios, tipoJugador *escenario_actual) {
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
    List *listaProcesada = NULL;
    if (list_first(escenario->items) != NULL) {
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
      }
    } 
    escenario->itemProcesados = listaProcesada;
    map_insert(escenarios, escenario->id, escenario);
  }
  fclose(archivo); // Cierra el archivo después de leer todas las líneas
  escenario_actual->tiempo_restante = 100; // Reinicia el tiempo restante
  escenario_actual->peso_total = 0; // Reinicia el peso total
  escenario_actual->puntaje_acumulado = 0; // Reinicia el puntaje acumulado
  escenario_actual->inventario = list_create(); // Reinicia el inventario
  MapPair *pair = map_search(escenarios, "1");
  if (pair != NULL) {
    escenario_actual->escenario_actual = (tipoGuardado *)pair->value;
  } else {
    printf("Error: Escenario con ID '1' no encontrado.\n");
    return;
  }
}


void recoger_items(tipoJugador *estado_actual, Map *escenarios) {
  limpiarPantalla();
  if (estado_actual->escenario_actual->itemProcesados == NULL || list_first(estado_actual->escenario_actual->itemProcesados) == NULL) {
      printf("No hay ítems disponibles en este escenario.\n");
      return;
  }
  printf("Ítems disponibles:\n");
  int index = 1;
  List *items = estado_actual->escenario_actual->itemProcesados;
  for (tipoItem *item = list_first(items); item != NULL; item = list_next(items)) {
      printf("%d) %s (%d pts, %d kg)\n", index++, item->nombre, item->valor, item->peso);
  }
  printf("Ingrese el números del ítem que desea recoger (0 para cancelar):\n");
  // Leer la entrada del usuario
  char entrada[256];
  getchar(); // Limpiar el buffer
  fgets(entrada, sizeof(entrada), stdin);
  // Procesar la entrada
  char *token = strtok(entrada, " ");
  while (token != NULL) {
      int opcion = atoi(token);
      if (opcion == 0) {
          printf("Operación cancelada.\n");
          return;
      }
      // Buscar el ítem correspondiente
      index = 1;
      for (tipoItem *item = list_first(items); item != NULL; item = list_next(items)) {
          if (index == opcion) {
              // Verificar si el peso total excede el límite (opcional)
              estado_actual->peso_total += item->peso;
              estado_actual->puntaje_acumulado += item->valor;
              estado_actual->tiempo_restante -= 1; // Descontar tiempo
              list_pushBack(estado_actual->inventario, item);
              list_popCurrent(items);
              printf("Has recogido el ítem: %s\n", item->nombre);
              break;
          }
          index++;
      }
      token = strtok(NULL, " ");
  }
  if (estado_actual->tiempo_restante <= 0) {
    printf("¡El tiempo se ha agotado! Has perdido.\n");
    printf("Puntaje total: %d\n", estado_actual->puntaje_acumulado);
    printf("Items en inventario:\n");
    for (tipoItem *item = list_first(estado_actual->inventario); item != NULL; item = list_next(estado_actual->inventario)) {
        printf("  - %s (%d pts, %d kg)\n", item->nombre, item->valor, item->peso);
    }
    printf("Reiniciando la partida...\n");
    reiniciar(escenarios, estado_actual);
    leer_escenarios(escenarios, estado_actual);
    return;
  }
}

void descartar_items(tipoJugador *estado_actual, Map *escenarios) {
  limpiarPantalla();
  if (estado_actual->inventario == NULL || list_first(estado_actual->inventario) == NULL) {
      printf("No hay ítems en el inventario para descartar.\n");
      return;
  }
  printf("Ítems en inventario:\n");
  int index = 1;
  List *inventario = estado_actual->inventario;
  for (tipoItem *item = list_first(inventario); item != NULL; item = list_next(inventario)) {
      printf("%d) %s (%d pts, %d kg)\n", index++, item->nombre, item->valor, item->peso);
  }
  printf("Ingrese el número del ítem que desea descartar (0 para cancelar):\n");
  // Leer la entrada del usuario
  char entrada[256];
  getchar(); // Limpiar el buffer
  fgets(entrada, sizeof(entrada), stdin);
  // Procesar la entrada
  char *token = strtok(entrada, " ");
  while (token != NULL) {
      int opcion = atoi(token);
      if (opcion == 0) {
          printf("Operación cancelada.\n");
          return;
      }
      // Buscar el ítem correspondiente
      index = 1;
      for (tipoItem *item = list_first(inventario); item != NULL; item = list_next(inventario)) {
          if (index == opcion) {
              // Actualizar estadísticas del jugador
              estado_actual->peso_total -= item->peso;
              estado_actual->puntaje_acumulado -= item->valor;
              estado_actual->tiempo_restante -= 1; // Descontar tiempo
              list_popCurrent(inventario); // Eliminar el ítem del inventario
              printf("Has descartado el ítem: %s\n", item->nombre);
              break;
          }
          index++;
      }
      token = strtok(NULL, " ");
  }
  if (estado_actual->tiempo_restante <= 0) {
    printf("¡El tiempo se ha agotado! Has perdido.\n");
    printf("Puntaje total: %d\n", estado_actual->puntaje_acumulado);
    printf("Items en inventario:\n");
    for (tipoItem *item = list_first(estado_actual->inventario); item != NULL; item = list_next(estado_actual->inventario)) {
        printf("  - %s (%d pts, %d kg)\n", item->nombre, item->valor, item->peso);
    }
    printf("Reiniciando la partida...\n");
    reiniciar(escenarios, estado_actual);
    leer_escenarios(escenarios, estado_actual);
    return;
  }
}



void avanzar_escenario(tipoJugador *estado_actual, Map *escenarios) {
  limpiarPantalla();
  printf("Direcciones disponibles:\n");
  // Mostrar las direcciones disponibles
  int contador = 0;
  printf("Escenario: %s\n", estado_actual->escenario_actual->id);
  if (strcmp(estado_actual->escenario_actual->arriba, "-1") != 0) {
      printf("  Arriba: %s\n", estado_actual->escenario_actual->arriba);
      contador++;
  }
  if (strcmp(estado_actual->escenario_actual->abajo, "-1") != 0) {
      printf("  Abajo: %s\n", estado_actual->escenario_actual->abajo);
      contador++;
  }
  if (strcmp(estado_actual->escenario_actual->izquierda, "-1") != 0) {
      printf("  Izquierda: %s\n", estado_actual->escenario_actual->izquierda);
      contador++;
  }
  if (strcmp(estado_actual->escenario_actual->derecha, "-1") != 0) {
      printf("  Derecha: %s\n", estado_actual->escenario_actual->derecha);
      contador++;
  }
  // Verificar si hay direcciones válidas
  if (contador == 0) {
      printf("No hay direcciones disponibles para avanzar.\n");
      return;
  }
  // Solicitar al usuario que elija una dirección
  printf("Ingrese el número de la dirección a la que desea avanzar (0 para cancelar): ");
  char opcion[10];
  scanf("%s", &opcion);
  if (strcmp(opcion, "0") == 0) {
      printf("Operación cancelada.\n");
      return;
  }
  // Validar la dirección seleccionada
  if (strcmp(estado_actual->escenario_actual->arriba, opcion) != 0 && strcmp(estado_actual->escenario_actual->abajo, opcion) != 0 && strcmp(estado_actual->escenario_actual->izquierda, opcion) != 0 && strcmp(estado_actual->escenario_actual->derecha, opcion) != 0) {
      printf("Dirección no válida. Intente nuevamente.\n");
      return;
  }
  // Calcular el tiempo usado
  int tiempo_usado = (int)ceil((estado_actual->peso_total + 1) / 10.0);
  estado_actual->tiempo_restante -= tiempo_usado;
  // Verificar si el tiempo se agotó
  if (estado_actual->tiempo_restante <= 0) {
      printf("¡El tiempo se ha agotado! Has perdido.\n");
      printf("Puntaje total: %d\n", estado_actual->puntaje_acumulado);
      printf("Items en inventario:\n");
      for (tipoItem *item = list_first(estado_actual->inventario); item != NULL; item = list_next(estado_actual->inventario)) {
          printf("  - %s (%d pts, %d kg)\n", item->nombre, item->valor, item->peso);
      }
      printf("Reiniciando la partida...\n");
      reiniciar(escenarios, estado_actual);
      leer_escenarios(escenarios, estado_actual);
      return;
  }
  // Actualizar el escenario actual
  MapPair *pair = map_search(escenarios, opcion);
  estado_actual->escenario_actual = (tipoGuardado *)pair->value;
  printf("Has avanzado a: %s\n", estado_actual->escenario_actual->room_name);
  printf("Descripción: %s\n", estado_actual->escenario_actual->description);
  // Verificar si se alcanzó el escenario final
  if (strcmp(estado_actual->escenario_actual->is_final, "Si") == 0) {
      printf("¡Has alcanzado el escenario final!\n");
      printf("Puntaje total: %d\n", estado_actual->puntaje_acumulado);
      printf("Tiempo restante: %d\n", estado_actual->tiempo_restante);
      printf("Items en inventario:\n");
      for (tipoItem *item = list_first(estado_actual->inventario); item != NULL; item = list_next(estado_actual->inventario)) {
          printf("  - %s (%d pts, %d kg)\n", item->nombre, item->valor, item->peso);
      }
      printf("Reiniciando la partida...\n");
      reiniciar(escenarios, estado_actual);
      leer_escenarios(escenarios, estado_actual);
  }
}



void iniciar_partida(Map *escenarios, tipoJugador *actual, List *escenarios_list) {
  char opcion;
  do {
    mostrarMenuPrincipal(actual);
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      recoger_items(actual, escenarios);
      break;
    case '2':
      descartar_items(actual, escenarios);
      break;
    case '3':
      avanzar_escenario(actual, escenarios);
      break;
    case '4':
      reiniciar(escenarios, actual);
      leer_escenarios(escenarios, actual);
      break;
    }
    presioneTeclaParaContinuar();
  } while (opcion != '5');
}

int main() {
  char opcion; // Variable para almacenar una opción ingresada por el usuario
  Map *escenarios = map_create(string_equal); // Lista para almacenar el escenario actual
  tipoJugador *estado_actual = malloc(sizeof(tipoJugador));
  List *escenarios_list = list_create(); // Lista para almacenar los escenarios
  do{
    mostrarMenuInicial();
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);
    switch (opcion) {
      case '1':
        leer_escenarios(escenarios, estado_actual); // Llama a la función para leer los escenarios desde el archivo CSV
        printf("Escenarios cargados correctamente.\n");
        presioneTeclaParaContinuar();
        break;
      case '2':
        iniciar_partida(escenarios, estado_actual, escenarios_list); // Llama a la función para iniciar la partida
        break;
    }
  }while (opcion != '3');
  list_clean(escenarios_list);
  limpiarPantalla();
  puts("========================================");
  puts("Gracias por usar el programa. ¡Hasta luego!");
  return 0;
}