
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
#ifndef __TESTES_H__
#define __TESTES_H__

void testOperations(char* code, char* data);
void testJson(char* whoami, char* jsonCode, char* expectedCode);

#endif