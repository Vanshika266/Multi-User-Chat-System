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
#define MAX_CLIENTS 256

int AllClients[MAX_CLIENTS]={0};
int Clients_socket[MAX_CLIENTS];

pthread_t ClientThreads[MAX_CLIENTS];
int thread_count=0;

int unique_id = 0;



void *ClientWork(void *client_id1)
{

          int client_id = *((int *)(client_id1));
         //   printf("Client %d connected \n",client_id);

            while(1)
            {
                char chat[256]={};
                int rec_value = recv(Clients_socket[client_id],chat,256,0);

                /*if(rec_value>0)
                {

                    printf("%s",chat);
                    chat[rec_value] = '\0';

                    printf("%s",chat);
                }
*/
                if(rec_value<0)
                {
                    printf("Error receiving message \n");
                    perror("recv");
                    break;
                    //exit(1);
                }

                char *msg[256]={};
                char *each = strtok(chat," ");
                char sendmsg[256]={};
                char *users[256]={};

                int index=0;
                int flag=0;
                int count=0;
                while(each!=NULL)
                {
                    if(flag==1)
                    {
                        strcat(sendmsg,each);
                        strcat(sendmsg," ");
                    }
                    if(strcmp(each,"-")==0)
                    {
                        flag=1;
                    }
                    if(flag==0)
                    {
                        users[count] = each;

                        count+=1;
                    }

                    msg[index] = each;
                    index++;
                    each = strtok(NULL," ");
                }


                if(strcmp(msg[0],"ALL")==0)
                {
                    for(int v=0;v<unique_id;v++)
                    {
                        if(AllClients[v]==1 && v!=client_id)
                        {
                            int send_value = send(Clients_socket[v],sendmsg,256,0);
                            if(send_value<0)
                            {
                                printf("Cannot send message");
                                perror("send");
                                break;
                            }
                        }
                    }
                }
                /*
                else if(strcmp(msg[0],"END")==0)
                {
                    AllClients[client_id]=-1;
                    Clients_socket[client_id]=-1;
                    close(Clients_socket[client_id]);

                    printf("Client disconnected \n");
                    exit(1);
                }
                */
                else
                {
                    for(int u=0;u<count;u++)
                    {
                        if(AllClients[atoi(users[u])]==1)
                        {
                            int send_value = send(Clients_socket[atoi(users[u])],sendmsg,256,0);
                            if(send_value<0)
                            {
                                printf("Cannot send message");
                                perror("send");
                                break;
                            }
                        }
                    }
                }

        }
}
int main()
{
    int b;
    struct sockaddr_un server_socket;
    struct sockaddr_un client_socket;

    b = socket(AF_UNIX,SOCK_STREAM,0);
    if(b==-1)
    {
        printf("Socket did not open \n");
        perror("socket");
        exit(0);
    }

    client_socket.sun_family = AF_UNIX;
    strcpy(client_socket.sun_path,SOCK_PATH);
    unlink(client_socket.sun_path);
    int len = strlen(client_socket.sun_path) + sizeof(client_socket.sun_family);
    int check = bind(b,(struct sockaddr *)&client_socket,len);
    if(check==-1)
    {
        printf("Unable to bind port \n");
        perror("bind");
        exit(1);
    }

    int valid_num = listen(b,4);
    if(valid_num==-1)
    {
        perror("listen");
        exit(1);
    }

    int socket_size;
    int connection;
    while(1)
    {
        printf("%s","Waiting For A Connection \n");
        socket_size = sizeof(server_socket);

        connection = accept(b,(struct sockaddr *)&server_socket,&socket_size);

        if(connection==-1)
        {
        //    printf("SRVDFDFV");
            perror("accept");
            exit(1);
        }

        printf("New Client connected at socket UserID %d \n",unique_id);

        printf("%s","Connected \n");
        AllClients[unique_id]=1;
        Clients_socket[unique_id]=connection;

        unique_id++;
            //int send_value = send(a,chat,strlen(chat),0);


        pthread_t client1;
        ClientThreads[thread_count] = client1;
        thread_count++;
        int temp = unique_id-1;
        pthread_create( &client1, NULL, ClientWork,&temp);

        /*if(created<0)
        {
            printf("Client could not connect");
            exit(1);
        }
*/
    }

    for(int g=0;g<thread_count;g++)
    {
        pthread_join(ClientThreads[g],NULL);
    }
        close(b);


}







/* RESOURCES -
 * http://beej.us/guide/bgipc/pdf/bgipc_USLetter.pdf
 * Lecture slides

 */






