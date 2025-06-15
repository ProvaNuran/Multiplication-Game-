#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
void clearScreen();
int computerAnswer();
void saveGame(Player *p1, Player *p2);
void loadGame(Player *p1, Player *p2);
void initPlayers(Player *human, Player *computer);
void playTurn(Player *player, int isHuman);
void endGame(Player *p1, Player *p2);

// Global register simulation variable
RegisterSim reg;

int main() {
    // Dynamically allocate memory for two players
    Player *human = malloc(sizeof(Player));
    Player *computer = malloc(sizeof(Player));

    int choice;
    srand((unsigned int)time(NULL));
    clearScreen();


    // Game menu
    printf("=== Welcome to Penguin Jump ===\n\n");
    printf("1. New Game\n2. Load Saved Game\nEnter choice: ");
    scanf("%d", &choice);
    getchar();// Consume newline

    // Load saved game or start new
    if (choice == 2) {
        loadGame(human, computer);
    } else {
        initPlayers(human, computer);
    }

    int turn = 0;
    // Main game loop
    while (human->position < TRACK_LENGTH && computer->position < TRACK_LENGTH) {
        displayTrack(human, computer);
         // Alternate turns
        if (turn % 2 == 0) {
            playTurn(human, 1);
        } else {
            playTurn(computer, 0);
        }
        
        // Save game after each turn
        saveGame(human, computer);
        turn++;
        printf("Press Enter to continue...\n");
        getchar();
        clearScreen();
    }

    // End game screen
    endGame(human, computer);

    free(human);
    free(computer);
    return 0;
}
// Initialize player details
void initPlayers(Player *human, Player *computer) {
    snprintf(human->name, sizeof(human->name), "You");
    human->position = human->score = 0;

    snprintf(computer->name, sizeof(computer->name), "Computer");
    computer->position = computer->score = 0;
}
// Clear screen depending on platform
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}
// Display the track and current positions of players
void displayTrack(Player *p1, Player *p2) {
    printf("\nTrack:\n");
    for (int i = 0; i < TRACK_LENGTH; i++) {
        if (i == p1->position && i == p2->position) printf("|B|");
        else if (i == p1->position) printf("|P|");
        else if (i == p2->position) printf("|C|");
        else printf("|_|");
    }
    printf("\n");
    printf("Player Score: %d | Computer Score: %d\n", p1->score, p2->score);
}
// Computer's simulated answer with 70% accuracy
int computerAnswer() {
    int chance = rand() % 100;
    if (chance < 70) return reg.result;
    int deviation[] = {-3, -2, -1, 1, 2, 3};
    return reg.result + deviation[rand() % 6];
}
// Handle player's or computer's turn
void playTurn(Player *player, int isHuman) {
    reg.operand1 = rand() % 10 + 1;
    reg.operand2 = rand() % 10 + 1;
    reg.result = reg.operand1 * reg.operand2;

    printf("\n%s's Turn:\n", player->name);
    printf("%d x %d = ?\n", reg.operand1, reg.operand2);

    int answer;
    if (isHuman) {
        printf("Your answer: ");
        scanf("%d", &answer);
        getchar();
    } else {
        answer = computerAnswer();
        printf("Computer answers: %d\n", answer);
    }

    if (answer == reg.result) {
        player->position++;
        player->score++;
        printf("Correct! %s jumps forward.\n", player->name);
    } else {
        printf("Wrong! No jump for %s.\n", player->name);
    }
}
// Save the game state to a binary file
void saveGame(Player *p1, Player *p2) {
    FILE *fp = fopen("savegame.dat", "wb");
    if (fp) {
        fwrite(p1, sizeof(Player), 1, fp);
        fwrite(p2, sizeof(Player), 1, fp);
        fclose(fp);
    }
}
// Load the game state from a binary file
void loadGame(Player *p1, Player *p2) {
    FILE *fp = fopen("savegame.dat", "rb");
    if (fp) {
        fread(p1, sizeof(Player), 1, fp);
        fread(p2, sizeof(Player), 1, fp);
        fclose(fp);
    } else {
        printf("No save file found. Starting a new game.\n");
        initPlayers(p1, p2);
    }
}
// Show game over message and winner
void endGame(Player *p1, Player *p2) {
    displayTrack(p1, p2);
    printf("\n=== Game Over ===\n");
    if (p1->position >= TRACK_LENGTH) printf("Congratulations! You win!\n");
    else printf("Computer wins! Better luck next time.\n");


}



