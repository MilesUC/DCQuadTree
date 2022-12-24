//#pragma once
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>


typedef struct node_t {
    int value;
    struct node_t *left_child;
    struct node_t *right_child;
} Node;

Node* node_create(int value);

void insert_node(Node* root, Node* node);

void print_tree(Node* root);

void print_path(Node* root, int value, FILE* output_file);

int height(Node* root, int value);

void in_order(Node* root, FILE* output_file);

int equal(Node* root_1, Node* root_2);

int sub_in(Node* root, Node* root_sub);