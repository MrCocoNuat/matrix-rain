#include <ncurses.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

struct letter_t{ //one letter in the digital rain
  int y; //duh, start at 0
  int x; //duh
  int age; //age 0 is white, 1-18 is green shades, then disappears
  char letter; //what char to show
  struct letter_t *next; //linked lists woo
} *head;
//head is never never never accessed other than directly to its next
//this way head will never be null even with 0 letters on screen 
struct letter_t *tail;

int main(int argc, char *argv[]){
  //seed PRNG in a totally predictable way
  srand((unsigned) time(NULL));
  
  //start up ncurses
  initscr(); 
  
  //find bounds of the terminal
  int maxY, maxX; 
  getmaxyx(stdscr,maxY,maxX);
  
  //test for color capability and start it
  if (!has_colors()){
    endwin();
    printf("This terminal does not support color.\n");
    exit(1);
  }
  if (!can_change_color()){
    endwin();
    printf("This terminal does not support changing colors.\n");
    exit(2);
  }
  start_color();

  //set the entire terminal to the defaultblack background in case
  //the terminal has a different background color by default
  mvaddch(' ',maxY,maxX);
  refresh();

  
  //set up color pairs for defaultblack, defaultwhite and 6 shades of green
  init_color(COLOR_RED,0,1000,0);
  init_color(COLOR_GREEN,0,833,0);
  init_color(COLOR_YELLOW,0,666,0);
  init_color(COLOR_BLUE,0,500,0);
  init_color(COLOR_MAGENTA,0,333,0);
  init_color(COLOR_CYAN,0,166,0);
  //create a color-time curve that is close to fluorescence instead of linear?
  init_pair(0,COLOR_WHITE,COLOR_BLACK);
  init_pair(1,COLOR_RED,COLOR_BLACK);
  init_pair(2,COLOR_GREEN,COLOR_BLACK);
  init_pair(3,COLOR_YELLOW,COLOR_BLACK);
  init_pair(4,COLOR_BLUE,COLOR_BLACK);
  init_pair(5,COLOR_MAGENTA,COLOR_BLACK);
  init_pair(6,COLOR_CYAN,COLOR_BLACK);
  init_pair(7,COLOR_BLACK,COLOR_BLACK);

  //set up head and tail of linked list
  head = (struct letter_t *) malloc(sizeof(struct letter_t));
  head -> y = head -> x = head -> age = head -> letter = 0;
  head -> next = NULL;
  tail = head;

  //debugging variables
  int count = 0; //how many letters in the list
  
  //start up
  while (1){
    napms(100); //10 cycles per second
    
    //to facilitate deleting, keep track of current and previous
    struct letter_t *prev = head, *current = head;

    //process every letter
    while (current -> next != NULL){ 
      current = current -> next; //advance
      
      //if current is new, spawn a random letter below
      if (current -> age == 0){
	struct letter_t *new = malloc(sizeof(struct letter_t));
	count++;
	new -> y = current -> y + 1; //move it down 1
	new -> x = current -> x; //same column
	new -> age = -1;
	//start out at age = -1, which gets set to 0 in time for the next cycle
	//otherwise this would cause infinite creation of new letters
	new -> letter = 32 + rand() % 95; //a random printable char
	new -> next = NULL;
	
	tail -> next = new;
	tail = new;
      }
      
      //if current is too old or below screen, delete it.
      //this way of deleting works as long as you only ever
      //traverse the list in the forward direction
      if (current -> age == 19 || current -> y == maxY){
	prev -> next = current -> next;
	if (tail == current)  tail = prev;
	free(current);
	count--;
	current = prev;
      }
      else{
	//putting else here fixed a memory leak, stopping the prev letter from
	//immediately getting processed/aged twice in one cycle!

	if (rand() % 20 == 0) current -> letter = 32 + rand() % 95;
	//5% chance to rerandomize the letter, changing it
	attrset(COLOR_PAIR((current -> age + 3)/3)); //set color
	mvaddch(current -> y, current -> x, current -> letter); //draw it
	current -> age ++; //age it
      }
      
      prev = current; //advance the previous to the current
    }
   
    //create a new letter in a random spot
    struct letter_t *new = malloc(sizeof(struct letter_t));
    count++;
    new -> y = 0; //move it down 1
    new -> x = rand() % maxX; //randomize x coordinate
    new -> age = 0;
    new -> letter = 32 + rand() % 95; //a random printable char
    new -> next = NULL;
    
    tail -> next = new;
    tail = new;

    //keep track of how long the linked list is, there is memory leak?
    if (0) mvprintw(0,0,"%d",count);

    //refresh to push updates to screen
    refresh();

    //while (getchar() != 'a'); //step through cycles
  }

  endwin();
}
