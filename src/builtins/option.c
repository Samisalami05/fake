#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../arraylist.h"
#include "../log.h"
#include <ncurses.h>

// TODO: fix scrolling if list is longer than the screen height

void render(arraylist options, int pos, char *msg) {
    erase();

    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);

    int start_y = (max_y - options.count) / 2;

	if (msg != NULL) {
		// Header
		attron(A_BOLD | COLOR_PAIR(2));
		mvprintw(start_y - 2, 2, "%s", msg);
		attroff(A_BOLD | COLOR_PAIR(2));
	}

    // Options
    for (int i = 0; i < options.count; i++) {
        char *opt = ((char **)options.items)[i];
        int y = start_y + i;

        if (i == pos) {
            attron(A_BOLD | COLOR_PAIR(1));
            mvprintw(y, 2, " * %-30s", opt);
            attroff(A_BOLD | COLOR_PAIR(1));
        } else {
            mvprintw(y, 2, " * %s", opt);
        }
    }

    attron(A_BOLD | COLOR_PAIR(3));
	mvprintw(start_y + options.count + 1, 2, "Select (\\n)");
    attroff(A_BOLD | COLOR_PAIR(3));

    attron(A_BOLD | COLOR_PAIR(4));
	mvprintw(start_y + options.count + 1, 15, "Exit (q)");
    attroff(A_BOLD | COLOR_PAIR(4));

    refresh();
}


bool builtin_option(arraylist* args, int count, arraylist* out) {
	if (count != 1 && count != 2) {
		log_error("@prompt(): Expected 1 or 2 arguments, got %d", count);
		return false;
	}

	char* msg = NULL;
	if (count == 2) {
		msg = ((char**)args->items)[0];
	}

	initscr();
	noecho();
	cbreak();
	keypad(stdscr, TRUE);
	curs_set(0);


	start_color();
	use_default_colors();

	init_pair(1, COLOR_BLACK, COLOR_CYAN);
	init_pair(2, COLOR_CYAN, -1);
	init_pair(3, COLOR_GREEN, -1);
	init_pair(4, COLOR_RED, -1);

	arraylist options = args[count == 1 ? 0 : 1];
	int pos = 0;

	render(options, pos, msg);
	
	int c;
	while ((c = getch()) != '\n' && c != 'q') {
		if (c == KEY_UP) {
			if (pos > 0) pos--;
		}
		else if (c == KEY_DOWN) {
			if (pos < options.count - 1) pos++;
		}

		render(options, pos, msg);
	}
	if (c == 'q') {
		endwin();
		log_info("Exiting options menu");
		return false;
	}
	arraylist_append(out, options.items + pos * options.item_size);
	
	endwin();
	return true;
}
