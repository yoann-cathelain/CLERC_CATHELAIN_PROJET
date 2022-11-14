
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/*
 * Code permettant de stocker différents types de couleurs : format 24 bits (RGB) ou format 32 bits (32). 
 */

 //Fonction permettant de décoder une chaîne de caractère au format JSON
json_object json_decode(char *);

//Fonction qui transforme une chaîne de caractère au format JSON
char* json_encode(json_object *, char*);

//Structure de données pour un objet json
typedef struct 
{
    char* code;
    char* valeurs;

}json_object