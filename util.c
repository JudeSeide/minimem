#include"util.h"



char *tostring(struct zone t){
  char * str = malloc( 2 * MAX_DIGIT + 1 + 1);
  sprintf(str,"%0*d:%0*d",MAX_DIGIT, t.pos,MAX_DIGIT,t.taille);
  return str;
}

void print_var(struct var v){
	char *tostr = tostring(v.tableau);
  	printf("%s(%s)\n",v.id, tostr);
	free(tostr);
}




void print_erreur(int err)
{
  switch (err)
  {
    case MEM_INSUFFISANTE :
      printf("Memoire insuffisante\n");
      break;
    case  NO_VAR :
      printf("Variable inexistante\n");
      break;
    case HORS_SEG :
      printf("Hors segment en cours\n");
      break;
    default:
      break;
  }
}

void print_limit(int val){
  printf("Registre limite : %d\n", val);
}


void print_prompt(){
  printf("#");
}

void print_vars(struct var * vars[]){
  if(vars[0] == NULL){
    printf("Aucune variable\n");
  }
  else
  {
    struct var **curseur = vars;
    do
    {
      print_var(**curseur);
    }
    while (*++curseur);
  }
}


void print_zone_contenu(const int const * reg_base,int reg_limit, int pos, int taille){
  if(pos+taille <= reg_limit)  
  {
    printf("-------");
    for(int i=0; i < NBRE_LIGNE; i++ )
    {
      printf("---%d---|",i);
    }
    printf("\n%04d | ",pos/NBRE_LIGNE);
    
    if( pos % NBRE_LIGNE != 0)
    {
      printf("%*c",pos % NBRE_LIGNE * 8,' ');
    }

    for(int i=pos; i < taille+pos; i++){
      printf("[%05d] ", reg_base[i]);      
      if ((i+1)%NBRE_LIGNE == 0){
        if(i+1 < taille+pos)
        {
          printf("\n%04d | ",i/NBRE_LIGNE+1);
        }
        else
        {
          printf("\n");
        }
      }
    }
    if ((taille+pos) % NBRE_LIGNE !=0)
    {
      printf("\n");
    }
  }
  else
  {
    print_erreur(HORS_SEG);
  }
}
