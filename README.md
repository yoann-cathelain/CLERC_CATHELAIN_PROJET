# Projet de programmation C

Ce dépôt contient un projet universitaire de programmation C. Il s'agit d'un serveur TCP qui permet d'effectuer des calculs, d'envoyer des messages, de récupérer des couleurs et des balises d'image, et d'analyser les couleurs d'une image.

Pour fonctionner, le programme utilise gnuplot afin afficher les graphiques. Si vous ne l'avez pas vous pouvez l'installer avec la commande suivante sur un système Debian:

```bash
sudo apt-get install gnuplot
```

## Installation

Exécuter les commandes suivantes ce qui créera les exécutables ```serveur``` et ```client```:

```bash
git clone https://github.com/yoann-cathelain/CLERC_CATHELAIN_PROJET
cd CLERC_CATHELAIN_PROJET/src
make
```

## Utilisation

Tous les messages sont encodés et affichés en JSON. Pour commencer à intéragir avec le serveur, lancez l'exécutable du serveur. Ensuite les commandes différentes seront du côté du client.

* Pour envoyer un message simple:

```bash
./client message
```

Le serveur vous répondra alors en vous demandant votre message et vous pourrez le lui envoyer en le tapant dans le terminal.

* Pour envoyer votre nom d'hôte:

```bash
./client nom
```

* Pour faire effectuer des opérations au serveur:

```bash
./client calcul
```

Il existe différents types d'opérations: l'addition, la soustraction, la multiplication la division, le minimum, le maximum, la moyenne, et l'écart-type. Le choix de cette opération se fera dans un second temps. Après la première commande, le serveur répondra en vous demandant l'opérateur et les opérandes à utiliser. Il vous suffira de les envoyer dans le bon format: ````<code d'operateur> <liste d'opérandes>````. Par exemple pour la moyenne:

```bash
moyenne 12 15 19.5 8.2
```

Les codes d'opérateurs sont les suivants:

```text
+ pour l'addition
- pour la soustraction
* pour la multiplication
/ pour la division
min pour le minimum
max pour le maximum
avg pour la moyenne
ecart pour l'écart-type.
```

Lorsque vous effectuez un calcul basique, il se fera successivement selon l'ordre des opérandres. Par exemple, pour l'addition ```+ 78 7 8 12.9```, le calcule sera ```78 + 7 + 8 + 12.9```.

* Pour envoyer des couleurs:

```bash
./client couleurs
```

Le serveur vous demandera alors de lui envoyer les couleurs que vous souhaitez. Il faudra alors taper le nombre de couleurs que vous voulez entrer puis les couleurs au format hexadécimal séparées par des virgules. Le serveur enregistrera les couleurs et vous l'indiquera.

* Pour envoyer des balises d'image:

```bash
./client balises
```

Cette commande fonctionne de la même manière que les couleurs. Le serveur vous répond et vous lui envoyer le nombre de balises à envoyer puis les balises séparées par une virgule (chaque balise commence par un #). Le serveur enregistrera les balises et vous l'indiquera.

* Pour analyser les couleurs prédominantes d'une image:

```bash
./client <fichier_bmp>
```

Le serveur vous enverra alors un graphique contenant les couleurs prédominantes de l'image envoyée.

## Bibliothèques

* sys/types
* sys/socket
* sys/epoll
* netinet/in
* stdio
* stdlib
* string
* unistd
* math

## Références

* Documentation de fonctions C: <https://koor.fr>
* Forum de développement: <https://stackoverflow.com/>

## Difficultés

* Estimation générale: moyenne
* Comprendre le code qui était donné de base.
* Le parser JSON a posé de nombreuses difficultés: construction d'une chaîne au bon format, diviser les arguments dans un tableau de valeurs.
