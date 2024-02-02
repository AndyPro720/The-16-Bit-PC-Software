#include<stdio.h>
#include<stdlib.h>

struct node {   //define a node
	int data; //any data type
	struct node* next; //pointer of type linked-list(struct node)
};


void push(struct node** head, int value) {
    struct node* newhead = NULL;
    newhead = (node *)malloc(sizeof(struct node));

    newhead->data = value;
    newhead->next = *head;
    *head = newhead; 
}

struct node* dummy() {
    struct node* head = NULL;
    struct node** tail = &head; 
    
    
   for(int i = 0; i < 5; i++) {
    push(tail, i);
    tail = &((*tail)->next);
   } 
   return head;
}


void printlist(struct node* head) {

    for(struct node* current = head; current != NULL; current = current->next) { //using for loop method
       printf("%d ", current->data);  //we don't use current->next in condition as we need the node which points to null(last) 
    }
}

int main() {
    struct node* head = dummy();
    printlist(head);
    return 0;
}
