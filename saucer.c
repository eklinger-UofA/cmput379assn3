/*
 * saucer.c: ITS A SPACESHIP GAME!!!
 *
 * Protect the home world, Terra from the evil alien invaders
 *
 * Details on controls to follow
 */

#include    "saucer.h"


int main(int ac, char *av[])
{
		int	            c;		        /* user input		*/
		pthread_t       rocket_threads[MAX_ROCKET];	/* the threads		*/
        pthread_t       spawn_thread;
        pthread_t       score_thread;
        struct cannon_info *cannon;   
        struct rocket   rockets[10];
		void	        *animate();	    /* the function		*/
        void            *spawn_ships(); /* the function */
        void            *keep_score(); /* the function */
		int	            num_msg ;	    /* number of strings	*/
		int	            i;

	    setup_ncurses();

        /* Create a thread to spawn and manage new ships */
        if(pthread_create(&spawn_thread, NULL, spawn_ships, head)){
 	            fprintf(stderr,"error creating thread");
 	   		    endwin();
 	   		    exit(0);
        }

        /* Create a thread to keep track and update the score */
        if(pthread_create(&score_thread, NULL, keep_score, head)){
 	            fprintf(stderr,"error creating thread");
 	   		    endwin();
 	   		    exit(0);
 	   }

        /* draw the cannon at the middle of the screen */
        char* cannon_string = "|";
        cannon = malloc(sizeof(struct cannon_info));
        cannon->str = cannon_string;
        draw_cannon(cannon);

		/* process user input */
		while(1) {
		        c = getch();
				if ( c == 'Q' ) break;
				if ( c == ' ' && game_over == 0 ){ 
                        /* Need to select a rocket and fire it */
                        char* rocket_string = "^";
                        int rocket_id = current_rocket_thread;
                        rockets[rocket_id].str = rocket_string;
                        rockets[rocket_id].row = cannon->row-1;
                        rockets[rocket_id].col = cannon->col;
                        rockets[rocket_id].delay = 3;
                        if ( pthread_create(&rocket_threads[rocket_id], NULL, fire_rocket, &rockets[rocket_id])){
				 	            fprintf(stderr,"error creating thread");
				 	            endwin();
				 	            exit(0);
				        }   
                        current_rocket_thread = (current_rocket_thread + 1)%MAX_ROCKET;
                }
                if ( c == 67 && game_over == 0 ) /* move the cannon right */
                        move_cannon(1, cannon);
                if ( c == 68 && game_over == 0 ) /* move the cannon left */
                        move_cannon(-1, cannon);
                /* TODO rm this since its just debug output */
                if ( c == 'i'){
 		   		        pthread_mutex_lock(&mx);	/* only one thread	*/
                        mvprintw(LINES-5, 0,"Row: %d ", current->row);
 	                    mvprintw(LINES-4, 0,"Col: %d ", current->col);
 		   		        move(LINES-1,COLS-1);	/* park cursor		*/
 		   		        refresh();			        /* and show it		*/
 		   		        pthread_mutex_unlock(&mx);	/* done with curses	*/
                }
                if(c == 'h'){
                        head->alive = 0;
                }

		        pthread_mutex_lock(&mx);	    /* only one thread	*/
	            mvprintw(LINES-2,0,"%d",c);  /* Print the input character */
		        refresh();			        /* and show it		*/
		        pthread_mutex_unlock(&mx);	    /* done with curses	*/
	    }

		/* cancel all the threads */
        /* TODO need a new method to cancel threads */
		pthread_mutex_lock(&mx);
		//for (i=0; i<num_msg; i++ )
			//pthread_cancel(thrds[i]);
		endwin();
    
        /* before quitting, and after threads exit free all remaining ship nodes */
		//pthread_t       rocket_threads[MAX_ROCKET];	/* the threads		*/
        //pthread_t       spawn_thread;
        //pthread_t       score_thread;
       
		return 0;
}

void setup_ncurses()
{
        /* set up ncurses */
		initscr();
		crmode();
		noecho();
		clear();
        /* print game information */
		mvprintw(LINES-1,0,"'Q' to quit");
		mvprintw(LINES-1,COLS/4,"Rockets: %d", total_rockets);
		mvprintw(LINES-1,COLS/2,"Escaped ships: %d", escaped_ships);
		mvprintw(LINES-1,(COLS/4)*3,"Score: %d", score);
}

void *spawn_ships(void *arg)
{
		pthread_t thrds[NUM_OF_SHIPS];	/* the threads */
        int current_ship_thread = 0;
        int row = 0;
    
        while (1){
                /* random sleep between spawning ships */
		 	    usleep(100*((rand()%3)+1)*TUNIT);
             
                /* build a new ship */
                row = rand()%NUM_OF_ROWS; 
                add_ship(row, 0);
             
                /* draw that ship in another thread */
                if(pthread_create(&thrds[current_ship_thread], NULL, animate, current)){
		                fprintf(stderr,"error creating thread");
		 	   	        endwin();
		 	   	        exit(0);
		        }
                current_ship_thread = (current_ship_thread+1)%NUM_OF_SHIPS;
        }
}

void *keep_score(void* arg)
{
        char *str = "THE INVASION CONTINUES!!!";
 
 	    while(1){
 	   	        usleep(10*TUNIT);
             
                if(game_over > 0){
 	                    clear();
 	                    mvprintw(LINES-8,(COLS/2)-(strlen(str)/2),str);
 	                    mvprintw(LINES-4,0,"GAME OVER!!!");
 	                    mvprintw(LINES-3,0,"You had this many rockets remaining: %d ", total_rockets);
 	                    mvprintw(LINES-2,0,"You let this many ships escape!: %d", escaped_ships);
 	                    mvprintw(LINES-1,0,"'Q' to quit");
                        pthread_exit(NULL);
                }
 
 	   	        pthread_mutex_lock(&mx);	/* only one thread	*/
                mvprintw(LINES-1,COLS/4,"Rockets: %d ", total_rockets);
 	            mvprintw(LINES-1,COLS/2,"Escaped ships: %d", escaped_ships);
 	            mvprintw(LINES-1,(COLS/4)*3,"Score: %d", score);
 	   	        move(LINES-1,COLS-1);	/* park cursor		*/
 	   	        refresh();			        /* and show it		*/
 	   	        pthread_mutex_unlock(&mx);	/* done with curses	*/
        }
}

/* the code that runs in each thread */
void *animate(void *arg)
{
        struct ship *ship_info = arg;
 
 	    while(1){
 	   	        usleep(ship_info->delay*TUNIT);
                if (ship_info->alive){
 	   		            pthread_mutex_lock(&mx);	/* only one thread	*/
 	   			        move( ship_info->row, ship_info->col );	/* can call curses	*/
 	   			        addch(' ');			/* at a the same time	*/
 	   			        addstr( ship_info->str );		/* Since I doubt it is	*/
 	   			        addch(' ');			/* reentrant		*/
 	   			        move(LINES-1,COLS-1);	/* park cursor		*/
 	   			        refresh();			/* and show it		*/
 	   		            pthread_mutex_unlock(&mx);	/* done with curses	*/
                }
                else{
                        /* increment rockets */
                        if(total_rockets <= MAX_ROCKET){
                                total_rockets += 1;
                        }
                        score += 100;

                        /* clear ship from the screen */
                        mvprintw(ship_info->row, ship_info->col, "     ");
                        /* exit from the thread */
                        pthread_exit(NULL);
                }
 
 	   	        /* move item to next column */
 	   	        ship_info->col += 1;
 	   	        if (ship_info->col >= COLS){
                        /* means ship has left the screen */
                        /* TODO increment the escaped ships counter */
                        escaped_ships += 1;
                        if(escaped_ships >= ESCAPES_FOR_LOSS){
                                game_over = 1;
                        }
                        pthread_exit(NULL);
                }
        }
}

/* the code that fires a rocket in a thread */
void *fire_rocket(void *arg)
{
        struct rocket *rocket_info = arg;		/* point to info block	*/
        struct ship *ship_hit;
 
        while(1)
 	    {
  	   	        usleep(rocket_info->delay*TUNIT);
  
  	   		   	pthread_mutex_lock(&mx);	/* only one thread	*/
  	   		   	move( rocket_info->row, rocket_info->col );	/* can call curses	*/
  	   		   	addch(' ');			/* at a the same time	*/
  	   		   	move( rocket_info->row-1, rocket_info->col );	/* can call curses	*/
  	   		   	addstr( rocket_info->str );		/* Since I doubt it is	*/
                //addch(' ');			/* reentrant		*/
  	   		   	move(LINES-1,COLS-1);	/* park cursor		*/
  	   		   	refresh();			/* and show it		*/
  	   		   	pthread_mutex_unlock(&mx);	/* done with curses	*/

                /* check if any ship is hit */
                //struct ship* find_ship(int row, int col){
                ship_hit = find_ship(rocket_info->row, rocket_info->col);
                if(ship_hit != NULL){
                        mvprintw(LINES-9, 0, "rocket row: %d", rocket_info->row);
                        mvprintw(LINES-8, 0, "rocket col: %d", rocket_info->col);
                        mvprintw(LINES-7, 0, "SHIP HIT row: %d", ship_hit->row);
                        mvprintw(LINES-6, 0, "SHIP HIT col: %d", ship_hit->col);
                        mvprintw(LINES-5, 0, "SHIP HIT");
                        ship_hit->alive=0;
  	   		   	        pthread_mutex_lock(&mx);	/* only one thread	*/
                        mvprintw(rocket_info->row-1, rocket_info->col, " ");
  	   		   	        refresh();			/* and show it		*/
  	   		   	        pthread_mutex_unlock(&mx);	/* done with curses	*/
                        pthread_exit(NULL);
                }
                else{
                        mvprintw(LINES-5, 0, "        ");
                }
  
  	   		   	/* make the rocket move up one row */
  	   		   	rocket_info->row = rocket_info->row -= 1;
                if (rocket_info->row < 0){
  	   		   	        //move( rocket_info->row+1, rocket_info->col );	/* can call curses	*/
                        //addch(' ');			/* at a the same time	*/
  	   		   	        pthread_mutex_lock(&mx);	/* only one thread	*/
                        mvprintw(rocket_info->row, rocket_info->col, " ");
                        addch(' ');			/* at a the same time	*/
  	   		   	        move(LINES-1,COLS-1);	/* park cursor		*/
  	   		   	        refresh();			/* and show it		*/
  	   		   	        pthread_mutex_unlock(&mx);	/* done with curses	*/
  
                        total_rockets -= 1;
                        if(total_rockets <= 0){
                                game_over = 1;
                        }
                        pthread_exit(NULL);
                }
        }
}

void draw_cannon(struct cannon_info *cannon){
        int i;
 	    pthread_mutex_lock(&mx);	/* only one thread	*/
 	    //mvprintw(LINES-2,0,"|"); /* Print the input character */
        for(i=0;i<10;i++){
 	            mvprintw(LINES-2,COLS-i,"%d",i); /* Print the input character */
        }
 	   	move(LINES-1,COLS-1);	/* park cursor		*/
 	    refresh();			/* and show it		*/
        cannon->row = LINES-3;
        cannon->col = COLS/2;
        mvprintw(cannon->row, cannon->col, cannon->str);
 	   	move(LINES-1,COLS-1);	/* park cursor		*/
 	    refresh();			/* and show it		*/
 	    pthread_mutex_unlock(&mx);	/* done with curses	*/
}

void move_cannon(int direction, struct cannon_info *cannon){
        pthread_mutex_lock(&mx);	/* only one thread	*/
        cannon->col += direction;
        if(cannon->col >= 0 && cannon->col <= COLS-1){
                mvprintw(cannon->row, cannon->col-direction, " ");
                mvprintw(cannon->row, cannon->col, "|");
		        move(LINES-1,COLS-1);	/* park cursor		*/
		        refresh();			/* and show it		*/
        }
        else{
                cannon->col -= direction;
        }
		pthread_mutex_unlock(&mx);	/* done with curses	*/
}

struct ship* create_list(int row, int col){
        struct ship *ptr = (struct ship *)malloc(sizeof(struct ship));
        if(ptr == NULL){
                return NULL;
        }
 
        char* ship_string = "<--->";
        ptr->str = ship_string;
        ptr->row = row;
        ptr->col = col;
        ptr->alive = 1;
        ptr->delay = 1+(rand()%15);
        ptr->next = NULL;
 
        head = current = ptr;
        return ptr;
}

struct ship* add_ship(int row, int col){
        if(head == NULL){
                return create_list(row, col);
        }
 
        struct ship *ptr = (struct ship *)malloc(sizeof(struct ship));
        if(ptr == NULL){
                return NULL;
        }
 
        char* ship_string = "<--->";
        ptr->str = ship_string;
        ptr->row = row;
        ptr->col = col;
        ptr->alive = 1;
        ptr->delay = 1+(rand()%15);
        ptr->next = NULL;
 
        /* we can assume always adding to the end of the LL */
        current->next = ptr;
        current = ptr;
        return ptr;
}

struct ship* find_ship(int row, int col){
    struct ship *ptr = head;
    struct ship *temp = NULL;
    int found = 0;

    while(ptr != NULL){
        if(ptr->row == row && ptr->alive == 1){
                //if(ptr->col <= col <= ptr->col+strlen(ptr->str)){
                if(ptr->col <= col && col <= ptr->col+strlen(ptr->str)){
                        found = 1;
                        break;
                }
        }
        temp = ptr;
        ptr = ptr->next;
    }

    if(found == 1){
        return ptr;
    }
    else{
        return NULL;
    }
}

/* TODO delete */
/*
int delete_ship(struct ship *ship_to_delete){
    struct ship *del = head;
    struct ship *prev = NULL;

    while(del != NULL){
        if(del == ship_to_delete){
            break;
        }
        prev = del;
        del = del->next;
    }
        
    if(del == NULL){
        return -1;
    }
    else{
        if(prev != NULL){
            prev->next = del->next;
        }
        if(del == current){
            current=prev;
        }
        else if(del == head){
            head = del->next;
        }
    }

    free(del);
    del=NULL;

    return 0;
}
*/
