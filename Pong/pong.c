# include <stdio.h>
# include <ncurses.h>
# include <string.h>
# include <unistd.h>
# include <stdlib.h>
# include <time.h>
# include <math.h>

# define X_DIMENSION 37
# define Y_DIMENSION 20
# define BALL_SPEED 1
# define FRAME_RATE 5
# define MARGINS 1

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
    printf("-- USE A TO MOVE LEFT AND D TO MOVE RIGHT --\n");
    sleep(1);

    initscr();

    // Define walls
    char * top_wall = generate_string(X_DIMENSION, '-');
    char side_wall = '|';

    // Define paddle
    int paddle_lenth;

    if (argc == 2 && 2 < *argv[1] && *argv[1] < X_DIMENSION - 5)
        paddle_lenth = *argv[1];
    else
        paddle_lenth = 10;

    char * paddle = generate_string(paddle_lenth, '=');
    int paddle_x = (X_DIMENSION / 2) - (paddle_lenth / 2); int paddle_y = Y_DIMENSION - 1;

    noecho(); nodelay(stdscr, TRUE); cbreak();  // make sure key input works as intended --> ncurses library

    // Define ball
    srand(time(NULL));
    char ball = 'O';
    int ball_x = (rand() % (X_DIMENSION - 2) + 1); int ball_y = Y_DIMENSION - 3; int ball_dx = BALL_SPEED; int ball_dy = -1 * BALL_SPEED;

    // Define margins
    char * side_margin = generate_string(MARGINS, ' ');
    char * top_margin = generate_string(MARGINS, '\n');

    char * inner_margin;

    // Define sleep time
    struct timespec rest, _;
    rest.tv_sec = (int) 1 / FRAME_RATE; rest.tv_nsec = (int) (pow(10, 9) / FRAME_RATE) % (int) pow(10, 9);

    // Main game loop
    while (1) {

        // Print pong box
        printw("%s", top_margin);
        printw("%s", side_margin);
        printw("%s\n", top_wall);

        for (int i = 1; i <= Y_DIMENSION - 1; i++) {
            printw("%s", side_margin);
            printw("%c\n", side_wall);

            // Print paddle
            if (i == paddle_y) {
                inner_margin = generate_string(paddle_x - 2, ' ');
                printw("%s", inner_margin);
                printw("%s", paddle);
                free(inner_margin);

                inner_margin = generate_string(X_DIMENSION - paddle_x - paddle_lenth - 2, ' ');
                printw("%s", inner_margin);
                free(inner_margin);
            // Print ball
            } else if (i == ball_y) {
                inner_margin = generate_string(ball_x - 2, ' ');
                printw("%s", inner_margin);
                printw("%c", ball);
                free(inner_margin);

                inner_margin = generate_string(X_DIMENSION - ball_x - 2, ' ');
                printw("%s", inner_margin);
                free(inner_margin);
            } else {
                inner_margin = generate_string(X_DIMENSION - 2, ' ');
                printw("%s", inner_margin);
                free(inner_margin);
            }

            printw("%c\n", side_wall);
        }

        printw("%s", side_margin);
        printw("%s\n", top_wall);

        // Reset screen
        nanosleep(&rest, &_);
        refresh();

        // Update ball positions
        if (ball_x == 1 || ball_x == X_DIMENSION - 2)
            ball_dx *= -1;
        else if (ball_y == 1 || ball_y == Y_DIMENSION)
            ball_dy *= -1;
        else if ((ball_y + 1 == paddle_y) && (paddle_x <= ball_x) && (ball_x <= paddle_x + paddle_lenth - 1))
            ball_dy *= -1;

        ball_x += ball_dx;
        ball_y += ball_dy;

        // Update paddle position
        char user_input = getch();

        switch (user_input) {
            case 'A':
                if (paddle_x > 1) {paddle_x--;}
            case 'D':
                if (paddle_x + paddle_lenth < X_DIMENSION) {paddle_x++;}
            default:
                ;
        }
    }

    free(top_wall);
    free(paddle);
    free(side_margin);
    free(top_margin);

    printf("You lose!\n");

    return 0;
}
