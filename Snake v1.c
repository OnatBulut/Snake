#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <conio.h>
#include <windows.h>

#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define START 32
#define EXIT 27

#define SIZE 20 // Map size in height and width, should be bigger than 5
#define START_LENGTH 3 // Length of the snake at start, should be bigger than 1

enum status { NRUN, RUN, LOSE, WIN };
enum status setStatus;

char location[SIZE][SIZE];
int tailCoords[SIZE * SIZE][2] = { 0 };
char keyPress, prevKeyPress = 0;
int eaten = 1;
int pCoordX, pCoordY, fCoordX, fCoordY;
unsigned int score = START_LENGTH, highestScore = 0, tick = 0;

void preRun();
void getKeyboardInput();
void initialization();
void userInput();
void tail();
void food();
void printMap();

int main()
{
    preRun();

    while (1)
    {
        getKeyboardInput();
        if (setStatus == RUN && keyPress != -32)
        {
            userInput();
            food();
            tail();
            printMap();
            Sleep(100);
        }
    }
}

// Variable checks etc
void preRun()
{
    setStatus = NRUN;
    srand(time(NULL));

    if (SIZE < 5)
    {
        puts("Map size should be bigger than 5!");
        exit(0);
    }

    if (START_LENGTH <= 1)
    {
        puts("Snake length should be bigger than 1!");
        exit(0);
    }

    puts("Press Spacebar to start!");
}

// Read which key is pressed
void getKeyboardInput()
{
    if (_kbhit())
    {
        keyPress = _getch();
        if (keyPress == START) initialization();
    }
}

// Start / restart the game
void initialization()
{
    pCoordX = rand() % SIZE;
    pCoordY = rand() % SIZE;
    int startDir = rand() % 4;

    for (size_t i = 0; i < SIZE; i++)
        for (size_t j = 0; j < SIZE; j++)
            location[i][j] = ' ';

    for (size_t i = 0; i < 100; i++)
        for (size_t j = 0; j < 2; j++)
            tailCoords[i][j] = SIZE + 1;

    for (size_t i = 0; i < START_LENGTH; i++)
    {
        tailCoords[i][0] = pCoordY;
        tailCoords[i][1] = pCoordX;
    }

    switch (startDir)
    {
    case 0:
        keyPress = UP;
        prevKeyPress = UP;
        break;

    case 1:
        keyPress = DOWN;
        prevKeyPress = DOWN;
        break;

    case 2:
        keyPress = LEFT;
        prevKeyPress = LEFT;
        break;

    case 3:
        keyPress = RIGHT;
        prevKeyPress = RIGHT;
        break;
    }

    eaten = 1;
    highestScore = score > highestScore ? score : highestScore;
    score = START_LENGTH;
    tick = 0;
    setStatus = RUN;
}

// Process keyboard input
void userInput()
{
    if (!(keyPress == UP || keyPress == DOWN || keyPress == LEFT || keyPress == RIGHT || keyPress == EXIT))
        keyPress = prevKeyPress;

    switch (keyPress)
    {
    case UP:
        if (pCoordY <= 0)
        {
            pCoordY = SIZE - 1;
            location[pCoordY][pCoordX] = 'O';
            break;
        }
        location[--pCoordY][pCoordX] = 'O';
        break;

    case DOWN:
        if (pCoordY >= SIZE - 1)
        {
            pCoordY = 0;
            location[pCoordY][pCoordX] = 'O';
            break;
        }
        location[++pCoordY][pCoordX] = 'O';
        break;

    case LEFT:
        if (pCoordX <= 0)
        {
            pCoordX = SIZE - 1;
            location[pCoordY][pCoordX] = 'O';
            break;
        }
        location[pCoordY][--pCoordX] = 'O';
        break;

    case RIGHT:
        if (pCoordX >= SIZE - 1)
        {
            pCoordX = 0;
            location[pCoordY][pCoordX] = 'O';
            break;
        }
        location[pCoordY][++pCoordX] = 'O';
        break;

    case EXIT:
        exit(0);
    }

    prevKeyPress = keyPress;
}

// Add food on random places, control if user has eaten the food and adjust score
void food()
{
    if (eaten == 1)
    {
        fCoordX = rand() % SIZE;
        fCoordY = rand() % SIZE;

        eaten--;
    }

    location[fCoordY][fCoordX] = '*';
    
    if (pCoordX == fCoordX && pCoordY == fCoordY)
    {
        location[fCoordY][fCoordX] = 'O';
        eaten++;
        score++;
    }
}

// Adjust tail length and detect collision with tail
void tail()
{
    location[tailCoords[0][0]][tailCoords[0][1]] = ' ';

    for (size_t i = 0; i < score; i++)
    {
        tailCoords[i][0] = tailCoords[i + 1][0];
        tailCoords[i][1] = tailCoords[i + 1][1];
    }

    tailCoords[score > 0 ? score - 1 : 0][0] = pCoordY;
    tailCoords[score > 0 ? score - 1 : 0][1] = pCoordX;

    for (size_t i = 0; i < score - 1; i++)
        if (pCoordX == tailCoords[i][1] && pCoordY == tailCoords[i][0] && !(tick < START_LENGTH))
        {
            setStatus = LOSE;
            highestScore = score > highestScore ? score : highestScore;
        }

    if (score >= SIZE * SIZE)
    {
        setStatus = WIN;
        highestScore = score > highestScore ? score : highestScore;
    }
}

// Print the display elements
void printMap()
{
    system("cls");

    printf("%s", " ");
    for (size_t i = 0; i < SIZE * 2 + 1; i++)
        printf("%s", "-");
    puts("");

    for (size_t i = 0; i < SIZE; i++)
    {
        printf("%s", "| ");
        for (size_t j = 0; j < SIZE; j++)
            printf("%c ", location[i][j]);
        if (i == 0) puts("|  Press Spacebar to restart");
        else if (i == 1) puts("|  Press ESC to quit");
        else if (i == 2) puts("|  Arrow keys to move");
        else if (i == 3) printf("|  Length: %-5d Size: %dx%d\n", score, SIZE, SIZE);
        else if (highestScore > START_LENGTH && i == 4) printf("|  Highest Length: %-5d\n", highestScore);
        else if (setStatus == LOSE && i == 5) puts("|  YOU LOST!!");
        else if (setStatus == WIN && i == 5) puts("|  YOU WON!!");
        else puts("|");
    }

    printf("%s", " ");
    for (size_t i = 0; i < SIZE * 2 + 1; i++)
        printf("%s", "-");
    puts("");

    tick++;
}