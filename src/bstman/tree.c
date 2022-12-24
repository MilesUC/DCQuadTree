#include "tree.h"

Node* node_create(int value){
    Node* node = (Node*) calloc(1, sizeof(Node));

    node->value = value;
    return node;

}

void insert_node(Node* root, Node* node){
    Node* actual_node = root;
    bool is_inserted = false;

    while (!is_inserted) 
    {
        if (actual_node->value > node->value){
            if (actual_node->left_child){
                actual_node = actual_node->left_child;
            }
            else{
                actual_node->left_child = node;
                is_inserted = true;
            }
        }
        else{
            if (actual_node->right_child){
                actual_node = actual_node->right_child;
            }
            else{
                actual_node->right_child = node;
                is_inserted = true;
            }
        }
    }   
}

void print_tree(Node* root){
    printf("%d ", root->value);

    if (root->left_child){
        print_tree(root->left_child);
    }
    if (root->right_child){
        print_tree(root->right_child);
    }
}

void print_path(Node* root, int value, FILE* output_file){

    fprintf(output_file, "%d ", root->value);

    if (root->value != value){
        if (root->value > value && root->left_child){
            print_path(root->left_child, value, output_file);
        }
        else if (root->value < value && root->right_child){
            print_path(root->right_child, value, output_file);
        }
        else{
            fprintf(output_file, "X");
        }
    }
}

int height(Node* root, int value){

    if (root->value == value){
        return 0; 
    }

    else{

        if (root->value > value && root->left_child){
            return 1 + height(root->left_child, value);
        }
        else if (root->value < value && root->right_child){
            return 1 + height(root->right_child, value);
        }
        else {
            return 0;
        }
    }
}

void in_order(Node* root, FILE* output_file){
    if (root){
        in_order(root->left_child, output_file);
        fprintf(output_file, "%d ", root->value);
        in_order(root->right_child, output_file);
    }
}

int equal(Node* root_1, Node* root_2){
    if (root_1 || root_2){
        return (root_1 && root_2 && root_1->value == root_2->value 
        && equal(root_1->left_child, root_2->left_child)
        && equal(root_1->right_child, root_2->right_child));
    }
    else{
        return 1;
    }
}

/* BONUS IMPLEMENTADO */
int sub_in(Node* root, Node* root_sub){
    if (root){
        if (equal(root, root_sub) == 1){
            return 1;
        }
        return sub_in(root->left_child, root_sub) + sub_in(root->right_child, root_sub);
    }
}


