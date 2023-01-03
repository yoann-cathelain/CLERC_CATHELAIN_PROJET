
/*
 * SPDX-FileCopyrightText: 2022 John Samuel
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 */

/* 
 * L'objectif principal de ce code est d'effectuer des tests unitaires et
 *  fonctionnels sur les différents composants créés dans ce projet.
 */
#include <stdio.h>
#include <string.h>

// Teste que les fonctionnalités du serveur sont supportées
void testOperations(char* code, char* data){
    if(strcmp(code, data) == 0){
        printf("Test de support de '%s': OK\n", code);
    }else{
        printf("Test de support de '%s': NOK\n", code);
    }
}

// Teste que le client et le serveur comprennent les messages JSON
void testJson(char* whoami, char* jsonCode, char* expectedCode){
    if(strcmp(jsonCode, expectedCode) == 0 ){
        printf("Test de compréhension du JSON par le %s: OK\n", whoami);
    }else{
        printf("Test de compréhension du JSON par le %s: NOK\n", whoami);
    }
}

/*
 Le test de protocole se fait avec un test fonctionnel en connectant un client au serveur et en analysant les messages échangés.
    On observe bien que le serveur et le client communiquent bien avec le protocole défini.
*/

/* 
 Le test multi client se fait avec un test fonctionnel en connectant plusieurs clients à la fois.
    On observe bien que le serveur est capable de gérer les différentes demandes.
*/