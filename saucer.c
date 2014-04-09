/*
 * saucer.c: ITS A SPACESHIP GAME!!!
 *
 * Protect the home work, Terra from the evil alien invaders
 *
 * Details on controls to follow
 */

/* TODO remove this block once i've taken everything i need from it */
/*
 * tanimate.c: animate several strings using threads, curses, usleep()
 *
 *	bigidea one thread for each animated string
 *		one thread for keyboard control
 *		shared variables for communication
 *	compile	cc tanimate.c -lcurses -lpthread -o tanimate
 *	to do   needs locks for shared variables
 *	        nice to put screen handling in its own thread
 */

#include    "saucer.h"


int main(int ac, char *av[])
{
	int	            c;		        /* user input		*/
	//pthread_t       thrds[MAX_ROCKET+1];	/* the threads		*/
	pthread_t       rocket_threads[MAX_ROCKET];	/* the threads		*/
    pthread_t       spawn_thread;
    pthread_t       score_thread;
	//struct propset  props[MAXMSG];	/* properties of string	*/
    struct cannon_info *cannon;   
    struct rocket   rockets[10];
	void	        *animate();	    /* the function		*/
    void            *spawn_ships(); /* the function */
    void            *keep_score(); /* the function */
	int	            num_msg ;	    /* number of strings	*/
	int	            i;

    //char* ship = "<--->";
    //char* ships[NUM_OF_SHIPS];
    //ships[0] = ship;
    //ships[1] = ship;
    //ships[2] = ship;
    //num_msg = setup(ac-1,av+1,props);
	//num_msg = setup_ncurses(NUM_OF_SHIPS, ships, props);
	setup_ncurses();

    /* initialize list of ships */
    struct ship *ptr = NULL;
    add_ship(0, 0);

    //if(pthread_create(&thrds[0], NULL, animate, head)){
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

	/* create all the threads */
	//for(i=0 ; i<NUM_OF_SHIPS; i++)
//		if ( pthread_create(&thrds[i], NULL, animate, &props[i])){
//			fprintf(stderr,"error creating thread");
//			endwin();
//			exit(0);
//		}

    /* draw the cannon at the middle of the screen */
    char* cannon_string = "|";
    cannon = malloc(sizeof(struct cannon_info));
    cannon->str = cannon_string;
    draw_cannon(cannon);

	/* process user input */
	while(1) {
		c = getch();
		if ( c == 'Q' ) break;
		if ( c == ' ' ){ 
            /* Need to select a rocket and fire it */
            char* rocket_string = "^";
            rockets[0].str = rocket_string;
            rockets[0].row = cannon->row-1;
            rockets[0].col = cannon->col;
            rockets[0].delay = 3;
            if ( pthread_create(&rocket_threads[9], NULL, fire_rocket, &rockets[0])){
			    fprintf(stderr,"error creating thread");
			    endwin();
			    exit(0);
		    }   
        }
        if ( c == 67 ) /* move the cannon right */
            move_cannon(1, cannon);
        if ( c == 68 ) /* move the cannon left */
            move_cannon(-1, cannon);

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
	mvprintw(LINES-1,0,"'Q' to quit, 'space' to shoot");
	mvprintw(LINES-1,COLS/4,"Rocket Remaining: %d", total_rockets);
	mvprintw(LINES-1,COLS/2,"Escaped ships: %d", escaped_ships);
	mvprintw(LINES-1,(COLS/4)*3,"Score: %d", score);
}

void *spawn_ships(void *arg){

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

void *keep_score(void* arg){
	while( 1 )
	{
		usleep(10*TUNIT);

		pthread_mutex_lock(&mx);	/* only one thread	*/
        mvprintw(LINES-1,COLS/4,"Rocket Remaining: %d ", total_rockets);
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
	//struct propset *info = arg;		/* point to info block	*/
    struct ship *ship_info = arg;
	//int	len = strlen(info->str)+2;	/* +2 for padding	*/
	//int	col = rand()%(COLS-len-3);	/* space for padding	*/

	while( 1 )
	{
		usleep(ship_info->delay*TUNIT);

		pthread_mutex_lock(&mx);	/* only one thread	*/
		   move( ship_info->row, ship_info->col );	/* can call curses	*/
		   addch(' ');			/* at a the same time	*/
		   addstr( ship_info->str );		/* Since I doubt it is	*/
		   addch(' ');			/* reentrant		*/
		   move(LINES-1,COLS-1);	/* park cursor		*/
		   refresh();			/* and show it		*/
		pthread_mutex_unlock(&mx);	/* done with curses	*/

		/* move item to next column and check for bouncing	*/

		ship_info->col += 1;
		if (ship_info->col+strlen(ship_info->str) >= COLS){
            /* means ship has left the screen */
            /* TODO increment the escaped ships counter */
            escaped_ships += 1;
            pthread_exit(NULL);
        }
	}
}

/* the code that fires a rocket in a thread */
void *fire_rocket(void *arg)
{
    struct rocket *rocket_info = arg;		/* point to info block	*/

    total_rockets -= 1;
	while( 1 )
	{
		usleep(rocket_info->delay*TUNIT);

		pthread_mutex_lock(&mx);	/* only one thread	*/
		   move( rocket_info->row, rocket_info->col );	/* can call curses	*/
		   addch(' ');			/* at a the same time	*/
		   move( rocket_info->row-1, rocket_info->col );	/* can call curses	*/
		   addstr( rocket_info->str );		/* Since I doubt it is	*/
		   addch(' ');			/* reentrant		*/
		   move(LINES-1,COLS-1);	/* park cursor		*/
		   refresh();			/* and show it		*/
		pthread_mutex_unlock(&mx);	/* done with curses	*/

		/* */
		rocket_info->row = rocket_info->row -= 1;
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
        if(cannon->col > 0 && cannon->col <= COLS-1){
            mvprintw(cannon->row, cannon->col, " ");
            cannon->col += direction;
            mvprintw(cannon->row, cannon->col, "|");
		    move(LINES-1,COLS-1);	/* park cursor		*/
	        refresh();			/* and show it		*/
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
    //ptr->delay = 5;
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
    //ptr->delay = 5;
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
        if(ptr->row == row){
            if(col <= ptr->col <= col+3){
                found = 1;
                break;
            }
        }
        temp = ptr;
        ptr = ptr->next;
    }

    if(found == 1){
        /* TODO add the previous pointer before returning */
        return ptr;
    }
    else{
        return NULL;
    }
}

/* TODO write delete */
