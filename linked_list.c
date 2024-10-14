#include "linked_list.h"
#include "memory_manager.h"

void list_init(Node** head, size_t size) {
    mem_init(size);
    if (head == NULL) {
        return;
    }
    
    if (size == 0) {
        *head = NULL;
        return;
    }
    
    //*head = (Node*)mem_alloc(sizeof(Node));
}

void list_insert(Node **head, uint16_t data) {
  
    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (new_node == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    new_node->data = data; 
    new_node->next = NULL;  
    printf("new node contains data: %d\n", data);
    
    if (*head == NULL) {
        *head = new_node;
        return;
    }

    
    Node* current = *head;
    while (current->next != NULL) {
        current = current->next; 
    }

   
    current->next = new_node;
}

void list_insert_after(Node *prev_node, uint16_t data)
{
    Node* new_node = (Node*)mem_alloc(sizeof(Node));

    new_node->data = data;
    new_node->next = prev_node->next;
    prev_node->next = new_node;
}

void list_insert_before(Node **head, Node *next_node, uint16_t data)
{
    if (next_node == NULL) {
        return;
    }

    Node* new_node = (Node*)mem_alloc(sizeof(Node));
    if (*head == next_node) {
        new_node->next = *head;
        *head = new_node;
        return;
    }

    Node* current = *head;
    while (current != NULL && current->next != next_node) {
        current = current->next;
    }

    if (current == NULL) {
        mem_free(new_node); 
        return;
    }

    new_node->next = next_node;
    current->next = new_node;
}

void list_delete(Node **head, uint16_t data) {
    if (*head == NULL) {
        return; 
    }

    Node* current = *head;
    Node* previous = NULL;

    while (current != NULL && current->data != data) {
        previous = current;
        current = current->next;
    }

    if (current == NULL) {
        return; 
    }

    if (previous == NULL) {
        *head = current->next; 
    } else {
        previous->next = current->next;
    }

    mem_free(current);
}

Node* list_search(Node **head, uint16_t data) {
    Node* current = *head;

    while (current != NULL) {
        if (current->data == data) {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

void list_display(Node **head) {
    list_display_range(head, NULL,NULL);
}

void list_display_range(Node **head, Node *start_node, Node *end_node)
{
    Node* current = start_node;

    if (*head == NULL) {
        printf("The list is empty.\n");
        return;
    }

    if (start_node == NULL) {
        current = *head;
    }

    printf("[");
    while (current != NULL) {
        printf("%u , ", current->data);
        if(current == end_node) {
            break;
        }
        current = current->next;
    }
    printf("]\n");
}

int list_count_nodes(Node **head)
{
   int counter = 0;
    Node* current = *head;

    while (current != NULL) {
        counter++;
        current = current->next;
    }

    return counter;
}


void list_cleanup(Node **head) {
    Node* current = *head;
    Node* next_node;

    while (current != NULL) {
        next_node = current->next;
        mem_free(current);
        current = next_node;
    }

    *head = NULL;
}
