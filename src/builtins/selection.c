#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include "../arraylist.h"
#include "../log.h"

#ifndef _WIN32
#include <ncurses.h>


#define SCROLL_PADDING 4

// TODO: fix scrolling if list is longer than the screen height

static int s = 0;

static void render(arraylist options, int pos, char *msg, bool* selected) {
    erase();

    int h, w;
    getmaxyx(stdscr, h, w);

	int start = 3;

	if (pos > s + h - start - SCROLL_PADDING) {
		s = pos - h + start + SCROLL_PADDING;
	}
	else if (pos < s - start + SCROLL_PADDING) {
		s = pos + start - SCROLL_PADDING;
		if (s < 0) s = 0;
	}


	if (msg != NULL) {
		// Header
		attron(A_BOLD | COLOR_PAIR(2));
		mvprintw(start - s - 2, 2, "%s", msg);
		attroff(A_BOLD | COLOR_PAIR(2));
	}

    // Options
    for (int i = 0; i < options.count; i++) {
        char *opt = ((char **)options.items)[i];
        int y = i - s + start;

		if (y < 0 || y >= h - 2) continue;

		char mark = selected[i] ? 'x' : ' ';

        if (i == pos) {
            attron(A_BOLD | COLOR_PAIR(1));
            mvprintw(y, 4, "[%c] %-30s", mark, opt);
            attroff(A_BOLD | COLOR_PAIR(1));
        } else {
            mvprintw(y, 4, "[%c] %s", mark, opt);
        }
    }

	int y = start + options.count + 1 < h - 1 ? start + options.count + 1 : h - 1;

	mvprintw(y, 2, "%4d/%-4ld", pos + 1, options.count);

    attron(A_BOLD | COLOR_PAIR(3));
	mvprintw(y, 13, "Select (\\n)");
    attroff(A_BOLD | COLOR_PAIR(3));

    attron(A_BOLD | COLOR_PAIR(4));
	mvprintw(y, 26, "Exit (q)");
    attroff(A_BOLD | COLOR_PAIR(4));

    refresh();
}

#endif


bool builtin_selection(arraylist* args, int count, arraylist* out) {
	if (count != 1 && count != 2) {
		log_error("@selection(): Expected 1 or 2 arguments, got %d", count);
		return false;
	}

#ifdef _WIN32
	return true;
#else

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
	bool selected[options.count];
	memset(selected, 0, sizeof(selected));
	int pos = 0;

	render(options, pos, msg, selected);
	
	int c;
	while ((c = getch()) != '\n' && c != 'q') {
		if (c == KEY_UP) {
			if (pos > 0) pos--;
		}
		else if (c == KEY_DOWN) {
			if (pos < (int64_t)options.count - 1) pos++;
		}
		else if (c == ' ') {
			selected[pos] = !selected[pos];
		}

		render(options, pos, msg, selected);
	}
	if (c == 'q') {
		endwin();
		log_info("Exiting options menu");
		return false;
	}
	for (int i = 0; i < options.count; i++) {
		if (selected[i]) {
			arraylist_append(out, options.items + i * options.item_size);
		}
	}
	
	endwin();
#endif
	return true;
}
