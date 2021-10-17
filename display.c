/**
 * display.c
 *
 * Defines functions used for drawing to the screen.
 */

#include "nc_2048.h"

#include <locale.h>
#include <ncurses.h>
#include <stdbool.h>
#include <string.h>

extern struct game g;

/*
 * Draws borders at the top and bottom of window.
 */
void draw_borders(void)
{
    // Get the window's dimensions.
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);

    // Enable colour.
    attron(COLOR_PAIR(PAIR_BORDER));

    // Draw border background.
    for (int i = 0; i < maxx; i++)
    {
        mvaddch(0, i, ' ');
        mvaddch(maxy-1, i, ' ');
    }

    // Header and footer text.
    const char *head[3] = { "[N]ew Game   [H]elp",
                            "nc2048",
                            "[Q]uit Game " };
    const char *foot[3] = { "[D]eterministic/[R]andom",
                            "[U]ndo move",
                            "[S]ave/[L]oad game " };

    // Draw header and footer text.
    mvaddstr(0, 1, head[0]);
    mvaddstr(0, (maxx - strlen(head[1])) / 2, head[1]);
    mvaddstr(0, maxx - strlen(head[2]), head[2]);

    mvaddstr(maxy - 1, 1, foot[0]);
    mvaddstr(maxy - 1,
             (maxx + strlen(foot[0]) - strlen(foot[1]) - strlen(foot[2])) / 2,
             foot[1]);
    mvaddstr(maxy - 1, maxx - strlen(foot[2]), foot[2]);

    // Disable colour.
    attroff(COLOR_PAIR(PAIR_BORDER));
}

/*
 * Draws the outline of the game board. Must be called before any other drawing
 * functions since it determines the locations of the top-left corner of the
 * board.
 */
void draw_grid(void)
{
    // Get the window's dimensions.
    int maxy, maxx;
    getmaxyx(stdscr, maxy, maxx);

    // Determine top-left corner of board.
    g.y = maxy/2 - 9;
    g.x = maxx/2 - 40;

    // Write the grid to the window.
    for (int i = 0; i < DIM; i++)
    {
        mvaddstr(g.y + 0 + 4 * i, g.x, "+---------+---------+---------+---------+");
        for (int j = 1; j < DIM; j++)
        {
            mvaddstr(g.y + j + 4 * i, g.x, "|         |         |         |         |");
        }
    }
    mvaddstr(g.y + 16, g.x, "+---------+---------+---------+---------+");
}

/*
 * Draws the game's tiles. Only call after draw_grid has been called at least
 * once.
 */
void draw_tiles(void)
{
    // If possible draw numbers in bold face.
    attron(A_BOLD);

    // Iterate over tile numbers.
    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            // Determine a colour number based on the tile number.
            int colour_num = 0;
            int tile_num = g.tiles[i][j];

            // Use log_2(tile_num) to get a colour number.
            if (tile_num)
            {
                while (tile_num >>= 1)
                {
                    colour_num++;
                }
            }

            // Apply the colour pair.
            attron(COLOR_PAIR(colour_num));

            // Write a line of spaces.
            move(g.y + 1 + 4*i, g.x + 1 + 10*j);
            for (int k = 0; k < 9; k++)
                addch(' ');

            // Determine a number string for the tile number.
            char num_str[6] = {'\0'};
            if (g.tiles[i][j] != 0)
                sprintf(num_str, "%i", g.tiles[i][j]);
            int len = strlen(num_str);

            // A prefix and suffix to centre the number string.
            int prefix = (9 - len) / 2;
            int suffix = len % 2 ? (9 - len) / 2 : (9 - len + 1) / 2;

            // Write spaces for the prefix, the number string, then spaces for
            // the suffix.
            move(g.y + 2 + 4*i, g.x + 1 + 10*j);
            for (int k = 0; k < prefix; k++)
                addch(' ');
            addstr(num_str);
            for (int k = 0; k < suffix; k++)
                addch(' ');

            // Write another line of spaces.
            for (int k = 0; k < 9; k++)
                mvaddch(g.y + 3 + 4*i, g.x + 1 + k + 10*j, ' ');

            // Disable colour.
            attroff(COLOR_PAIR(colour_num));
        }
    }
    attroff(A_BOLD);
    refresh();
}

/*
 * Draws a game logo to the right of the game board. Only call after draw_grid
 * has been called at least once.
 */
void draw_logo(void)
{
    // Determine starting coordinates for logo.
    int logo_x = g.x + 44;
    int y = g.y + 1;

    // Clear the area.
    for (int r = 0; r < MAX_HEIGHT_LOGO_HELP; r++)
    {
        move(y + r, logo_x);
        for (int c = 0; c < MAX_WIDTH_LOGO_HELP; c++)
        {
            addch(' ');
        }
    }

    // Enable colour.
    attron(COLOR_PAIR(PAIR_INFO));

    // Draw logo.
    mvaddstr(y + 0, logo_x, "            ___   ___  _  _   ___  ");
    mvaddstr(y + 1, logo_x, "           |__ \\ / _ \\| || | / _ \\ ");
    mvaddstr(y + 2, logo_x, " _ __   ___   ) | | | | || || (_) |");
    mvaddstr(y + 3, logo_x, "| '_ \\ / __| / /| | | |__   _> _ < ");
    mvaddstr(y + 4, logo_x, "| | | | (__ / /_| |_| |  | || (_) |");
    mvaddstr(y + 5, logo_x, "|_| |_|\\___|____|\\___/   |_| \\___/ ");

    // Disable colour.
    attroff(COLOR_PAIR(PAIR_INFO));
}

/*
 * Displays help text to the right of the game board. Only call after draw_grid
 * has been called at least once.
 */
void display_help(void)
{
    // Determine starting coordinates for help text.
    int x = g.x + 44;
    int y = g.y + 1;

    // Clear the area.
    for (int r = 0; r < MAX_HEIGHT_LOGO_HELP; r++)
    {
        move(y + r, x);
        for (int c = 0; c < MAX_WIDTH_LOGO_HELP; c++)
        {
            addch(' ');
        }
    }

    // An array of help text.
    const char *help[MAX_HEIGHT_LOGO_HELP] = {
                             "To play, use the arrow keys to move",
                             "tiles. Two tiles with matching",
                             "numbers will merge when pushed",
                             "together. Whenever tiles move a new",
                             "tile is added.",
                             " ",
                             "Useful keys:",
                             "N - Start a new game",
                             "H - Toggle help display",
                             "Q - Quit the game",
                             "D - Deterministic mode",
                             "R - Random mode",
                             "U - Undo (up to three moves)",
                             "S - Save current game",
                             "L - Load previously saved game" };

    // Enable colour.
    attron(COLOR_PAIR(PAIR_INFO));

    // Write the text to the window.
    for (int i = 0; i < MAX_HEIGHT_LOGO_HELP; i++)
    {
        move(y + i, x);
        addstr(help[i]);
    }

    // Disable colour.
    attroff(COLOR_PAIR(PAIR_INFO));
}

/*
 * Displays a message below and to the right of the game board. Only call after
 * draw_grid has been called at least once.
 */
void display_message(char *s)
{
    // Determine starting coordinates for message text.
    int x = g.x + 44;
    int y = g.y + 18;

    // Clear the area.
    move(y, x);
    for (int c = 0; c < MAX_WIDTH_LOGO_HELP; c++)
    {
        addch(' ');
    }

    // Enable colour.
    attron(COLOR_PAIR(PAIR_INFO));

    // Write the message to the window.
    mvaddstr(y, x + MAX_WIDTH_LOGO_HELP - strlen(s), s);

    // Disable colour.
    attroff(COLOR_PAIR(PAIR_INFO));
}

/*
 * Update the scoreboard, called whenever score changes or game ends. Only call
 * after draw_grid has been called at least once.
 */
void update_scoreboard(bool game_over)
{
    // Reset scoreboard, overwrite with spaces.
    for (int i = 0; i < 34; i++)
        mvaddch(g.y + 18, g.x + 6 + i, ' ');

    // The maximum theoretical score is 3,932,100.
    // https://oeis.org/A058922
    // https://www.reddit.com/r/2048/comments/214njx/highest_possible_score_for_2048_warning_math/

    // Use a thousands separator.
    setlocale(LC_NUMERIC, "");

    // Determine a score string.
    char score_str[34] = {'\0'};
    if (game_over)
        sprintf(score_str, "Game Over! Final Score: %'d", g.score);
    else
        sprintf(score_str, "Score: %'d", g.score);

    // Enable colour.
    attron(COLOR_PAIR(PAIR_INFO));

    // Write score string to window relative to top-left corner of board.
    mvaddstr(g.y + 18, g.x + 40 - strlen(score_str), score_str);

    // Disable colour.
    attroff(COLOR_PAIR(PAIR_INFO));
}

/*
 * (Re)draws everything to the window.
 */
void redraw_all(void)
{
    // Reset ncurses.
    endwin();
    refresh();

    // Clear screen.
    clear();

    // Re-draw everything.
    draw_borders();
    draw_grid();
    draw_logo();
    draw_tiles();
    update_scoreboard(!move_available());
}

