#ifndef EDIT_FUNCTIONS_H
#define EDIT_FUNCTIONS_H
#include <ncurses.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include "lua.h"
#include "lauxlib.h"

#define DEBUG 1

#define SIGNAL_EXECUTE SIGUSR1 //the signal to execute the process
#define SPACES_TAB 3 //number of blank spaces associated with a TAB
#define default_file_name ".execut.lua" //default name of the to be executed
#define MAX_FILE_LENGHT 30 //the maximum number of characters of the file name
#define SAVE 0x08 //CTRL+H Save the file
#define CLEAR_EDITOR 0X17 //CTRL+W Clear the editor
#define EXECUTE 0x18 //CTRL+X Execute the file and exit the editor
#define LOAD_FILE 0x0C //CTRL+L load a file
#define EXIT_EDITOR 0x06 //CTRL+F exit editor

#define STATUS_EXECUTE 2 //if the editor process write this to the shared memory, the script will be executed

struct line
{
 int num_line;//number of line of this line
 int size;//the number of characters of this line
 int num_tabs;//number of tabs of this line
 struct line *prev,*next; //previous and next line
 struct character *first_ch; // the first character in the line
};

struct character
{
 char c;//the character itself 
 struct character *prev,*next;//previous and next character
};


typedef struct character character;
typedef struct line line;


//the actual rows and cols, taking into account the bottom bar
extern int actual_rows;
extern int actual_cols;
extern int errno;

extern line *first_line; 
extern line *current_line;
extern character *prev_character,*current_character;
extern character *eof; 

extern int modified;
extern int ncurrent_line;
extern int num_lines; 
extern int num_lines_scrolled;

extern void freeEditor();

extern void init_editor();

extern int gsl_editor();

extern int launch_editor();

extern int getScreenPos(line *ln, int rpos);

extern void empty_line(int nline,int size_line);

extern void print_ln(const line *ln, WINDOW *wn);

extern void init_line(line **l);

extern void init_character(character **ch);

extern void print_options();

extern void freeCharacter(character **c);

extern void freeLine(line **l);

extern character* getChLine(line *ln,int pos); //get the character of line ln, in the position pos

extern void print_all(int ln_scr, int num_rows); 

extern int saveToFile(int file);

extern void loadFile(int file);

extern void print_tab();

extern int getPosCh(line *ln, character *ch);

extern int getNumTabs(line *ln, int pos);

extern int getActualPos(line *ln,int rpos);

#endif

