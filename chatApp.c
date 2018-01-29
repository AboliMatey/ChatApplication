
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

//-----------------------------------------------------------------------------------------------------------

typedef struct sock_data
{
    int conn_fd;
    struct sockaddr_in ip_addr;
}sock_data;

typedef struct socketData
{
    int listener_fd,conn_fd;
    struct sockaddr_in listener_addr, client_addr;
}socketData;

//------------------------------------------------------------------------------------------------------------

sock_data * clientSocket (char *ipaddr, int port)
{
    sock_data *sock;
    sock=(sock_data *) malloc (sizeof(sock_data));

    memset(&sock->ip_addr,'0',sizeof(sock->ip_addr));
    sock->conn_fd = socket(AF_INET,SOCK_STREAM,0);

        if (sock->conn_fd == -1)
        {
            printf("Unable To Initiate Client Socket");
            exit(1);
        }
    sock->ip_addr.sin_family = AF_INET;
    sock->ip_addr.sin_port = htons(port);
    sock->ip_addr.sin_addr.s_addr = inet_addr(ipaddr);

    return sock;
}

//-------------------------------------------------------------------------------------------------------------


socketData * listenerSocket(int port)
{
 socketData *sock;
 sock=(socketData *)malloc(sizeof(socketData));
 sock->listener_fd=socket(AF_INET,SOCK_STREAM,0);

 if(sock->listener_fd==-1)
 {
   perror("Unable to initiate listener socket");
   exit(1);
 }

 memset(&sock->listener_addr,'0',sizeof(sock->listener_addr));     //fill addr with '0'

 sock->listener_addr.sin_family = AF_INET;
 sock->listener_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 sock->listener_addr.sin_port=htons(port);

 if(bind(sock->listener_fd , (struct sockaddr *) &sock->listener_addr , sizeof(sock->listener_addr) ) == -1)
 {
     fprintf(stdout,"Address Binding Failed");
     exit(1);
 }

 if(listen(sock->listener_fd,50) == -1)
 {
    fprintf(stdout,"Not Listening");
    exit(1);
 }

 return sock;
}

//--------------------------------------------------------------------------------------------------------------

char *get_user_ip()
{
  FILE *fp1;
  static char l1[40],l2[40];
  int i=1,c;
  printf("Whom U want to send msg (Enter Int) \n");
  fp1 = fopen("mycontects","r+");
  while(fscanf(fp1,"%s %s",l1,l2)==2)
  { 
    printf("%d.) %s\n",i,l1);
    i++;
  }
  fclose(fp1);
  scanf("%d",&c);
  i = 1;
  fp1 = fopen("mycontects","r+");
  while(fscanf(fp1,"%s %s",l1,l2))
  { 
    if (i == c)
    {
      c = strlen(l2);
      l2[c] = '\0';
      return l2;
    } 
    i++;
  }
  fclose(fp1);
}

int main()
{

   int pid,i = 0,chk = 0,l,addrlen,ch = 1;
   char un[40],line[1024],msg[1024],t;
   FILE *unfd,*ufd,*fp;
   char *ipaddr;

   system("clear");

   printf("Enter username:- ");
   scanf("%s",un);

   pid = fork();
   if (pid == 0) // Child as sender
   {
     sock_data *sock;
     while(1)
     {
       ipaddr = get_user_ip();
       sock = clientSocket(ipaddr,55555);
       memset(msg,'\0',1024);
       memset(line,'\0',1024);
       printf("Enter massage \n");
       fflush(stdin);
       fflush(stdout);
       scanf("%c",&t);
       gets(msg);
       strcat(line,un);
       strcat(line,":~ ");
       strcat(line,msg);
       if (connect(sock->conn_fd , (struct sockaddr *) &sock->ip_addr , sizeof(sock->ip_addr)) != (-1))
       {
         printf("hjgaj");
         write(sock->conn_fd,line,1024); // massage write to Socket
       }
       else
       {
         close(sock->conn_fd);
         printf("your Connection is not Online your msg will be sent when he/she will be Online \n");
         pid = fork();
         if (pid == 0)
         {
           while(1)
           {
             sock = clientSocket(ipaddr,55555);
             if (connect(sock->conn_fd , (struct sockaddr *) &sock->ip_addr , sizeof(sock->ip_addr)) != -1)
             {
               write(sock->conn_fd,line,1024); // massage write to Socket
               sleep(5);
               exit(0);
             }
             close(sock->conn_fd);
             sleep(5);
           }
         }
       }
     }
   }

   else // Parent as Reciver
   {
     socketData *sock;
     sock=listenerSocket(55555);
     addrlen = sizeof(sock->client_addr);
     while(1)
     {
       if ((sock->conn_fd=accept(sock->listener_fd , (struct sockaddr *) &sock->client_addr  , &addrlen )) != -1)
       {
         memset(msg,'\0',1024);
         read(sock->conn_fd,msg,1024); // received massage
         printf("%s\n",msg);
       }
       close(sock->conn_fd);
     }
   }
}

