#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <errno.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <linux/if.h>
#include <ifaddrs.h>
#define SIZE 1024

// fonction pour rÃ©cupÃ©rer l'@ip du client0000

char* extract_ipaddress()  //  retourner une @ ip 
    {
        //create an ifreq struct for passing data in and out of ioctl
        struct ifreq my_struct;
 
        //declare and define the variable containing the name of the interface
        char *interface_name="ens33";   //a very frequent interface name is "eth0";
 
        //the ifreq structure should initially contains the name of the interface to be queried. Which should be copied into the ifr_name field.
        //Since this is a fixed length buffer, one should ensure that the name does not cause an overrun
        size_t interface_name_len=strlen(interface_name);
        if(interface_name_len<sizeof(my_struct.ifr_name))
        {
            memcpy(my_struct.ifr_name,interface_name,interface_name_len);
            my_struct.ifr_name[interface_name_len]=0;
        }
        else
        {
            perror("Copy name of interface to ifreq struct");
            printf("The name you provided for the interface is too long...\n");
        }
 
        //provide an open socket descriptor with the address family AF_INET
        /* ***************************************************************
         * All ioctl call needs a file descriptor to act on. In the case of SIOCGIFADDR this must refer to a socket file descriptor. This socket must be in the address family that you wish to obtain (AF_INET for IPv4)
         * ***************************************************************
         */
 
        int file_descriptor=socket(AF_INET, SOCK_DGRAM,0);
 
        if(file_descriptor==-1)
        {
            perror("Socket file descriptor");
            printf("The construction of the socket file descriptor was unsuccessful.\n");
            exit(1);
        }
 
        //invoke ioctl() because the socket file descriptor exists and also the struct 'ifreq' exists
        int myioctl_call=ioctl(file_descriptor,SIOCGIFADDR,&my_struct); 
        if (myioctl_call==-1)
        {
            perror("ioctl");
            printf("Ooops, error when invoking ioctl() system call.\n");
            close(file_descriptor);
            exit(1);
        }
 
        close(file_descriptor);

        /* **********************************************************************
         * If this completes without error , then the hardware address of the interface should have been returned in the  'my_struct.ifr_addr' which is types as struct sockaddr_in.
         * ***********************************************************************/
 
      //extract the IP Address (IPv4) from the my_struct.ifr_addr which has the type 'ifreq'
 
        /* *** Cast the returned address to a struct 'sockaddr_in' *** */
        struct sockaddr_in * ipaddress= (struct sockaddr_in *)&my_struct.ifr_addr;
       /* *** Extract the 'sin_addr' field from the data type (struct) to obtain a struct 'in_addr' *** */
      printf("[+] l'@IP du client est : %s.\n", inet_ntoa(ipaddress->sin_addr));
	return inet_ntoa(ipaddress->sin_addr); 
    }

void inscrire(int fd){
	char* ip = extract_ipaddress();
	send(fd, ip, strlen(ip),0);

}

struct data{
        int listenfd;
        char hostname[128];
        struct sockaddr_in serv_addr;
};

typedef struct data Data;


Data  Connect_Port(int port, char* IP){
    /*La socket Cliente */
    printf("[+] Commencer la connexion au port %d ...\n", port);
    Data data_port ;   
    // CrÃ©ation de la socket
    if((data_port.listenfd = socket(AF_INET, SOCK_STREAM, 0))<0) 
    {
        printf("[-] erreur : la socket n'a pas Ã©tÃ© crÃ©Ã© \n");
        return data_port;
    }
    printf("[+] La socket du port %d est crÃ©e \n",port);  
    //Initialisation de la structure sockaddr
    data_port.serv_addr.sin_family = AF_INET;
    //Accepte les connexions depuis n'importe quelle adresse
    data_port.serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    // Le port sur lequel la socket va Ã©couter
    data_port.serv_addr.sin_port = htons(port);
    // Copie l'adresse ip du serveur dans la structure serv_addr
    if(inet_pton(AF_INET,IP, &data_port.serv_addr.sin_addr)<=0)
    {
        printf("[-] Erreur : net_pton() \n");
        return data_port;
    } 

    // Connection au serveur
    if( connect(data_port.listenfd, (struct sockaddr *)&data_port.serv_addr, sizeof(data_port.serv_addr)) < 0)
    {
        printf("[-] Erreur : Connection au port %d a Ã©tÃ© Ã©chouuÃ©e\n", port);
        return data_port;
    }
    
    printf("[+] La connexion au serveur sur le  port %d est effectuee\n",port);
    return data_port;
    }



void recevoir(int connfd, char* buffer){
        if(recv(connfd, buffer, SIZE, 0) < 0)
        {
                perror("[-] Erreur: le message n'a pas Ã©tÃ© recu \n");
                exit(errno);
        }
        else{
                printf("[+] le message a Ã©tÃ© recu : %s\n", buffer);
        }
}


void envoyer(int connfd, char* buffer){
   if(send(connfd, buffer, sizeof(buffer), 0)< 0){
           perror("[-] Erreur: le message n'a pas Ã©tÃ© recu \n");
           exit(errno);
   } else {
           printf("[+] le message a Ã©tÃ© envoyÃ©: %s\n", buffer);}
}


void recevoir_fichier(int sockfd){
  int n=0;
  FILE *fp;
  char *filename = "recv.sh";
  char buffer[SIZE];
  fp = fopen(filename, "w");

  while (recv(sockfd, buffer, SIZE, 0) > 0) {
	printf("  [+] ligne %d: %s\n",n, buffer);
	n+=1;
	if ((strncmp(buffer, "exit...", 7)) == 0) {
        	printf("  [+] Fin Fichier...\n");
        	break;
        }
  	fprintf(fp, "%s", buffer);
  	bzero(buffer, SIZE);
  }
  printf("[+] le fichier a ete recu\n");
  fclose(fp);
  return;
}

void envoi_fichier(FILE *fp, int sockfd){
  int n=0;
  char data[SIZE] = {0};
  printf("[+] l'envoi du fichier...\n");
  while(fgets(data, SIZE, fp) != NULL) {
    if (send(sockfd, data, sizeof(data), 0) == -1) {
      perror("[-] Error in sending file.");
      exit(1);
    }
    printf("  [+] ligne %d: %s\n",n,data);
    bzero(data, SIZE);
    n+=1;
  }
  char data1[SIZE] = "exit...";
  send(sockfd, data1, sizeof(data1), 0);
  printf("[+] le fichier a Ã©tÃ© envoyÃ©.\n");
}

int main(int argc, char *argv[])
{
    /*
     * Si l'IP du serveur n'a pas Ã©tÃ© passÃ©e en argument
     * le programme se termine
     */
    if(argc != 2)
    {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    }

    Data data_port1 ;
    Data data_port2;

    int port1 =7000;
    int port2=7001;

    // La socket client sur le port 7000  
    data_port1 = Connect_Port(port1, argv[1]);      
    inscrire(data_port1.listenfd);

   // la socket client sur le port 7001 
    data_port2 = Connect_Port(port2, argv[1]);
  // recevoir le rÃ©sultat de la fct d'hashage
    char hash1[SIZE];
    bzero(hash1, SIZE);
    recevoir(data_port2.listenfd,hash1);
    //Recevoir le fichier:
    recevoir_fichier(data_port2.listenfd);
  
   // exÃ©cuter le script
   printf("[+] le fichier est en train de l'execution...\n");
   system("chmod +x /home/user/projet_application/src/recv.sh");
   system("/home/user/projet_application/src/recv.sh");
   
   // ouvrir et envoyer le  fichier de l'exÃ©cution
   FILE* fp = fopen("/tmp/df.txt","a+");
   printf("[+] l'envoi du fichier...\n");
   envoi_fichier(fp, data_port2.listenfd);

    return 0;
}
