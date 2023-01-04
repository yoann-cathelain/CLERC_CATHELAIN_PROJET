
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */


/*
 *  Fonction de validation de format JSON
 *  Renvoi 0 si la chaîne de caractère JSON n'est pas au bon format ou 1 si le format est correct
 */
int validate_json(char*);

int validate_json_nom(char*);

int validate_json_calcul(char*);

int validate_json_message(char*);

int validate_json_couleurs(char*);

int validate_json_balises(char*);
/* 
 * Le but principal de ce code est de s'assurer que les messages envoyés par le client sont
 * valides, c'est-à-dire que les messages respectent le format JSON et
 * respectent le protocole spécifié par le côté serveur.
 */
