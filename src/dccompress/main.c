#include "../imagelib/imagelib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <stdbool.h>

/** Retorna true si ambos strings son iguales */
bool string_equals(char* string1, char* string2) {
  return !strcmp(string1, string2);
}

/** Revisa que los parametros del programa sean válidos */
bool check_arguments(int argc, char** argv) {
  if(argc != 5) {
    printf("Modo de uso: %s INPUT OUTPUT MODE PARAM\n", argv[0]);
    printf("Donde:\n");
    printf("\tINPUT es la ruta a la imagen .png a procesar\n");
    printf("\tOUTPUT es la ruta donde se guardara la imagen .png resultante\n");
    printf("\tMODE es el modo de operacion del programa:\n");
    printf("\t\tMODE = filter filtra la imagen usando un QuadTree, \n\t\tdejando como hoja todos los nodos \n\t\tcuya desviación estandar sea menor a PARAM\n");
    printf("\t\tMODE = compress busca la desviación estandar entera\n\t\tmás baja tal que la cantidad de hojas \n\t\tal filtrar sea inferior a PARAM, \n\t\tusando el valor encontrado para filtrar la imagen\n");
    return false;
  }
  /* Revisa que PARAM sea válido segun el modo */
  else {
    /* Si estamos filtrando */
    if (string_equals(argv[3], "filter")) {
      /* PARAM es una desviacion estandar. Esta no puede ser negativa */
      if (atof(argv[4]) < 0) {
        printf("Límite inválido: %s", argv[4]);
        return false;
      }
    }
    /* Si estamos comprimiendo */
    else if (string_equals(argv[3], "compress")) {
      /* Param es una cantidad de nodos, por lo que no puede ser menor a 1 */
      if (atoi(argv[4]) <= 0) {
        printf("Cantidad máxima de hojas inválida: %s", argv[4]);
        return false;
      }
    } else {
      printf("Modo inválido: %s\n", argv[3]);
      return false;
    }
  }
  return true;
}

int main(int argc, char** argv) {
  /* Si los parámetros del programa son inválidos */
  if(!check_arguments(argc, argv)) {
    /* Salimos del programa indicando que no terminó correctamente */
    return 1;
  }

  /* Ruta del archivo PNG de input */
  char* INPUT = argv[1];
  /* La imagen de input */
  Image* img = img_png_read_from_file(INPUT);

  /* Momento en el que empezamos a procesar la imagen */
  clock_t start = clock();


  /* Creamos el árbol con la imagen */
  QuadNode* nodo = arbol_init(img, 0, img->width - 1, 0, img->height - 1);
  desv_est1(nodo);

  /* Modo de uso del programa */
  char* MODE = argv[3];
  /* El parametro correspondiente */
  char* PARAM = argv[4];

  /* La desviación estandar máxima permitida para un nodo */
  double alpha;

  /* Si estamos en modo filtrar */
  if (string_equals(MODE, "filter")) {
    /* Entonces usaremos PARAM como límite */
    alpha = atof(PARAM);

  }
  /* Si estamos en modo comprimir */
  else if (string_equals(MODE, "compress")) {
    /* Cantidad máxima de hojas que le permitiremos tener al árbol */
    int h = atoi(PARAM);

    /* Buscar el menor alpha tal que el árbol filtrado tiene menos de h hojas */
    alpha = compression1(nodo, img, h, 0, 128);   //O 256
  }

  /* Una vez definido el alpha, podemos filtrar la imagen */
  filter_alpha1(nodo, alpha, img, 1);

  //paint_image(img, nodo);


  // Pintamos un cuadrado de ejemplo //
  // Para tu tarea usa este ejemplo para pintar las imagenes //
  /*
  img_square_paint(
    img, // Imagen donde se pintara el cuadrado
    img -> height / 3, // Fila donde comienza el cuadrado
    img -> width / 4, // Columna donde comienza el cuadrado
    img -> height / 5, // Largo del lado del cuadrado
    (Color) {.L = 46.97, .a = 38.96, .b = -69.98} // Color a pintar
  );
  */
  // Fin ejemplo //

  /* Momento en el que terminamos de procesar la imagen */
  clock_t end = clock();

  /* Tiempo que usamos en procesar la imagen */
  double elapsed = ((double)end - (double)start) / CLOCKS_PER_SEC;
  printf("Tiempo en que tomó procesar la imagen: %lf segundos\n", elapsed);

  /* Ruta del archivo PNG de input */
  char* OUTPUT = argv[2];

  /* Escribimos la imagen modificada al archivo de salida */
  img_png_write_to_file(img, OUTPUT);

  /* Liberamos la memoria del árbol */
  free_tree(nodo);

  /* Liberamos la memoria de la imagen */
  img_png_destroy(img);

  return 0;
}
