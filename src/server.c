#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

int main(int argc, char *argv[])
{
    // La socket serveur
    int listenfd = 0;
    // La socket client (récupérée avec accept())
    int connfd = 0;
    // La structure avec les informations du serveur
    struct sockaddr_in serv_addr = {0};
    //--------------------------->
    
    addrlen=sizeof(connfd);
    struct ifaddrs *addr, *intf;
    int family, skt;
    
    //-----------------------
    // Le buffer pour envoyer les données
    char sendBuff[1025] = {0};
    
    // Création de la socket serveur
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    
    //Initialisation de la structure sockaddr
    serv_addr.sin_family = AF_INET;
    //Accepte les connexions depuis n'importe quelle adresse
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    //------------------------------------------------>
    socketAccept = accept(listenfd,&connfd,&addrlen);
    //------------------------------------------------>
    // Le port sur lequel la socket va écouter
    serv_addr.sin_port = htons(5000);
    
    // Association de la socket avec la structure sockaddr
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    
    //La socket écoute pour des connexions
    listen(listenfd, 10);
    
    // Récupération du nom de la machine
    char hostname[128];
    gethostname(hostname, sizeof hostname);
    geti
    
    int pid = 0;
    while(1)
    {
        
        // Accepte la connexion d'une socket client
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
         //------------------------------------------------>
         if (getifaddrs(&intf) == -1) {
            perror("getifaddrs");
            exit(EXIT_FAILURE);
           //------------------------------------------------>
        // Exécution d'un fork pour gérer la connexion
        if((pid=fork())==-1) {
            printf("erreur\n");
            close(connfd);
        }
        else if(pid>0) { // Le processus père
            close(connfd);
        }
        else if(pid==0) { // Le processus fils
            snprintf(sendBuff, sizeof(sendBuff), "%s\n", hostname);
            write(connfd, sendBuff, strlen(sendBuff));
            //------------------------------------------------>
             for (addr = intf; addr != NULL; addr = addr->ifa_next) {
               family = addr->ifa_addr->sa_family;
               //AF_INET est la famille d'adresses pour IPv4
               if (family == AF_INET) {
              //getnameinfo permet la résolution de noms.
              s = getnameinfo(addr->ifa_addr, 
                          sizeof(struct sockaddr_in),  hostname,    NI_MAXHOST,NULL,0,NI_NUMERICHOST);
          printf("<interface>: %s \t <adresse> %s\n", addr->ifa_name, hostname);
        }//------------------------------------------------>
            close(connfd);
        }
    }
}
