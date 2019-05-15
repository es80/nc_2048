# ncurses 2048

A 2048 clone for the terminal using ncurses.

Play the original [here](https://play2048.co/). 2048 was created by [Gabriele
Cirulli](http://gabrielecirulli.com) based on
[1024 by Veewo Studio](https://itunes.apple.com/us/app/1024!/id823499224) and
conceptually similar to [Threes by Asher Vollmer](http://asherv.com/threes/).
See also [2048 on wikipedia](https://en.wikipedia.org/wiki/2048_(video_game)).

The present version includes options to spawn new tiles either randomly or
deterministically and the option to undo moves and save progress.

## Instructions

```
make
./nc_2048
```

To play a game use the arrow keys to move tiles. Two tiles with matching
numbers will merge when pushed together. Whenever tiles move a new tile is
added.

Press 'n' to start a new game, 'h' to display help, 'q' to quit.

Use 'd' for new tiles to be spawned deterministically, and 'r' for new tiles to
be spawned randomly (90% chance of '2', 10% chance of '4').

To save a game press 's', to load a previously saved game press 'l'.

Use 'u' to undo up to three moves.

### Screenshot

![ncurses 2048 screenshot](/nc_2048_screenshot.png?raw=true)

