#include "edit_functions.h"

/**
* Get the actual position of the character in the pos position on the screen
*
* @param ln
* @param rpos
* @return 
*/
int getActualPos(line *ln,int rpos)
{
 int pos_screen=0,pos_real=0;
 character *caux=ln->first_ch;
 while(caux)
 {
  if(caux->c == '\t')
    pos_screen+=SPACES_TAB-1;
  pos_screen++;
  if(pos_screen > rpos)
    return pos_real;
  caux=caux->next;
  pos_real++;
 }
 return --pos_real;
}

/**
* Get the actual position of the character in the pos position on the screen
*
* @param ln
* @param rpos
* @return 
*/
int getScreenPos(line *ln, int rpos)
{
  int cont=0;
  character *aux=ln->first_ch;
  while(aux && cont<rpos)
  {
   if(aux->c=='\t')
     cont+=SPACES_TAB-1;
   cont++;
   aux=aux->next;
    
  }
  if(cont>rpos)
    cont-=SPACES_TAB;
  if(!aux)
    cont--;
  return cont;
  
}

/**
* Get the number of tabs of the line ln, starting in position pos
*
* @param ln
* @param pos
* @return 
*/
int getNumTabs(line *ln, int pos)
{
 character *aux=ln->first_ch;
 int cont=0,num_tbs=0;
 while( aux && cont < pos )
 {
   aux=aux->next;
   cont++;
 }
 
 while(aux)
 {
  
  if(aux->c == '\t')
  {
    num_tbs++;
  }
  aux=aux->next;
 }
 return num_tbs;
}


/**
* Get the position of the character ch in the line ln
*
* @param ln
* @param ch
* @return 
*/
int getPosCh(line *ln, character *ch)
{
 character *aux=ln->first_ch;
 int cont=0;
 while(aux && aux!=ch)
 {
   cont++;
   aux=aux->next;
 }
 if(!aux)
 {
  printw("ERROR ON getPosCH\n");
  return -1;
 }
 return cont;
  
}

/**
* Print SPACES_TAB blank spaces 
*/
void print_tab()
{
  int i;
  for(i=0;i<SPACES_TAB;i++)
    printw(" ");
}

void empty_line(int nline,int size_line)
{
 int i;
 move(nline,0);
 for(i=size_line-1;i>=0;i--)
  delch();
}

/**
* Print the all the options at the bottom of the screen
*/
void print_options()
{
  move(LINES-1,0);
  printw("^F:EXIT  ^X:EXECUTE  ^H:SAVE  ^L:LOAD  ^W:CLEAR");
}

/**
* Free all the memory allocated to data structures of the editor
*
*/
void freeEditor()
{
 line *aux_l=first_line,*aux_l2;
 character *aux_c=aux_l->first_ch,*aux_c2;
 while(aux_l)
 {
   aux_l2=aux_l->next;
   while(aux_c)
   {
    aux_c2=aux_c->next;
    freeCharacter(&aux_c);
    aux_c=aux_c2;
   } 
   freeLine(&aux_l);
   aux_l=aux_l2;
 }
 clear();
 move(0,0);
 refresh();
}


/**
  * Initialize the data structures of the editor
  *
  */
void init_editor()
{
  modified=0;//if the user has pressed any key
  ncurrent_line=0;//number of the current line
  num_lines=1; //number of lines
  actual_rows=LINES-2;
  actual_cols=COLS;
  num_lines_scrolled=0;
  init_line(&first_line);
  init_character(&eof);
  eof->c='\0';//EOF
  init_character(&prev_character);
  current_line=first_line;
  current_line->num_line=1;
  current_character=eof;
  prev_character=eof;
  current_line->first_ch=current_character;
  current_line->size=1;
  print_options();
  move(0,0);
}

/**
 * Returns the character in the pos position in the ln line
 * 
 * @param ln 
 * @param pos 
 * @return 
 */
character* getChLine(line *ln,int pos)
{
  character *ret=ln->first_ch;
  int n=0;
  
  while(n<pos && ret)
  {
   ret=ret->next;
   n++;
  }
  
  return ret;
  
}

/**
 * Free the memory allocated to character c
 * @param c
 */

void freeCharacter(character **c)
{
 free(*c);
 *c=NULL;
}


/**
 * Free the memory allocated to line l
 * @param l
 */
void freeLine(line **l)
{
 free(*l);
 *l=NULL;
}


/**
 * Print the text written in the editor, from the line ln, in the Window wn
 * to the end of the text
 * @param ln
 * @param wn
 */

void print_ln(const line *ln, WINDOW *wn)
{
  character *aux=ln->first_ch;
  while(aux)
  {
    if(aux->c != '\n' && aux->c !='\0' && aux->c!='\t')
      wprintw(wn,"%c",aux->c);
    else if(aux->c == '\t')
      print_tab();
   aux=aux->next;
  }
}


/**
 * Allocates memory to the character ch
 * @param ch
 */
void init_character(character **ch)
{
  *ch=malloc(sizeof(struct character));
  (*ch)->next=NULL;
  (*ch)->prev=NULL;
}

/**
 * Allocates memory to the line l
 * @param l
 */
void init_line(line **l)
{
  *l=malloc(sizeof(struct line));
  (*l)->next=NULL;
  (*l)->prev=NULL;  
  (*l)->first_ch=NULL;
  (*l)->size=0;
  (*l)->num_tabs=0;
}

/**
 * Print a subset of the written text in the editor, taking into account the 
 * number of lines scrolled and the number of rows of the screen
 * @param ln_scr number of lines scrolled
 * @param num_rows number of rows of the screen
 */
void print_all(int ln_scr, int num_rows)
{
  int n=0;
  line *aux=first_line;
  while(n<ln_scr && aux)
  {
    aux=aux->next;
    n++;
  }
  clear();
  n=0;
  while(aux && n<num_rows)
  {
   move(n,0);
   print_ln(aux,stdscr);
   aux=aux->next;
   n++;
  } 
  print_options();
  refresh();
}



/**
* Loads the file file
* @param file The descriptor of the file
*/

void loadFile(int file)
{
  char buff[1];
  line *prev_line=NULL,*curr_line=first_line;
  character *curr_c=NULL,*prev_character=NULL;
  curr_line->size=0;int cont=0;
  while(read(file,buff,1))
  {
    if(buff[0] == '\t')
      curr_line->num_tabs++;
    init_character(&curr_c);
    curr_c->c=buff[0];
    if(prev_character)
      prev_character->next=curr_c;
    curr_c->prev=prev_character;
    if(curr_line->size == 0)
      curr_line->first_ch=curr_c;
    curr_line->size++;
    prev_character=curr_c;
    if(buff[0] == '\n')
    {
      num_lines++;
      curr_c->next=NULL;
      prev_character=NULL;
      prev_line=curr_line;
      init_line(&curr_line);
      prev_line->next=curr_line;
      curr_line->prev=prev_line;
      curr_line->size=0;
      curr_line->num_line=num_lines;
      curr_line->first_ch=eof;
    }
    
    //printw("c[%d]=%c;\n",cont,buff[0]);
    cont++;
  }
  //the last character is eof
  if(curr_line->first_ch != eof)
  {
    curr_c->next=eof;
    eof->prev=curr_c;
  }
  curr_line->size++;//the eof character
  current_character=first_line->first_ch;
  current_line=first_line;
}


/**
 * Save the contents of the editor to the file file, 
 * @param file The descriptor of the file
 * @return 
 */
int saveToFile(int file)
{
  int i,last=0;
  character *caux;
  char *buffer;
  line *aux=first_line;
  do
  {
    buffer=malloc(aux->size);
    caux=aux->first_ch;
    for(i=0;i<aux->size && caux;i++)
    {
      if(caux -> c != '\0')
      {
	buffer[i]=caux->c;
      }
      else
	last=1;
      caux=caux->next;
    }
    write(file,buffer, last==0 ? aux->size: aux->size-1);
    aux=aux->next;
  }
  while(aux);
  return 1;
}
