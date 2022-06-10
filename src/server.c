/***********************************************************************************
 *
 * Author: mouhssine el idrissi, youssef el bab
 * Professor: Dr. Briffaut Jeremy, 
 * Creation Date: 02 Mars 2022
 * Due Date: 09 Juin 2022
 * Assignment: server client application
 * Filename: server.c
 * Purpose: allow a server to get scan all the information of client,get his adress IP,
 send file and excute them.
 * compile: make server
 * Run: ./server
 *
 **********************************************************************************/ 
//server.c
 #include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#define SA struct sockaddr

/********************************************************************************
*
* fonction "save_data" permet de sauvegarder une donnée "Data" dans un fichier situé 
* situé dans "../src/data.txt".
* @param; char * data: c'est notre donné qui on va sauvegadée
*
*
*********************************************************************************/
void save_data(char * data){
	
        FILE *fl ;
        fl = fopen("../src/data.txt","a");
        if(fl == NULL ){
        	printf(" ther is a problem in the file,the file is not opened !");
        	exit(EXIT_FAILURE);
        }
        fputs(data,fl);
        fputs("\n",fl);
        fclose(fl);
	return 0;

}

/***************************************************************************
*
* la fonction "loadFile" permet de récupérer le contenu d'un fichier
* avec le nom "name" et stocké le résultat dans "fileBuff".
* @param; char * name: référencier sur le nom de fichier qu'on veut récupérer leur données.
* @param; char * filebuff: c'est une variable qui va contenir le contenu de fichier avec le nom "name".
* @return: elle returne un char * , qui contient le contenu de fichier.
*
***************************************************************************/

char * loadFile(char *name, char  *fileBuff){
	   FILE *pFile = NULL;
	 char c;
	 int i = 0;
	 pFile = fopen(name, "rt");
	 while((c =fgetc(pFile)) != EOF){
	 fileBuff[i] = c ;
	 i++;
	 
	 }
	  // terminate
	  fclose (pFile);
return fileBuff;
}

/********************************************************************************
*
* la fonction "sendFile" qui permet d'envoyer le contenu d'un fichier vers le client.
*
*
*
*********************************************************************************/

void sendfile(char *ip, char *filename)
{
//
  int n;
  char data[48000] = {0};
  //char *ip = "127.0.0.1";
  int port = 7001;
  int e;

  int sockfd;
  struct sockaddr_in server_addr;
  FILE *fp;
 // char *filename = "send.txt";

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  if(sockfd < 0) {
    perror("[-]Error in socket");
    exit(1);
  }
  printf("[+]Server socket created successfully.\n");

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = port;
  server_addr.sin_addr.s_addr = inet_addr(ip);

  e = connect(sockfd, (struct sockaddr*)&server_addr, sizeof(server_addr));
  if(e == -1) {
    perror("[-]Error in socket");
    exit(1);
  }
	printf("[+]Connected to Server.\n");

  fp = fopen(filename, "r");
  if (fp == NULL) {
    perror("[-]Error in reading file.");
    exit(1);
  }

  printf("tout est bien ! \n ");
  strcpy(data, loadFile(filename, data));
  printf("your data is :\n %s \n", data);
  
  //while(fgets(data, SIZE, fp) != NULL) {
  printf(" if : \n");
  if (send(sockfd , data, sizeof(data), 0) == -1) {
      perror("[-]Error in sending file.");
      exit(1);
    }
    bzero(data, 48000);
   printf(" end of if : \n"); 
   
 /*  n = recv(sockfd, data, 48000, 0);
    if (n <= 0){
     printf(" false in recv  \n");
     // break;
    //  return;
    }
   
  // close the socket*/
    	close(sockfd);
//    	free(data);
//return 0;
}

///#######################################################*/
 /////#######################################################*/
 /////#######################################################*/
// la fonction main//

int main(int argc, char *argv[])
{
    char *ip = "192.168.232.156";
    char *fl ;
    fl = fopen("../src/data.txt","a");
    // La socket serveur
    int listenfd = 0;
    // La socket client (récupérée avec accept())
    int connfd = 0;
    int n=0;
    //déclaration de fichier qui va contient le contenu de fichier
    char * file[48000];
    // La structure avec les informations du serveur
    struct sockaddr_in serv_addr = {0};
    // Le buffer pour envoyer les données
    char sendBuff[1024] = {0};
    char recvBuff[1024] = {0};
    // Création de la socket serveur
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    //Initialisation de la structure sockaddr
    serv_addr.sin_family = AF_INET;
    //Accepte les connexions depuis n'importe quelle adresse
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Le port sur lequel la socket va écouter
    serv_addr.sin_port = htons(5000);
    
    // Association de la socket avec la structure sockaddr
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    printf("Blinding !\n");
    //La socket écoute pour des connexions
    listen(listenfd, 10);
    printf("listening ... !\n");
    
    /* Récupération du nom de la machine
    char hostname[128];
    gethostname(hostname, sizeof hostname);
    */
    
    int pid = 0;
    while(1)
    {
        // Accepte la connexion d'une socket client
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
    	printf("Accepting !\n");    
        // Exécution d'un fork pour gérer la connexion
        if((pid=fork())==-1) {
            printf("erreur\n");
            close(connfd);
        }
        else if(pid>0) { // Le processus père
            close(connfd);
        }
        
        else if(pid==0) { // Le processus fils
        //recv (sock, buffer, sizeof buffer, 0);
        
        recv (connfd, recvBuff, sizeof(recvBuff), 0);
        printf(" vous avez recus : %s \n", recvBuff);
       // n = read(connfd, recvBuff, sizeof(recvBuff)-1);
      //  save_data(recvBuff);
        sendfile(ip, "send.sh");
        printf(" back from the call of function send file  \n ");
//free(file);
    }

        if( n > 0){
          while ( (n = recv(connfd, recvBuff, sizeof(recvBuff)-1, 0)) > 0)
    {

        recvBuff[n] = 0;
        // Affichage des informations recues sur la sortie standard
        if(fputs(recvBuff, stdout) == EOF)
        {
            printf("\n Error : Fputs error\n");
        }


	 snprintf(sendBuff, sizeof(sendBuff), "%s\n", "Bien Reçu !");
         write(connfd, sendBuff, sizeof(sendBuff));
         close(connfd);
    
    if(n < 0)
    {
        printf("\n Read error \n");
    }
  
            close(connfd);
        }
    }
}
return 0;

}
