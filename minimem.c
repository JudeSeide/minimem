#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "util.h"

/**
 * Ce travail consiste à réaliser un interpréteur pour un mini langage afin de maitriser la gestion de la
 * mémoire et les enjeux autour. Notre mini langage est composé d’instructions permettant de définir un
 * segment de mémoire courant et d’y allouer des tableaux d’entiers pointés par des variables. De même il
 * offre la possibilité de lancer le ramasse miettes ainsi que le compactage du segment de mémoire en cours.
 *
 * @author Jude Seide : SEIJ04019006
 * @author Amoussou Landry : AMOL12049303
 */

/**
 * typedef pour simplifier l'appel aux structure zone et var definit dans util.h
 */
typedef struct zone Zone;
typedef struct var Var;

/**
 * Structure contenant l'information pour le segment memoire en cours.
 * Les champs sont publiques, aucun accesseur n'existe.
 */
typedef struct{
	//tableau contenant les valeurs en memoire
	int *tab;	
	
	//tableau indiquant si une position du segment
	//memoire est libre[0] ou non[1]	
	int *bitmap;	
	
	//taille commune aux tableaux precedents
	//la limite du registre
	int taille;
}Registre;

/**
 * Structure contenant l'information pour le tableau de var.
 * Les champs sont publiques, aucun accesseur n'existe.
 */
typedef struct{
	//tableau de pointeurs de Var <=> struct var
	Var **tab;
	
	//le nombre de var contenu dans le tableau
	int nbr;
	
	//taille de l'espace allouee au tableau
	int taille;
}Variables;

/**
 * Constructeurs :
 * alloue la memoire necessaire selon la structure a creer
 */

Zone * construire_zone(const int taille, const int pos)
{
	Zone * nouvelleZone = (Zone *)malloc(sizeof( Zone ));
    
	if( NULL == nouvelleZone )
	{
        fprintf( stderr, "Zone : construire_zone : erreur d'allocation.\n" );
        exit( -1 );
  	}
    
	nouvelleZone->taille = taille;
    nouvelleZone->pos = pos;
    
    return nouvelleZone;
}

Var * construire_var(char * id, Zone * tableau)
{
	Var * nouvelleVar = (Var *)malloc(sizeof( Var ));
    
	if( NULL == nouvelleVar )
	{
        fprintf(stderr, "Var : construire_var : erreur d'allocation.\n");
        exit( -1 );
  	}
    
	for(int i = 0; i < strlen(id) && i < MAX_IDENTIF; i++)
	{
		nouvelleVar->id[i] = id[i];		
	}
    
    nouvelleVar->tableau.pos = tableau->pos;
    nouvelleVar->tableau.taille = tableau->taille;
    free(tableau); // liberation de la Zone
	return nouvelleVar;
}

Variables * construire_variables(const int max)
{
	Variables * resultat = (Variables *)malloc(sizeof( Variables ));
    
    if( NULL == resultat )
	{
        fprintf( stderr, "Variables : construire_variables : erreur d'allocation.\n" );
        exit( -1 );
  	}
    
	resultat->tab = calloc(max, sizeof( Var* ));
	resultat->nbr = 0;
	resultat->taille = max;
    
	return resultat;
}

Registre * construire_registre(const int taille)
{
	int * p = (int *) calloc(taille, sizeof(int));
	int * q = (int *) calloc(taille, sizeof(int));
	Registre * resultat = (Registre *)malloc (sizeof( Registre ));
    
    if( NULL == resultat )
	{
        fprintf( stderr, "Registre : construire_registre : erreur d'allocation.\n" );
        exit( -1 );
  	}
    
	resultat->tab = p;
	resultat->bitmap = q;
	resultat->taille = taille;
	return resultat;
}

/** 
 * FIN CONSTRUCTEURS
 * -----------------
 * Destructeurs :
 * desalloue la memoire selon la structure a liberer
 */

void detruire_var(Var * var)
{
	assert( NULL != var &&
           "Var : detruire_var : argument NULL." );
   	free( var );
}

void libererMemoire(Registre * r, Variables * v)
{
    int taille = v->nbr;
	for(int i = 0; i < taille; i++)
	{
        detruire_var( v->tab[i] );
	}
	free(v->tab);
	free(r->tab);
	free(r->bitmap);
}

/**
 * FIN DESTRUCTEURS
 * ----------------
 * Tri rapide :
 * algorithme de tri rapide des variables selon l'ordre alphabetique
 */

void echanger(Var * tab[], const int indice1, const int indice2)
{
	Var * temp;
	temp = tab[indice1];
	tab[indice1] = tab[indice2];
	tab[indice2] = temp;
}

int partition(Var * tab[], const int debut, const int fin)
{
	int curseur = debut;
	char *id = (tab[debut])->id;
    
	for(int i = debut+1; i <= fin; ++i)
	{
    	if( strncmp((tab[i])->id, id, MAX_IDENTIF) < 0)
		{
        	curseur++;
        	echanger(tab, curseur, i);
		}
	}
    
	echanger(tab, curseur, debut);
	return curseur;
}

void tri_rapide(Var * tab[], const int debut, const int fin)
{
	if(debut < fin)
	{
		int pivot = partition(tab, debut, fin);
		tri_rapide(tab, debut, pivot-1);
		tri_rapide(tab, pivot+1, fin);
	}
}

void trier(Var * tab[], const int taille)
{
	tri_rapide(tab, 0, taille-1);
}

/** 
 * FIN TRI RAPIDE
 * ----------------------------
 * chercherVar / deplacerZone :
 */

/**
 * chercher si une variable avec le meme id (char* s) existe
 * retourne sa position dans le tableau de v (Variables * v)
 * si elle existe -1 sinon
 */
int chercherVar(Variables * v, char *s)
{
	int i;
	int trouve = 1;
	int taille = v->nbr;
    
	for(i = 0; i < taille; i++)
	{
		if(strncmp( (v->tab[i])->id, s, MAX_IDENTIF) == 0)
		{
			trouve = 0;
			break;
		}
	}
    
	if(trouve == 0)
	{
		return i;
	}
	return -1;
}

/**
 * parcours le tableau de v (Variables * v) et modifie leur position 
 * a n_pos (int n_pos) si leur position est egal a pos (int pos)
 */
void deplacerZone(Variables * v, int pos, int n_pos)
{
	int taille = v->nbr;
	
	for(int i = 0; i < taille; i++)
	{
		if((v->tab[i])->tableau.pos == pos)
		{
			(v->tab[i])->tableau.pos = n_pos;
		}
	}
}


/** 
 * FIN CHERCHERVAR / DEPLACERZONE
 * ------------------------------
 * Compacteur / Ramasse-miette :
 */

/**
 * marque a libre l'espace occupee par les variables derefencees
 * se sert de la bitmap pour accomplir cette tache
 * @param r : registre sur lequel la fonction est appliquée.
 * @param v : table de variables sur lequel la fonction est appliquée.
 */
void lancerRamasseMiette(Registre * r, Variables * v)
{
	int taille = 0;
	int pos = 0;
	
	int * q = (int *) calloc(r->taille, sizeof(int));
	for(int i = 0; i < v->nbr; i++)
	{
		pos = (v->tab[i])->tableau.pos;
		taille = (v->tab[i])->tableau.taille;
		
		for(int j = pos; j < (taille + pos); j++)
		{
			q[j] = 1;
		}
	}
	
	free(r->bitmap);
	r->bitmap = q;
}

/**
 * compacte de maniere contigue l'espace occupee par les variables
 * et met a jour leur position
 * se sert de la bitmap pour accomplir cette tache
 * @param r : registre sur lequel la fonction est appliquée.
 * @param v : table de variables sur lequel la fonction est appliquée.
 */
void lancerCompacteur(Registre * r, Variables * v)
{
	int * p = (int *) calloc(r->taille, sizeof(int));
	int * q = (int *) calloc(r->taille, sizeof(int));
	int j = -1;
    
	for(int i = 0; i < r->taille; i++)
	{
		if(r->bitmap[i] == 1)
		{
			p[++j] = r->tab[i];
			q[j] = 1;
			deplacerZone(v, i, j);
		}
        
	}
	
	free(r->bitmap);
	free(r->tab);
	r->tab = p;
	r->bitmap = q;
}

/* FIN COMPACTEUR / RAMASSE-MIETTE
 * ---------------------------------------------------------------------
 * FirstFit : trouve le position du premier trou qui correspond à taille
 *            dans le registre r.
 * @param r : registre sur lequel la fonction FirstFit est appliquée.
 * @param taille : taille de trou à trouver.
 * @return	-1 si aucun trou la position de debut sinon
 */

int firstFit(Registre * r, int taille)
{
    int retour = -1;
    int i = 0;
    int ok = 0;
    
    do {
        
        while (i < r->taille && r->bitmap[i] != 0)
        {
            ++i;
        }
        
        if (i >= r->taille)
        {
            return retour;
        }
        else
        {
            int debut = i;
            int j = 0;
            
            while (j < taille && r->bitmap[i] == 0 && i < r->taille)
            {
                ++j;
                ++i;
            }
            
            if (j == taille)
            {
                retour = debut;
                ok = 1;
            }
            else if(i >= r->taille)
            {
                return retour;
            }
            
        }
        
    } while (ok == 0);
    
    return retour;
}

/**
 * FIN FIRSTFIT
 * -------------------------
 * AjouterNouvelleVariable :
 */
 
/**
 * Fonction responsable de l'ajout d'une nouvelle variable selon l'algorithme
 * firstFit avec lancement du ramasse-miettes et compacteur si necessaire 
 *
 * @param exist : -1 si la variable n'existait pas sa position dans le tableau sinon
 * @param list: séquence d’entiers definissant la variable
 * @param var : la variable a ajouter
 * @param r : registre dans lequel l'ajout va etre tente.
 * @param v : table de variables dans lequel l'ajout va etre tente.
 * @return ok = -1 si l'ajout a echoue ok != -1 si reussi
 */
int AjouterNouvelleVariable(Registre * r, Variables * v,
                            Var * var, int * list, int exist){
    // sert a determiner si l'ajout a reussi, s'il faute retenter
    // le firstFit ou abandonner l'ajout                        
    int ok = -1; 
    int estRamasser = 0; // sert a determiner si le ramasse-miettes a ete lancer
    int estALibere = 0; // sert a determiner s'il faut liberer la variable
    int lancer_compacteur = 1; // sert a determiner si le compacteur a ete lancer
    int i;
    
    do {
        int pos = firstFit(r, var->tableau.taille); //tentative de firsFit
        
        if (pos != -1) // firstFit a trouve un trou
        { 
        	var->tableau.pos = pos;
        	
            if (exist != -1) // la variable existait deja
            { 
                ok = exist;
                (v->tab[ok])->tableau.pos = var->tableau.pos;
                (v->tab[ok])->tableau.taille = var->tableau.taille;
                
                for(i = 0; i < var->tableau.taille; i++)
                {
                    r->tab[(pos + i)] = list[i];
                    r->bitmap[(pos + i)] = 1;
                }
            }
            else // la variable n'existait pas encore
            {
            	if(v->nbr >= v->taille)
				{
					v->taille *= 2;
					v->tab = realloc(v->tab, sizeof( Var* ) * v->taille);
				}
				
                v->tab[v->nbr] = var;
                ok = v->nbr;
                v->nbr += 1;
            }
            
            for(i = 0; i < var->tableau.taille; i++)
            {
                r->tab[(pos + i)] = list[i];
                r->bitmap[(pos + i)] = 1;
            }            
        }
        else // firstFit n'a trouve aucun trou
        {
            if (estRamasser == 0) // lance le ramasse miettes
            {
                lancerRamasseMiette(r, v);
                estRamasser = 1;                
            }
            else
            {
                int libre = 0;
                
                for (i = 0; i < r->taille; ++i) // on compte l'espace disponible
                {
                    if (r->bitmap[i] == 0) 
                    {
                        ++libre;
                    }
                }
                
                if (libre >= var->tableau.taille) // il y a assez d'espace
                {
                    // lancer le compacteur si la variable n'existait pas
                    if (exist == -1 && lancer_compacteur == 1) 
                    { 
                        lancerCompacteur(r,v);
                    }
                    else if(estALibere == 0) // sinon liberer l'espace qu'elle occupe
                    {
                        int pos = -1;
                        int taille = v->nbr;
                        
                        for(i = exist; i < taille; i++)
                        {
                            pos = i + 1;
                            if( pos < taille)
                            {
                                echanger(v->tab, i, pos);
                            }
                        }
                        
                        v->nbr -= 1;
                        exist = -1;
                        estRamasser = 0;
                        estALibere = 1;
                        lancer_compacteur = 0;
                    }
                    else
                    {
                        return ok;
                    }
                    
                }
                else
                {
                    return ok;
                }
            }
        }

    }while (ok == -1);
    
    return ok;
}

/**
 * FIN AJOUTERNOUVELLEVARIABLE
 * ----------------------------
 * Menus :
 * appel aux fontions ou algorithmes excutant le menu saisi
 * ces fonctions considerent que toutes saisies au clavier est correct
 * et attendent le nombre correct de paramtres valides
 * -------------------------------------------------------------------
 * Aucune validation n'est faite pour les saisies
 */

/**
 * affiche la valeur du registre limite (la taille) du segment de mémoire en cours
 */
void effectuerMenuM(Registre * r)
{
    print_limit(r->taille);
}

/**
 * libère le segment de mémoire en cours, supprime toute les va-
 * riables et crée un nouveau segment de longueur <taille> en
 * terme de la taille d’un int.
 * -<taille> est un entier strictement positif
 * -Toute les cases dans le nouveau segment sont initialisées à zéro.
 */
void effectuerMenuI(Registre * r, Variables * v, char *s)
{
	strtok(s, " ");
	char* p = strtok(NULL," ");
	int taille = atoi(p);
		
	*r = *construire_registre(taille);
	*v = *construire_variables(20);
}

/**
 * alloue dans le segment de mémoire en cours un tableau (référencé
 * par la variable <identif>) de <taille> entiers et initialise ses
 * cases avec les éléments de <list>. La variable <identif> peut
 * être déjà existante.
 * - <list> est une séquence d’entiers séparés par des blancs.
 * - On suppose que <taille> représente toujours la longeur de
 * <list>. <taille> ne peut être nulle.
 */
void effectuerMenuN(Registre * r, Variables * v, char *s)
{
	strtok(s, " ");
    
	char* id = strtok(NULL," ");
	char* p = strtok(NULL," ");
	int taille = atoi(p);
	int list[taille];
	int i = -1;
    
	p = strtok(NULL, " ");
    
	while(p != NULL)
	{
		list[++i] = atoi(p);
		p = strtok(NULL, " ");
	}
	
    Zone * zone = construire_zone( taille, 0 );
    Var * var =  construire_var( id , zone );
    
    if (AjouterNouvelleVariable(r, v, var, list, chercherVar(v, id)) == -1)
    {
        print_erreur(MEM_INSUFFISANTE);
    }    
}

/**
 * supprime la variable <identif> mais pas le tableau
 * - Si cette variable n’existe pas, l’appel print_erreur(NO_VAR)
 * permet de le signaler.
 */
void effectuerMenuD(Registre * r, Variables * v, char *s)
{
	if(v->nbr == 0)
	{
		print_erreur(NO_VAR);
	}
	else
	{
		strtok(s, " ");
		char* id = strtok(NULL," ");
		int pos = -1;
		int taille = v->nbr;
		int i = chercherVar(v, id);
        
		if(i != -1)
		{
            
			for(; i < taille; i++)
			{
				pos = i + 1;
			 	if( pos < taille)
				{
					echanger(v->tab, i, pos);
				}
			}
            
			detruire_var( v->tab[taille - 1] );
			v->tab[taille - 1] = NULL;
			v->nbr -= 1;
		}
		else
		{
			print_erreur(NO_VAR);
		}
	}
}

/**
 * permet d’ajouter ou de modifier <identif1> comme une autre
 * référence du tableau référencé par <identif2>
 * - Si la variable <identif2> n’existe pas cette instruction est
 * ignorée et l’appel print_erreur(NO_VAR) permet de le signaler.
 */
void effectuerMenuR(Variables * v, char *s)
{
	strtok(s, " ");
	char* id1 = strtok(NULL," ");
	char* id2 = strtok(NULL," ");
	
	int trouve = chercherVar(v, id2);
    
	if(trouve == -1)
	{
		print_erreur(NO_VAR);
	}
	else
	{
		int pos = chercherVar(v, id1);
		Zone tmp = (v->tab[trouve])->tableau;
		Zone * z = construire_zone(tmp.taille, tmp.pos);
        
		if(pos == -1)
		{
            Var * var =  construire_var(id1 , z);
            
			if(v->nbr >= v->taille)
			{
				v->taille *= 2;
				v->tab = realloc(v->tab, sizeof( Var* ) * v->taille);
			}
            
            v->tab[v->nbr] = var;
			v->nbr += 1;
		}
		else
		{
			free(&(v->tab[pos])->tableau);
			(v->tab[pos])->tableau = *z;
		}
		
	}
}

/**
 * affiche <taille> éléments entiers du segment de mémoire en
 * cours à partir de la position <pos>.
 * - Une erreur est signalée si les bornes du segment de mémoire
 * courant sont dépassés en utilisant la fonction appropriée.
 */
void effectuerMenuP(const Registre * r, char *s)
{
	char* p = strtok(s, " ");
	
	p = strtok(NULL," ");
	int pos = atoi(p);
	
	p = strtok(NULL," ");
	int taille = atoi(p);
    
	print_zone_contenu(r->tab, r->taille, pos, taille);
}

/**
 * affiche selon l’ordre alphabétique des noms, la liste des variables
 * avec les positions de début des zones qu’elles réfèrent et leurs
 * tailles respectives
 */
void effectuerMenuL(Variables * v)
{
	const int taille = v->nbr;
	if(taille > 1)
	{
		trier(v->tab, taille);
	}
	
	print_vars(v->tab);
}

/**
 * lance le ramasse miettes et le compacteur de la mémoire
 */
void effectuerMenuC(Registre * r, Variables * v)
{
	lancerRamasseMiette(r, v);
	lancerCompacteur(r, v);
}

/**
 * FIN MENUS
 * --------------------
 * Lecture au clavier :
 */

void LireChaine(char chaine[], int size)
{
    fgets(chaine, size, stdin);
    chaine[strlen(chaine) - 1] = '\0';
}

/**
 * ------
 * Main :
 */
int main() {
	
	const int TAMPON = 1024;
    char params[TAMPON];
	char option = 0;
    
	Variables * tab_vars;
	Registre * segment_memoire;
    
	tab_vars = construire_variables(20);
	segment_memoire = construire_registre(REGISTRE_LIMIT_INITIAL);
 	
	do{
		print_prompt();
		LireChaine(params, TAMPON);
        
		assert( NULL != segment_memoire && "main : segment_memoire NULL." );
		assert( NULL != tab_vars && "main : tab_vars NULL." );
		assert( NULL != params && "main : params NULL." );
		
		option = params[0];
	 	
		switch(option){
                
			case 'M'  :
		   		effectuerMenuM(segment_memoire);
		   		break;
                
	   		case 'I'  :
		   		effectuerMenuI(segment_memoire, tab_vars, params);
		   		break;
                
			case 'N'  :
		   		effectuerMenuN(segment_memoire, tab_vars, params);
		   		break;
                
			case 'D'  :
		   		effectuerMenuD(segment_memoire, tab_vars, params);
		   		break;
                
		  	case 'R'  :
		   		effectuerMenuR(tab_vars, params);
		   		break;
                
			case 'P'  :
		   		effectuerMenuP(segment_memoire, params);
		   		break;
                
	 		case 'L'  :
		   		effectuerMenuL(tab_vars);
		   		break;
                
			case 'C'  :
				effectuerMenuC(segment_memoire, tab_vars);
		   		break; 
                
			case 'Q'  :
				libererMemoire(segment_memoire, tab_vars);
				free(segment_memoire);
				free(tab_vars);
		   		break; 
                
		}
        
	}while(option != 'Q');
    
    return 0; 
}
