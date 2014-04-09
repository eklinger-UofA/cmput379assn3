#include	<stdio.h>
#include	<ncurses.h>
#include	<pthread.h>
#include	<stdlib.h>
#include	<unistd.h>
#include	<string.h>
#include    <time.h>

/* Constants */
#define	MAX_ROCKET	        10		/* limit to number of rockets	*/
#define	TUNIT               20000	/* timeunits in microseconds */
#define NUM_OF_SHIPS        50      /* the max number of ships supported */
#define NUM_OF_ROWS         5       /* the number of rows for ships */
#define ESCAPES_FOR_LOSS    5       /* the number of escapes before loss */

/* struct to rep a ship in the game */
struct ship {
        char *str;
        int row;
        int col;
        int delay;
        int alive;
        struct ship *next;
};

/* struct to rep the cannon info */
struct cannon_info {
        char *str;
        int row;
        int col;
};

/* struct to rep a rocket in the game*/
struct rocket {
        char *str;
        int row;
        int col;
        int delay;
};

/* global mutex lock */
pthread_mutex_t mx = PTHREAD_MUTEX_INITIALIZER;

/* Pointers for managing linked-list of ships */
struct ship *head = NULL;
struct ship *current = NULL;

/* Globals for game data and counters */
int total_rockets = 5;
int escaped_ships = 0;
int score = 0;
int game_over = 0;
int current_rocket_thread = 0;

/* Ship linked-list management functions */
struct ship* create_list(int, int);
struct ship* add_ship(int, int);
struct ship* find_ship(int, int);

/* Remaining function prototypes */
void setup_ncurses();
void *spawn_ships(void*);
void *keep_score(void*);
void *animate(void*);
void *fire_rocket(void*);
void draw_cannon(struct cannon_info *cannon);
void move_cannon(int, struct cannon_info *cannon);
