/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * Code du côté client. Le but principal de ce code est de communiquer avec le serveur, 
 * d'envoyer et de recevoir des messages. Ces messages peuvent être du simple texte ou des chiffres.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "client.h"
#include "bmp.h"
#include "json.h"

/*
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */

int envoie_recois_message(int socketfd)
{

  char* data;
  data = malloc(sizeof(char)*1024);
  // la réinitialisation de l'ensemble des données
  memset(data, 0, 1024);

  // Demandez à l'utilisateur d'entrer un message
  json_object message;
  char* code = "message";

  message.code = malloc(strlen(code)+1);
  message.valeurs = malloc(1024);

  printf("Votre message (max 1000 caracteres): ");
  fgets(message.valeurs, 1024, stdin);
  strcpy(message.code, code);
  strcpy(data, message.code);
  strcat(data, message.valeurs);

  data = json_encode(&message, '\x32');
  
  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, 1024);

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu:%s \n", data);

  return 0;
}

void analyse(char *pathname, char *data)
{
  // compte de couleurs
  couleur_compteur *cc = analyse_bmp_image(pathname);

  int count;
  strcpy(data, "couleurs: ");
  char temp_string[10] = "10,";
  if (cc->size < 10)
  {
    sprintf(temp_string, "%d,", cc->size);
  }
  strcat(data, temp_string);

  // choisir 10 couleurs
  for (count = 1; count < 11 && cc->size - count > 0; count++)
  {
    if (cc->compte_bit == BITS32)
    {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc24[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }
    if (cc->compte_bit == BITS24)
    {
      sprintf(temp_string, "#%02x%02x%02x,", cc->cc.cc32[cc->size - count].c.rouge, cc->cc.cc32[cc->size - count].c.vert, cc->cc.cc32[cc->size - count].c.bleu);
    }
    strcat(data, temp_string);
  }

  // enlever le dernier virgule
  data[strlen(data) - 1] = '\0';
}



/*

int envoie_couleurs(int socketfd, char *pathname)
{
  char data[1024];
  memset(data, 0, sizeof(data));
  analyse(pathname, data);

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  return 0;
}*/

int envoie_info_calcul(int socketfd)
{

  char* datajson;
  datajson = malloc(sizeof(char)*1024);
  // la réinitialisation de l'ensemble des données
  //memset(data, 0, sizeof(data));
  memset(datajson, 0, 1024);

  // Demandez à l'utilisateur d'entrer un le nom client
  //char calcul[1024];
  char* code =" calcule";
  json_object calcul_json;

  calcul_json.code = malloc(strlen(code)+1);
  calcul_json.valeurs = malloc(sizeof(char)*1024);

  printf("Indiquer le detail du calcul (max 1000 caracteres): ");
  //fgets(calcul, sizeof(calcul), stdin);
  fgets(calcul_json.valeurs, 1024, stdin);

  //strcpy(data, "calcul: ");
  //strcat(data, calcul);

  strcpy(calcul_json.code, code);
  strcpy(datajson, calcul_json.code);
  strcat(datajson,calcul_json.valeurs);

  datajson = json_encode(&calcul_json,'\x32');

  int write_status = write(socketfd, datajson, strlen(datajson));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  //memset(data, 0, sizeof(data));
  memset(datajson, 0, 1024);

  // lire les données de la socket
  int read_status = read(socketfd, datajson, sizeof(datajson));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Resultat recu: %s\n", datajson);

  return 0;
}


int envoie_couleurs(int socketfd){

  //char data[1024];
  char* data;
  data = malloc(sizeof(char) * 1024);
  // la réinitialisation de l'ensemble des données
  //memset(data, 0, sizeof(data));
  memset(data, 0, 1024);

  json_object couleur_json;
  couleur_json.code = malloc(sizeof(char)*1024);
  couleur_json.valeurs = malloc(sizeof(char)*1024);

  char* code = "couleur";
  // Demandez à l'utilisateur le nombre de couleurs
  char nb[32]; 
  char* pos;
  printf("Combien de couleurs voulez vous entrer (max 30): ");
  fgets(nb, 32,  stdin); 

    if((pos=strchr(nb, '\n')) != NULL){
       *pos = '\0';    
    }

  strcpy(couleur_json.code, code);
  strcpy(data, couleur_json.code);
  strcpy(couleur_json.valeurs,nb);
  strcat(data, nb);

  char couleur[16];
  int nbCoul =  0;
  int conv = sscanf(nb, "%d", &nbCoul);

  if(conv == 0){
   perror("Ce n'est pas un nombre");
   return(EXIT_FAILURE);
  }


  // Demande les couleurs
  for(int i=0; i<nbCoul; i++){
    printf("Votre couleur (format RGB hexa): ");
    fgets(couleur, 16, stdin);

    if((pos=strchr(couleur, '\n')) != NULL){
       *pos = '\0';    
    }
    
    strcat(data, ",");
    strcat(couleur_json.valeurs,",");
    strcat(data, couleur);
    printf("%s",data);
    strcat(couleur_json.valeurs,couleur);


  //printf("%s : %s", couleur_json.code, couleur_json.valeurs);
     
    memset(couleur, 0, 16);
    printf("\n");
  }

  data = json_encode(&couleur_json, '\x44');

  // Ecriture des couleurs
  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, 1024);

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
 

}


// Envoyer le nom
int envoie_nom_de_client(int socketfd){

  char* data;
  data = malloc(sizeof(char)*1024);
  memset(data, 0, 1024);


 // Demandez à l'utilisateur d'entrer un message
  json_object hostname;
  char* code = "nom";
  hostname.code = malloc(strlen(code)+1);
  hostname.valeurs = malloc(1024);

  gethostname(hostname.valeurs, 1023);
  
  strcpy(hostname.code, code);
  strcpy(data,hostname.code);
  strcat(data, hostname.valeurs);

  data = json_encode(&hostname, '\x32');

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, 1024);

  // lire les données de la socket
  int read_status = read(socketfd, data, 4096);
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  char* res = malloc(sizeof(char)*1024);
  res = strcpy(res, data);
  printf("Message recu : %s",res);
  return 0;
}

int envoie_balises(int socketfd){

  char data[1024];
  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Demandez à l'utilisateur le nombre de balises
  char nb[32]; 
  char* pos;
  printf("Combien de balises voulez vous entrer (max 30): ");
  fgets(nb, 32,  stdin); 

    if((pos=strchr(nb, '\n')) != NULL){
       *pos = '\0';    
    }

  strcpy(data, "couleurs: ");
  strcat(data, nb);

  char balise[16];
  int nbBal =  0;
  int conv = sscanf(nb, "%d", &nbBal);

  if(conv == 0){
   perror("Ce n'est pas un nombre");
   return(EXIT_FAILURE);
  }

  // Demande les balises
  for(int i=0; i<nbBal; i++){
    printf("Votre Balise: ");
    fgets(balise, 16, stdin);

    if((pos=strchr(balise, '\n')) != NULL){
       *pos = '\0';    
    }

    strcat(data, ",");
    strcat(data, balise);
     
    memset(balise, 0, 16);
    printf("\n");
  }


  // Ecriture des balises
  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // la réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // lire les données de la socket
  int read_status = read(socketfd, data, sizeof(data));
  if (read_status < 0)
  {
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  return 0;
 
}

int main(int argc, char **argv)
{
  int socketfd;

  struct sockaddr_in server_addr;

  if (argc < 2)
  {
    printf("usage: ./client chemin_bmp_image\n");
    return (EXIT_FAILURE);
  }

  /*
   * Creation d'une socket
   */
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // demande de connection au serveur
  int connect_status = connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (connect_status < 0)
  {
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }
 /* if (argc != 2)
  {	*/ 
	if(strcmp(argv[1], "nom") == 0){
	envoie_nom_de_client(socketfd);
	}
  else	if(strcmp(argv[1], "message") == 0){
	envoie_recois_message(socketfd);
	}
	else if(strcmp(argv[1], "calcule") == 0){
	envoie_info_calcul(socketfd);
	}
	else if(strcmp(argv[1], "couleurs") == 0){
	  envoie_couleurs(socketfd);
	}
	else if(strcmp(argv[1], "balises") == 0){
	  envoie_balises(socketfd);
	}
	else{
	envoie_recois_message(socketfd);
	}

 /* }
  else
  {*/
    // envoyer et recevoir les couleurs prédominantes
    // d'une image au format BMP (argv[1])
    //envoie_couleurs(socketfd, argv[1]);
 // }

  close(socketfd);
}
