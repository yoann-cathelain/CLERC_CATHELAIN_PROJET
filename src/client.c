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
#include "validation.h"

#include "testes.h"


/*
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */
int envoie_recois_message(int socketfd){

  char* data;
  char* code = "message";

  data = malloc(sizeof(char)*1024);
  json_object json_message;
  json_message.code = malloc(strlen(code)+1);
  json_message.valeurs = malloc(sizeof(char)*1024);
  // la réinitialisation de l'ensemble des données
  memset(data, 0, 1024);

  // Demandez à l'utilisateur d'entrer un message
  char message[1024];
  printf("Votre message (max 1000 caracteres): ");
	  fgets(message, sizeof(message), stdin);

  for(unsigned int i = 0; i < strlen(message); i++){
    if(message[i] == '\x0A'){
      message[i] = '\0';
    }
  }

	  strcpy(data, code);
    strcpy(json_message.code, code);
	  strcat(data, message);
    strcat(json_message.valeurs, message);

    data = json_encode(&json_message, '\x32');
    if(validate_json_message(data) != 0) {
    printf("JSON valide\n");
    }else {
    printf("JSON non valide\n");
  }


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

	  printf("Message recu: %s\n", data);

    // Libération de la mémoire
    free(data);

	  return 0;
	}



	/*
	 * Fonction qui analyse une image
 * */
void analyse(char *pathname, char *data){

  // Init
  couleur_compteur *cc = analyse_bmp_image(pathname);
  json_object json_image;
  char* code = "image";
  json_image.code = malloc(strlen(code)+1);
  json_image.valeurs = malloc(sizeof(char)*1024);
  int count;
  int nbCoul = 31;
  strcpy(data, "");

  //Demande le nombre de couleurs <=30
  while(nbCoul > 30){
  	printf("\nNombre de couleurs (<=30): ");
  	scanf("%d", &nbCoul);
  }

  // Construction de tempstring qui vaut "nbCoul,"
  char temp_string[nbCoul];
  sprintf(temp_string, "%d", nbCoul);
  strcat(temp_string, ",");
  
  if (cc->size < nbCoul){
    sprintf(temp_string, "%d,", cc->size);
  }

  strcat(data, temp_string);

  // Choisir nbCoul couleurs
  for (count = 1; count <= nbCoul && cc->size - count > 0; count++)
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

  // Enlever la dernière virgule
  data[strlen(data) - 1] = '\x32';

  // Formatage du JSON
  json_image.code = code;
  json_image.valeurs = data;
  strcpy(data, json_encode(&json_image, '\x44'));
}



/*
 * Envoie les couleurs prédominantes d'une image en paramètre
 * */
int envoie_couleursPred(int socketfd, char *pathname)
{
  char* data = malloc(sizeof(char)*1024);
  analyse(pathname, data);

  // Attendre le serveur
  sleep(1);

  int write_status = write(socketfd, data, strlen(data));
  if (write_status < 0)
  {
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }
 
  free(data);
  return 0;
}


/*
 * Fonction qui envoie les infos du calcul au serveur => opérandes & opérateur
 * */
int envoie_info_calcul(int socketfd){

  char* data;
  char* code = "calcul";

  data = malloc(sizeof(char)*1024);
  
  json_object json_calcul;
  json_calcul.code = malloc(strlen(code)+1);
  json_calcul.valeurs = malloc(sizeof(char)*1024);

  // Réinitialisation de l'ensemble des données
  memset(data, 0, 1024);

  // Demande les détails du calcul
  char calcul[1024];
  printf("Indiquer le detail du calcul (max 1000 caracteres, format: <operateur> <operande> <operande>): ");
  fgets(calcul, sizeof(calcul), stdin);

  for(unsigned int i = 0; i < strlen(calcul); i++){
    if(calcul[i] == '\x0A'){
      calcul[i] = '\0';
    }
  }

  strcpy(data, code);
  strcpy(json_calcul.code, code);
  strcat(data, calcul);
  strcat(json_calcul.valeurs, calcul);

  data = json_encode(&json_calcul, '\x32');
  if(validate_json_calcul(data) != 0) {
    printf("JSON valide\n");
  }else {
    printf("JSON non valide\n");
  }

  // Envoie les données
  int write_status = write(socketfd, data, strlen(data));

  if (write_status < 0){
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // Réinitialisation de l'ensemble des données
  memset(data, 0, 1024);

  // Lit les données du socket
  int read_status = read(socketfd, data, 4096);

  if (read_status < 0){
    perror("erreur lecture");
    return -1;
  }

  printf("Resultat recu: %s\n", data);

  // Libération de la mémoire
  free(data);

  return 0;
}


/*
 * Fonction qui envoie les couleurs au serveur => nombre de couleurs et détail des couleurs
 * */
int envoie_couleurs(int socketfd){

  char* data;
  char* code = "couleurs";

  data = malloc(sizeof(char)*1024);

  json_object couleur_json;
  couleur_json.code = malloc(strlen(code)+1);
  couleur_json.valeurs = malloc(sizeof(char)*1024);

  // Réinitialisation de l'ensemble des données
  memset(data, 0, 1024);

  // Demande à l'utilisateur le nombre de couleurs
  char nb[32]; 
  char* pos;
  printf("Combien de couleurs voulez vous entrer (max 30): ");
  fgets(nb, 32,  stdin); 

  if((pos=strchr(nb, '\n')) != NULL){
     *pos = '\0';    
  }

  // Formatage de l'envoi
  strcpy(data, code);
  strcpy(couleur_json.code, code);
  strcat(data, nb);
  strcat(couleur_json.valeurs, nb);

  char couleur[16];
  int nbCoul =  0;
  int conv = sscanf(nb, "%d", &nbCoul);

  // Erreur de type
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
    strcat(couleur_json.valeurs, ",");
    strcat(data, couleur);
    strcat(couleur_json.valeurs, couleur);

    memset(couleur, 0, 16);
    printf("\n");
  }

  data = json_encode(&couleur_json, '\x44');
  printf("%s\n", data);
  if(validate_json_couleurs(data) != 0) {
    printf("JSON valide\n");
  }else {
    printf("JSON non valide\n");
  }

  // Envoi des couleurs
  int write_status = write(socketfd, data, strlen(data));

  // Erreur d'envoi
  if (write_status < 0){
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // Réinitialisation de l'ensemble des données
  memset(data, 0, 1024);

  // Lit les données du socket
  int read_status = read(socketfd, data, 4096);

  // Erreur de lecture
  if (read_status < 0){
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  // Libération de la mémoire
  free(data);

  return 0;
}


/*
 * Fonction qui envoie le nom du client au serveur puis reçoit sa réponse
 * */
int envoie_nom_de_client(int socketfd){

  // Init et réinitialisation de l'ensemble des données
  char* data;
  char* code = "nom";
  json_object json_nom;

  // Attendre le serveur avant d'envoyer le nom
  sleep(1);

  data = malloc(sizeof(char)*1024);
  json_nom.code = malloc(sizeof(char)*1024);
  json_nom.valeurs = malloc(sizeof(char)*1024);

  memset(data, 0, 1024);

  // Récupération de l'hostname
  gethostname(json_nom.valeurs, 1023);
  json_nom.valeurs[1023] = '\0';
  
  // Formatage de l'envoi
  strcpy(data, code);
  strcpy(json_nom.code, code);
  strcat(data, json_nom.valeurs);

  data = json_encode(&json_nom, '\x32');
      if(validate_json_nom(data) != 0) {
      printf("JSON valide\n");
    }else {
      printf("JSON non valide\n");
    }

  // Envoie du nom
  int write_status = write(socketfd, data, strlen(data));

  // Erreur d'envoi
  if (write_status < 0){
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // Réinitialisation de l'ensemble des données
  memset(data, 0, 1024);

  // lit les données du socket
  int read_status = read(socketfd, data, 4096);
  
  // Erreur de lecture
  if (read_status < 0){
    perror("erreur lecture");
    return -1;
  }

  printf("%s\n", data);

  // Libération de la mémoire
  free(data);

  return 0;
}


/*
 * Fonction qui envoie les balises au serveur => nombre de balises et détail des balises
 * */
int envoie_balises(int socketfd){

  char* data;
  char* code = "balises";

  data = malloc(sizeof(char)*1024);

  json_object balises_json;
  balises_json.code = malloc(strlen(code)+1);
  balises_json.valeurs = malloc(sizeof(char)*1024);

  // Réinitialisation de l'ensemble des données
  memset(data, 0, 1024);

  // Demande à l'utilisateur le nombre de couleurs
  char nb[32]; 
  char* pos;
  printf("Combien de balises voulez vous entrer (max 30): ");
  fgets(nb, 32,  stdin); 

  if((pos=strchr(nb, '\n')) != NULL){
     *pos = '\0';    
  }

  // Formatage de l'envoi
  strcpy(data, code);
  strcpy(balises_json.code, code);
  strcat(data, nb);
  strcat(balises_json.valeurs, nb);

  char balises[16];
  int nbBal =  0;
  int conv = sscanf(nb, "%d", &nbBal);

  // Erreur de type
  if(conv == 0){
   perror("Ce n'est pas un nombre");
   return(EXIT_FAILURE);
  }

  // Demande les balises
  for(int i=0; i<nbBal; i++){
    printf("Votre balise: ");
    fgets(balises, 16, stdin);

    if((pos=strchr(balises, '\n')) != NULL){
       *pos = '\0';    
    }

    strcat(data, ",");
    strcat(balises_json.valeurs, ",");
    strcat(data, balises);
    strcat(balises_json.valeurs, balises);

    memset(balises, 0, 16);
    printf("\n");
  }

  data = json_encode(&balises_json, '\x44');
  printf("%s\n", data);
  if(validate_json(data) != 0) {
    printf("JSON valide\n");
  }else {
    printf("JSON non valide\n");
  }

  // Envoi des balises
  int write_status = write(socketfd, data, strlen(data));

  // Erreur d'envoi
  if (write_status < 0){
    perror("erreur ecriture");
    exit(EXIT_FAILURE);
  }

  // Réinitialisation de l'ensemble des données
  memset(data, 0, 1024);

  // Lit les données du socket
  int read_status = read(socketfd, data, 4096);

  // Erreur de lecture
  if (read_status < 0){
    perror("erreur lecture");
    return -1;
  }

  printf("Message recu: %s\n", data);

  // Libération de la mémoire
  free(data);

 return 0;
}


/*
 * Fonction principale => créé un socket et envoie les infos que le serveur doit traîter
 * */
int main(int argc, char **argv){

  // Init
  int socketfd;
  struct sockaddr_in server_addr;
  char* hostname = malloc(sizeof(char)*1024);


  // Message à afficher si aucun paramètre n'est donné au client
  if (argc < 2)
  {
    printf("usage: ./client chemin_bmp_image\n");
    return (EXIT_FAILURE);
  }

  // Creation d'un socket et gestion des erreurs
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0){
    perror("socket");
    exit(EXIT_FAILURE);
  }

  // Détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Demande de connection au serveur
  int connect_status = connect(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (connect_status < 0){
    perror("connection serveur");
    exit(EXIT_FAILURE);
  }

  // On envoie le nom du client au serveur
  gethostname(hostname, 256);
  hostname[256] = '\0';

  int write_status = write(socketfd, hostname, strlen(hostname));

  // Erreur d'envoi
  if (write_status < 0){
    perror("erreur nom client");
    exit(EXIT_FAILURE);
  }

  // Si on a un paramètres, on envoie/reçoit le bon code de message
  if (argc == 2){ 
	if(strcmp(argv[1], "nom") == 0){
		envoie_nom_de_client(socketfd);
	}
	else if(strcmp(argv[1], "calcul") == 0){
		envoie_info_calcul(socketfd);
	}
	else if(strcmp(argv[1], "couleurs") == 0){
		envoie_couleurs(socketfd);
	}
	else if(strcmp(argv[1], "balises") == 0){
		envoie_balises(socketfd);
	}
	else if(strcmp(argv[1], "message") == 0){
		envoie_recois_message(socketfd);
	}
  else if(strcmp(argv[1], "testOp") == 0){
    
  }
  else if(strcmp(argv[1], "testJson") == 0){

    // Init
    json_object json_test;
    char* data = "{\"code\": \"testJson\",\"valeur\": [\"testJson\"]}";

    // Décodage du JSON
    json_test = json_decode(data);

    // Test de la compréhension du JSON côté client
    testJson("client", json_test.code, json_test.valeurs);

    // Envoi du JSON pour test côté serveur
    write(socketfd, data, strlen(data));

  }
  else if(strcmp(argv[1], "testProt") == 0){

    // Init
    json_object json_test;
    json_test.code = malloc(sizeof(char)*8);
    json_test.valeurs = malloc(sizeof(char)*8);
    char* rcvdData = malloc(sizeof(char)*1024);
    char* socketNo = malloc(sizeof(char)*2);

    strcpy(json_test.code, "testProt");
    strcpy(json_test.valeurs, "testProt");

    char* sentData = json_encode(&json_test, '\x32');

    // Envoi
    sleep(1);
    write(socketfd, sentData, strlen(sentData));

    // Réception
    read(socketfd, rcvdData, 1024);
    recv(socketfd, socketNo, sizeof(socketNo), 0);

    // Test du protocole
    testProtocole(sentData, rcvdData, socketNo);

    // Libération de la mémoire
    free(rcvdData);
    free(socketNo);
    free(sentData);
    free(json_test.code);
    free(json_test.valeurs);
    
  }
  else if(strcmp(argv[1],"testMulti") == 0){
    testMultiUsers();
  }
	else{

		// Envoyer et recevoir les couleurs prédominantes d'une image au format BMP (argv[1])
		envoie_couleursPred(socketfd, argv[1]);
	}

 }

  close(socketfd);
  free(hostname);

  return 0;
}

