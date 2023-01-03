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

#ifndef __CLIENT_H__
#define __CLIENT_H__

/*
 * port d'ordinateur pour envoyer et recevoir des messages
 */
#define PORT 8089

/*
 * Fonction d'envoi et de réception de messages
 * Il faut un argument : l'identifiant de la socket
 */
int envoie_recois_message(int socketfd);
void analyse(char *pathname, char *data);
int envoie_couleursPred(int socketfd, char *pathname);
int envoie_info_calcul(int socketfd);
int envoie_couleurs(int socketfd);
int envoie_nom_de_client(int socketfd);
int envoie_balises(int socketfd);
int main(int argc, char **argv);

#endif
