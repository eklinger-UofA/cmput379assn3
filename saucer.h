#include	<stdio.h>
//#include	<curses.h>
#include	<ncurses.h>
#include	<pthread.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include    <time.h>

#define	MAXMSG	10		/* limit to number of strings	*/
#define	MAX_ROCKET	10		/* limit to number of rockets	*/
#define	TUNIT   20000		/* timeunits in microseconds */

/* My constants */
#define NUM_OF_SHIPS    50
#define NUM_OF_ROWS     5

struct	propset {
		char	*str;	/* the message */
		int	row;	/* the row     */
		int	delay;  /* delay in time units */
		int	dir;	/* +1 or -1	*/
	};

struct ship {
        char *str;
        int row;
        int col;
        int delay;
        struct ship *next;
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

struct ship *head = NULL;
struct ship *current = NULL;

sig_atomic_t total_rockets = 5;
sig_atomic_t escaped_ships = 0;
sig_atomic_t score = 0;
int current_rocket_thread = 0;

/* ship LL management functions */
struct ship* create_list(int, int);
struct ship* add_ship(int, int);
struct ship* find_ship(int, int);
int delete_ship(int, int);

/* Function prototypes */
//void setup_ncurses(int, char* [], struct propset []);
void setup_ncurses();
void *spawn_ships(void*);
void *animate(void*);
void *fire_rocket(void*);
void draw_cannon(struct cannon_info *cannon);
void move_cannon(int, struct cannon_info *cannon);
