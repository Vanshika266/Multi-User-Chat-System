#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/un.h>
#include<unistd.h>
#include<pthread.h>


#define SOCK_PATH "mysocket.server"

void *Action(void *a1)
{
    int a = *((int *)a1);
    while(1)
    {
        char chat[256]={};
        int rec_value = recv(a,chat,256,0);
     //   printf("CLIENT  \n");
      //  printf("%s\n",chat);
        if(rec_value>0)
        {
            chat[rec_value] = '\0';
            printf("%s",chat);
        }
        else if(rec_value<0)
        {
            printf("Error receiving message");
            perror("recv");
            exit(1);
        }
    }
}
int main()
{
    int a;
    struct sockaddr_un server_socket;
    // sockaddr_un remote;

    char chat[256];

    a = socket(AF_UNIX,SOCK_STREAM,0);
    if(a==-1)
    {
        perror("socket");
        exit(1);
    }

    server_socket.sun_family = AF_UNIX;
    strcpy(server_socket.sun_path, SOCK_PATH);
    int len = strlen(server_socket.sun_path) + sizeof(server_socket.sun_family);
    int check = connect(a,(struct sockaddr *)&server_socket,len);
    if(check==-1)
    {
        perror("connect");
        exit(0);
    }
    printf("Connected \n");

    int i=0;
    int rec_value;

    pthread_t cthread;
    pthread_create(&cthread,NULL,Action,(void *)&a);
    // feof() to check whether there is an error in receiving values or no data is entered
    while(fgets(chat,256,stdin),!feof(stdin))
    {

        int send_value = send(a,chat,strlen(chat),0);
        if(send_value==-1)
        {
            printf("Error in sending message \n");
            perror("send");
            exit(1);
        }
        /*
        else
        {
            printf("Server connection closed \n");
            exit(1);
        }
        */


    }

    printf("Connection closed \n");
    close(a);
    return 0;

}










/* RESOURCES -
 * http://beej.us/guide/bgipc/pdf/bgipc_USLetter.pdf
 * Lecture slides

 */
