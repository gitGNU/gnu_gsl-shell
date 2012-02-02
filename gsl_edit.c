#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "gsl-shell.h"
#include "edit_functions.h"


void do_resize();
int saveToNewFile();
int loadNewFile();
void executeFile();


WINDOW *wnd;
line *first_line;//first line 
line *current_line;//current line of the cursor
character *prev_character,*current_character;
character *eof;//the final (empty) character 


int actual_rows;
int actual_cols;

int modified;//if the user has pressed any key
int ncurrent_line;//number of the current line
int num_lines; //number of lines
int num_lines_scrolled;




void executeFile()
{
  luaL_dofile(globalL,default_file_name);
}

int launch_editor()
{
  
  int status;
  pid_t pid;
  if((pid=fork()) == 0)//child process, the editor
  {
    gsl_editor();
  }
  else //parent process
  {
    //the parent will ignore the CTRL+C signal while editor is executing and the previous CTRL+C behaviour is saved
    struct sigaction act_ign,old_ign,act_sigexec,old_sigexec;
    act_ign.sa_handler = SIG_IGN;
    sigemptyset(&act_ign.sa_mask);
    act_ign.sa_flags = 0;
    sigaction(SIGINT, &act_ign, &old_ign);
    
    act_sigexec.sa_handler=executeFile;
    sigemptyset(&act_sigexec.sa_mask);
    act_sigexec.sa_flags=0;
    sigaction(SIGNAL_EXECUTE, &act_sigexec, &old_sigexec);
    
    while(1)
    {
      waitpid(pid,&status,0);
      if(WIFEXITED(status))
	break;
    }
    if(!WIFEXITED(status)) //improve the error handling
      printf("Child terminated with error.\n");
    //once the editor has finished, the previous behaviour is restored
    sigaction(SIGINT, &old_ign, NULL);
    sigaction(SIGNAL_EXECUTE, &old_sigexec, NULL);
    
  }
  return 1;
}

int gsl_editor()
{
  line *prev_line,*aux_line;
  character *aux_character;
  
  int row,col,nrows,ncols,aux_size,aux;

  int file;
  int ch;
  int end=0;
  int cont;
  (void) signal(SIGINT, SIG_IGN);
  signal(SIGWINCH, do_resize);
  initscr(); 
  wnd=stdscr;
  idlok(wnd,TRUE);
  idcok(wnd,TRUE);
  scrollok(wnd, TRUE);
  cbreak();  
  noecho();  
  keypad(wnd, TRUE);
  getmaxyx(wnd,nrows,ncols);  
  clear();  
  refresh(); 
  init_editor();
  
  while( (ch=wgetch(wnd))!= EXIT_EDITOR && !end) 
  {
    getyx(wnd, row,col);
    if(ch == LOAD_FILE)
    {  
      if(loadNewFile())
      {
	print_all(num_lines_scrolled,actual_rows+1);
	move(0,0);
      }
      else
      {
	print_all(num_lines_scrolled,actual_rows+1);
	move(row,col);
      }
    }
    else if(ch == SAVE)
    {
      saveToNewFile();
      clear();
      print_all(num_lines_scrolled,actual_rows+1);
      move(row,col);
    }
    else if(ch == CLEAR_EDITOR)
    {
      freeEditor();
      init_editor();
    }
    else if(ch == EXECUTE)
    {
      file=open(default_file_name, O_CREAT | O_WRONLY,S_IRWXU);
      saveToFile(file);
      close(file);
      kill(getppid(),SIGNAL_EXECUTE);
      print_all(num_lines_scrolled,actual_rows+1);
      move(row,col);
    }
    else if(ch == KEY_UP)
    {
      if(row==0 && current_line->prev!=NULL)
      {
	num_lines_scrolled--;
	current_line=current_line->prev;
	ncurrent_line--;
	print_all(num_lines_scrolled,actual_rows+1);
	current_character=getChLine(current_line,getActualPos(current_line,col));
	move(0,getScreenPos(current_line,col));
      }
      else     
      {
	if(current_line->prev!=NULL)
	{
	  ncurrent_line--;
	  current_line=current_line->prev; 
	  current_character=getChLine(current_line,getActualPos(current_line,col));
	  move(row-1,getScreenPos(current_line,col));
	}
      }
    }
    else if (ch == KEY_LEFT)
    {
      if(current_character->prev == NULL &&  row ==0 && current_line->prev!=NULL)
      {
	num_lines_scrolled--;
	ncurrent_line--;  
	current_line=current_line->prev;
	current_character=getChLine(current_line,current_line->size-1);
	print_all(num_lines_scrolled,actual_rows+1);
	move(0,current_line->size + current_line->num_tabs*(SPACES_TAB-1)-1);
      }
      else
      {
	if(current_character->prev == NULL && current_line->prev!=NULL)//go up one line
	{
	  ncurrent_line--;
	  current_line=current_line->prev;
	  current_character=getChLine(current_line,current_line->size-1);
	  move(row-1,current_line->size + current_line->num_tabs*(SPACES_TAB-1)-1);
	}
	else if(current_character->prev!=NULL)
	{
	  if(current_character->prev->c == '\t')
	    move(row,col-SPACES_TAB);
	  else
	    move(row,col-1);
	  current_character=current_character->prev;
	}
      }
    }
    else if (ch == KEY_RIGHT)
    {
      if(current_character->next!=NULL)
      {
	if(current_character->c == '\t')
	  move(row,col+SPACES_TAB);
	else
	  move(row,col+1);
	current_character=current_character->next;
      }
      else
      {
	if((row == (actual_rows)) && current_line->next!=NULL)
	{
	  num_lines_scrolled++;
	  ncurrent_line++;
	  current_line=current_line->next;
	  current_character=current_line->first_ch;
	  print_all(num_lines_scrolled,actual_rows+1);
	  move(row,0);
	}
	else
	{
	  if(current_line->next!=NULL)
	  {
	    move(row+1,0);
	    ncurrent_line++;
	    current_line=current_line->next;
	    current_character=current_line->first_ch;
	  }
	}
      }
    }
    else if (ch == KEY_DOWN)
    {
     
      if( (row == (actual_rows)) && current_line->next!=NULL )
      {
	num_lines_scrolled++;
	ncurrent_line++;     
	current_line=current_line->next;
	print_all(num_lines_scrolled,actual_rows+1);
	current_character=getChLine(current_line,getActualPos(current_line,col));
	move(row,getScreenPos(current_line,col));
      }
      else
      {
	if(current_line->next!=NULL)
	{
	  ncurrent_line++;
	  current_line=current_line->next;
	  current_character=getChLine(current_line,getActualPos(current_line,col));
	  move(row+1,getScreenPos(current_line,col));
	}
      }
    }
    else if (ch == KEY_DC)
    {
      if(current_character != eof)
      {
	if(current_character->c != '\n')
	{
	  aux_character=current_character->next;
	  current_character->next->prev=current_character->prev;
	  if(current_character == current_line->first_ch)
	  {
	    current_line->first_ch=current_character->next;
	  }
	  else
	    current_character->prev->next=current_character->next;
	  
	  if(current_character->c == '\t')
	  {
	    for(cont=0;cont<SPACES_TAB-1;cont++)
	      delch();
	    current_line->num_tabs--;
	  }
	  delch();
	  freeCharacter(&current_character);
	  current_character=aux_character;
	  current_line->size--;
	  
	}
	else //delete a line
	{
	  aux_line=current_line->next;
	  aux_size=current_line->size + getNumTabs(current_line,0)*(SPACES_TAB-1);
	  if(current_character == current_line->first_ch)
	    current_line->first_ch=current_line->next->first_ch;
	  else
	    current_character->prev->next=current_line->next->first_ch;
	  aux_character=current_line->next->first_ch;
	  current_line->next->first_ch->prev=current_character->prev;
	  current_line->size+=current_line->next->size-1;
	  if(current_line->next->next)
	    current_line->next->next->prev=current_line;
	  current_line->next=current_line->next->next;
	  current_line->num_tabs+=aux_line->num_tabs;
	  freeLine(&aux_line);
	  freeCharacter(&current_character);
	  current_character=aux_character;
	  print_all(num_lines_scrolled,actual_rows+1);
	  move(row,aux_size-1);
	}
      }
    }
    else if(ch == KEY_BACKSPACE)
    {
      if(current_character->prev == NULL && current_line->prev != NULL) //move up a line
      {
	aux_line=current_line->prev;
	aux_line->num_tabs+=current_line->num_tabs;
	aux_size=current_line->prev->size + getNumTabs(current_line->prev, 0)*(SPACES_TAB-1);
	aux_character=getChLine(current_line->prev,current_line->prev->size-1);//get the last character
	if(aux_character != current_line->prev->first_ch)
	{
	  aux_character->prev->next=current_character;
	}
	else
	{
	 current_line->prev->first_ch=current_character;
	}
	current_character->prev=aux_character->prev;
	current_line->prev->next=current_line->next;
	if(current_line->next!=NULL)
	  current_line->next->prev=current_line->prev;
	freeCharacter(&aux_character);
	aux_line->size+=current_line->size-1;
	freeLine(&current_line);
	current_line=aux_line;
	if(row == 0)
	{
	  num_lines_scrolled--;
	}
	print_all(num_lines_scrolled,actual_rows+1);
	move(row == 0 ? 0:row-1 ,aux_size-1);
      }
      else if(current_character->prev!=NULL)
      {
	aux_character=current_character->prev;
	if(current_character->prev == current_line->first_ch)
	{
	 current_character->prev=NULL;
	 current_line->first_ch=current_character;
	}
	else
	{
	  current_character->prev->prev->next=current_character;
	  current_character->prev=current_character->prev->prev;
	}
	
	current_line->size--;
	if(aux_character->c == '\t')
	{
	 move(row,col - SPACES_TAB);
	 for(cont=0;cont<SPACES_TAB-1;cont++)
	   delch();
	 current_line->num_tabs--;
	}
	else
	  move(row,col-1);
	delch();
	freeCharacter(&aux_character);
      }
    }
    else //normal characters
    {
      if(modified==0)
      {
	modified=1;
      }
      current_line->size++;
      if(current_character == eof)
      {
	if(current_line->first_ch==eof)
	{
	  init_character(&prev_character);
	  current_line->first_ch=prev_character;
	  prev_character->c=ch;
	  if(ch!='\n')
	  {
	    current_character=eof;
	    prev_character->next=current_character;
	    current_character->prev=prev_character;
	  }
	  else
	  {
	    prev_character->next=NULL;
	    current_line->size--;//eof erased
	  }
	}
	else
	{
	  prev_character=current_character->prev;
	  init_character(&current_character);
	  current_character->c=ch;
	  prev_character->next=current_character;
	  current_character->prev=prev_character;
	  if(ch!='\n')
	  {
	    current_character->next=eof;
	    eof->prev=current_character;
	    prev_character=current_character;
	    current_character=eof;
	  }
	  else
	  {
	    current_character->next=NULL;
	    current_line->size--;//eof erased
	  }
	} 
	if(ch == '\n')
	{
	  move(row+1,0);
	  num_lines++;
	  ncurrent_line++;
	  prev_line=current_line;
	  init_line(&current_line);
	  init_character(&eof);
	  eof->c='\0';
	  current_character=eof;
	  current_line->size=1;
	  current_line->first_ch=current_character;
	  prev_line->next=current_line;
	  current_line->prev=prev_line;
	  current_line->num_line=ncurrent_line;
	  if(row == (actual_rows))
	  {
	    /*wscrl(wnd,1);
	    move(row,0);*/
	    num_lines_scrolled++;
	    print_all(num_lines_scrolled,actual_rows+1);
	    move(row,0);
	  }
	}
	else
	{
	  if(ch == '\t')
	  {
	    current_line->num_tabs++;
	    print_tab();
	  }
	  else
	    printw("%c",ch); 
	}
      }
      else //insert a character
      {
	init_character(&aux_character);
	aux_character->c=ch;
	aux_character->prev=current_character->prev;
	aux_character->next=NULL;
	if(ch!='\n')
	{
	  aux_character->next=current_character;
	  if(current_character == current_line->first_ch)
	    current_line->first_ch=aux_character;
	  else
	    current_character->prev->next=aux_character;
	  current_character->prev=aux_character;
	  move(row,0);
	  print_ln(current_line,wnd);
	  if(ch == '\t')
	  {
	    move(row,col+SPACES_TAB);
	    current_line->num_tabs++;
	  }
	  else
	    move(row,col+1);
	}
	else //insert a line
	{
	  current_line->size--;//eof erased
	  aux=getPosCh(current_line,current_character);
	  init_line(&aux_line);
	  current_line->num_tabs-=getNumTabs(current_line,aux);;
	  aux_line->num_tabs=getNumTabs(current_line,aux);
	  num_lines++;
	  ncurrent_line++;
	  if(current_character == current_line->first_ch)
	    current_line->first_ch=aux_character;
	  else  
	    current_character->prev->next=aux_character;
	  current_character->prev=NULL;
	  aux_line->first_ch=current_character;
	  aux_line->next=current_line->next;
	  aux_line->prev=current_line;
	  aux_line->size=current_line->size-aux;
	  if(current_line->next)
	    current_line->next->prev=aux_line;
	  current_line->size=aux+1;
	  current_line->next=aux_line;
	  prev_line=current_line;
	  current_line=aux_line;
	  if(row == actual_rows)
	  {
	    num_lines_scrolled++;
	    print_all(num_lines_scrolled,actual_rows+1);
	    move(row,0);
	  }
	  else
	  {
	    print_all(num_lines_scrolled,actual_rows+1);
	    move(row+1,0);
	  }
	}
      }
    }
    refresh();
  }
  endwin();           
  exit(EXIT_SUCCESS);
}


int loadNewFile()
{
  echo();
  char fileName[MAX_FILE_LENGHT];int file,er;
  clear();
  printw("Enter the name of the file to load:");
  getstr(fileName);
  noecho();
  file=open(fileName,O_RDONLY,S_IRWXU);
  er=errno;
  if(file==-1)
  {
    printw("Error opening file:%s\nPress a key to continue",strerror(er));
    getch();
    return 0;
  }
  freeEditor();
  init_editor();
  loadFile(file);
  close(file);
  clear();
  return 1;
}

int saveToNewFile()
{
  echo();
  char fileName[MAX_FILE_LENGHT];int file,er;
  clear();
  printw("Enter the name of the file:");
  getstr(fileName);
  noecho();
  file=open(fileName, O_CREAT | O_WRONLY,S_IRWXU);
  er=errno;
  if(file==-1)
  {
    printw("Error opening file:%s\nPress a key to continue",strerror(er));
    getch();
    return 0;
  }
  saveToFile(file);
  close(file);
  clear();
  return 1;
}


void do_resize()
{
  const char *tty = ttyname(0);
  int fd, result = 0;
  struct winsize win;

  if (tty == NULL)
      return;
  fd = open(tty, O_RDWR);
  if (fd == -1)
      return;
  result = ioctl(fd,TIOCGWINSZ, &win);
  close(fd);
  if (result == -1)
      return;
  COLS = win.ws_col;
  LINES = win.ws_row;
  resize_term(LINES,COLS);
  print_all(0,actual_rows);
  wrefresh(wnd);
  /*clear();
  refresh();
  print_all(first_line,0,LINES);
  sleep(3);
  clear();
  refresh();
  print_all(first_line,0,LINES);*/
}