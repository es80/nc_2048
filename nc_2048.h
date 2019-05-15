/**
 * nc_2048.h
 *
 * Header file for nc_2048.
 */

#include <stdbool.h>

#ifndef NC2048_H
#define NC2048_H

// Minimum required window size.
#define MIN_WINDOW_WIDTH 80
#define MIN_WINDOW_HEIGHT 21

// Maximum height and width for the display of help and the logo.
#define MAX_WIDTH_LOGO_HELP 35
#define MAX_HEIGHT_LOGO_HELP 15

// If we cannot change colours then use six default colours for tiles.
#define TILE_A  COLOR_BLUE
#define TILE_B  COLOR_GREEN
#define TILE_C  COLOR_RED
#define TILE_D  COLOR_CYAN
#define TILE_E  COLOR_MAGENTA
#define TILE_F  COLOR_YELLOW

// Otherwise, define some custom colours.
#define BLACK  COLOR_BLACK,    0,   0,   0
#define RED    COLOR_RED,    750,   0,   0
#define WHITE  COLOR_WHITE,  999, 999, 999

#define ORANGE_1  TILE_ORANGE_1,  999,  875, 675
#define ORANGE_2  TILE_ORANGE_2,  998,  753, 353
#define ORANGE_3  TILE_ORANGE_3,  961,  616,  75
#define ORANGE_4  TILE_ORANGE_4,  675,  416,   0
#define ORANGE_5  TILE_ORANGE_5,  353,  220,   0
#define GREEN_1   TILE_GREEN_1,   337,  784, 435
#define GREEN_2   TILE_GREEN_2,    86,  690, 220
#define GREEN_3   TILE_GREEN_3,     0,  451,  98
#define PURPLE_1  TILE_PURPLE_1,  573,  325, 706
#define PURPLE_2  TILE_PURPLE_2,  427,  102, 608
#define PURPLE_3  TILE_PURPLE_3,  267,   20, 400
#define RED_1     TILE_RED_1,     992,  427, 443
#define RED_2     TILE_RED_2,     906,  110, 129
#define RED_3     TILE_RED_3,     592,    0,  16
#define BLUE_1    TILE_BLUE_1,    353,  443, 710
#define BLUE_2    TILE_BLUE_2,    137,  259, 616
#define BLUE_3    TILE_BLUE_3,     43,  133, 404

// Enum for custom colours.
enum { TILE_ORANGE_1 = 17, TILE_ORANGE_2, TILE_ORANGE_3, TILE_ORANGE_4,
       TILE_ORANGE_5, TILE_GREEN_1, TILE_GREEN_2, TILE_GREEN_3, TILE_PURPLE_1,
       TILE_PURPLE_2, TILE_PURPLE_3, TILE_RED_1, TILE_RED_2, TILE_RED_3,
       TILE_BLUE_1, TILE_BLUE_2, TILE_BLUE_3 };

// Enum for colour pairs. (Swap the order of the lines between PAIR_1 to
// PAIR_17 to change which number tile gets which colour.
enum { START = 0,
       PAIR_1,
       PAIR_2,
       PAIR_3,
       PAIR_4,
       PAIR_5,
       PAIR_6,
       PAIR_7,
       PAIR_8,
       PAIR_9,
       PAIR_10,
       PAIR_11,
       PAIR_12,
       PAIR_13,
       PAIR_14,
       PAIR_15,
       PAIR_16,
       PAIR_17,
       PAIR_INFO, PAIR_BORDER };

// Dimension of board.
#define DIM 4

#define SAVEFILE "nc2048_save.dat"

// To allow a user to undo moves we use a circular stack in which we store the
// tiles and scores for the most recent non-trivial (i.e. a tile actually
// moved) moves. UNDO_CAPACITY is the maximum number of moves that the user can
// undo plus one.
#define UNDO_CAPACITY 4

// A stack structure to allow undoing moves.
struct stack
{
    // An array of two-dimensional arrays for the board's tile numbers.
    int tiles[UNDO_CAPACITY][DIM][DIM];

    // An array for the scores.
    int score[UNDO_CAPACITY];

    // The top of the stack.
    int top;

    // The current size of the stack (at most equal to UNDO_CAPACITY).
    int size;
};

// We use a single global variable g as a wrapper to contain all game data.
struct game
{
    // Track the x,y co-ordinates for the top left of the board to aid
    // functions that draw on the window.
    int x, y;

    // Use a two-dimensional array for the board's current tile numbers.
    int tiles[DIM][DIM];

    // The current score.
    int score;

    // A stack for undoing moves.
    struct stack undo;
}
g;


////////////////////////////////////////////////////////////////////////////////
// Functions used for drawing on the window, defined in display.c.
////////////////////////////////////////////////////////////////////////////////

/*
 * Draws borders at the top and bottom of window.
 */
void draw_borders(void);

/*
 * Draws the outline of the game board. Must be called before any other drawing
 * functions since it determines the locations of the top-left corner of the
 * board.
 */
void draw_grid(void);

/*
 * Draws the game's tiles. Only call after draw_grid has been called at least
 * once.
 */
void draw_tiles(void);

/*
 * Draws a game logo to the right of the game board. Only call after draw_grid
 * has been called at least once.
 */
void draw_logo(void);

/*
 * Displays help text to the right of the game board. Only call after draw_grid
 * has been called at least once.
 */
void display_help(void);

/*
 * Displays a message below and to the right of the game board. Only call after
 * draw_grid has been called at least once.
 */
void display_message(char *s);

/*
 * Update the scoreboard, called whenever score changes or game ends. Only call
 * after draw_grid has been called at least once.
 */
void update_scoreboard(bool game_over);

/*
 * (Re)draws everything to the window.
 */
void redraw_all(void);


////////////////////////////////////////////////////////////////////////////////
// Functions dealing with the game's logic, defined in logic.c.
////////////////////////////////////////////////////////////////////////////////

/*
 * Pushes tiles together in the left direction. Returns true if tiles have
 * moved and false if no tiles moved.
 * When pushed, tiles pass through any empty spaces and two tiles of the same
 * value will merge into one. For example,
 * the following rows in our g.tiles array would be transformed as
 * [0,2,0,2] ---> [4,0,0,0]
 * [4,0,4,4] ---> [8,4,0,0]
 * [2,2,2,2] ---> [4,4,0,0]
 * [2,4,4,2] ---> [2,8,2,0]
 * Note from the second row that of possible merges, the leftmost merges happen
 * first. Also note from the third row the merged tiles do not merge a second
 * time, so [2,2,2,2] becomes [4,4,0,0] not [8,0,0,0]. Only a second call to
 * left() would produce [8,0,0,0] on that row.
 */
bool left(void);

/*
 * Pushes tiles together in the right direction. Returns true if tiles have
 * moved and false if no tiles moved.
 */
bool right(void);

/*
 * Pushes tiles together in the up direction. Returns true if tiles have moved
 * and false if no tiles moved.
 */
bool up(void);

/*
 * Pushes tiles together in the down direction. Returns true if tiles have
 * moved and false if no tiles moved.
 */
bool down(void);

/*
 * Places a new tile on the board. If random_tiles is false, places a '2' tile
 * at the first available location on the board. If random_tiles is true,
 * randomly selects an available location on the board and places a '2' tile
 * there with probability 90%, or a '4' tile there with probability 10%.
 */
void new_tile(bool random_tiles);

/*
 * Returns true if it is possible for the user to make a move, otherwise
 * returns false indicating game over.
 */
bool move_available(void);

/*
 * Push the current tiles and score to the undo stack, cyclically overwriting
 * the oldest values if the stack capacity has been reached.
 */
void push_undo(void);

/*
 * If no undos are available, return false. Otherwise, pop from the undo stack
 * reverting the tiles and score to the values they had prior to the last
 * (non-trivial) move and return true.
 */
bool pop_undo(void);

/*
 * Saves the current state of the game to the filename SAVEFILE defined in
 * nc_2048.h and if successful returns true, otherwise returns false.
 */
bool save_game(void);

/*
 * Loads a previously saved game from the filename SAVEFILE defined in
 * nc_2048.h and if successful returns true, otherwise returns false.
 */
bool load_game(void);

#endif

