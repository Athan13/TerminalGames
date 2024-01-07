#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include <time.h>
#include <unistd.h>

#include <ncurses.h>

#define MAX_WIDTH 100

char* get_ref_string() {
    srand(time(NULL));
    int file_n = rand() % 6;

    char* filepath = malloc(11 * sizeof(char));
    sprintf(filepath, "texts/input%d.txt", file_n);
    FILE* text_f = fopen(filepath, "r");
    free(filepath);

    fseek(text_f, 0, SEEK_END);
    int len = ftell(text_f);
    rewind(text_f);

    char* return_c = calloc(len + 1, sizeof(char));
    fread(return_c, len, 1, text_f);

    fclose(text_f);
    return return_c;
}

// https://gist.github.com/reagent/9819045 -- thanks!
void draw_borders(WINDOW *screen) {
  int x, y, i;

  getmaxyx(screen, y, x);

  // 4 corners
  mvwprintw(screen, 0, 0, "+");
  mvwprintw(screen, y - 1, 0, "+");
  mvwprintw(screen, 0, x - 1, "+");
  mvwprintw(screen, y - 1, x - 1, "+");

  // sides
  for (i = 1; i < (y - 1); i++) {
    mvwprintw(screen, i, 0, "|");
    mvwprintw(screen, i, x - 1, "|");
  }

  // top and bottom
  for (i = 1; i < (x - 1); i++) {
    mvwprintw(screen, 0, i, "-");
    mvwprintw(screen, y - 1, i, "-");
  }
}

char* format_ref_string(int start, size_t length, char* ref_string) {
    char* return_c;
    size_t ref_length = strlen(ref_string);

    if (start >= ref_length || start < 0) {
        return_c = calloc(1, sizeof(char));
        strncpy(return_c, "", 0);
    } else if (start + length >= ref_length) {
        return_c = calloc(ref_length - start + length + 1, sizeof(char));
        strncpy(return_c, ref_string + start, ref_length - start + length);
    } else {
        return_c = calloc(length + 1, sizeof(char));
        strncpy(return_c, ref_string + start, length);
    }

    return return_c;
}

void clear_str(char* input_str, char replacement) {
    int len = strlen(input_str);
    memset(input_str, replacement, len);
    input_str[len] = '\0';
}

int main(int argc, char *argv[]) {
    int time;
    char curr_c;

    printf("Type as fast as you can! Press ESCAPE to exit.\n");
    sleep(2);

    if (argc < 2) {
        time = 60;
    } else {
        time = atoi(argv[1]);
    }

    time = time > 9999 ? 9999 : time;

    const int start_time = time;
    char* ref_string = get_ref_string();

    // set up ncurses
	initscr();			/* Start curses mode 		*/
	raw();				/* Line buffering disabled	*/
	keypad(stdscr, TRUE);		/* We get F1, F2 etc..		*/
	noecho();			/* Don't echo() while we do getch */  
    curs_set(0); 

    int width = COLS > MAX_WIDTH ? MAX_WIDTH : COLS;

    WINDOW* instructions_test = newwin(1, width, LINES - 11, 0);
    WINDOW* ref_text_window = newwin(6, width, LINES - 10, 0);
    WINDOW* input_window = newwin(3, width, LINES - 4, 0);

    nodelay(instructions_test, true);
    nodelay(ref_text_window, true);
    nodelay(input_window, true);

    // set up WPM counter
    clock_t start = clock();
    char ref_c;
    int correct_chars = 0;

    int ref_text_pos = 0;
    int input_word_pos = 0;

    char input_word[width - 15];
    memset(input_word, '\0', sizeof(input_word));
    int max_length = (width - 15) * sizeof(char);
    int wpm = 0;

    // set up string formatter
    int line_counter = 0;
    char* line0 = format_ref_string(-1, width - 2, ref_string);
    char* line1 = format_ref_string(0, width - 2, ref_string);
    char* line2 = format_ref_string(width - 2, width - 2, ref_string);

    // main game loop
    while(time >= 0) {
        mvwprintw(instructions_test, 0, 0, "Type as fast as you can! Press ESCAPE to exit.\n");
        wrefresh(instructions_test);

        // draw reference text window
        draw_borders(ref_text_window);

        mvwprintw(ref_text_window, 1, 1, "Time remaining: %d", time);
        wpm = 12 * correct_chars / (start_time - time);
        mvwprintw(ref_text_window, 1, width - 9, "WPM: %d", wpm);

        mvwprintw(ref_text_window, 2, 1, line0);
        mvwprintw(ref_text_window, 3, 1, line1);
        mvwprintw(ref_text_window, 4, 1, line2);

        wrefresh(ref_text_window);

        // draw input window
        draw_borders(input_window);
        mvwprintw(input_window, 1, 1, "Type here: %s", input_word);
        wrefresh(input_window);

        // compute time
        if ((double) clock() - start >= 1000000) {
            start = clock();
            time--;
        }

        // add string
        ref_c = line1[ref_text_pos];
        if (ref_c == '\0') {
            line_counter++;

            free(line0);
            line0 = line1;
            line1 = line2;
            line2 = format_ref_string((line_counter + 1) * (width - 2), width - 2, ref_string);
            ref_text_pos = 0;
            clear_str(input_word, ' ');
        }

        char in_c = wgetch(input_window);
        if (in_c == 27) {  // Escape key
            break;
        } else if (in_c == ref_c) {
            input_word[input_word_pos] = in_c;
            input_word[input_word_pos + 1] = '\0';
            input_word_pos++;
            ref_text_pos++;

            if (!isspace(in_c)) {
                correct_chars++;
            } else {
                clear_str(input_word, ' ');
                input_word_pos = 0;
            }

            if (input_word_pos == max_length - 1) {
                clear_str(input_word, ' ');
                input_word_pos = 0;
            }
        }
    }

    endwin();

    // Cleanup
    free(ref_string);
    delwin(ref_text_window);
    delwin(input_window);

    char* quality;

    if (wpm < 30) {
        quality = "bad";
    } else if (wpm < 60) {
        quality = "good";
    } else if (wpm < 110) {
        quality = "great";
    } else {
        quality = "world class";
    }

    printf("You were able to type %d words per minute! This makes you a %s typer.\n", wpm, quality);

    return 0;
}
