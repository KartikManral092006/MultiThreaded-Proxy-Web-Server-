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

int main(int argc , char* argv[]){
    int client_socketId ,client_len;
    struct sockaddr server_addr , client_addr;
    sem_init(&semaphore,0,MAX_CLIENTS);
    pthread_mutex_init(&lock , NULL);
   if (argc == 2) {
    port = atoi(argv[1]);
    }else{
        printf("TOO FEW ARGUMENTS\n");
        exit(1);
    }
    printf("Starting proxy Server at Port %d" , port);
}
