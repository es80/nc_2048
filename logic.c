/**
 * logic.c
 *
 * Defines functions used for the game's logic, undoing moves and saving or
 * loading a game.
 */

#define _XOPEN_SOURCE 500

#include "nc_2048.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
bool left(void)
{
    /* The functions left(), right(), up() and down() are all very similar and
     * much code is repeated. The only difference is the order in which we
     * iterate over the two dimensions of our g.tiles array.
     * To avoid repeated code it is also possible to write a single push
     * function for, say, the left direction. Then for the other directions
     * copy the array into a temporary transposed and/or reversed array, call
     * the push function on that array, then copy back that array into the
     * original tiles array. But the saving is not dramatic and the longer
     * approach used here is also more efficient in execution.
     */

    // In the main game loop, new tiles are only added to the board when tiles
    // move so we must track if this happens and return the result.
    bool new_tile_needed = false;

    // We need a single loop over the tiles array and can change the values
    // in place.
    for (int i = 0; i < DIM; i++)
    {
        // For each row of tiles we track the number of zeros seen, and we
        // remember the value of the last non-zero tile seen which hasn't yet
        // been merged with another.
        int zeros = 0;
        int unmerged = 0;

        for (int j = 0; j < DIM; j++)
        {
            if (g.tiles[i][j] == 0)
            {
                zeros++;
            }
            else if (g.tiles[i][j] == unmerged)
            {
                // We have found two tiles to merge.
                // Place the resulting tile in the leftmost position.
                g.tiles[i][j-zeros-1] = unmerged * 2;
                g.score += unmerged * 2;
                new_tile_needed = true;
                zeros++;
                unmerged = 0;
            }
            else if (unmerged)
            {
                // The unmerged tile cannot be merged.
                if (zeros)
                {
                    new_tile_needed = true;
                }
                // It goes back into the array and the present tile becomes
                // the next unmerged tile.
                g.tiles[i][j-zeros-1] = unmerged;
                unmerged = g.tiles[i][j];
            }
            else
            {
                // The last possibility is that there was no unmerged tile, so
                // just select the present tile to become the unmerged tile.
                unmerged = g.tiles[i][j];
            }
        }

        // Now we are finished merging tiles and need to fill out any possible
        // remaining spaces in the row. Firstly, with the current unmerged tile
        // if one exists..
        if (unmerged)
        {
            if (g.tiles[i][DIM-zeros-1] != unmerged)
            {
                new_tile_needed = true;
                g.tiles[i][DIM-zeros-1] = unmerged;
            }
        }

        // ..secondly with empty tiles.
        while (zeros)
        {
            g.tiles[i][DIM-zeros] = 0;
            zeros--;
        }
    }

    return new_tile_needed;
}

/*
 * Pushes tiles together in the right direction. Returns true if tiles have
 * moved and false if no tiles moved.
 */
bool right(void)
{
    bool new_tile_needed = false;
    for (int i = 0; i < DIM; i++)
    {
        int zeros = 0;
        int unmerged = 0;
        // The only difference with the left() function is that we iterate
        // in reverse over each row.
        for (int j = DIM - 1; j >= 0; j--)
        {
            if (g.tiles[i][j] == 0)
            {
                zeros++;
            }
            else if (g.tiles[i][j] == unmerged)
            {
                g.tiles[i][j+zeros+1] = unmerged * 2;
                g.score += unmerged * 2;
                new_tile_needed = true;
                zeros++;
                unmerged = 0;
            }
            else if (unmerged)
            {
                if (zeros)
                {
                    new_tile_needed = true;
                }
                g.tiles[i][j+zeros+1] = unmerged;
                unmerged = g.tiles[i][j];
            }
            else
            {
                unmerged = g.tiles[i][j];
            }
        }
        if (unmerged)
        {
            if (g.tiles[i][zeros] != unmerged)
            {
                new_tile_needed = true;
                g.tiles[i][zeros] = unmerged;
            }
        }
        while (zeros)
        {
            g.tiles[i][zeros-1] = 0;
            zeros--;
        }
    }
    return new_tile_needed;
}

/*
 * Pushes tiles together in the up direction. Returns true if tiles have moved
 * and false if no tiles moved.
 */
bool up(void)
{
    bool new_tile_needed = false;
    // The only difference with the left() function is that we iterate over
    // columns first and then over rows.
    for (int j = 0; j < DIM; j++)
    {
        int zeros = 0;
        int unmerged = 0;
        for (int i = 0; i < DIM; i++)
        {
            if (g.tiles[i][j] == 0)
            {
                zeros++;
            }
            else if (g.tiles[i][j] == unmerged)
            {
                g.tiles[i-zeros-1][j] = unmerged * 2;
                g.score += unmerged * 2;
                new_tile_needed = true;
                zeros++;
                unmerged = 0;
            }
            else if (unmerged)
            {
                if (zeros)
                {
                    new_tile_needed = true;
                }
                g.tiles[i-zeros-1][j] = unmerged;
                unmerged = g.tiles[i][j];
            }
            else
            {
                unmerged = g.tiles[i][j];
            }
        }
        if (unmerged)
        {
            if (g.tiles[DIM-zeros-1][j] != unmerged)
            {
                new_tile_needed = true;
                g.tiles[DIM-zeros-1][j] = unmerged;
            }
        }
        while (zeros)
        {
            g.tiles[DIM-zeros][j] = 0;
            zeros--;
        }
    }
    return new_tile_needed;
}

/*
 * Pushes tiles together in the down direction. Returns true if tiles have
 * moved and false if no tiles moved.
 */
bool down(void)
{
    bool new_tile_needed = false;
    // The only differences with the left() function is that we iterate over
    // columns first and then over rows and when we iterate over rows we do so
    // in reverse.
    for (int j = 0; j < DIM; j++)
    {
        int zeros = 0;
        int unmerged = 0;
        for (int i = DIM - 1; i >= 0; i--)
        {
            if (g.tiles[i][j] == 0)
            {
                zeros++;
            }
            else if (g.tiles[i][j] == unmerged)
            {
                g.tiles[i+zeros+1][j] = unmerged * 2;
                g.score += unmerged * 2;
                new_tile_needed = true;
                zeros++;
                unmerged = 0;
            }
            else if (unmerged)
            {
                if (zeros)
                {
                    new_tile_needed = true;
                }
                g.tiles[i+zeros+1][j] = unmerged;
                unmerged = g.tiles[i][j];
            }
            else
            {
                unmerged = g.tiles[i][j];
            }
        }
        if (unmerged)
        {
            if (g.tiles[zeros][j] != unmerged)
            {
                new_tile_needed = true;
                g.tiles[zeros][j] = unmerged;
            }
        }
        while (zeros)
        {
            g.tiles[zeros-1][j] = 0;
            zeros--;
        }
    }
    return new_tile_needed;
}

/*
 * Places a new tile on the board. If random_tiles is false, places a '2' tile
 * at the first available location on the board. If random_tiles is true,
 * randomly selects an available location on the board and places a '2' tile
 * there with probability 90%, or a '4' tile there with probability 10%.
 */
void new_tile(bool random_tiles)
{
    // Count the number of available locations for a new tile to be placed.
    int zeros_count = 0;
    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            if (g.tiles[i][j] == 0)
            {
                zeros_count++;
            }
        }
    }

    // Pick a location to use and a tile to place there.
    int new_placement;
    int new_tile;
    if (random_tiles)
    {
        new_placement = (int) (drand48() * zeros_count);
        new_tile = drand48() < 0.9 ? 2 : 4;
    }
    else
    {
        new_placement = 0;
        new_tile = 2;
    }

    // Place the tile on the board.
    zeros_count = 0;
    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            if (g.tiles[i][j] == 0)
            {
                if (zeros_count == new_placement)
                {
                    g.tiles[i][j] = new_tile;
                    return;
                }
                else
                {
                    zeros_count++;
                }
            }
        }
    }
}

/*
 * Returns true if it is possible for the user to make a move, otherwise
 * returns false indicating game over.
 */
bool move_available(void)
{
    // If a move is available then either there is a zero tile or two adjacent
    // tiles have the same value.
    for (int i = 0; i < DIM; i++)
    {
        for (int j = 0; j < DIM; j++)
        {
            if (g.tiles[i][j] == 0)
            {
                return true;
            }
            if (i < DIM - 1 && g.tiles[i][j] == g.tiles[i+1][j])
            {
                return true;
            }
            if (j < DIM - 1 && g.tiles[i][j] == g.tiles[i][j+1])
            {
                return true;
            }
        }
    }

    return false;
}

/*
 * Push the current tiles and score to the undo stack, cyclically overwriting
 * the oldest values if the stack capacity has been reached.
 */
void push_undo(void)
{
    // Cyclically increment the stack top.
    g.undo.top = (g.undo.top + 1) % UNDO_CAPACITY;
    // If not yet at capacity, increment the stack size.
    if (g.undo.size < UNDO_CAPACITY)
    {
        g.undo.size++;
    }

    // Copy the relevant values.
    memcpy(g.undo.tiles[g.undo.top], g.tiles, sizeof g.tiles);
    g.undo.score[g.undo.top] = g.score;
}

/*
 * If no undos are available, return false. Otherwise, pop from the undo stack
 * reverting the tiles and score to the values they had prior to the last
 * (non-trivial) move and return true.
 */
bool pop_undo(void)
{
    // Check there are still valid values to restore.
    if (g.undo.size <= 1)
    {
        return false;
    }

    // Locate the index prior to the current top.
    int index = g.undo.top ? g.undo.top - 1 : UNDO_CAPACITY - 1;

    // Copy the relevant values.
    memcpy(g.tiles, g.undo.tiles[index], sizeof g.tiles);
    g.score = g.undo.score[index];

    // Cyclically decrement the stack top.
    g.undo.top = (g.undo.top + UNDO_CAPACITY - 1) % UNDO_CAPACITY;
    // Decrement the stack size.
    g.undo.size--;

    return true;
}

/*
 * Saves the current state of the game to the filename SAVEFILE defined in
 * nc_2048.h and if successful returns true, otherwise returns false.
 */
bool save_game(void)
{
    FILE *fp = fopen(SAVEFILE, "wb");
    if (!fp)
    {
        return false;
    }

    if (fwrite(&g, sizeof g, 1, fp) != 1)
    {
        fclose(fp);
        return false;
    }

    fclose(fp);
    return true;
}

/*
 * Loads a previously saved game from the filename SAVEFILE defined in
 * nc_2048.h and if successful returns true, otherwise returns false.
 */
bool load_game(void)
{
    FILE *fp = fopen(SAVEFILE, "rb");
    if (!fp)
    {
        return false;
    }

    // Try and load into a temporary game structure in case of errors.
    struct game temp;
    if (fread(&temp, sizeof temp, 1, fp) != 1)
    {
        fclose(fp);
        return false;
    }

    // Success, copy the data read in to the global game structure.
    memcpy(&g, &temp, sizeof g);
    fclose(fp);
    return true;
}

