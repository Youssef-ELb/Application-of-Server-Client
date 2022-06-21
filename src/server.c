#include <libgen.h> 
#include <sys/socket.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <errno.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/stat.h>
#include <time.h> 
#include <fcntl.h>
#define BUFLEN 1024

#define SIZE 3000
#define TAILLE 1024
typedef struct data Data;

struct data{
	int listenfd;
	char hostname[128];
	struct sockaddr_in serv_addr;
	};
Data  Connect_Port(int port){
    printf("[+] Création de la socket Serveur du port %d...\n",port);
    // la structure Data contenant les informations de la socket
    Data data_port ;
    // Création de la socket serveur
    data_port.listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if(data_port.listenfd<0){
	    perror("[-] Erreur: la socket ne peut pas etre crée \n");
	    return data_port;
    }
    printf("[+] la socket a été bien crée \n");
    //Initialisation de la structure sockaddr
    data_port.serv_addr.sin_family = AF_INET;
    //Accepte les connexions depuis n'importe quelle adresse
    data_port.serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Le port sur lequel la socket va écouter
    data_port.serv_addr.sin_port = htons(port);
    
    char erreur[SIZE]; 
    // Association de la socket avec la structure sockaddr
    if(bind(data_port.listenfd, (struct sockaddr*)&data_port.serv_addr, sizeof(data_port.serv_addr))<0){
		 sprintf(erreur,"[-] Erreur : Bind a été echoué pour le port %d\n", port);
         	 perror(erreur);
		 return data_port;}
    printf("[+] Bind du serveur a été bien créé %d\n",port);
    
    //La socket écoute pour des connexions
    if(listen(data_port.listenfd, 10)<0){
		sprintf(erreur,"[-] Erreur : listen a été echoué pour le port %d\n", port);
		perror(erreur);
         	return data_port;}
    printf("[+] Le port %d est en train d'écouter ...\n", port);
  
    gethostname(data_port.hostname, sizeof data_port.hostname);
    return data_port;
}
FILE* ouvrir_fichier(char* nom_fichier){
	FILE* fp = fopen(nom_fichier,"a+");
	if(fp == NULL){
	        char erreur[SIZE];
		sprintf(erreur,"[-] Erreur : l'ouverture du fichier '%s' est echouée..\n",nom_fichier); 
		perror(erreur);	
		}
	return fp;
}

void compare_ip(int connfd, char* ip_recu){
	int found=0;
	printf("[+] lire et ajouter l' @ip recu au fichier ListClient.txt...\n");
	FILE* fichier = ouvrir_fichier("ListClient.txt");
	char chaine[SIZE];
	bzero(chaine,SIZE);
	// récupérer les @ip enregitrees dans ListClient.txt 
	while(fgets(chaine,sizeof(chaine),fichier) != NULL){
				chaine[strcspn(chaine,"\n\r")]=0;
				//comparer l'@ip recu avec celles du fichier
				int result= strcmp(ip_recu, chaine);
				if(result==0){
					printf("[+] le client existe déjà dans la liste des clients\n");
					found=1;
					break;
								}//fin while
	if(!found){
		fputs(ip_recu,fichier);
		fputs("\n",fichier);
		printf("[+] le client a été bien enregistré\n");
			}
	fclose(fichier);
	}
}

void envoyer(int connfd, char* buffer){
   if(send(connfd, buffer, strlen(buffer)+1, 0)< 0){
	        char erreur[SIZE];
		sprintf(erreur,"[-] Erreur : le message '%s' n'a pas été envoyé...\n",buffer); 
		perror(erreur);	
	   }
 else {
	   printf("[+] le message a été envoyé: %s\n", buffer);}
}

void recevoir(int connfd, char* buffer){
	if(recv(connfd, buffer, SIZE, 0) < 0)
        {
	   perror("[-] Erreur: le message n'a pas été recu \n");
           exit(errno);
        }
	else{
		printf("[+] le message a été recu : %s\n", buffer);
	}
}

void envoi_fichier(char *nom_fichier, int sockfd){
  char data[SIZE] = {0};
  printf("[+] l'envoi du fichier '%s'...\n",nom_fichier);
  FILE* fp = ouvrir_fichier(nom_fichier);
  while(fgets(data, SIZE, fp) != NULL) {
	envoyer(sockfd,data);
    	bzero(data, SIZE);
  }
  fclose(fp);
  char fin_fichier[SIZE] = "exit...";
  send(sockfd, fin_fichier, sizeof(fin_fichier), 0);
  printf("[+] le fichier '%s' a été envoyé.\n",nom_fichier);
}


void recevoir_fichier(char* nom_fichier,int sockfd){
  int n = 0;
  FILE *fp = ouvrir_fichier(nom_fichier);
  char buffer[SIZE];
  while (recv(sockfd, buffer, SIZE, 0) > 0) {
  	printf(" [+] ligne %d: %s\n",n,buffer);
	n+=1;
    	if ((strncmp(buffer, "exit...", 7)) == 0) {
            printf("[+] Fin du Fichier...\n");
            break;
        }
    	fprintf(fp, "%s", buffer);
    	bzero(buffer, SIZE);
  }
  printf("[+] le fichier a été recu\n");
  fclose(fp);
  return;
}

char* generer_hsa1(char* fichier){
	char cmd[SIZE];
	char* token;
	char ligne[SIZE];
        char hsa1_file[TAILLE];
	sprintf(hsa1_file, "hsa1_%s", fichier);
	sprintf(cmd, "sha1sum -t %s > %s", fichier, hsa1_file);
	system(cmd);
	
	FILE* fp = ouvrir_fichier(hsa1_file);
	fgets(ligne, SIZE, fp);
	fclose(fp);
	char *str1;
	str1 = strdup(ligne);
	token = strsep(&str1, " ");
	printf("[+] resultat : %s\n", token);
	return token;

}
int main(int argc, char *argv[])
{
    Data data_port1 ;
    Data data_port2;
    int port1 =7000;
    int port2=7001;

    data_port1 = Connect_Port(port1);
    data_port2 = Connect_Port(port2);
     char mon_ip[SIZE];
    while(1)
    {
        // Accepte la connexion d'une socket client
	printf("[+] Acceptation de la connexion au port %d\n",port1);
        int connfd = accept(data_port1.listenfd, (struct sockaddr*)NULL, NULL);
        bzero(mon_ip,SIZE);

	//reception de l'@ip du client
     	recevoir(connfd, mon_ip);

	//comparaison de l'@ip recu avec les @ip de la liste existante
        compare_ip(connfd, mon_ip);
	close(connfd);


        // Accepte la connexion d'une socket client
	printf("[+] Acceptation  de la connexion au port %d\n",port2);
        int connfd2 = accept(data_port2.listenfd, (struct sockaddr*)NULL, NULL);
	
	//envoyer le fichier
	char *nom_fichier1 = "send.sh";
        char *hsa1; 
	hsa1 = generer_hsa1("send.sh");
        envoyer(connfd2,hsa1);
	envoi_fichier(nom_fichier1, connfd2);
	//afficher le resultat de l'execution
	char *nom_fichier2 = "resultat.txt";
	recevoir_fichier(nom_fichier2, connfd2);
	close(connfd2);
}
	return 0;

}
