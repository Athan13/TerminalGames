# include <stdio.h>
# include <stdlib.h>
# include <assert.h>
# include <time.h>
# include <unistd.h>
# include <string.h>

# define MAX_LENGTH 30 // max word length of 30 characters

int print_hangman(int n) {
    FILE *textfile;
    char ch;
    int print = 0;

    assert(0 <= n & n <= 6);

    textfile = fopen("hangmen.txt", "r");

    while ((ch = fgetc(textfile)) != EOF) {
        if (ch == (n + '0')) {
            print = 1;
            continue;
        } else if (ch == ((n + 1) + '0')) {
            fclose(textfile);
            return 0;
        }
        
        if (print) {
            putchar(ch);
        }
    }

    return 1;
}

char * get_random_word() {
    FILE *word_file;
    char c;
    char line[MAX_LENGTH];

    int num_lines = 0;

    char * random_word = calloc(MAX_LENGTH, sizeof(char));

    word_file = fopen("60kWords.txt", "r");

    // Get number of lines in the file
    while ((c = fgetc(word_file)) != EOF) {
        if (c == '\n')
            num_lines++;
    }

    fclose(word_file);

    word_file = fopen("60kWords.txt", "r");

    // Get random number
    srand(time(NULL));
    int rand_num = rand() % num_lines - 1;

    int counter = 0;
    while (fgets(line, MAX_LENGTH, word_file) != NULL) {
        if (counter == rand_num) {
            line[strlen(line) - 1] = '\0';  // Remove and replace newline character from end of line
            strcpy(random_word, line);
            break;
        }
        counter++;
    }
    
    fclose(word_file);
    return random_word;
}

int main() {
    int players;
    
    int num_wrong_guesses = 0;
    char * wrong_guesses = calloc(6, sizeof(char));

    char * answer = malloc(MAX_LENGTH * sizeof(char)); 
    char guess;
    
    int win = 0;

    // Get number of players and solution word
    while (1) {
        printf("Number of players: ");
        scanf("%d", &players);

        if (players == 1) {
            char* rand_word = get_random_word();
            strcpy(answer, rand_word);
            free(rand_word);
            printf("%s", answer);
            break;
        } else if (players == 2) {
            printf("Word to be guessed: ");
            scanf("%s", answer);
            break;
        }
        else {printf("Number of players must either be 1 or 2.\n\n");}
    }

    // Set guessed_word to blank characters
    int answer_length = 0;
    while (answer[answer_length] != '\0') {
        answer_length++;
    }

    char guessed_word[answer_length + 1];
    for (int i = 0; i < answer_length; i++) {
        guessed_word[i] = '-';
    }
    guessed_word[answer_length] = '\0';

    // main game loop
    while (1) {
        printf("\ec");

        // Check if out of guesses or if word has been guessed
        if (num_wrong_guesses == 6) {
            break;
        } else if (strchr(guessed_word, '-') == NULL) {
            win = 1;
            break;
        }

        print_hangman(num_wrong_guesses);
        printf("Wrong guesses: %s\n", wrong_guesses);
        printf("Word: %s\n", guessed_word);

        printf("Guess: ");
        scanf(" %c", &guess);

        if (strchr(answer, guess) != NULL) {
            for (int i = 0; i < strlen(answer); i++) {
                if (answer[i] == guess) {
                    guessed_word[i] = guess;
                }
            }
        } else if (strchr(wrong_guesses, guess) != NULL) {
            // If guessed character has already been guessed
            printf("The letter %c has already been guessed!\n", guess);
            sleep(1);
        } else {
            // If guess is wrong and hasn not been guessed before
            wrong_guesses[num_wrong_guesses] = guess;
            num_wrong_guesses++;
        }
    }

    // win
    if (win)
        printf("%s is correct. Guesser wins!\n", answer);
    else
        printf("Guesser loses! The word was %s.\n", answer);

    free(answer);
    free(wrong_guesses);

    return 0;
}