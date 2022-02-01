// Snake game by Onat Bulut 2022

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <windows.h>

#define I_RED 0x000C // FOREGROUND_RED | FOREGROUND_INTENSITY
#define I_GREEN 0x000A // FOREGROUND_GREEN | FOREGROUND_INTENSITY
#define WHITE 0x0007 // FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE

#define UP 72
#define DOWN 80
#define LEFT 75
#define RIGHT 77
#define START 32
#define EXIT 27
#define PAUSE 112

#define SPEED 7 // Determines how fast the snake moves (steps per second)
#define START_LENGTH 3 // Length of the snake at start

short int mapSize = 10; // Map size in height and width

int pCoordX, pCoordY, fCoordX, fCoordY;
char keyPress, prevKeyPress = 0;
char eaten = 1, win = 0, run = 0, pause = 0, tick = 0;
unsigned int score = 0, highestScore = 0;

COORD *tailCoords; // COORD struct array for tail coordinates

// Set cursor position and print a character
void gotoxy(int x, int y, char character)
{
    COORD CRD;
    CRD.X = x;
    CRD.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), CRD);
    printf("%c", character);
}

// Set text color
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
        if (run == 1 && pause == 0 && keyPress != -32) // Arrow keys return the value -32 before their individual values thus delaying the input, this eliminates the issue
        {
            userInput();
            food();
            tail();
            printMap();
            Sleep(1000/SPEED);
        }
    }
}

// Variable checks etc
void preRun()
{
    srand(time(NULL));

    while (1)
    {
        printf("Enter a map size (5-50): ");
        scanf_s("%hi", &mapSize);
        system("cls");

        if (mapSize <= 5 || mapSize > 50)
        {
            
            puts("Map size should be between 5 and 50!");
            continue;
        }

        break;
    }
    
    printf("Map size has been selected as %dx%d!\n", mapSize, mapSize);

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
    // Deallocate memory when the user restarts while the game is running
    if (run == 1)
        free(tailCoords);

    // Allocate memory for tailCoords COORD struct array
    if ((tailCoords = malloc((START_LENGTH + 1) * sizeof(COORD))) == NULL)
    {
        puts("Not enough memory!");
        exit(1);
    }

    // Start in the middle
    pCoordX = mapSize % 2 == 0 ? mapSize + 1: mapSize;
    pCoordY = mapSize / 2 + 2;      

    // Write head coordinates to all tailCoords COORD struct array elements
    for (size_t i = 0; i < START_LENGTH; i++)
    {
        tailCoords[i].Y = pCoordY;
        tailCoords[i].X = pCoordX;
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
    // Use previous keyPress value if pressed key is not one of those
    if (!(keyPress == UP || keyPress == DOWN || keyPress == LEFT || keyPress == RIGHT))
        keyPress = prevKeyPress;

    setColor(I_GREEN);

    switch (keyPress)
    {
    case UP: // Go up
        if (pCoordY <= 1) break;
        gotoxy(pCoordX, --pCoordY, 'O');
        break;

    case DOWN: // Go down
        if (pCoordY >= mapSize) break;
        gotoxy(pCoordX, ++pCoordY, 'O');
        break;
        
    case LEFT: // Go left
        if (pCoordX <= 1) break;
        pCoordX -= 2;
        gotoxy(pCoordX, pCoordY, 'O');
        break;

    case RIGHT: // Go right
        if (pCoordX >= mapSize * 2) break;
        pCoordX += 2;
        gotoxy(pCoordX, pCoordY, 'O');
        break;
    }

    prevKeyPress = keyPress;
}

// Spawn food on random places, control if user has eaten the food and adjust score
void food()
{
    // Spawn new food if no food is present
    if (eaten == 1)
    {
        char check = 0;

        // Check if food spawn location collides with the snake and find a new location if it does
        while (check == 0)
        {
            check = 1;
            fCoordX = rand() % mapSize * 2 + 1;
            fCoordY = rand() % mapSize + 1;

            for (size_t i = 0; i < score; i++)
                if (fCoordX == tailCoords[i].X && fCoordY == tailCoords[i].Y)
                    check = 0;
        }
        
        eaten = 0;
    }

    setColor(I_RED);
    gotoxy(fCoordX, fCoordY, '*');
    
    // Things to do when the food is eaten
    if (pCoordX == fCoordX && pCoordY == fCoordY)
    {
        setColor(I_GREEN);
        gotoxy(fCoordX, fCoordY, 'O');
        eaten = 1;

        // Increment score by 1 and allocate more memory to tailCoords COORD struct array
        if ((tailCoords = realloc(tailCoords, (++score + 1) * sizeof(COORD))) == NULL)
        {
            system("cls");
            puts("Not enough memory!");
            exit(1);
        }
    }
}

// Adjust tail length and detect collision with tail
void tail()
{
    gotoxy(tailCoords[0].X, tailCoords[0].Y, ' ');

    // Move tail coordinates to the previous array element on every move
    for (size_t i = 0; i < score; i++)
    {
        tailCoords[i].X = tailCoords[i + 1].X;
        tailCoords[i].Y = tailCoords[i + 1].Y;
    }

    // Write head coordinates to the (score - 1)th array element
    tailCoords[score > 0 ? score - 1 : 0].X = pCoordX;
    tailCoords[score > 0 ? score - 1 : 0].Y = pCoordY;

    // Tail collision check
    for (size_t i = 0; i < score - 1; i++)
        if (pCoordX == tailCoords[i].X && pCoordY == tailCoords[i].Y && !(tick < START_LENGTH))
        {
            win = 0;
            run = 0;
            highestScore = score > highestScore ? score : highestScore;
            free(tailCoords);
        }

    // Win status check
    if (score >= mapSize * mapSize)
    {
        win = 1;
        run = 0;
        highestScore = score > highestScore ? score : highestScore;
        free(tailCoords);
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
        for (size_t i = 0; i < mapSize * 2 + 1; i++)
            printf("%s", "-");
        puts("+");

        for (size_t i = 0; i < mapSize; i++)
        {
            printf("%s", "| ");
            for (size_t j = 0; j < mapSize; j++)
                printf("%c ", ' ');
            puts("|");
        }

        printf("%s", "+");
        for (size_t i = 0; i < mapSize * 2 + 1; i++)
            printf("%s", "-");
        puts("+");
    }

    gotoxy(mapSize * 2 + 4, 1, '\0'); puts("Press Spacebar to restart");
    gotoxy(mapSize * 2 + 4, 2, '\0'); puts("Press P to pause");
    gotoxy(mapSize * 2 + 4, 3, '\0'); puts("Arrow keys to move");
    gotoxy(mapSize * 2 + 4, 4, '\0'); printf("Length: %-5d Size: %dx%d\n", score, mapSize, mapSize);
    gotoxy(mapSize * 2 + 4, 5, '\0'); if (highestScore > START_LENGTH) printf("Highest Length: %-5d\n", highestScore);
    gotoxy(mapSize * 2 + 4, 6, '\0'); if (win == 0 && run == 0) puts("YOU LOST!!"); else if (win == 1 && run == 0) puts("YOU WON!!");

    gotoxy(0, mapSize + 2, '\0');

    if (tick <= START_LENGTH)
    {
        tick++;
    }
}