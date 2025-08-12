#include "proxy_parse.h"
#include<stdio.h>
#include<string.h>
#include<unistd.h>
#include<time.h>
#include<pthread.h>
#include<semaphore.h>
#include<sys/types.h>
#include<netinet/in.h>
#include<netdb.h>
#include<arpa/inet.h>
#include<fcntl.h>
#include<sys/wait.h>
#include<errno.h>

#define MAX_CLIENTS 10

#define MAX_BYTES 1024
// limited size cache

typedef struct cache_element cache_element ;
struct cache_element{
    char* data;  //
    int len ;  // kitne bytesor length  ka data aaya hai
    char *url ;  //  url ko store krke rkhne ke liye whenever there is any request from the client we should direct redirect to this url
    time_t LRU_time_track;
     cache_element* next;

};

cache_element *find(char *url);
int add_cache_element(char *data, int size , char *url);
void remove_cache_element();
int port = 8080;
int proxy_socket_id ;
pthread_t tid[MAX_CLIENTS];
sem_t semaphore ;  // similar to lock but will have multiple values
pthread_mutex_t lock ;  // iske paas sirf 2 values hai either 0 ya 1 agr koi client ke baad client access krna chahega toh use lock 1 milega toh tabtak wo wait mai rhega


// Cache head to traverse in the LRU CACHE
cache_element* head;
int cache_size ;



void * handle_client(void *arg){
    sem_wait(&semaphore);
    int p;
    sem_getvalue(&semaphore,p);
    printf("Semaphore value: %d\n", p);
    int * t = (int*)arg;
    int client_socketId = *t;
    int byte_send_client ,len;
    char * buffer = (char*)calloc(MAX_BYTES, sizeof(char));
}

int main(int argc , char* argv[]){
    int client_socketId ,client_len;
    struct sockaddr_in server_addr , client_addr;
    sem_init(&semaphore,0,MAX_CLIENTS);
    pthread_mutex_init(&lock , NULL);
   if (argc == 2) {
    port = atoi(argv[1]);
    }else{
        printf("TOO FEW ARGUMENTS\n");
        exit(1);
    }
    printf("Starting proxy Server at Port %d" , port);
    proxy_socket_id = socket(AF_INET, SOCK_STREAM, 0);
    if(proxy_socket_id < 0){
        perror("[FAILED]: Creation of Socket ! \n");
        exit(1);
    }
    int reuse = 1 ;
    if(setsockopt(proxy_socket_id,SOL_SOCKET ,SO_REUSEADDR,(const char*)&reuse , sizeof(reuse) < 0 )){
        perror("SetSock option failed !");
    }

    bzero((char*)& server_addr , sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port= htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    if(bind(proxy_socket_id , (struct sockaddr*)&server_addr , sizeof(server_addr)) < 0){
        perror("[FAILED]: Binding of Socket ! \n");
        exit(1);
    }
    printf("[SUCCESS]: Binding of Socket ! \n");
    int listen_status =listen(proxy_socket_id, MAX_CLIENTS);
    if(listen_status < 0 ){
        perror("[FAILED]: Listening on Socket ! \n");
        exit(1);
    }
    printf("[SUCCESS]: Listening on Socket ! \n");
    int i = 0;
    int connected_Sockets[MAX_CLIENTS];
    while(1){
        bzero((char*)&client_addr, sizeof(client_addr));
        client_len = sizeof(client_addr);
        client_socketId = accept(proxy_socket_id, (struct sockaddr*)&client_addr, &client_len);
        if(client_socketId < 0){
            perror("[FAILED]: Accepting Client Connection ! \n");
            exit(1);

        }else{
            connected_Sockets[i] = client_socketId;
        }
        struct sockaddr_in* client_pt = (struct sockaddr_in*)&client_addr;
        struct in_addr ip_addr = client_pt->sin_addr;
        char str[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &ip_addr, str, INET_ADDRSTRLEN);
        printf("[SUCCESS]: Client Connected with IP: %s and Port: %d \n", str, ntohs(client_pt->sin_port));

        p_thread_create(&tid[i], NULL, handle_client, (void*)&connected_Sockets[i]);
        i++;
    }
    code(proxy_socket_id);
    return 0;
}
