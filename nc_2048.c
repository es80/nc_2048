/**
 * nc_2048.c
 *
 * Main file for nc_2048.
 *
 * nc_2048 is a 2048 clone for the terminal using ncurses.
 *
 * Play the original here: https://play2048.co/
 * 2048 was created by Gabriele Cirulli (http://gabrielecirulli.com) based on
 * 1024 by Veewo Studio (https://itunes.apple.com/us/app/1024!/id823499224) and
 * conceptually similar to Threes by Asher Vollmer (http://asherv.com/threes/).
 * See also https://en.wikipedia.org/wiki/2048_(video_game).
 *
 * The present version includes options to spawn new tiles either randomly or
 * deterministically and the option to undo moves and save progress.
 *
 * Instructions:
 * $ make
 * $ ./nc_2048
 *
 * To play, use the arrow keys to move tiles. Two tiles with matching numbers
 * will merge when pushed together. Whenever tiles move a new tile is added.
 * Other keys: n - new game, h - display help, q - quit, d - deterministic mode,
 * r - random mode, u - undo (up to three moves), s - save game, l - load saved
 * game.
 */

#define _XOPEN_SOURCE 500

#include "nc_2048.h"

#include <ctype.h>
#include <ncurses.h>
#include <signal.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Macro for processing control characters.
#define CTRL(x) ((x) & ~0140)

struct game g;

/*
 * Resets all game data ready for a new game and redraws.
 */
void new_game(bool random_tiles);

/*
 * Starts up ncurses. Checks window size and initialises colours. Returns true
 * iff successful.
 */
bool startup(void);

/*
 * Handle terminal window size changed signal, if received calls redraw_all.
 */
void handle_signal(int signum);


int main(int argc, char *argv[])
{
    // Start up ncurses.
    if (!startup())
    {
        fprintf(stderr, "Error starting up ncurses!\n");
        return 1;
    }

    // Register handler for SIGWINCH (SIGnal WINdow CHanged).
    signal(SIGWINCH, (void (*)(int)) handle_signal);

    // Seed random number generator.
    srand48((long int) time(NULL));

    // Some toggles for use in the game loop.
    bool new_tile_needed = false;
    bool help_toggle = false;
    bool game_over = false;
    bool random_tiles = true;

    // Initialize the game.
    new_game(random_tiles);

    // The user's input.
    int ch;

    // Main game loop.
    do
    {
        // Refresh the screen.
        refresh();

        // Get user's input and capitalize.
        ch = getch();
        ch = toupper(ch);

        // Process user's input.
        switch (ch)
        {
            // Start a new game.
            case 'N':
                new_game(random_tiles);
                break;

            // Let user manually redraw screen with ctrl-L.
            case CTRL('l'):
                redraw_all();
                break;

            // Change manner in which new tiles spawn.
            case 'D':
                random_tiles = false;
                display_message("New tiles spawn deterministically.");
                break;

            case 'R':
                random_tiles = true;
                display_message("New tiles spawn randomly.");
                break;

            // Toggle display of help.
            case 'H':
                help_toggle = !help_toggle;
                if (help_toggle)
                {
                    display_help();
                }
                else
                {
                    draw_logo();
                }
                break;

            // Undo a move.
            case 'U':
                if (pop_undo())
                {
                    draw_tiles();
                    game_over = false;
                }
                else
                {
                    display_message("No undos available.");
                }
                break;

            // Save the current game.
            case 'S':
                if (!save_game())
                {
                    display_message("Error saving game!");
                }
                else
                {
                    display_message("Game saved.");
                }
                break;

            // Load a previously saved game.
            case 'L':
                if (!load_game())
                {
                    display_message("Error loading game!");
                }
                else
                {
                    redraw_all();
                    display_message("Game loaded.");
                }
                break;

            // Move the tiles with keypad.
            case KEY_LEFT:
                new_tile_needed = left();
                break;

            case KEY_RIGHT:
                new_tile_needed = right();
                break;

            case KEY_UP:
                new_tile_needed = up();
                break;

            case KEY_DOWN:
                new_tile_needed = down();
                break;
        }

        // Add new tile if needed then add game state to undo stack.
        if (new_tile_needed)
        {
            new_tile(random_tiles);
            draw_tiles();
            new_tile_needed = false;
            push_undo();
            display_message("");
        }

        // Check moves are still available and update scoreboard.
        game_over = !move_available();
        update_scoreboard(game_over);
    }
    while (ch != 'Q');

    // Shut down ncurses and tidy up screen.
    endwin();
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);

    return 0;
}

/*
 * Resets all game data ready for a new game and redraws.
 */
void new_game(bool random_tiles)
{
    memset(g.tiles, 0, sizeof g.tiles);
    g.score = 0;
    g.undo.top = 0;
    g.undo.size = 0;
    new_tile(random_tiles);
    push_undo();
    redraw_all();
}

/*
 * Starts up ncurses. Checks window size and initialises colours. Returns true
 * iff successful.
 */
bool startup(void)
{
    // Initialize ncurses.
    if (initscr() == NULL)
    {
        return false;
    }

    // Check window dimensions are sufficient.
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);
    if (maxy < MIN_WINDOW_HEIGHT || maxx < MIN_WINDOW_WIDTH)
    {
        endwin();
        fprintf(stderr, "Terminal size must be at least %i by %i.\n",
                MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);
        return false;
    }

    // Check we can use colour.
    if (has_colors() == false)
    {
        endwin();
        fprintf(stderr, "Your terminal does not support colour.\n");
        return false;
    }

    // Enable colour.
    if (start_color() == ERR)
    {
        endwin();
        return false;
    }

    // If we can change colours then initialise some custom colours.
    if (can_change_color() == true)
    {
        // Initialise the colours defined in nc_2048.h.
        init_color(BLACK); init_color(RED); init_color(WHITE);
        init_color(ORANGE_1); init_color(ORANGE_2); init_color(ORANGE_3);
        init_color(ORANGE_4); init_color(ORANGE_5);
        init_color(GREEN_1); init_color(GREEN_2); init_color(GREEN_3);
        init_color(PURPLE_1); init_color(PURPLE_2); init_color(PURPLE_3);
        init_color(RED_1); init_color(RED_2); init_color(RED_3);
        init_color(BLUE_1); init_color(BLUE_2); init_color(BLUE_3);

        // Initialise the colour pairs we need.
        if (init_pair(PAIR_INFO, COLOR_RED, COLOR_BLACK) == ERR ||
            init_pair(PAIR_BORDER, COLOR_WHITE, COLOR_RED) == ERR ||
            init_pair(PAIR_1,  COLOR_BLACK, TILE_ORANGE_1) == ERR ||
            init_pair(PAIR_2,  COLOR_BLACK, TILE_ORANGE_2) == ERR ||
            init_pair(PAIR_3,  COLOR_BLACK, TILE_ORANGE_3) == ERR ||
            init_pair(PAIR_4,  COLOR_WHITE, TILE_ORANGE_4) == ERR ||
            init_pair(PAIR_5,  COLOR_WHITE, TILE_ORANGE_5) == ERR ||
            init_pair(PAIR_6,  COLOR_WHITE, TILE_GREEN_1) == ERR ||
            init_pair(PAIR_7,  COLOR_WHITE, TILE_GREEN_2) == ERR ||
            init_pair(PAIR_8,  COLOR_WHITE, TILE_GREEN_3) == ERR ||
            init_pair(PAIR_9,  COLOR_WHITE, TILE_PURPLE_1) == ERR ||
            init_pair(PAIR_10, COLOR_WHITE, TILE_PURPLE_2) == ERR ||
            init_pair(PAIR_11, COLOR_WHITE, TILE_PURPLE_3) == ERR ||
            init_pair(PAIR_12, COLOR_WHITE, TILE_RED_1) == ERR ||
            init_pair(PAIR_13, COLOR_WHITE, TILE_RED_2) == ERR ||
            init_pair(PAIR_14, COLOR_WHITE, TILE_RED_3) == ERR ||
            init_pair(PAIR_15, COLOR_WHITE, TILE_BLUE_1) == ERR ||
            init_pair(PAIR_16, COLOR_WHITE, TILE_BLUE_2) == ERR ||
            init_pair(PAIR_17, COLOR_WHITE, TILE_BLUE_3) == ERR )
        {
            endwin();
            return false;
        }
    }
    else
    {
        // If we can't change colours then use the eight default colours only.
        if (init_pair(PAIR_INFO, COLOR_RED, COLOR_BLACK) == ERR ||
            init_pair(PAIR_BORDER, COLOR_WHITE, COLOR_RED) == ERR ||
            init_pair(PAIR_1, COLOR_WHITE, TILE_A) == ERR ||
            init_pair(PAIR_2, COLOR_WHITE, TILE_B) == ERR ||
            init_pair(PAIR_3, COLOR_WHITE, TILE_C) == ERR ||
            init_pair(PAIR_4, COLOR_WHITE, TILE_D) == ERR ||
            init_pair(PAIR_5, COLOR_WHITE, TILE_E) == ERR ||
            init_pair(PAIR_6, COLOR_WHITE, TILE_F) == ERR ||
            init_pair(PAIR_7, COLOR_WHITE, TILE_A) == ERR ||
            init_pair(PAIR_8, COLOR_WHITE, TILE_B) == ERR ||
            init_pair(PAIR_9, COLOR_WHITE, TILE_C) == ERR ||
            init_pair(PAIR_10, COLOR_WHITE, TILE_D) == ERR ||
            init_pair(PAIR_11, COLOR_WHITE, TILE_E) == ERR ||
            init_pair(PAIR_12, COLOR_WHITE, TILE_F) == ERR ||
            init_pair(PAIR_13, COLOR_WHITE, TILE_A) == ERR ||
            init_pair(PAIR_14, COLOR_WHITE, TILE_B) == ERR ||
            init_pair(PAIR_15, COLOR_WHITE, TILE_C) == ERR ||
            init_pair(PAIR_16, COLOR_WHITE, TILE_D) == ERR ||
            init_pair(PAIR_17, COLOR_WHITE, TILE_E) == ERR )
        {
            endwin();
            return false;
        }
    }

    // Don't echo keyboard input.
    if (noecho() == ERR)
    {
        endwin();
        return false;
    }

    // Disable line buffering, allow ctrl-C signal.
    if (cbreak() == ERR)
    {
        endwin();
        return false;
    }

    // Enable arrow keys.
    if (keypad(stdscr, true) == ERR)
    {
        endwin();
        return false;
    }

    // Hide the cursor if we can.
    curs_set(0);

    // Wait 1000 ms at a time for input.
    timeout(1000);

    return true;
}

/*
 * Handle terminal window size changed signal, if received calls redraw_all.
 */
void handle_signal(int signum)
{
    // If the window size changes then redraw everything.
    if (signum == SIGWINCH)
    {
        redraw_all();
    }

    // Re-register this function to handle future signals.
    signal(signum, (void (*)(int)) handle_signal);
}

