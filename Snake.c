// Snake game by Onat Bulut 2022

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
#define PAUSE 112

#define SIZE 10 // Map size in height and width, should be bigger than 5 (for your own good)
#define START_LENGTH 3 // Length of the snake at start, should be bigger than 1

#define I_RED 0x000C // FOREGROUND_RED | FOREGROUND_INTENSITY
#define I_GREEN 0x000A // FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define WHITE 0x0007 // FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE

int tailCoords[SIZE * SIZE][2] = { 0 };
char keyPress, prevKeyPress = 0;
int eaten = 1, win = 0, run = 0, pause = 0;
int pCoordX, pCoordY, fCoordX, fCoordY;
unsigned int score = START_LENGTH, highestScore = 0, tick = 0;

// Set cursor position and print a character
void gotoxy(int x, int y, char character)
{
    COORD CRD;
    CRD.X = x;
    CRD.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CRD);
    printf("%c", character);
}

// Set text or background color
void setColor(WORD color)
{
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}

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
        if (run == 1 && pause == 0 && keyPress != -32) // keyPress != -32 is important here because arrow keys also returns this value
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
        if (keyPress == PAUSE)
            if (pause == 0) pause = 1; else pause = 0;
        if (keyPress == EXIT) exit(0);
    }
}

// Start / restart the game
void initialization()
{
    pCoordX = SIZE % 2 == 0 ? SIZE + 1: SIZE;
    pCoordY = SIZE / 2 + 2;

    for (size_t i = 0; i < 100; i++)
        for (size_t j = 0; j < 2; j++)
            tailCoords[i][j] = SIZE + 2;

    for (size_t i = 0; i < START_LENGTH; i++)
    {
        tailCoords[i][0] = pCoordY;
        tailCoords[i][1] = pCoordX;
    }

    system("cls");

    keyPress = UP;
    prevKeyPress = UP;
    eaten = 1;
    highestScore = score > highestScore ? score : highestScore;
    score = START_LENGTH;
    tick = 0;
    run = 1;
    pause = 0;
}

// Process keyboard input
void userInput()
{
    if (!(keyPress == UP || keyPress == DOWN || keyPress == LEFT || keyPress == RIGHT || keyPress == EXIT))
        keyPress = prevKeyPress;

    setColor(I_GREEN);

    switch (keyPress)
    {
    case UP:
        if (pCoordY <= 1)
        {
            win = 0;
            highestScore = score > highestScore ? score : highestScore;
            break;
        }
        gotoxy(pCoordX, --pCoordY, 'O');
        break;

    case DOWN:
        if (pCoordY >= SIZE)
        {
            win = 0;
            highestScore = score > highestScore ? score : highestScore;
            break;
        }
        gotoxy(pCoordX, ++pCoordY, 'O');
        break;

    case LEFT:
        if (pCoordX <= 1)
        {
            win = 0;
            highestScore = score > highestScore ? score : highestScore;
            break;
        }
        pCoordX -= 2;
        gotoxy(pCoordX, pCoordY, 'O');
        break;

    case RIGHT:
        if (pCoordX >= SIZE * 2)
        {
            win = 0;
            highestScore = score > highestScore ? score : highestScore;
            break;
        }
        pCoordX += 2;
        gotoxy(pCoordX, pCoordY, 'O');
        break;
    }

    prevKeyPress = keyPress;
}

// Add food on random places, control if user has eaten the food and adjust score
void food()
{
    if (eaten == 1)
    {
        int check = 0;

        while (check == 0)
        {
            fCoordX = rand() % SIZE * 2 + 1;
            fCoordY = rand() % SIZE + 1;

            for (size_t i = 0; i < score; i++)
                if (fCoordX != tailCoords[i][1] && fCoordY != tailCoords[i][0])
                    check = 1;
        }
        
        eaten = 0;
    }

    setColor(I_RED);
    gotoxy(fCoordX, fCoordY, '*');
    
    if (pCoordX == fCoordX && pCoordY == fCoordY)
    {
        setColor(I_GREEN);
        gotoxy(fCoordX, fCoordY, 'O');
        eaten = 1;
        score++;
    }
}

// Adjust tail length and detect collision with tail
void tail()
{
    gotoxy(tailCoords[0][1], tailCoords[0][0], ' ');

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
            win = 0;
            run = 0;
            highestScore = score > highestScore ? score : highestScore;
        }

    if (score >= SIZE * SIZE)
    {
        win = 1;
        run = 0;
        highestScore = score > highestScore ? score : highestScore;
    }
}

// Print the display elements
void printMap()
{
    gotoxy(0, 0, '\0');

    setColor(WHITE);

    if (tick == 0)
    {
        printf("%s", "+");
        for (size_t i = 0; i < SIZE * 2 + 1; i++)
            printf("%s", "-");
        puts("+");

        for (size_t i = 0; i < SIZE; i++)
        {
            printf("%s", "| ");
            for (size_t j = 0; j < SIZE; j++)
                printf("%c ", ' ');
            puts("|");
        }

        printf("%s", "+");
        for (size_t i = 0; i < SIZE * 2 + 1; i++)
            printf("%s", "-");
        puts("+");
    }

    gotoxy(SIZE * 2 + 4, 1, '\0'); puts("Press Spacebar to restart");
    gotoxy(SIZE * 2 + 4, 2, '\0'); puts("Press ESC to quit");
    gotoxy(SIZE * 2 + 4, 3, '\0'); puts("Arrow keys to move");
    gotoxy(SIZE * 2 + 4, 4, '\0'); printf("Length: %-5d Size: %dx%d\n", score, SIZE, SIZE);
    gotoxy(SIZE * 2 + 4, 5, '\0'); if (highestScore > START_LENGTH) printf("Highest Length: %-5d\n", highestScore);
    gotoxy(SIZE * 2 + 4, 6, '\0'); if (win == 0 && run == 0) puts("YOU LOST!!"); else if (win == 1 && run == 0) puts("YOU WON!!");

    gotoxy(0, SIZE + 2, '\0');

    tick++;
}