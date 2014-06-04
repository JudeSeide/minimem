=============
DESCRIPTION :
=============

Ce travail consiste en une simulation d'un interpréteur pour un mini langage afin de maitriser la gestion de la
mémoire et les enjeux autour. Notre mini langage est composé d’instructions permettant de définir un
segment de mémoire courant et d’y allouer des tableaux d’entiers pointés par des variables. De même il
offre la possibilité de lancer le ramasse miettes ainsi que le compactage du segment de mémoire en cours.

=============
COMPILATION :
=============

Taper dans un terminal _make_ pour compiler le projet
Taper dans un terminal _make clean_ pour nettoyer

=============
UTILISATION :
=============

Apres compilation taper _./minimem_ pour lancer le programme.
Le programme fonctionne comme un command prompt ou shell le symbole _#_ est utilise comme demarqueur

Un ensemble de menu est fourni le majuscule est imperatif : 

* M : affiche la valeur du registre limite (la taille) du segment de mémoire en cours
    * synopsis : M
    * _exemple_ : M

* I : libère le segment de mémoire en cours, supprime toute les variables et crée un nouveau segment de longueur taille en
	  terme de la taille d’un int. taille est un entier strictement positif. Toute les cases dans le nouveau segment sont
	  initialisées à zéro.
    * synopsis : I taille
    * _exemple_ : I 40
	  
* N : alloue dans le segment de mémoire en cours un tableau (référencé par la variable identif) de taille entiers et initialise
	  ses cases avec les éléments de list. La variable identif peut être déjà existante. list est une séquence d’entiers séparés
	  par des blancs. On suppose que taille représente toujours la longeur de list. taille ne peut être nulle.
    * synopsis : N identif taille list
    * _exemple_ : N v1 4 61 62 63 64
	  
* D : supprime la variable identif mais pas le tableau. Si cette variable n’existe pas, un message d'erreur permet de le signaler.
    * synopsis : D identif
    * _exemple_ : D v0
	  
* R : permet d’ajouter ou de modifier identif1 comme une autre référence du tableau référencé par identif2. Si la variable identif2
	  n’existe pas cette instruction est ignorée et un message d'erreur permet de le signaler.
    * synopsis : R identif1 identif2
    * _exemple_ : R v0 v1
	  
* P : affiche taille éléments entiers du segment de mémoire en cours à partir de la position pos. Une erreur est signalée si les bornes
	  du segment de mémoire courant sont dépassés en utilisant la fonction appropriée.
    * synopsis : P pos taille
    * _exemple_ : P 0 32
	  
* L : affiche selon l’ordre alphabétique des noms, la liste des variables avec les positions de début des zones qu’elles réfèrent et leurs
	  tailles respectives 
    * synopsis : L
    * _exemple_ : L
	 
* C : lance le ramasse miettes et le compacteur de la mémoire
    * synopsis : C
    * _exemple_ : C

* Q : permet de quitter le programme
    * synopsis : Q
    * _exemple_ : Q
	  

