// This is the main file for the game logic and function
//
//test
#include "game.h"
#include "Framework\console.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>
#include <limits>
#include <algorithm>
#include <mmsystem.h>
#pragma comment(lib, "Winmm.lib")


using namespace std;

double  g_dElapsedTime;
double  g_dDeltaTime;
double monsterSPEED;
double iceSPEED;
bool    g_abKeyPressed[K_COUNT];


char map[25][80];
bool levelChange = false;

// Game specific variables here
SGameChar   g_sChar;
EGAMESTATES g_eGameState = S_SPLASHSCREEN;
ELEVELS g_currentlevel;
double  g_dBounceTime; // this is to prevent key bouncing, so we won't trigger keypresses more than once
WORD charColor = 0x02; // initialise character colour

//monsters
COORD monONE;		
COORD monTWO;		
COORD monTHREE;		
COORD monBOSS;


int puzzle1Integer1 = rand() % 10 + 1;
int puzzle1Integer2 = rand() % 10 + 1;
int puzzle1Ans;
int puzzle1Input;
P1_NUMBER digit1 = NUM_NIL;
P1_NUMBER digit2 = NUM_NIL;
int d1 = NULL;
int d2 = NULL;

SLIDE_DIRECTION directionICE = D_NONE;

// Console object
Console g_Console(80, 25, "Monuzzled");


//--------------------------------------------------------------
// Purpose  : Initialisation function
//            Initialize variables, allocate memory, load data from file, etc. 
//            This is called once before entering into your main loop
// Input    : void
// Output   : void
//--------------------------------------------------------------
void init( void )
{
	//seed for randomiser
	srand((int)time(NULL));

    // Set precision for floating point output
    g_dElapsedTime = 0.0;
    g_dBounceTime = 0.0;
	monsterSPEED = 0.0;
	iceSPEED = 0.0;

    // sets the initial state for the game
    g_eGameState = S_SPLASHSCREEN;


    g_sChar.m_cLocation.X = g_Console.getConsoleSize().X / 2;
    g_sChar.m_cLocation.Y = g_Console.getConsoleSize().Y / 2 + 1;
    g_sChar.m_hasKey = false;

	puzzle1Integer1 = rand() % 9 + 1;
	puzzle1Integer2 = rand() % 9 + 1;

	monsterLocation();

    // sets the width, height and the font name to use in the console
    g_Console.setConsoleFont(0, 16, L"Consolas");
	PlaySound(TEXT("mainmenu.wav"), NULL, SND_ASYNC | SND_LOOP);
}

//--------------------------------------------------------------
// Purpose  : Reset before exiting the program
//            Do your clean up of memory here
//            This is called once just before the game exits
// Input    : Void
// Output   : void
//--------------------------------------------------------------
void shutdown( void )
{
    // Reset to white text on black background
    colour(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

    g_Console.clearBuffer();
}

//--------------------------------------------------------------
// Purpose  : Getting all the key press states
//            This function checks if any key had been pressed since the last time we checked
//            If a key is pressed, the value for that particular key will be true
//
//            Add more keys to the enum in game.h if you need to detect more keys
//            To get other VK key defines, right click on the VK define (e.g. VK_UP) and choose "Go To Definition" 
//            For Alphanumeric keys, the values are their ascii values (uppercase).
// Input    : Void
// Output   : void
//--------------------------------------------------------------
void getInput( void )
{    
    g_abKeyPressed[K_UP]     = isKeyPressed(VK_UP);
    g_abKeyPressed[K_DOWN]   = isKeyPressed(VK_DOWN);
    g_abKeyPressed[K_LEFT]   = isKeyPressed(VK_LEFT);
    g_abKeyPressed[K_RIGHT]  = isKeyPressed(VK_RIGHT);
    g_abKeyPressed[K_SPACE]  = isKeyPressed(VK_SPACE);
    g_abKeyPressed[K_ESCAPE] = isKeyPressed(VK_ESCAPE);
	g_abKeyPressed[K_ONE] = isKeyPressed(49);
	g_abKeyPressed[K_TWO] = isKeyPressed(50);
	g_abKeyPressed[K_THREE] = isKeyPressed(51);
	g_abKeyPressed[K_FOUR] = isKeyPressed(52);
	g_abKeyPressed[K_FIVE] = isKeyPressed(53);
	g_abKeyPressed[K_SIX] = isKeyPressed(54);
	g_abKeyPressed[K_SEVEN] = isKeyPressed(55);
	g_abKeyPressed[K_EIGHT] = isKeyPressed(56);
	g_abKeyPressed[K_NINE] = isKeyPressed(57);
	g_abKeyPressed[K_ZERO] = isKeyPressed(48);
	g_abKeyPressed[K_ENTER] = isKeyPressed(VK_RETURN);
	g_abKeyPressed[K_BACKSPACE] = isKeyPressed(VK_BACK);
}

//--------------------------------------------------------------
// Purpose  : Update function
//            This is the update function
//            double dt - This is the amount of time in seconds since the previous call was made
//
//            Game logic should be done here.
//            Such as collision checks, determining the position of your game characters, status updates, etc
//            If there are any calls to write to the console here, then you are doing it wrong.
//
//            If your game has multiple states, you should determine the current state, and call the relevant function here.
//
// Input    : dt = deltatime
// Output   : void
//--------------------------------------------------------------
void update(double dt)
{
    // get the delta time
    g_dElapsedTime += dt;
    g_dDeltaTime = dt;

    switch (g_eGameState)
    {
        case S_SPLASHSCREEN : splashScreenSelect(); // game logic for the splash screen
            break;
        case S_GAME: gameplay(); // gameplay logic when we are in the game
            break;
		case S_LEVELSELECT: LevelScreenSelect(); // game logic for the level select screen
			break;
		case S_CHARACTERCOLOR: charactercolourselect();
			break;
		case S_ENCOUNTERMONSTER: monsterPuzzle();
			break;
		case S_WINSCREEN: winscreen();
			break;
    }
}
//--------------------------------------------------------------
// Purpose  : Render function is to update the console screen
//            At this point, you should know exactly what to draw onto the screen.
//            Just draw it!
//            To get an idea of the values for colours, look at console.h and the URL listed there
// Input    : void
// Output   : void
//--------------------------------------------------------------
void render()
{
    clearScreen();      // clears the current screen and draw from scratch 
    switch (g_eGameState)
    {
        case S_SPLASHSCREEN: renderSplashScreen();
            break;
        case S_GAME: renderGame();
            break;
		case S_LEVELSELECT: renderLevelSelect();
			break;
		case S_CHARACTERCOLOR: renderCharacterColour();
			break;
		case S_ENCOUNTERMONSTER: renderEncounterMonster();
			break;
		case S_WINSCREEN: winscreenRender();
			break;
    }
    renderFramerate();  // renders debug information, frame rate, elapsed time, etc
    renderToScreen();   // dump the contents of the buffer to the screen, one frame worth of game
}

void splashScreenWait() {
	//if (g_dElapsedTime > 3.0) // wait for 3 seconds to switch to game mode, else do nothing
	// g_eGameState = S_GAME;
}

void resetPos() {
	g_sChar.m_cLocation.X = g_Console.getConsoleSize().X / 2;
	g_sChar.m_cLocation.Y = g_Console.getConsoleSize().Y / 2 + 1;
}

void splashScreenSelect()    // waits for time to pass in splash screen
{
	bool bSomethingHappened = false;
	if (g_dBounceTime > g_dElapsedTime)
		return;

	if (g_abKeyPressed[K_ONE])
	{
		g_eGameState = S_LEVELSELECT;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_TWO])
	{
		g_eGameState = S_CHARACTERCOLOR;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_ESCAPE])
	{
		g_bQuitGame = true;
	}
	if (bSomethingHappened)
	{
		// set the bounce time to some time in the future to prevent accidental triggers
		g_dBounceTime = g_dElapsedTime + 0.25;
	}
}

void LevelScreenSelect() // LOGIC FOR KEY PRESS in level select
{
	bool bSomethingHappened = false;
	if (g_dBounceTime > g_dElapsedTime)
		return;
	if (g_abKeyPressed[K_ONE])
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELONE;
		monsterLocation();
		levelChange = true;
		bSomethingHappened = true;
		PlaySound(TEXT("levels1-3.wav"), NULL, SND_ASYNC | SND_LOOP);
		system("0");
	}
	if (g_abKeyPressed[K_TWO])
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELTWO;
		monsterLocation();
		levelChange = true;
		bSomethingHappened = true;
		PlaySound(TEXT("levels1-3.wav"), NULL, SND_ASYNC | SND_LOOP);
		system("0");
	}
	if (g_abKeyPressed[K_THREE])
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELTHREE;
		monsterLocation();
		levelChange = true;
		bSomethingHappened = true;
		PlaySound(TEXT("levels1-3.wav"), NULL, SND_ASYNC | SND_LOOP);
		system("0");
	}
	if (g_abKeyPressed[K_FOUR])
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELFOUR;
		monsterLocation();
		levelChange = true;
		bSomethingHappened = true;
		PlaySound(TEXT("levels4-5.wav"), NULL, SND_ASYNC | SND_LOOP);
		system("0");
	}
	if (g_abKeyPressed[K_FIVE])
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELFIVE;
		monsterLocation();
		levelChange = true;
		bSomethingHappened = true;
		PlaySound(TEXT("levels4-5.wav"), NULL, SND_ASYNC | SND_LOOP);
		system("0");
	}
	if (g_abKeyPressed[K_ESCAPE])
	{
		g_eGameState = S_SPLASHSCREEN;
		bSomethingHappened = true;
	}
	if (bSomethingHappened)
	{
		// set the bounce time to some time in the future to prevent accidental triggers
		g_dBounceTime = g_dElapsedTime + 0.25;
	}
}

void charactercolourselect() {
	bool bSomethingHappened = false;
	if (g_dBounceTime > g_dElapsedTime)
		return;

	if (g_abKeyPressed[K_ONE])
	{
		charColor = 0x08;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_TWO])
	{
		charColor = 0x02;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_THREE])
	{
		charColor = 0x03;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_FOUR])
	{
		charColor = 0x04;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_FIVE])
	{
		charColor = 0x05;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_ESCAPE])
	{
		g_eGameState = S_SPLASHSCREEN;
		bSomethingHappened = true;
	}
	if (bSomethingHappened)
	{
		// set the bounce time to some time in the future to prevent accidental triggers
		g_dBounceTime = g_dElapsedTime + 0.25;
	}
}

void winscreen() {
	init();
}

void gameplay()            // gameplay logic
{
	processUserInput(); // checks if you should change states or do something else with the game, e.g. pause, exit
	loadMap();			// loads map into memory

	switch (directionICE)
	{
	case D_NONE:
		moveCharacterICE();		// input for ice movement
		moveCharacter(); 	    // moves the character, collision detection, physics, etc
		break;

	default: slideICE();		// logic for ice movement
		break;
	}


	monsterAI();       // moves the monster to the player's character's location
}

bool isSolid(int x, int y) {
	switch (map[y][x]) {
		case WALL: 
		case ICE:
			return true;
			break;
		case GATE:
			if (!g_sChar.m_hasKey) {
				return true;
			}
			else {
				return false;
			}
			break;
		default:
			return false;
			break;
		
	}
}

void moveCharacter()
{
	bool bSomethingHappened = false;
	if (g_dBounceTime > g_dElapsedTime)
		return;

	COORD target;
	bool keyPressed = false;
	if (g_abKeyPressed[K_UP]) {
		target.X = g_sChar.m_cLocation.X;
		target.Y = g_sChar.m_cLocation.Y - 1;
		keyPressed = true;
	}
	if (g_abKeyPressed[K_LEFT]) {
		target.X = g_sChar.m_cLocation.X - 1;
		target.Y = g_sChar.m_cLocation.Y;
		keyPressed = true;
	}
	if (g_abKeyPressed[K_DOWN]) {
		target.X = g_sChar.m_cLocation.X;
		target.Y = g_sChar.m_cLocation.Y + 1;
		keyPressed = true;
	}
	if (g_abKeyPressed[K_RIGHT]) {
		target.X = g_sChar.m_cLocation.X + 1;
		target.Y = g_sChar.m_cLocation.Y;
		keyPressed = true;
	}
	if (g_abKeyPressed[K_UP] && g_abKeyPressed[K_RIGHT])
	{
		target.X = g_sChar.m_cLocation.X + 1;
		target.Y = g_sChar.m_cLocation.Y - 1;
		keyPressed = true;
	}
	if (g_abKeyPressed[K_UP] && g_abKeyPressed[K_LEFT])
	{
		target.X = g_sChar.m_cLocation.X - 1;
		target.Y = g_sChar.m_cLocation.Y - 1;
		keyPressed = true;
	}
	if (g_abKeyPressed[K_DOWN] && g_abKeyPressed[K_RIGHT])
	{
		target.X = g_sChar.m_cLocation.X + 1;
		target.Y = g_sChar.m_cLocation.Y + 1;
		keyPressed = true;
	}
	if (g_abKeyPressed[K_DOWN] && g_abKeyPressed[K_LEFT])
	{
		target.X = g_sChar.m_cLocation.X - 1;
		target.Y = g_sChar.m_cLocation.Y + 1;
		keyPressed = true;
	}
	
	
	if (keyPressed && !isSolid(target.X, target.Y)) {
		g_sChar.m_cLocation.X = target.X;
		g_sChar.m_cLocation.Y = target.Y;
		bSomethingHappened = true;
	}

	//// Updating the location of the character based on the key press
	//// providing a beep sound whenver we shift the character
	//if (g_abKeyPressed[K_UP] && g_sChar.m_cLocation.Y > 0 && !isSolid(g_sChar.m_cLocation.X))
	//{	
	//	//Beep(1440, 30);
	//	g_sChar.m_cLocation.Y--;
	//	bSomethingHappened = true;
	//}
	//if (g_abKeyPressed[K_LEFT] && g_sChar.m_cLocation.X > 0 && (map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X - 1]) != WALL && (map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X - 1]) != ICE)
	//{
	//	//Beep(1440, 30);
	//	g_sChar.m_cLocation.X--;
	//	bSomethingHappened = true;
	//}
	//if (g_abKeyPressed[K_DOWN] && g_sChar.m_cLocation.Y < g_Console.getConsoleSize().Y - 1 && (map[g_sChar.m_cLocation.Y + 1][g_sChar.m_cLocation.X]) != WALL && (map[g_sChar.m_cLocation.Y + 1][g_sChar.m_cLocation.X]) != ICE)
	//{
	//	//Beep(1440, 30);
	//	g_sChar.m_cLocation.Y++;
	//	bSomethingHappened = true;
	//}
	//if (g_abKeyPressed[K_RIGHT] && g_sChar.m_cLocation.X < g_Console.getConsoleSize().X - 1 && (map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X + 1]) != WALL && (map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X + 1]) != ICE)
	//{
	//	//Beep(1440, 30);
	//	g_sChar.m_cLocation.X++;
	//	bSomethingHappened = true;
	//}
	//
	// ice sliding logic ; if you walk on ice, you're on a loop until you hit a wall/gate.


	//every time elapsed > speed, sequence triggers


	if (bSomethingHappened)
	{
		// set the bounce time to some time in the future to prevent accidental triggers
		g_dBounceTime = g_dElapsedTime + 0.08;
	}

}

void moveCharacterICE()
{
	bool bSomethingHappened = false;
	if (g_dBounceTime > g_dElapsedTime)
		return;


	if (g_abKeyPressed[K_UP] && (map[g_sChar.m_cLocation.Y - 1][g_sChar.m_cLocation.X]) == ICE)
	{
		g_sChar.m_cLocation.Y--;
		directionICE = D_UP;
		bSomethingHappened = true;
	}
	else if (g_abKeyPressed[K_DOWN] && (map[g_sChar.m_cLocation.Y + 1][g_sChar.m_cLocation.X]) == ICE)
	{
		g_sChar.m_cLocation.Y++;
		directionICE = D_DOWN;
		bSomethingHappened = true;
	}
	else if (g_abKeyPressed[K_LEFT] && (map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X - 1]) == ICE)
	{
		g_sChar.m_cLocation.X--;
		directionICE = D_LEFT;
		bSomethingHappened = true;
	}
	else if (g_abKeyPressed[K_RIGHT] && (map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X + 1]) == ICE)
	{
		g_sChar.m_cLocation.X++;
		directionICE = D_RIGHT;
		bSomethingHappened = true;
	}

	if (bSomethingHappened)
	{
		// set the bounce time to some time in the future to prevent accidental triggers
		g_dBounceTime = g_dElapsedTime + 0.25;
	}
}

void slideICE()
{

	if (iceSPEED < g_dElapsedTime)
	{
		switch (directionICE)
		{
		case D_UP:

			if ((map[g_sChar.m_cLocation.Y - 1][g_sChar.m_cLocation.X]) != WALL)
			{
				g_sChar.m_cLocation.Y--;
				if ((map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X]) == BLANK_SPACE)
				{
					directionICE = D_NONE;
					break;
				}
				break;
			}
			else
			{
				directionICE = D_NONE;
				break;
			}

		case D_DOWN:

			if ((map[g_sChar.m_cLocation.Y + 1][g_sChar.m_cLocation.X]) != WALL)
			{
				g_sChar.m_cLocation.Y++;
				if ((map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X]) == BLANK_SPACE)
				{
					directionICE = D_NONE;
					break;
				}
				break;
			}
			else
			{
				directionICE = D_NONE;
				break;
			}

		case D_LEFT:

			if ((map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X - 1]) != WALL)
			{
				g_sChar.m_cLocation.X--;
				if ((map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X]) == BLANK_SPACE)
				{
					directionICE = D_NONE;
					break;
				}
				break;
			}
			else
			{
				directionICE = D_NONE;
				break;
			}

		case D_RIGHT:

			if ((map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X + 1]) != WALL)
			{
				g_sChar.m_cLocation.X++;
				if ((map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X]) == BLANK_SPACE)
				{
					directionICE = D_NONE;
					break;
				}
				break;
			}
			else
			{
				directionICE = D_NONE;
				break;
			}
		default:
			directionICE = D_NONE;
			break;
		}
		iceSPEED = g_dElapsedTime + 0.05;
	}


}

void processUserInput()
{
	bool bSomethingHappened = false;
	if (g_dBounceTime > g_dElapsedTime) {
		return;
	}

    // exits back to level select if escape is pressed
	if (g_abKeyPressed[K_ESCAPE])
	{
		init();
		monsterLocation();
		bSomethingHappened = true;
	}
	if (map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X] == ENDPOINT) {
		switch (g_currentlevel) {
			case L_LEVELONE:
				g_currentlevel = L_LEVELTWO;
				levelChange = true;
				resetPos();
				monsterLocation();
				bSomethingHappened = true;
				break;
			case L_LEVELTWO:
				g_currentlevel = L_LEVELTHREE;
				levelChange = true;
				resetPos();
				monsterLocation();
				bSomethingHappened = true;
				break;
			case L_LEVELTHREE:
				g_currentlevel = L_LEVELFOUR;
				levelChange = true;
				resetPos();
				monsterLocation();
				bSomethingHappened = true;
				break;
			case L_LEVELFOUR:
				g_currentlevel = L_LEVELFIVE;
				levelChange = true;
				resetPos();
				monsterLocation();
				bSomethingHappened = true;
				break;
			case L_LEVELFIVE:
				g_eGameState = S_WINSCREEN;
				levelChange = true;
				monsterLocation();
				bSomethingHappened = true;
				break;

		}
	}

	if (map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X] == KEY) {
		g_sChar.m_hasKey = true;
		map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X] = BLANK_SPACE;
	}

	if (map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X] == GATE) {
		map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X] = BLANK_SPACE;
	}

	if ((monONE.X == g_sChar.m_cLocation.X && monONE.Y == g_sChar.m_cLocation.Y) || (monTWO.X == g_sChar.m_cLocation.X && monTWO.Y == g_sChar.m_cLocation.Y))	// When monster touches player
	{
		g_eGameState = S_ENCOUNTERMONSTER;
	}

	if (bSomethingHappened)
    {
        // set the bounce time to some time in the future to prevent accidental triggers
        g_dBounceTime = g_dElapsedTime + 0.25; 
    }

}

void clearScreen()
{
    // Clears the buffer with this colour attribute
    g_Console.clearBuffer(0x0F);
}

void renderSplashScreen()  // renders the splash screen
{
    COORD c = g_Console.getConsoleSize();
    c.Y /= 2;
    c.X = c.X / 2 - 8;
    g_Console.writeToBuffer(c, "1. Level Select", 0x06);
    c.Y += 2;
    c.X = g_Console.getConsoleSize().X / 2 - 13;
    g_Console.writeToBuffer(c, "2. Change Character Colour", 0x06);
    c.Y += 3;
    c.X = g_Console.getConsoleSize().X / 2 - 9;
    g_Console.writeToBuffer(c, "<Press Esc to Quit>", 0x02);
	c.X = g_Console.getConsoleSize().X / 2 - 33;
	c.Y = 2;
	g_Console.writeToBuffer(c, " ___ ___   ___   ____   __ __  _____  _____  _        ___  ___   ", 0x02);
	c.Y += 1;
	g_Console.writeToBuffer(c, "|   |   | /   \\ |    \\ |  |  ||     ||     || |      /  _]|   \\  ", 0x02);
	c.Y += 1;
	g_Console.writeToBuffer(c, "| _   _ ||     ||  _  ||  |  ||__/  ||__/  || |     /  [_ |    \\ ", 0x02);
	c.Y += 1;
	g_Console.writeToBuffer(c, "|  \\_/  ||  O  ||  |  ||  |  ||   __||   __|| |___ |    _]|  D  |", 0x02);
	c.Y += 1;
	g_Console.writeToBuffer(c, "|   |   ||     ||  |  ||  :  ||  /  ||  /  ||     ||   [_ |     |", 0x02);
	c.Y += 1;
	g_Console.writeToBuffer(c, "|   |   ||     ||  |  ||     ||     ||     ||     ||     ||     |", 0x02);
	c.Y += 1;
	g_Console.writeToBuffer(c, "|___|___| \\___/ |__|__| \\__,_||_____||_____||_____||_____||_____|", 0x02);
}

void winscreenRender() {
	//TODO
}

void renderGame()
{
    renderMap();        // renders the map to the buffer first
    renderCharacter();  // renders the character into the buffer
	monsterALL();		// renders the monsters into the buffer
}

void renderLevelSelect()
{
	renderLevelSelectBG();        // renders the map to the buffer first
}

void renderCharacterColour() {
	COORD c = g_Console.getConsoleSize();
	c.Y = 5;
	c.X = c.X / 2 - 9;
	g_Console.writeToBuffer(c, "1. WHITE", 0x08);
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	g_Console.writeToBuffer(c, "2. GREEN", 0x02);
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	g_Console.writeToBuffer(c, "3. BLUE", 0x03);
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	g_Console.writeToBuffer(c, "4. RED", 0x04);
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	g_Console.writeToBuffer(c, "5. PINK", 0x05);
	c.Y += 3;
	c.X = g_Console.getConsoleSize().X / 2 - 13;
	g_Console.writeToBuffer(c, "<Press Esc to go back>", 0x06);
	c.Y = g_Console.getConsoleSize().Y / 2;
	c.X = 2;
	g_Console.writeToBuffer(c, "Current Colour", charColor);
}

void renderLevelSelectBG()
{
	COORD c = g_Console.getConsoleSize();
	c.Y = 5;
	c.X = c.X / 2 - 9;
	g_Console.writeToBuffer(c, "1. Level One", 0x06);
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	g_Console.writeToBuffer(c, "2. Level Two", 0x06);
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	g_Console.writeToBuffer(c, "3. Level Three", 0x06);
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	g_Console.writeToBuffer(c, "4. Level Four", 0x06);
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	g_Console.writeToBuffer(c, "5. Level Five", 0x06);
	c.Y += 3;
	c.X = g_Console.getConsoleSize().X / 2 - 13;
	g_Console.writeToBuffer(c, "<Press Esc to go back>", 0x06);
}

void renderMap()
{
	for (int r = 0; r < 25; r++) {
		for (int c = 0; c < 80; c++) {
			switch (map[r][c]) {
				case WALL: g_Console.writeToBuffer(c, r, (char)219, 0x07);
					break;
				case ENDPOINT: g_Console.writeToBuffer(c, r, (char)219, 0x05);
					break;
				case ICE: g_Console.writeToBuffer(c, r, (char)176, 0x07);
					break;
				case KEY: g_Console.writeToBuffer(c, r, (char)229, 0x0A);
					break;
				case GATE: g_Console.writeToBuffer(c, r, (char)219, 0x0A);
					break;

			}
		}
	}
}

void loadMap() {

	if (levelChange) {
		ifstream file;
		string row;
		switch (g_currentlevel) {
		case L_LEVELONE:
			file.open("levels/level1.txt");
			break;
		case L_LEVELTWO:
			file.open("levels/level2.txt");
			break;
		case L_LEVELTHREE:
			file.open("levels/level3.txt");
			break;
		case L_LEVELFOUR:
			file.open("levels/level4.txt");
			break;
		case L_LEVELFIVE:
			file.open("levels/level5.txt");
			break;

		}

		for (int i = 0; i < 25; i++) {
			getline(file, row);
			for (int j = 0; j < 80; j++) {
				map[i][j] = row[j];
			}
		}
		file.close();
		levelChange = false;
	}
}

void renderCharacter()
{
    // Draw the location of the character
    g_Console.writeToBuffer(g_sChar.m_cLocation, (char)1, charColor);
}

void renderFramerate()
{
    COORD c;
    // displays the framerate
    std::ostringstream ss;
    ss << std::fixed << std::setprecision(3);
    ss << 1.0 / g_dDeltaTime << "fps";
    c.X = g_Console.getConsoleSize().X - 9;
    c.Y = 0;
    g_Console.writeToBuffer(c, ss.str());

    // displays the elapsed time
    ss.str("");
    ss << g_dElapsedTime << "secs";
    c.X = 0;
    c.Y = 0;
    g_Console.writeToBuffer(c, ss.str(), 0x59);
}
void renderToScreen()
{
    // Writes the buffer to the console, hence you will see what you have written
    g_Console.flushBufferToConsole();
}





/*
void monsterONE()
{
	MON1_MOV movANI;
    switch (movANI)
	{
	case MON1_DOWN :
		if (monONE.Y--)
		{
			g_Console.writeToBuffer(monONE, " (_^q^_) ", 0x0C);
		}
		else 
		{
			g_Console.writeToBuffer(monONE, "(^q^)", 0x0C);
		}
	case MON1_UP :
		if (monONE.Y++) 
		{
			g_Console.writeToBuffer(monONE, " (-^q^-) ", 0x0C);
		}
		else
		{
			g_Console.writeToBuffer(monONE, "(^q^)", 0x0C);
		}
	case MON1_LEFT:
		if (monONE.X--)
		{
			g_Console.writeToBuffer(monONE, " (_^p^)_ ", 0x0C);
		}
		else 
		{
			g_Console.writeToBuffer(monONE, " (^q^) ", 0x0C);
		}
	case MON1_RIGHT:
		if (monONE.X++) 
		{
			g_Console.writeToBuffer(monONE, " _(^q^_) ", 0x0c);
		}
		else 
		{
			g_Console.writeToBuffer(monONE, "(^q^)", 0x0C);
		}
		}
	
	}
	*/

void monsterALL()				// how the monster is seen in game
{
	g_Console.writeToBuffer(monONE, "M", 0x0C);
	g_Console.writeToBuffer(monTWO, "M", 0x0C);
	g_Console.writeToBuffer(monTHREE, "M", 0x0C);
	g_Console.writeToBuffer(monBOSS, "@M@", 0x0C);
}

void monsterLocation()			// logic for the monsters location
{
	if (g_currentlevel == L_LEVELONE) {
		monONE.X = NULL;
		monONE.Y = NULL;
		monTWO.X = (g_Console.getConsoleSize().X / 2 + 36);
		monTWO.Y = (g_Console.getConsoleSize().Y / 2 + 10);
		monTHREE.X = NULL;
		monTHREE.Y = NULL;
		monBOSS.X = NULL;
		monBOSS.Y = NULL;
	}
	else if (g_currentlevel == L_LEVELTWO)
	{
		monONE.X = (g_Console.getConsoleSize().X / 2 - 33);
		monONE.Y = (g_Console.getConsoleSize().Y / 2 - 5);
		monTWO.X = (g_Console.getConsoleSize().X / 2 + 26);
		monTWO.Y = (g_Console.getConsoleSize().Y / 2 + 8);
		monTHREE.X = NULL;
		monTHREE.Y = NULL;
		monBOSS.X = NULL;
		monBOSS.Y = NULL;
	}
	else if (g_currentlevel == L_LEVELTHREE)
	{
		monONE.X = (g_Console.getConsoleSize().X / 2 - 35);
		monONE.Y = (g_Console.getConsoleSize().Y / 2 + 7);
		monTWO.X = (g_Console.getConsoleSize().X / 2 + 31);
		monTWO.Y = (g_Console.getConsoleSize().Y / 2 + 10);
		monTHREE.X = (g_Console.getConsoleSize().X / 2 + 8);
		monTHREE.Y = (g_Console.getConsoleSize().Y / 2 - 1);
		monBOSS.X = NULL;
		monBOSS.Y = NULL;
	}
	else if (g_currentlevel == L_LEVELFOUR)
	{
		monONE.X = (g_Console.getConsoleSize().X / 2 - 33);
		monONE.Y = (g_Console.getConsoleSize().Y / 2 - 5);
		monTWO.X = (g_Console.getConsoleSize().X / 2 + 26);
		monTWO.Y = (g_Console.getConsoleSize().Y / 2 + 8);
		monTHREE.X = (g_Console.getConsoleSize().X / 2 + 36);
		monTHREE.Y = (g_Console.getConsoleSize().Y / 2 - 7);
		monBOSS.X = NULL;
		monBOSS.Y = NULL;
	}
	else if (g_currentlevel == L_LEVELFIVE)
	{
		monONE.X = NULL;
		monONE.Y = NULL;
		monTWO.X = NULL;
		monTWO.Y = NULL;
		monTHREE.X = NULL;
		monTHREE.Y = NULL;
		monBOSS.X = (g_Console.getConsoleSize().X / 2 - 5);
		monBOSS.Y = (g_Console.getConsoleSize().Y / 2 );
	}
}

void monsterAI()				// base code for how monster  acts
{
	MON_IDLE currentMOV;
	currentMOV = monsterIDLEMOV();
	
	MON_NO MONS_NUM;
	MONS_NUM = monsterTABLE();
	COORD MON_ALL;

	COORD m_target;
	

switch (MONS_NUM) {
	case monsONE:
	MON_ALL.X = monONE.X;
	MON_ALL.Y = monONE.Y;
		break;
	case monsTWO:
	MON_ALL.X = monTWO.X;
	MON_ALL.Y = monTWO.Y;
		break;
	case monsTHREE:
	MON_ALL.X = monTHREE.X;
	MON_ALL.Y = monTHREE.Y;
	break;
	}

	if (monsterSPEED < g_dElapsedTime)
	{
		monsterCHASE();
		//Monsters roaming movements
			switch (currentMOV) {
			case MON_NOTHING:
				monsterCHASE();
				break;
			case MON_UP:
				if ((MON_ALL.Y > 0) && (map[MON_ALL.Y - 1][MON_ALL.X]) == BLANK_SPACE)
				{
					MON_ALL.Y--;

					break;
				}
				else
				{
					break;
				}
			case MON_DOWN:
				if ((MON_ALL.Y < g_Console.getConsoleSize().Y - 1) && (map[MON_ALL.Y + 1][MON_ALL.X]) == BLANK_SPACE)
				{
					MON_ALL.Y++;

					break;
				}
				else
				{

					break;
				}
			case MON_LEFT:
				if ((MON_ALL.X > 0) && (map[MON_ALL.Y][MON_ALL.X - 1]) == BLANK_SPACE)
				{
					MON_ALL.X--;

					break;
				}
				else
				{

					break;
				}
			case MON_RIGHT:
				if ((MON_ALL.X < (g_Console.getConsoleSize().X + 79)) && (map[MON_ALL.Y][MON_ALL.X + 1]) == BLANK_SPACE)
				{
					MON_ALL.X++;

					break;
				}
				else
				{

					break;
				}
			default:
				break;
			}
		
		
	
			if (MONS_NUM == monsONE)
			{
				monONE.X = MON_ALL.X;
				monONE.Y = MON_ALL.Y;

			}
			else if (MONS_NUM == monsTWO)
			{
				monTWO.X = MON_ALL.X;
				monTWO.Y = MON_ALL.Y;

			}
			else if (MONS_NUM == monsTHREE)
			{
				monTHREE.X = MON_ALL.X;
				monTHREE.Y = MON_ALL.Y;

			}
			else
			{

				return;
			}
	// monster speed
	monsterSPEED = g_dElapsedTime + 0.06;

	}
}

void monsterCHASE()						// sub-function for monster to chase player
{
	MON_NO MONS_NUM;
	MONS_NUM = monsterTABLE();
	
	if (monsterSPEED < g_dElapsedTime)
	{
		/* Monster 1 */
		if ((monONE.X >= (g_sChar.m_cLocation.X - MON_DETECT_RANGE_X)) && (monONE.X <= (g_sChar.m_cLocation.X + MON_DETECT_RANGE_X)) && (monONE.Y <= (g_sChar.m_cLocation.Y + MON_DETECT_RANGE_Y)) && (monONE.Y >= (g_sChar.m_cLocation.Y - MON_DETECT_RANGE_Y)))			//for the LEFT, RIGHT, TOP and BOTTOM side detection of the monster
		{
			if (monONE.X <= g_sChar.m_cLocation.X && monONE.X != g_sChar.m_cLocation.X && (map[monONE.Y][monONE.X + 1]) == BLANK_SPACE)
			{
				if ((map[monONE.Y][monONE.X + 1]) == WALL)		// allows monster to somewhat walk around WALLS
				{
					if ((map[monONE.Y + 1][monONE.X]) == WALL)
					{
						if ((map[monONE.Y][monONE.X - 1]) == WALL)
						{
							if ((map[monONE.Y - 1][monONE.X]) == BLANK_SPACE)
							{
								monONE.Y--;

							}
							else
							{

							}
						}
						else
						{
							monONE.X--;

						}
					}
					else
					{
						monONE.Y++;

					}
				}
				else
				{
					monONE.X++;

				}
			}
			if (monONE.X >= g_sChar.m_cLocation.X && monONE.X != g_sChar.m_cLocation.X && (map[monONE.Y][monONE.X - 1]) == BLANK_SPACE)
			{
				if ((map[monONE.Y][monONE.X - 1]) == WALL)		// allows monster to somewhat walk around WALLS
				{
					if ((map[monONE.Y - 1][monONE.X]) == WALL)
					{
						if ((map[monONE.Y + 1][monONE.X]) == WALL)
						{
							if ((map[monONE.Y][monONE.X + 1]) == BLANK_SPACE)
							{
								monONE.X++;

							}
							else
							{

							}
						}
						else
						{
							monONE.Y++;

						}
					}
					else
					{
						monONE.Y--;

					}
				}
				else
				{
					monONE.X--;

				}
			}
			if (monONE.Y >= g_sChar.m_cLocation.Y && monONE.Y != g_sChar.m_cLocation.Y && (map[monONE.Y - 1][monONE.X]) == BLANK_SPACE)
			{
				if ((map[monONE.Y - 1][monONE.X]) == WALL)		// allows monster to somewhat walk around WALLS
				{
					if ((map[monONE.Y][monONE.X + 1]) == WALL)
					{
						if ((map[monONE.Y][monONE.X - 1]) == WALL)
						{
							if ((map[monONE.Y + 1][monONE.X]) == BLANK_SPACE)
							{
								monONE.Y++;

							}
							else
							{

							}
						}
						else
						{
							monONE.X--;

						}
					}
					else
					{
						monONE.X++;

					}
				}
				else
				{
					monONE.Y--;

				}
			}
			if (monONE.Y <= g_sChar.m_cLocation.Y && monONE.Y != g_sChar.m_cLocation.Y && (map[monONE.Y + 1][monONE.X]) == BLANK_SPACE)
			{
				if ((map[monONE.Y + 1][monONE.X]) == WALL)		// allows monster to somewhat walk around WALLS
				{
					if ((map[monONE.Y][monONE.X - 1]) == WALL)
					{
						if ((map[monONE.Y][monONE.X + 1]) == WALL)
						{
							if ((map[monONE.Y - 1][monONE.X]) == BLANK_SPACE)
							{
								monONE.Y--;

							}
							else
							{

							}
						}
						else
						{
							monONE.X++;

						}
					}
					else
					{
						monONE.X--;

					}
				}
				else
				{
					monONE.Y++;

				}
			}
		}
		/* Monster 2 */
		if ((monTWO.X >= (g_sChar.m_cLocation.X - MON_DETECT_RANGE_X)) && (monTWO.X <= (g_sChar.m_cLocation.X + MON_DETECT_RANGE_X)) && (monTWO.Y <= (g_sChar.m_cLocation.Y + MON_DETECT_RANGE_Y)) && (monTWO.Y >= (g_sChar.m_cLocation.Y - MON_DETECT_RANGE_Y)))			//for the LEFT, RIGHT, TOP and BOTTOM side detection of the monster
		{
			if (monTWO.X <= g_sChar.m_cLocation.X && monTWO.X != g_sChar.m_cLocation.X && (map[monTWO.Y][monTWO.X + 1]) == BLANK_SPACE)
			{
				if ((map[monTWO.Y][monTWO.X + 1]) == WALL)		// allows monster to somewhat walk around WALLS
				{
					if ((map[monTWO.Y + 1][monTWO.X]) == WALL)
					{
						if ((map[monTWO.Y][monTWO.X - 1]) == WALL)
						{
							if ((map[monTWO.Y - 1][monTWO.X]) == BLANK_SPACE)
							{
								monTWO.Y--;

							}
							else
							{

							}
						}
						else
						{
							monTWO.X--;

						}
					}
					else
					{
						monTWO.Y++;

					}
				}
				else
				{
					monTWO.X++;

				}
			}
			if (monTWO.X >= g_sChar.m_cLocation.X && monTWO.X != g_sChar.m_cLocation.X && (map[monTWO.Y][monTWO.X - 1]) == BLANK_SPACE)
			{
				if ((map[monTWO.Y][monTWO.X - 1]) == WALL)		// allows monster to somewhat walk around WALLS
				{
					if ((map[monTWO.Y - 1][monTWO.X]) == WALL)
					{
						if ((map[monTWO.Y + 1][monTWO.X]) == WALL)
						{
							if ((map[monTWO.Y][monTWO.X + 1]) == BLANK_SPACE)
							{
								monTWO.X++;

							}
							else
							{

							}
						}
						else
						{
							monTWO.Y++;

						}
					}
					else
					{
						monTWO.Y--;

					}
				}
				else
				{
					monTWO.X--;

				}
			}
			if (monTWO.Y >= g_sChar.m_cLocation.Y && monTWO.Y != g_sChar.m_cLocation.Y && (map[monTWO.Y - 1][monTWO.X]) == BLANK_SPACE)
			{
				if ((map[monTWO.Y - 1][monTWO.X]) == WALL)		// allows monster to somewhat walk around WALLS
				{
					if ((map[monTWO.Y][monTWO.X + 1]) == WALL)
					{
						if ((map[monTWO.Y][monTWO.X - 1]) == WALL)
						{
							if ((map[monTWO.Y + 1][monTWO.X]) == BLANK_SPACE)
							{
								monTWO.Y++;

							}
							else
							{

							}
						}
						else
						{
							monTWO.X--;

						}
					}
					else
					{
						monTWO.X++;

					}
				}
				else
				{
					monTWO.Y--;

				}
			}
			if (monTWO.Y <= g_sChar.m_cLocation.Y && monTWO.Y != g_sChar.m_cLocation.Y && (map[monTWO.Y + 1][monTWO.X]) == BLANK_SPACE)
			{
				if ((map[monTWO.Y + 1][monTWO.X]) == WALL)		// allows monster to somewhat walk around WALLS
				{
					if ((map[monTWO.Y][monTWO.X - 1]) == WALL)
					{
						if ((map[monTWO.Y][monTWO.X + 1]) == WALL)
						{
							if ((map[monTWO.Y - 1][monTWO.X]) == BLANK_SPACE)
							{
								monTWO.Y--;

							}
							else
							{

							}
						}
						else
						{
							monTWO.X++;

						}
					}
					else
					{
						monTWO.X--;

					}
				}
				else
				{
					monTWO.Y++;

				}
			}
		}
		/* Monster 3 */
		if ((monTHREE.X >= (g_sChar.m_cLocation.X - MON_DETECT_RANGE_X)) && (monTHREE.X <= (g_sChar.m_cLocation.X + MON_DETECT_RANGE_X)) && (monTHREE.Y <= (g_sChar.m_cLocation.Y + MON_DETECT_RANGE_Y)) && (monTHREE.Y >= (g_sChar.m_cLocation.Y - MON_DETECT_RANGE_Y)))			//for the LEFT, RIGHT, TOP and BOTTOM side detection of the monster
		{
			if (monTHREE.X <= g_sChar.m_cLocation.X && monTHREE.X != g_sChar.m_cLocation.X && (map[monTHREE.Y][monTHREE.X + 1]) == BLANK_SPACE)
			{
				if ((map[monTHREE.Y][monTHREE.X + 1]) == WALL)		// allows monster to somewhat walk around WALLS
				{
					if ((map[monTHREE.Y + 1][monTHREE.X]) == WALL)
					{
						if ((map[monTHREE.Y][monTHREE.X - 1]) == WALL)
						{
							if ((map[monTHREE.Y - 1][monTHREE.X]) == BLANK_SPACE)
							{
								monTHREE.Y--;

							}
							else
							{

							}
						}
						else
						{
							monTHREE.X--;

						}
					}
					else
					{
						monTHREE.Y++;

					}
				}
				else
				{
					monTHREE.X++;

				}
			}
			if (monTHREE.X >= g_sChar.m_cLocation.X && monTHREE.X != g_sChar.m_cLocation.X && (map[monTHREE.Y][monTHREE.X - 1]) == BLANK_SPACE)
			{
				if ((map[monTHREE.Y][monTHREE.X - 1]) == WALL)		// allows monster to somewhat walk around WALLS
				{
					if ((map[monTHREE.Y - 1][monTHREE.X]) == WALL)
					{
						if ((map[monTHREE.Y + 1][monTHREE.X]) == WALL)
						{
							if ((map[monTHREE.Y][monTHREE.X + 1]) == BLANK_SPACE)
							{
								monTHREE.X++;

							}
							else
							{

							}
						}
						else
						{
							monTHREE.Y++;

						}
					}
					else
					{
						monTHREE.Y--;

					}
				}
				else
				{
					monTHREE.X--;

				}
			}
			if (monTHREE.Y >= g_sChar.m_cLocation.Y && monTHREE.Y != g_sChar.m_cLocation.Y && (map[monTHREE.Y - 1][monTHREE.X]) == BLANK_SPACE)
			{
				if ((map[monTHREE.Y - 1][monTHREE.X]) == WALL)		// allows monster to somewhat walk around WALLS
				{
					if ((map[monTHREE.Y][monTHREE.X + 1]) == WALL)
					{
						if ((map[monTHREE.Y][monTHREE.X - 1]) == WALL)
						{
							if ((map[monTHREE.Y + 1][monTHREE.X]) == BLANK_SPACE)
							{
								monTHREE.Y++;

							}
							else
							{

							}
						}
						else
						{
							monTHREE.X--;

						}
					}
					else
					{
						monTHREE.X++;

					}
				}
				else
				{
					monTHREE.Y--;

				}
			}
			if (monTHREE.Y <= g_sChar.m_cLocation.Y && monTHREE.Y != g_sChar.m_cLocation.Y && (map[monTHREE.Y + 1][monTHREE.X]) == BLANK_SPACE)
			{
				if ((map[monTHREE.Y + 1][monTHREE.X]) == WALL)		// allows monster to somewhat walk around WALLS
				{
					if ((map[monTHREE.Y][monTHREE.X - 1]) == WALL)
					{
						if ((map[monTHREE.Y][monTHREE.X + 1]) == WALL)
						{
							if ((map[monTHREE.Y - 1][monTHREE.X]) == BLANK_SPACE)
							{
								monTHREE.Y--;

							}
							else
							{

							}
						}
						else
						{
							monTHREE.X++;

						}
					}
					else
					{
						monTHREE.X--;

					}
				}
				else
				{
					monTHREE.Y++;

				}
			}
		}
		monsterSPEED = g_dElapsedTime + 0.03;
	}
}

MON_IDLE monsterIDLEMOV()	// When monster is not near the player
{
	MON_IDLE MON_MOVEMENT = MON_NOTHING;


	int monz;
	monz = rand() % 10000;			//using randomiser

	if (monz <= 1900)
	{
		MON_MOVEMENT = MON_NOTHING;
	}
	else if (monz <= 3925 && monz > 1900)
	{
		MON_MOVEMENT = MON_UP;
	}
	else if (monz <= 5950 && monz > 3925)
	{
		MON_MOVEMENT = MON_DOWN;
	}
	else if (monz <= 7975 && monz > 5950)
	{
		MON_MOVEMENT = MON_LEFT;
	}
	else if (monz <= 10000 && monz > 7975)
	{
		MON_MOVEMENT = MON_RIGHT;
	}

	return MON_MOVEMENT;
}

MON_NO monsterTABLE()
{
	MON_NO MONSTER_NO = monsONE;

	int monALL;
	monALL = rand() % 1500;

	if (monALL <= 500)
	{
		MONSTER_NO = monsONE;
	}
	else if (monALL <= 1000 && monALL > 500)
	{
		MONSTER_NO = monsTWO;
	}
	else if (monALL <= 1500 && monALL > 1000)
	{
		MONSTER_NO = monsTHREE;
	}

	return MONSTER_NO;
}

void renderMonster() {
	COORD c = g_Console.getConsoleSize();
	// place monsters images here
	int monsterrng = 1; //change 2 with number of monsters + 1.

	switch (monsterrng) {
	case 1: { // slime.

		c.X = g_Console.getConsoleSize().X / 2 - 15;
		c.Y = 0;
		g_Console.writeToBuffer(c, "           `......`           ", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "        -ossoo+++///:-`       ", 0x02);
		c.Y += 1;
		g_Console.writeToBuffer(c, "      `shhyyss++o+/////.      ", 0x02);
		c.Y += 1;
		g_Console.writeToBuffer(c, "      ohyyso(----)////:/`     ", 0x02);
		c.Y += 1;
		g_Console.writeToBuffer(c, "     `yyyyssssso+++/////-     ", 0x02);
		c.Y += 1;
		g_Console.writeToBuffer(c, "     `yyysosyyso+////:-/-     ", 0x02);
		c.Y += 1;
		g_Console.writeToBuffer(c, "     `yysoossoo+/:://:-/-     ", 0x02);
		c.Y += 1;
		g_Console.writeToBuffer(c, "     :syssooo//+/://:-:::     ", 0x02);
		c.Y += 1;
		g_Console.writeToBuffer(c, "   `oo+oyyso+/+////:-.:+/:`   ", 0x02);
		c.Y += 1;
		g_Console.writeToBuffer(c, "   :yys++os+:/++////::/s+:-   ", 0x02);
		c.Y += 1;
		g_Console.writeToBuffer(c, "    :syyssso++////-..:sy//-   ", 0x02);
		c.Y += 1;
		g_Console.writeToBuffer(c, "    `syyyso//::///-..-oy+:`   ", 0x02);
		c.Y += 1;
		g_Console.writeToBuffer(c, "     osssss+///++/:://///`    ", 0x02);
		c.Y += 1;
		g_Console.writeToBuffer(c, "     .+osssso+++/::--//+-     ", 0x02);
		c.Y += 1;
		g_Console.writeToBuffer(c, "   ``.---:///:::---..--.      ", 0x02);
	}
	default:
		break;
	}

}

// FOR PUZZLES, NOTE THAT YOU ARE WORKING WITH 16<= c.Y <= 25. Scale puzzles accordingly. Will look into other resolutions after core game is done.
void renderEncounterMonster() {
	renderMonster();
	COORD c = g_Console.getConsoleSize();
	c.X = 0;
	c.Y = 15;
	g_Console.writeToBuffer(c, "--------------------------------------------------------------------------------", 0x02);
	renderMonsterPuzzle();
}

void renderMonsterPuzzle() {
	// This pretty much handles the logic and visual-side of the puzzles.

	//To add: switch-case for multiple puzzles. For now it's a basic maths puzzle.
	// What's a x b ?
	// y=17: qn, y= 19: answer

	COORD c = g_Console.getConsoleSize();

	// Display "What's int1 x int2 ?"
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	c.Y = 17;
	g_Console.writeToBuffer(c, "What is ", 0x02);
	c.X = c.X + 8;
	g_Console.writeToBuffer(c, to_string(puzzle1Integer1), 0x02);
	c.X = c.X + 3;
	g_Console.writeToBuffer(c, "x", 0x02);
	c.X = c.X + 2;
	g_Console.writeToBuffer(c, to_string(puzzle1Integer2), 0x02);
	c.X = c.X + 3;
	g_Console.writeToBuffer(c, "?", 0x02);

	// Answer portion
	c.X = g_Console.getConsoleSize().X / 2 - 6;
	c.Y = 19;

	g_Console.writeToBuffer(c, "Answer: ", 0x02);

	//for answer container; answer will be 2 digits at most.

	c.X = 42;
	switch (digit1) {
	case NUM_ZERO:
	{
		g_Console.writeToBuffer(c, "0", 0x02);
		break;
	}
	case NUM_ONE:
	{
		g_Console.writeToBuffer(c, "1", 0x02);
		break;
	}
	case NUM_TWO:
	{
		g_Console.writeToBuffer(c, "2", 0x02);
		break;
	}
	case NUM_THREE:
	{
		g_Console.writeToBuffer(c, "3", 0x02);
		break;
	}
	case NUM_FOUR:
	{
		g_Console.writeToBuffer(c, "4", 0x02);
		break;
	}
	case NUM_FIVE:
	{
		g_Console.writeToBuffer(c, "5", 0x02);
		break;
	}
	case NUM_SIX:
	{
		g_Console.writeToBuffer(c, "6", 0x02);
		break;
	}
	case NUM_SEVEN:
	{
		g_Console.writeToBuffer(c, "7", 0x02);
		break;
	}
	case NUM_EIGHT:
	{
		g_Console.writeToBuffer(c, "8", 0x02);
		break;
	}
	case NUM_NINE:
	{
		g_Console.writeToBuffer(c, "9", 0x02);
		break;
	}
	case NUM_NIL:
	{
		g_Console.writeToBuffer(c, "_", 0x02);
		break;
	}
	default:
	{
		break;
	}
	}

	c.X = 44;
	switch (digit2) {
	case NUM_ZERO:
	{
		g_Console.writeToBuffer(c, "0", 0x02);
		break;
	}
	case NUM_ONE:
	{
		g_Console.writeToBuffer(c, "1", 0x02);
		break;
	}
	case NUM_TWO:
	{
		g_Console.writeToBuffer(c, "2", 0x02);
		break;
	}
	case NUM_THREE:
	{
		g_Console.writeToBuffer(c, "3", 0x02);
		break;
	}
	case NUM_FOUR:
	{
		g_Console.writeToBuffer(c, "4", 0x02);
		break;
	}
	case NUM_FIVE:
	{
		g_Console.writeToBuffer(c, "5", 0x02);
		break;
	}
	case NUM_SIX:
	{
		g_Console.writeToBuffer(c, "6", 0x02);
		break;
	}
	case NUM_SEVEN:
	{
		g_Console.writeToBuffer(c, "7", 0x02);
		break;
	}
	case NUM_EIGHT:
	{
		g_Console.writeToBuffer(c, "8", 0x02);
		break;
	}
	case NUM_NINE:
	{
		g_Console.writeToBuffer(c, "9", 0x02);
		break;
	}
	case NUM_NIL:
	{
		g_Console.writeToBuffer(c, "_", 0x02);
		break;
	}
	default:
	{
		break;
	}
	}
}


void monsterPuzzle()
{

	COORD c = g_Console.getConsoleSize();

	bool bSomethingHappened = false;
	if (g_dBounceTime > g_dElapsedTime)
		return;
	/* INPUT, digit1 */
	c.X = g_Console.getConsoleSize().X / 2;

	if (g_abKeyPressed[K_ONE])
	{
		if (digit1 == NUM_NIL)
		{
			digit1 = NUM_ONE;
			d1 = 1;
			bSomethingHappened = true;
		}
		else if (digit2 == NUM_NIL && digit1 != NUM_NIL)
		{
			digit2 = NUM_ONE;
			d2 = 1;
			bSomethingHappened = true;
		}
	}
	if (g_abKeyPressed[K_TWO])
	{
		if (digit1 == NUM_NIL)
		{
			digit1 = NUM_TWO;
			d1 = 2;
			bSomethingHappened = true;
		}
		else if (digit2 == NUM_NIL && digit1 != NUM_NIL)
		{
			digit2 = NUM_TWO;
			d2 = 2;
			bSomethingHappened = true;
		}
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_THREE])
	{
		if (digit1 == NUM_NIL)
		{
			digit1 = NUM_THREE;
			d1 = 3;
			bSomethingHappened = true;
		}
		else if (digit2 == NUM_NIL && digit1 != NUM_NIL)
		{
			digit2 = NUM_THREE;
			d2 = 3;
			bSomethingHappened = true;
		}
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_FOUR])
	{
		if (digit1 == NUM_NIL)
		{
			digit1 = NUM_FOUR;
			d1 = 4;
			bSomethingHappened = true;
		}
		else if (digit2 == NUM_NIL && digit1 != NUM_NIL)
		{
			digit2 = NUM_FOUR;
			d2 = 4;
			bSomethingHappened = true;
		}
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_FIVE])
	{
		if (digit1 == NUM_NIL)
		{
			digit1 = NUM_FIVE;
			d1 = 5;
			bSomethingHappened = true;
		}
		else if (digit2 == NUM_NIL && digit1 != NUM_NIL)
		{
			digit2 = NUM_FIVE;
			d2 = 5;
			bSomethingHappened = true;
		}
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_SIX])
	{
		if (digit1 == NUM_NIL)
		{
			digit1 = NUM_SIX;
			d1 = 6;
			bSomethingHappened = true;
		}
		else if (digit2 == NUM_NIL && digit1 != NUM_NIL)
		{
			digit2 = NUM_SIX;
			d2 = 6;
			bSomethingHappened = true;
		}
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_SEVEN])
	{
		if (digit1 == NUM_NIL)
		{
			digit1 = NUM_SEVEN;
			d1 = 7;
			bSomethingHappened = true;
		}
		else if (digit2 == NUM_NIL && digit1 != NUM_NIL)
		{
			digit2 = NUM_SEVEN;
			d2 = 7;
			bSomethingHappened = true;
		}
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_EIGHT])
	{
		if (digit1 == NUM_NIL)
		{
			digit1 = NUM_EIGHT;
			d1 = 8;
			bSomethingHappened = true;
		}
		else if (digit2 == NUM_NIL && digit1 != NUM_NIL)
		{
			digit2 = NUM_EIGHT;
			d2 = 8;
			bSomethingHappened = true;
		}
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_NINE])
	{
		if (digit1 == NUM_NIL)
		{
			digit1 = NUM_NINE;
			d1 = 9;
			bSomethingHappened = true;
		}
		else if (digit2 == NUM_NIL && digit1 != NUM_NIL)
		{
			digit2 = NUM_NINE;
			d2 = 9;
			bSomethingHappened = true;
		}
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_ZERO])
	{
		if (digit1 == NUM_NIL)
		{
			digit1 = NUM_ZERO;
			d1 = 0;
			bSomethingHappened = true;
		}
		else if (digit2 == NUM_NIL && digit1 != NUM_NIL)
		{
			digit2 = NUM_ZERO;
			d2 = 0;
			bSomethingHappened = true;
		}
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_BACKSPACE])
	{
		if ((digit1 != NUM_NIL) && (digit2 != NUM_NIL)) { // if both filled
			digit2 = NUM_NIL;
		}
		else if ((digit1 != NUM_NIL) && (digit2 == NUM_NIL)) { //if only digit 1 digit filled.
			digit1 = NUM_NIL;
		}
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_ENTER])
	{
		puzzle1Input = 0;

		if ((d1 == NULL)&&(d2 == NULL))
		{
			puzzle1Input = 0;
		}
		else if ((d1 != NULL)&&(d2 == NULL))
		{
			puzzle1Input = d1;
		}
		else if ((d1 != NULL)&&(d2 != NULL))
		{
			puzzle1Input = ((d1 * 10) + d2);
		}

		//now check answer.
		puzzle1Ans = puzzle1Integer1 * puzzle1Integer2;

	//	if (puzzle1Input != puzzle1Ans) //wrong ans
	//		g_eGameState = S_SPLASHSCREEN; //output for now, in future replace with a more suitable sequence.

		if (puzzle1Ans == puzzle1Input) {	// correct answer
			g_eGameState = S_GAME;
			puzzle1Integer1 = rand() % 10 + 1;
			puzzle1Integer2 = rand() % 10 + 1;
			digit1 = NUM_NIL;
			digit2 = NUM_NIL;
			d1 = NULL;
			d2 = NULL;
		}
		bSomethingHappened = true;
	}

	//if (g_abKeyPressed[K_ESCAPE])
	//{
	//	g_eGameState = S_SPLASHSCREEN;
	//	bSomethingHappened = true;
	//}


	if (bSomethingHappened)
	{
		// set the bounce time to some time in the future to prevent accidental triggers
		g_dBounceTime = g_dElapsedTime + 0.25;
	}

}
