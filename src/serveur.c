/*
 * SPDX-FileCopyrightText: 2021 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Le code côté serveur. L'objectif principal est de recevoir des messages des clients,
 * de traiter ces messages et de répondre aux clients.
 */

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "serveur.h"
#include "json.h"




/*
 *
 * Créé un plot de couleur
 */
void plot(char *data){

  // Extraire le compteur et les couleurs RGB
  FILE *p = popen("gnuplot -persist", "w");
  printf("Plot\n");
  int count = 0;
  json_object json_image = json_decode(data);
  int n;
  int intNbCoul;
  char *saveptr = NULL;
  char *str = json_image.valeurs;
  
  // Récupération du nombre de couleurs
  char *copy = malloc(sizeof(char)*1024);
  strcpy(copy, str);
  char* nbCoul = strtok(copy, "image: ");
  nbCoul = strtok(nbCoul, ",");
  
  printf("%s\n", data);
  sscanf(nbCoul, "%d", &intNbCoul);

  // Construction de la chaîne de titre
  char titre[1024] = "set title 'Top ";
  strcat(titre, nbCoul);
  strcat(titre, " colors'\n");

  // Construction de l'échelle
  int scaleInt=1.5*intNbCoul;
  char* scale = malloc(sizeof(char*)*2);
  sprintf(scale, "%d", scaleInt);
  char xrange[1024] = "set xrange [-";
  strcat(xrange, scale);
  strcat(xrange, ":");
  strcat(xrange, scale);
  strcat(xrange, "]\n");

  char yrange[1024] = "set yrange [-";
  strcat(yrange, scale);
  strcat(yrange, ":");
  strcat(yrange, scale);
  strcat(yrange, "]\n");


  // Présentation
  fprintf(p, "%s", xrange);
  fprintf(p, "%s", yrange);
  fprintf(p, "%s", "set style fill transparent solid 0.9 noborder\n");
  fprintf(p, "%s", titre);
  fprintf(p, "%s", "plot '-' with circles lc rgbcolor variable\n");
  while (1)
  {
    char *token = strtok_r(str, ",", &saveptr);
    if (token == NULL)
    {
      break;
    }
    str = NULL;
    printf("%d: %s\n", count, token);
    if (count == 1)
    {
      n = atoi(token);
      printf("n = %d\n", n);
    }
    else
    {
      // On prend numero tel que 360° (cercle) / nbCoul = 36
      int numero = 360 / intNbCoul;
      fprintf(p, "0 0 %d %d %d 0x%s\n", intNbCoul, (count-1) * numero, count * numero, token + 1);
    }
    count++;
  }
  fprintf(p, "e\n");
  printf("Plot: FIN\n");
  pclose(p);
}



/* Renvoyer un message (*data) au client (client_socket_fd)
 */
int renvoie_message(int client_socket_fd, char *data){

  // Envoi
  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  // Erreurs
  if (data_size < 0){
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }

  return (EXIT_SUCCESS);
}



/* Renvoyer le nom du client contenu dans (*data)
*/
int renvoie_nom_client(int client_socket_fd, char *data)
{

  // Envoi
  int data_size = write(client_socket_fd, (void *)data, strlen(data));

  // Erreurs
  if (data_size < 0){
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }

  return (EXIT_SUCCESS);
}



/* Renvoyer le resultat du calcul contenu dans (*data)
*/
int renvoi_res_calcul(int client_socket_fd, char *data)
{
  // Init
  int result = 0;
  int operand = 0;

  // Récupération des données JSON
  json_object json_data = json_decode(data);
  char* valeurs = json_data.valeurs;
  
  // Récupération du code d'opération
  char* token = strtok(valeurs, ",");

  // Gestion du code d'opération
   // Addition
  if(strcmp(token, "+") == 0){

    token = strtok(NULL, ",");
    
    while(token != NULL) 
    {
	    sscanf(token,"%d", &operand);
	    result += operand;
	    token = strtok(NULL, ",");
    }
  }
  // Soustraction
  else if(strcmp(token, "-") == 0){

  token = strtok(NULL, ",");
  sscanf(token,"%d", &result);

  result -= operand;

  while(token != NULL){
	    token = strtok(NULL, ",");

      // Sécurité nombre d'opérandes impaire
      if(token != NULL){
        sscanf(token,"%d", &operand);
        result -= operand;
      }
  }
  }
  // Multiplication
  else if(strcmp(token, "*") == 0){
    result = 1;
    token = strtok(NULL, ",");
    
    while(token != NULL) 
    {
	    sscanf(token,"%d", &operand);
	    result *= operand;
	    token = strtok(NULL, ",");
    }
  }
  // Division
  else if(strcmp(token, "/") == 0){
    printf("Div");
  }
    // Minimum
  else if(strcmp(token, "minimum") == 0){
    printf("Min");
  }
  // Maximum
  else if(strcmp(token, "maximum") == 0){
    printf("Max");
  }
  // Moyenne
  else if(strcmp(token, "moyenne") == 0){
    printf("Avg");
  }
  // Ecart-type
  else if(strcmp(token, "ecart-type") == 0){
    printf("Var");
  }
  // Erreur de saisie
  else {
    perror("Erreur operateur ou mauvaise saisie");
    return(EXIT_FAILURE);
  }

  // Renvoi des données


  return(EXIT_SUCCESS);

/*
  // Initialisation
  json_object json_resultat;
  json_resultat.code = malloc(sizeof(char)*1024);
  json_resultat.valeurs = malloc(sizeof(char)*1024);
  json_object json_calcul;
  json_calcul = json_decode(data);


  char *datacopy = malloc(sizeof(char)*1024);
  datacopy = json_calcul.valeurs;
  int operation;
  char resultat[1024];
  int firstoperand = 0;
  int secondoperand = 0;

  // Transformation de la chaîne en calcul
  char *operator = strtok(datacopy, ",");
  sscanf(strtok(NULL, ","), "%d", &firstoperand);
  sscanf(strtok(NULL, ","), "%d", &secondoperand);

  // Calcul en fonction des opérateurs:
  // Addition
  if(strcmp(operator, "+") == 0){
    operation = firstoperand + secondoperand;
    sprintf(resultat, "%d", operation);
  }
  // Soustraction
  else if(strcmp(operator, "-") == 0){
    operation = firstoperand - secondoperand;
    sprintf(resultat, "%d", operation);
  }
  // Multiplication
  else if(strcmp(operator, "*") == 0){
    operation = firstoperand * secondoperand;
    sprintf(resultat, "%d", operation);
  }
  // Division
  else if(strcmp(operator, "/") == 0){
    operation = firstoperand/secondoperand;
    sprintf(resultat, "%d", operation);
  }
    // Minimum
  else if(strcmp(operator, "minimum") == 0){
    printf("Min");
  }
  // Maximum
  else if(strcmp(operator, "maximum") == 0){
    printf("Max");

  }
  // Moyenne
  else if(strcmp(operator, "moyenne") == 0){
    printf("Avg");

  }
  // Ecart-type
  else if(strcmp(operator, "ecart-type") == 0){
    printf("Var");
  }
  // Erreur de saisie
  else {
    perror("Erreur operateur ou mauvaise saisie");
    return(EXIT_FAILURE);
  }

  // Envoi du résultat
  json_resultat.code = "Resultat";
  json_resultat.valeurs = resultat;
  int data_size = write(client_socket_fd, json_encode(&json_resultat, '\x32'), 1024);
  
  if (data_size < 0){
    perror("erreur ecriture");
    return (EXIT_FAILURE);
  }
  
  return (EXIT_SUCCESS);*/
}



/* Accepter la nouvelle connection d'un client et lire les données
 * envoyées par le client. En suite, le serveur envoie un message
 * en retour
 */
int recois_envoie_message(int socketfd){

  // Init
  struct sockaddr_in client_addr;
  char data[1024];
  unsigned int client_addr_len = sizeof(client_addr);
  json_object json_message;


  // Nouvelle connection de client
  int client_socket_fd = accept(socketfd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_socket_fd < 0){
    perror("accept");
    return (EXIT_FAILURE);
  }

  // Réinitialisation de l'ensemble des données
  memset(data, 0, sizeof(data));

  // Lecture de données envoyées par un client
  int data_size = read(client_socket_fd, (void *)data, 4096);

  if (data_size < 0){
    perror("erreur lecture");
    return (EXIT_FAILURE);
  }


  /*
   * Extraire le code des données envoyées par le client.
   * Les données envoyées par le client peuvent commencer par le mot "message :" ou un autre mot.
   */
  printf("Message recu: %s\n", data);
  json_message = json_decode(data);
  //char code[10];
  //sscanf(json_, "%s", code);

  // Si le message commence par le mot: 'message:'
  if (strcmp(json_message.code, "message") == 0){
	renvoie_message(client_socket_fd, data);
  }
  // Si le message commence par le mot: 'calcule:'
  else if(strcmp(json_message.code, "calcul") == 0){
	renvoi_res_calcul(client_socket_fd, data);
  }
  // Si le message commence par le mot: 'nom:'
  else if(strcmp (json_message.code, "nom") == 0){
	renvoie_nom_client(client_socket_fd, data);
  }
  // Si le message commence par le mot: 'couleurs:'
  else if(strcmp(json_message.code, "couleurs") == 0){
 
	// Init
	json_object json_res;
	json_res.code = malloc(sizeof(char)*1024);
	json_res.valeurs = malloc(sizeof(char)*1024);
  	char *filename = "couleurs.txt";
  	FILE *fp = fopen(filename, "w");

	// Erreur d'ouverture
  	if (fp == NULL){
     		perror("Error opening your file");
     		return(EXIT_FAILURE);
  	}

	// Ecriture du fichier
	json_res.code = json_message.code;
	json_res.valeurs = "enregistre";
  	fprintf(fp, "%s" ,json_message.valeurs);
  	fclose(fp);
  
	// Renvoi du message
	renvoie_message(client_socket_fd, json_encode(&json_res, '\x32'));
  }
  // Si le message commence par le mot: 'balises:'
  else if (strcmp(json_message.code, "balises") == 0){
  
	// Init
	json_object json_res;
	json_res.code = malloc(sizeof(char)*1024);
	json_res.valeurs = malloc(sizeof(char)*1024);
  	char *filename = "balises.txt";
  	FILE *fp = fopen(filename, "w");

	// Erreur d'ouverture
  	if (fp == NULL){
     		perror("Error opening your file");
     		return(EXIT_FAILURE);
  	}

	// Ecriture du fichier
	json_res.code = json_message.code;
	json_res.valeurs = "enregistre";
  	fprintf(fp, "%s" ,json_message.valeurs);
  	fclose(fp);


	// Renvoi du message
  	renvoie_message(client_socket_fd, json_encode(&json_res, '\x32'));
  }
  else{
    plot(data);
  }

  // Fermer le socket
  close(socketfd);
  return (EXIT_SUCCESS);
}




/*
 * Fonction principale => créé le socket, écoute les messages et renvoie les réponses adéquates 
 */
int main()
{

  // Init
  int socketfd;
  int bind_status;
  struct sockaddr_in server_addr;

  
  // Creation d'une socket
  socketfd = socket(AF_INET, SOCK_STREAM, 0);
  if (socketfd < 0)
  {
    perror("Unable to open a socket");
    return -1;
  }

  int option = 1;
  setsockopt(socketfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof(option));

  // Détails du serveur (adresse et port)
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  // Relier l'adresse à la socket
  bind_status = bind(socketfd, (struct sockaddr *)&server_addr, sizeof(server_addr));
  if (bind_status < 0)
  {
    perror("bind");
    return (EXIT_FAILURE);
  }

  // Écouter les messages envoyés par le client
  listen(socketfd, 10);
  recois_envoie_message(socketfd);

  return 0;
}
