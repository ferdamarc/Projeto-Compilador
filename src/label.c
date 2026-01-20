#include "synthesis.h"

/* Global vector for storing all labels */
label_vector_t * label_vector = NULL;

void initialize_labels() {
    label_vector = (label_vector_t *)malloc(sizeof(label_vector_t));
    
    if (label_vector == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for label vector\n");
        exit(EXIT_FAILURE);
    }
    
    label_vector->size = -1;
    label_vector->vector = NULL;
}


label_t * create_label_node(char* id, int address) {
    label_t * new_label_node = (label_t *)malloc(sizeof(label_t));
    
    if (new_label_node == NULL) {
        fprintf(stderr, "Error: Failed to allocate memory for new label\n");
        exit(EXIT_FAILURE);
    }
    
    new_label_node->id = strdup(id);
    if (new_label_node->id == NULL) {
        fprintf(stderr, "Error: Failed to duplicate label string\n");
        free(new_label_node);
        exit(EXIT_FAILURE);
    }
    
    new_label_node->address = address;
    new_label_node->next = NULL;
    
    return new_label_node;
}


void add_label(char* id, int address) {
    label_t * new_label_node = create_label_node(id, address);
    
    /* First label in the list */
    if (label_vector->size == -1) {
        label_vector->vector = new_label_node;
        label_vector->size = 0;
        return;
    }

    /* Find the end of the list and append */
    label_t * current = label_vector->vector;
    while (current->next != NULL) {
        current = current->next;
    }
    
    current->next = new_label_node;
    label_vector->size++;
}


int get_label_address(char* id) {
    label_t * current = label_vector->vector;
    
    while (current != NULL) {
        if (strcmp(current->id, id) == 0) {
            return current->address;
        }
        current = current->next;
    }
    
    /* Label not found */
    return -1;
}


void free_labels() {
    label_t * current = label_vector->vector;
    
    while (current != NULL) {
        label_t * next = current->next;
        free(current->id);
        free(current);
        current = next;
    }
    
    free(label_vector);
    label_vector = NULL;
}


void print_labels() {
    label_t * current = label_vector->vector;

    printf("\n============= Label Vector =============\n");
    
    while (current != NULL) {
        printf("%-30s Address: %d\n", current->id, current->address);
        current = current->next;
    }
    
    printf("========================================\n");
}