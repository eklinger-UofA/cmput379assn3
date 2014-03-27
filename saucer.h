#include	<stdio.h>
//#include	<curses.h>
#include	<ncurses.h>
#include	<pthread.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>

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

pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;

/* Function prototypes */
int setup(int, char* [], struct propset []);
void *animate(void*);
