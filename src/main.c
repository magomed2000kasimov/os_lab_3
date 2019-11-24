#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <pthread.h>
#include "queue.h"




typedef struct ThreadParams thread_params;

void pre_check(bool* flag) {
    FILE* file = fopen("in.txt","r");
    char temp = fgetc(file);
    putchar(temp);
    while (temp != EOF) {
        if ( temp < '0' && temp != '+' && temp != '/' && temp != ' ' && temp != '\n' && temp != EOF || 	temp != '\n'&& temp != EOF && temp > '9' && temp != '+' && temp != '/' && temp != ' '  ) {
            *flag = true;

            fclose(file);
            return;
        }
        temp = fgetc(file);
        if (temp != EOF)
            putchar(temp);

    }
    fclose(file);
}

Tree* Tree_create(int v) {
    Tree* tree = (Tree *) malloc(sizeof(Tree));
    if (!tree){
        printf("Out of memory\n");
        exit(0);
    }
    tree->val = v;
    tree->son = NULL;
    tree->bro = NULL;
    return tree;
}

Tree* find_in_son (Tree* root,int v) {
    if (root ->son == NULL)
        return root;
    Tree* tmp = root->son;
    while (tmp->val != v) {
        tmp = tmp->bro;
    }
    return tmp;
}


struct ThreadParams {
    Tree* root;
    int num;
    bool* found;
    int* thread_count;
    pthread_mutex_t* count_mutex;
};



void* tree_find(void* arg) {
    thread_params* params = (thread_params*) arg;
    bool* found = params->found;
    int num = params->num;
    Tree* root = params->root;
    int* thread_count = params->thread_count;
    pthread_mutex_t* count_mutex = params->count_mutex;

    if (root == NULL){
        pthread_exit(NULL);
    }

    if (root->val == num) {
        *(params->found) = true;
        pthread_exit(NULL);
    }

    if (root->son == NULL) {
        pthread_exit(NULL);
    }
    
    int son_count = 0;
    Tree* son = root->son;
    while(son != NULL) {
        if (son->val == num) {
		*(params->found) = true;
		pthread_exit(NULL);  
	}
	son_count++;
        son = son->bro;
	
    }
    son = root->son;
    Tree* sons[son_count];
    int counter = 0;
    while(son != NULL) {
        sons[counter] = son;
        counter++;
        son = son->bro;
    }
    pthread_mutex_lock(count_mutex);
    int create_threads = (*thread_count > son_count) ? son_count : *thread_count;
    (*thread_count) -= create_threads;
    pthread_mutex_unlock(count_mutex);
    pthread_t threads[create_threads];
    thread_params* new_params = malloc(sizeof(thread_params) * create_threads);
    for (int i = 0; i < create_threads; ++i) {
        new_params[i] = *params;
        new_params[i].root = sons[i];
        pthread_create(&threads[i], NULL, tree_find, &new_params[i]);
    }
    queue q;
    q_init(&q);
    for (int i = create_threads; i < son_count; ++i) {
        q_push(&q, sons[i]);
    }
    while (!q_empty(&q)) {
        Tree* item = q_pop(&q);
        if (item->val == num) {
            *found = true;
            break;
        }
        Tree* item_son = item->son;
        while (item_son != NULL) {
            q_push(&q, item_son);
            item_son = item_son->bro;
        }
    }
    for (int i = 0; i < create_threads; ++i) {
        pthread_join(threads[i], NULL);
    }
    pthread_mutex_lock(count_mutex);
    (*thread_count) += create_threads;
    pthread_mutex_unlock(count_mutex);
    free(new_params);
    q_destroy(&q);
    pthread_exit(NULL);
}

void pars(Tree** root,char* filename) {
    FILE* in = fopen (filename,"r");
    char tmp;
    int status;
    while ((tmp = fgetc(in)) != EOF) {
        if (tmp == ' ' || tmp == '\n') {
            tmp = fgetc(in);
            continue;
        }
        if (tmp == '+') {
                Tree** ptr = root;
                int v = 0,trash = 0,n = 0;
                while ((tmp = fgetc(in)) != ' ') {
                    v *= 10;
                    v += tmp - '0';
                }
                //fgetc(in);
                while ((tmp = fgetc(in)) != '\n') {
                    if ( tmp !=  '/') {
                        n *= 10;
                        n += tmp - '0';
                    }
                    else {
                        while (*ptr != NULL) {
                            if ((*ptr)->val == n)
                                ptr = &(*ptr) -> son;
                            else {
                                ptr = &(*ptr)->bro;
                                if ((*ptr)->val == n)
                                    ptr = &(*ptr) ->son;
                            }
                            break;
                        }
                        n = 0;
                    }
                }

                while (*ptr != NULL)
                    ptr = &(*ptr) -> bro;
                *ptr = (Tree*) malloc(sizeof(Tree));
                (*ptr)->val = v;
                (*ptr)->son = NULL;
                (*ptr)->bro = NULL;
        }
    }
}

void tabs(int n) {
    printf("%d: ",n);
    for (int i = 0; i < n; ++i)
        printf("\t");
}

void print_tree(Tree* root,int lvl) {
    if (!root)
        return;
    Tree* temp = root;
    while (temp != NULL) {
        tabs(lvl);
        printf("%d\n",temp -> val);
        print_tree(temp -> son,lvl + 1);
        temp = temp->bro;
    }
}



int main(int argc,char** argv)
{
    bool flag = false;
    Tree * root = NULL;
    if (flag) {
        printf("error\n");
        return 0;
    }
    pars(&root,argv[1]);
    print_tree(root,0);
    printf("enter key to find\n");
    int num;
    scanf("%d",&num);
    bool* found = malloc(sizeof(bool));
    *found = false;
    pthread_mutex_t* count_mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(count_mutex, NULL);
    printf("enter max thread count\n");
    int* thread_count = malloc(sizeof(int));
    scanf("%d",thread_count);
    thread_params params = {
            .found = found,
            .num = num,
            .root = root,
            .thread_count = thread_count,
            .count_mutex = count_mutex
    };
    pthread_t first_thread;
    pthread_create(&first_thread, NULL, tree_find, &params);
    pthread_join(first_thread, NULL);
    printf("Node is%s found", *found ? "" : " not");
    printf("\n");
    return 0;
}
