#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <curses.h>   // PDCurses on Windows

#define TRACK_LENGTH 10

// Structure to simulate a simple CPU register operation
typedef struct {
    int operand1;
    int operand2;
    int result;
} RegisterSim;

// Structure to store player information
typedef struct {
    char name[20];
    int position;
    int score;
} Player;

// Function declarations
void displayTrack(Player *p1, Player *p2);
int computerAnswer();
void saveGame(Player *p1, Player *p2);
void loadGame(Player *p1, Player *p2);
void initPlayers(Player *human, Player *computer);
void playTurn(Player *player, int isHuman);
void endGame(Player *p1, Player *p2);
void initGraphics();

// Global register simulation variable
RegisterSim reg;

// Color pair IDs
#define CP_PLAYER   1
#define CP_COMPUTER 2
#define CP_BOTH     3
#define CP_TITLE    4
#define CP_TEXT     5

int main() {
    Player *human = malloc(sizeof(Player));
    Player *computer = malloc(sizeof(Player));

    int choice;
    srand((unsigned int)time(NULL));

    initGraphics();

    attron(COLOR_PAIR(CP_TITLE));
    mvprintw(1, 2, "=== Welcome to Penguin Jump ===");
    attroff(COLOR_PAIR(CP_TITLE));
    mvprintw(3, 2, "1. New Game");
    mvprintw(4, 2, "2. Load Saved Game");
    mvprintw(5, 2, "Enter choice: ");
    echo();
    scanw("%d", &choice);
    noecho();

    if (choice == 2) {
        loadGame(human, computer);
    } else {
        initPlayers(human, computer);
    }

    int turn = 0;
    while (human->position < TRACK_LENGTH && computer->position < TRACK_LENGTH) {
        displayTrack(human, computer);

        if (turn % 2 == 0) {
            playTurn(human, 1);
        } else {
            playTurn(computer, 0);
        }

        saveGame(human, computer);
        turn++;
        mvprintw(20, 2, "Press Enter to continue...");
        refresh();
        getch();
        clear();
    }

    endGame(human, computer);

    getch();
    endwin();

    free(human);
    free(computer);
    return 0;
}

// Start PDCurses, set up colors
void initGraphics() {
    initscr();
    start_color();
    cbreak();
    keypad(stdscr, TRUE);

    init_pair(CP_PLAYER,   COLOR_CYAN,   COLOR_BLACK);
    init_pair(CP_COMPUTER, COLOR_RED,    COLOR_BLACK);
    init_pair(CP_BOTH,     COLOR_YELLOW, COLOR_BLACK);
    init_pair(CP_TITLE,    COLOR_GREEN,  COLOR_BLACK);
    init_pair(CP_TEXT,     COLOR_WHITE,  COLOR_BLACK);
}

void initPlayers(Player *human, Player *computer) {
    snprintf(human->name, sizeof(human->name), "You");
    human->position = human->score = 0;

    snprintf(computer->name, sizeof(computer->name), "Computer");
    computer->position = computer->score = 0;
}

// Draw the track using colored blocks in a bordered window
void displayTrack(Player *p1, Player *p2) {
    clear();
    attron(COLOR_PAIR(CP_TITLE));
    mvprintw(1, 2, "TRACK");
    attroff(COLOR_PAIR(CP_TITLE));

    int startX = 2, y = 3;
    mvaddch(y, startX - 1, '[');
    for (int i = 0; i < TRACK_LENGTH; i++) {
        if (i == p1->position && i == p2->position) {
            attron(COLOR_PAIR(CP_BOTH));
            mvaddch(y, startX + i, 'B');
            attroff(COLOR_PAIR(CP_BOTH));
        } else if (i == p1->position) {
            attron(COLOR_PAIR(CP_PLAYER));
            mvaddch(y, startX + i, 'P');
            attroff(COLOR_PAIR(CP_PLAYER));
        } else if (i == p2->position) {
            attron(COLOR_PAIR(CP_COMPUTER));
            mvaddch(y, startX + i, 'C');
            attroff(COLOR_PAIR(CP_COMPUTER));
        } else {
            mvaddch(y, startX + i, '_');
        }
    }
    mvaddch(y, startX + TRACK_LENGTH, ']');

    attron(COLOR_PAIR(CP_TEXT));
    mvprintw(5, 2, "Player Score: %d | Computer Score: %d", p1->score, p2->score);
    attroff(COLOR_PAIR(CP_TEXT));
    refresh();
}

int computerAnswer() {
    int chance = rand() % 100;
    if (chance < 70) return reg.result;
    int deviation[] = {-3, -2, -1, 1, 2, 3};
    return reg.result + deviation[rand() % 6];
}

void playTurn(Player *player, int isHuman) {
    reg.operand1 = rand() % 10 + 1;
    reg.operand2 = rand() % 10 + 1;
    reg.result = reg.operand1 * reg.operand2;

    mvprintw(8, 2, "%s's Turn:", player->name);
    mvprintw(9, 2, "%d x %d = ?", reg.operand1, reg.operand2);

    int answer;
    if (isHuman) {
        mvprintw(10, 2, "Your answer: ");
        echo();
        scanw("%d", &answer);
        noecho();
    } else {
        answer = computerAnswer();
        mvprintw(10, 2, "Computer answers: %d", answer);
    }

    if (answer == reg.result) {
        player->position++;
        player->score++;
        attron(COLOR_PAIR(CP_PLAYER));
        mvprintw(12, 2, "Correct! %s jumps forward.", player->name);
        attroff(COLOR_PAIR(CP_PLAYER));
    } else {
        attron(COLOR_PAIR(CP_COMPUTER));
        mvprintw(12, 2, "Wrong! No jump for %s.", player->name);
        attroff(COLOR_PAIR(CP_COMPUTER));
    }
    refresh();
}

void saveGame(Player *p1, Player *p2) {
    FILE *fp = fopen("savegame.dat", "wb");
    if (fp) {
        fwrite(p1, sizeof(Player), 1, fp);
        fwrite(p2, sizeof(Player), 1, fp);
        fclose(fp);
    }
}

void loadGame(Player *p1, Player *p2) {
    FILE *fp = fopen("savegame.dat", "rb");
    if (fp) {
        fread(p1, sizeof(Player), 1, fp);
        fread(p2, sizeof(Player), 1, fp);
        fclose(fp);
    } else {
        mvprintw(7, 2, "No save file found. Starting a new game.");
        refresh();
        initPlayers(p1, p2);
    }
}

void endGame(Player *p1, Player *p2) {
    displayTrack(p1, p2);
    attron(COLOR_PAIR(CP_TITLE));
    mvprintw(15, 2, "=== Game Over ===");
    attroff(COLOR_PAIR(CP_TITLE));
    if (p1->position >= TRACK_LENGTH)
        mvprintw(16, 2, "Congratulations! You win!");
    else
        mvprintw(16, 2, "Computer wins! Better luck next time.");
    mvprintw(18, 2, "Press any key to exit...");
    refresh();
}
