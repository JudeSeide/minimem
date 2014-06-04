#ifndef UTIL_H
#define UTIL_H
#include <stdio.h>
#include <stdlib.h>

//taille initial du segment de mémoire courant
#define REGISTRE_LIMIT_INITIAL 32

#define NBRE_LIGNE 10
#define MAX_IDENTIF 32
#define MAX_DIGIT 10

#define MEM_INSUFFISANTE 0
#define NO_VAR 1
#define HORS_SEG 2

/**
 * struct zone est utilisé dans struct var
 */ 
struct zone{
  int pos;
  int taille;
};

/**
 * struct var représente une variable
 * tableau de type struct zone représente la zone mémoire allouée pour la variable
 */
struct var{
  char id[MAX_IDENTIF];
  struct zone  tableau;
};

/**
 * Affiche des éléments à partir d'un tableau d'entiers 
 *
 * @param reg_base   pointe  le début du tableau
 * @param reg_limit  taille à ne pas dépasser à partir du debut du tableau
 * @param pos        indexe du premier élément à afficher
 * @param taille     le nombre d'éléments à afficher
 */
void print_zone_contenu(const int const * reg_base,int reg_limit, 
                          int pos, int taille);



/**
 * Affiche les éléments de type struct var pointés par un tableau de pointeurs 
 *
 * La fonction suppose que la dernière case du tableau contient 
 * un pointeur NULL marquant la fin du tableau
 *
 * @param vars tableau de pointeur vers les stucture à afficher
 */
void print_vars(struct var * vars[]);

/**
* À utiliser pour afficher la taille du segment de mémoire en cours
*/
void print_limit(int);

/**
* À utiliser pour afficher les erreurs
*/
void print_erreur(int err);

/**
* À utiliser pour afficher le prompt 
*/
void print_prompt();
#endif
