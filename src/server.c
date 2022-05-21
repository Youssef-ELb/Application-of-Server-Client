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

// la fonctin save_data//

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
}

char * loadFile(char *name, char  *fileBuff){
	/* la foinction loadfile prend en arg, le nom de fichier,
	* et le tableau qui va contient le contenu de ce fichier
	*
		*/
	   FILE *pFile = NULL;
	   size_t result;
           long lSize;
	 //   bzero(fileBuff, sizeof(fileBuff));
	    char c;
	    pFile=fopen(name,"r");
	     // obtain file size:
	   fseek (pFile , 0 , SEEK_END);
	   lSize = ftell (pFile);
	   rewind (pFile);

	    // allocate memory to contain the whole file:
	  fileBuff = (char*) malloc (sizeof(char)*lSize);
	  if (fileBuff == NULL) {fputs ("Memory error",stderr); exit (2);}
	printf("lire le fichier en succés !\n");
	  // copy the file into the buffer:
	  result = fread (fileBuff,1,lSize,pFile);
	  if (result != lSize) {fputs ("Reading error",stderr); exit (3);}
	  //else{fputs ("File error",stderr); exit (1);}
	  //lire le contenu de fichier :)
	  printf("le contenu est : %s\n", fileBuff);

	 
	  // terminate
	  fclose (pFile);
	  free (fileBuff);  
return fileBuff;
}

//--------------------------------------------------

void sendfile(char *ip)
{
    FILE *sendfile;

    int port = 7001;
    int e;
    char * data[48000];
    FILE * fl;
    
    int sockfd, connfd;
    struct sockaddr_in servaddr, cli;
   
    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    }
    else
        printf("Socket successfully created..\n");
    bzero(&servaddr, sizeof(servaddr));
   
    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("192.168.13.133");
    servaddr.sin_port = htons(7001);
   
    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    }
    else
        printf("connected to the server..\n");
  
   
    fl = fopen("../src/send.txt", "r");
    if(fl == NULL)
     {
         perror("[-]Error in reading file.");
         exit(1);
     }
  
    while(fgets(data, 48000, fl)!=NULL)
    {
        if(send(sockfd, data, sizeof(data), 0)== -1)
        {
            perror("[-] Error in sendung data");
            exit(1);
        }
        bzero(data, 48000);
        
    }
     fclose(fl);
     // close the socket
    close(sockfd);
}

//--------------------------------------------------

int main(int argc, char *argv[])
{
    char *ip = "192.168.13.133";
    
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
        save_data(recvBuff);
        

     
        
        //********************--*
        

        //lire le contenu de fichier 
      //  loadFile("../src/data.txt", file);
         printf("tout est bien !");
         strcpy(file, loadFile("data.txt", file));
         printf("your data is :::: %s \n", file);
        
           sendfile(ip);
        
        
        
         
    }

        if( n > 0){
           // snprintf(sendBuff, sizeof(sendBuff), "%s\n", hostname);
           // write(connfd, sendBuff, strlen(sendBuff));
            
          
        
          while ( n = recv(connfd, recvBuff, sizeof(recvBuff)-1, 0) > 0)
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
}}
