#include	<stdio.h>
//#include	<curses.h>
#include	<ncurses.h>
#include	<pthread.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include    <time.h>

#define	MAXMSG	10		/* limit to number of strings	*/
#define	TUNIT   20000		/* timeunits in microseconds */

/* My constants */
#define NUM_OF_SHIPS    3

struct	propset {
		char	*str;	/* the message */
		int	row;	/* the row     */
		int	delay;  /* delay in time units */
		int	dir;	/* +1 or -1	*/
	};

struct cannon_info {
        char *str;
        int row;
        int col;
};

struct rocket {
        char *str;
        int row;
        int col;
        int delay;
};

pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;

sig_atomic_t rockets = 0;
int current_rocket = 0;

/* Function prototypes */
int setup(int, char* [], struct propset []);
void *animate(void*);
void *fire_rocket(void*);
void draw_cannon(struct cannon_info *cannon);
void move_cannon(int, struct cannon_info *cannon);
