#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "tree.h"

/* Retorna true si ambos strings son iguales */
bool string_equals(char *string1, char *string2) {
  return !strcmp(string1, string2);
}

/* Revisa que los parametros del programa sean válidos */
bool check_arguments(int argc, char **argv) {
  if (argc != 3) {
    printf("Modo de uso: %s INPUT OUTPUT\n", argv[0]);
    printf("Donde:\n");
    printf("\tINPUT es la ruta del archivo de input\n");
    printf("\tOUTPUT es la ruta del archivo de output\n");
    exit(1);
  }

  return true;
}


int main(int argc, char** argv) {
  check_arguments(argc, argv);

  FILE* input_file = fopen(argv[1], "r");
  FILE* output_file = fopen(argv[2], "w");

  int node_count;
  int query_count;

  fscanf(input_file, "%d", &node_count);

  /* Definimos array de nodos */
  Node* array_nodes[node_count];

  /* Definimos la raíz y nodo */
  Node* root;
  Node* node;

  /* leemos Cada nodo */
  int k;
  for (int i=0; i<node_count; i++) {
    fscanf(input_file, "%d", &k);
    printf("%d ", k);

    if (i == 0){ // Creamos la raíz
      root = node_create(k);
      array_nodes[i] = root;
    }
    else{ // Creamos nuevo nodo hijo
      node = node_create(k);
      array_nodes[i] = node;
      insert_node(root, node);
    }
  }
  printf("\n");

  fscanf(input_file, "%d", &query_count);
  /* leemos las consultas */
  char command[32];
  int value;
  for (int i=0; i<query_count; i++) {
    fscanf(input_file, "%s %d", command, &value);
    printf("%s %d\n", command, value);
    /* completar la revision de comando y ejecucion de los mismos */

    if (string_equals(command, "PATH")){
      print_path(root, value, output_file);
      fprintf(output_file, "\n");
    }
    

    else if (string_equals(command, "DEEP")){
      fprintf(output_file, "%d", height(root, value));
      fprintf(output_file, "\n");
    }

    else if (string_equals(command, "ORDER")){
      in_order(root, output_file);
      fprintf(output_file, "\n");
    }

    
    else if (string_equals(command, "SUBTREE")){
      Node* root_sub;
      Node* node_sub;
      int w;
      for (int j=0; j<value; j++) {
        fscanf(input_file, "%d", &w);
        if (j == 0){ 
          root_sub = node_create(w);} 
        else{ 
          node_sub = node_create(w);
          insert_node(root_sub, node_sub);} }

      fprintf(output_file, "%d\n", sub_in(root, root_sub));
    }

  }

  /* Liberamos memoria */

  for (int i = 0; i < node_count; i++){
    free(array_nodes[i]);
  }

  fclose(input_file);
  fclose(output_file);
  return 0;
}
