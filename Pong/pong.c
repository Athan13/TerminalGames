# include <stdio.h>
# include <ncurses.h>
# include <string.h>
# include <unistd.h>
# include <stdlib.h>
# include <time.h>
# include <math.h>

# define X_DIMENSION 40
# define Y_DIMENSION 20
# define BALL_SPEED 1
# define FRAME_RATE 5
# define MARGINS 3

char * generate_string(int length, char character)
{
    // Returns a string of length `length + 1` filled with character
    int string_length = (length + 1) * sizeof(char);  // +1 for the null pointer at the end
    char * return_string = malloc(string_length);

    memset(return_string, character, string_length);
    return_string[string_length] = '\0';
    return return_string;
}

int main(int argc, char const *argv[])
{
    printf("\ec\n-- USE A TO MOVE LEFT AND D TO MOVE RIGHT --\n");
    sleep(1);


    // Set up ncurses
    initscr();  // initialise

    WINDOW *win = newwin(Y_DIMENSION, X_DIMENSION, MARGINS+1, MARGINS+1);  // new window for pong

    noecho();            // don't output pressed characters
    nodelay(win, TRUE);  // remove delay for inputed characters (don't wait until newline)
    cbreak();            // use non-standard characters (arrow keys, control, etc) --> KeyboardInterrupt is unaffected

    // Define walls
    char * top_wall = generate_string(X_DIMENSION+1, '-');
    char side_wall = '|';

    // Define paddle
    int paddle_lenth;

    if (argc == 2 && 2 < *argv[1] && *argv[1] < X_DIMENSION - 5)
        paddle_lenth = *argv[1];
    else
        paddle_lenth = 10;

    char * paddle = generate_string(paddle_lenth, '=');
    int paddle_x = (X_DIMENSION / 2) - (paddle_lenth / 2); int paddle_y = Y_DIMENSION - 1;

    // Define ball
    srand(time(NULL));
    char ball = 'O';

    int ball_x = (rand() % (X_DIMENSION - 4) + 3);
    int ball_y = Y_DIMENSION - 3;
    int ball_dx = BALL_SPEED;
    int ball_dy = -1 * BALL_SPEED;

    // Define sleep time
    struct timespec rest, _;
    rest.tv_sec = (int) 1 / FRAME_RATE; rest.tv_nsec = (int) (pow(10, 9) / FRAME_RATE) % (int) pow(10, 9);

    // Print pong box
    mvaddstr(MARGINS, MARGINS, top_wall);
    mvaddstr(Y_DIMENSION + MARGINS + 1, MARGINS, top_wall);

    for (int i = 1; i < Y_DIMENSION + 1; i++) {
        mvaddch(i + MARGINS, MARGINS, side_wall);
        mvaddch(i + MARGINS, X_DIMENSION + MARGINS + 1, side_wall);
    }

    refresh();

    // Score counter
    int score = 0;

    // Main game loop
    while (1) {
        werase(win);

        // Print ball, paddle and score
        mvwaddch(win, ball_y, ball_x, ball);
        mvwaddstr(win, paddle_y, paddle_x, paddle);

        char scorestr[20];  // if you cause a buffer overflow you win lol
        sprintf(scorestr, "Score: %d", score);

        mvwaddstr(win, 0, 0, scorestr);

        // Reset screen
        wmove(win, 0, 0);
        nanosleep(&rest, &_);
        rest.tv_nsec *= 0.999;
        wrefresh(win);

        // Update ball positions
        if (ball_y == Y_DIMENSION + 1)
            break;
        if (ball_x == 0 || ball_x == X_DIMENSION - 1)
            ball_dx *= -1;
        if (ball_y == 0)
            ball_dy *= -1;
        if ((ball_y + 1 == paddle_y) && (paddle_x <= ball_x) && (ball_x <= paddle_x + paddle_lenth - 1)) {
            score += 10;
            ball_dy *= -1;
        }

        ball_x += ball_dx;
        ball_y += ball_dy;

        // Update paddle position
        char user_input = wgetch(win);

        if (user_input != ERR) {
            switch (user_input) {
                case 'a':
                    if (paddle_x > 0) {paddle_x--; break;}
                case 'd':
                    if (paddle_x + paddle_lenth < X_DIMENSION - 1) {paddle_x++;}
            }
        }
    }

    // Cleanup
    delwin(win);
    endwin();

    free(top_wall);
    free(paddle);

    printf("\ec");
    printf("-- YOU LOSE! --\n");

    return 0;
}
