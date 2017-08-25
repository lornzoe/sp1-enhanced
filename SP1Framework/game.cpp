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
lastSavedData g_save;

//monsters
COORD monONE;		
COORD monTWO;		
COORD monTHREE;		
COORD monBOSS;

//level completion
bool levelOneC;
bool levelTwoC;
bool levelThreeC;
bool levelFourC;
bool levelFiveC;

int puzzle1Integer1 = rand() % 9 + 1;
int puzzle1Integer2 = rand() % 9 + 1;
int puzzle1Ans;
int puzzle1Input;
P1_NUMBER digit1 = NUM_NIL;
P1_NUMBER digit2 = NUM_NIL;
int d1 = NULL;
int d2 = NULL;

SLIDE_DIRECTION directionICE = D_NONE;
bool puzzleChange = false;
PUZZLE_TYPE P_TYPE = PUZZLE_NIL;

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
	g_currentlevel = L_LEVELONE;
	bool levelOneC = false;
	bool levelTwoC = false;
	bool levelThreeC = false;
	bool levelFourC = false;
	bool levelFiveC = false;


    g_sChar.m_cLocation.X = g_Console.getConsoleSize().X / 2;
    g_sChar.m_cLocation.Y = g_Console.getConsoleSize().Y / 2 + 1;
    g_sChar.m_hasKey = false;

	puzzle1Integer1 = rand() % 9 + 1;
	puzzle1Integer2 = rand() % 9 + 1;

	Mon_Char_Location();

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
		case S_OPTIONMENU: OptionScreenSelect(); // option screen logic
			break;
		case S_VOLUME: VolumeScreenSelect();
			break;
		case S_CREDITS:  CreditScreenSelect();
			break;
		case S_LEVELSELECT: LevelScreenSelect(); // game logic for the level select screen
			break;
		case S_CHARACTERCOLOR: charactercolourselect();
			break;
		case S_ENCOUNTERMONSTER: monsterPuzzle();
			break;
		case S_WINSCREEN: winscreen();
			break;
		case S_BOSSENCOUNTER: bossEncounter();
			break;
		case S_CORRECT: correctScreen();
			break;
		case S_WRONG: wrongScreen();
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
		case S_OPTIONMENU: renderOptions();
			break;
		case S_VOLUME: renderVolume();
			break;
        case S_GAME: renderGame();
            break;
		case S_LEVELSELECT: renderLevelSelect();
			break;
		case S_CREDITS: renderCredits();
			break;
		case S_LOADGAME: renderSavedFile();
			break;
		case S_CHARACTERCOLOR: renderCharacterColour();
			break;
		case S_ENCOUNTERMONSTER: renderEncounterMonster();
			break;
		case S_WINSCREEN: winscreenRender();
			break;
		case S_BOSSENCOUNTER: renderBossEncounter();
			break;
		case S_CORRECT: renderCorrect();
			break;
		case S_WRONG: renderWrong();
			break;
    }
    renderFramerate();  // renders debug information, frame rate, elapsed time, etc
    renderToScreen();   // dump the contents of the buffer to the screen, one frame worth of game
}

void splashScreenWait() {
	//if (g_dElapsedTime > 3.0) // wait for 3 seconds to switch to game mode, else do nothing
	// g_eGameState = S_GAME;
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
		g_eGameState = S_OPTIONMENU;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_THREE])
	{
		g_eGameState = S_CREDITS;
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

void VolumeScreenSelect()		//buttons for Volume Screen
{
	bool bSomethingHappened = false;
	if (g_dBounceTime > g_dElapsedTime)
		return;

	if (g_abKeyPressed[K_ONE])
	{
		waveOutSetVolume(NULL, 0x05FF05FF);
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_TWO])
	{
		waveOutSetVolume(NULL, 0x0FAA0FAA);
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_THREE])
	{
		waveOutSetVolume(NULL, 0x5FFF5FFF);
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_ESCAPE])
	{
		g_eGameState = S_OPTIONMENU;
		bSomethingHappened = true;
	}
	if (bSomethingHappened)
	{
		// set the bounce time to some time in the future to prevent accidental triggers
		g_dBounceTime = g_dElapsedTime + 0.25;
	}
}

void OptionScreenSelect()			//Buttons for Options Screen
{
	bool bSomethingHappened = false;
	if (g_dBounceTime > g_dElapsedTime)
		return;

	if (g_abKeyPressed[K_ONE])
	{
		g_eGameState = S_CHARACTERCOLOR;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_TWO])
	{
		g_eGameState = S_VOLUME;
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

void CreditScreenSelect()
{
	bool bSomethingHappened = false;
	if (g_dBounceTime > g_dElapsedTime)
		return;
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

void LevelScreenSelect() // LOGIC FOR KEY PRESS in level select
{
	bool bSomethingHappened = false;
	if (g_dBounceTime > g_dElapsedTime)
		return;
	if (g_abKeyPressed[K_ONE])
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELONE;
		Mon_Char_Location();
		levelChange = true;
		bSomethingHappened = true;
		PlaySound(TEXT("levels1-2.wav"), NULL, SND_ASYNC | SND_LOOP);
		system("0");
	}
	if (g_abKeyPressed[K_TWO] && levelOneC)
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELTWO;
		Mon_Char_Location();
		levelChange = true;
		bSomethingHappened = true;
		PlaySound(TEXT("levels1-2.wav"), NULL, SND_ASYNC | SND_LOOP);
		system("0");
	}
	if (g_abKeyPressed[K_THREE] && levelTwoC)
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELTHREE;
		Mon_Char_Location();
		levelChange = true;
		bSomethingHappened = true;
		PlaySound(TEXT("levels3-4.wav"), NULL, SND_ASYNC | SND_LOOP);
		system("0");
	}
	if (g_abKeyPressed[K_FOUR] && levelThreeC)
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELFOUR;
		Mon_Char_Location();
		levelChange = true;
		bSomethingHappened = true;
		PlaySound(TEXT("levels3-4.wav"), NULL, SND_ASYNC | SND_LOOP);
		system("0");
	}
	if (g_abKeyPressed[K_FIVE] && levelFourC)
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELFIVE;
		Mon_Char_Location();
		levelChange = true;
		bSomethingHappened = true;
		PlaySound(TEXT("boss.wav"), NULL, SND_ASYNC | SND_LOOP);
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
		g_eGameState = S_OPTIONMENU;
		bSomethingHappened = true;
	}
	if (bSomethingHappened)
	{
		// set the bounce time to some time in the future to prevent accidental triggers
		g_dBounceTime = g_dElapsedTime + 0.25;
	}
}

void winscreen() {
	if (g_abKeyPressed[K_ESCAPE]) {
		g_bQuitGame = true; // exit game
	}else if (g_abKeyPressed[K_ENTER]) {
		g_eGameState = S_SPLASHSCREEN; // reset game
		PlaySound(TEXT("mainmenu.wav"), NULL, SND_ASYNC | SND_LOOP);
	}
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

void renderSavedFile()
{
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
		case INVISBLOCKS:
			return true;
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
		g_eGameState = S_SPLASHSCREEN;
		
		Mon_Char_Location();
		bSomethingHappened = true;
	}
	if (map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X] == ENDPOINT) {
		switch (g_currentlevel) {
			case L_LEVELONE:
				g_currentlevel = L_LEVELTWO;
				levelChange = true;
				
				Mon_Char_Location();
				levelOneC = true;
				bSomethingHappened = true;
				break;
			case L_LEVELTWO:
				g_currentlevel = L_LEVELTHREE;
				levelChange = true;
				
				Mon_Char_Location();
				levelTwoC = true;
				PlaySound(TEXT("levels3-4.wav"), NULL, SND_ASYNC | SND_LOOP);
				system("0");
				bSomethingHappened = true;
				break;
			case L_LEVELTHREE:
				g_currentlevel = L_LEVELFOUR;
				levelChange = true;
				
				Mon_Char_Location();
				levelThreeC = true;
				PlaySound(TEXT("levels3-4.wav"), NULL, SND_ASYNC | SND_LOOP);
				system("0");
				bSomethingHappened = true;
				break;
			case L_LEVELFOUR:
				g_currentlevel = L_LEVELFIVE;
				levelChange = true;
				
				Mon_Char_Location();
				levelFourC = true;
				PlaySound(TEXT("boss.wav"), NULL, SND_ASYNC | SND_LOOP);
				system("0");
				bSomethingHappened = true;
				break;
			case L_LEVELFIVE:
				g_eGameState = S_WINSCREEN;
				levelChange = true;
				
				Mon_Char_Location();
				levelFiveC = true;
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

	if (monONE.X == g_sChar.m_cLocation.X && monONE.Y == g_sChar.m_cLocation.Y)			// When monster touches player
	{
		saveData();
		puzzleChange = true;
		g_eGameState = S_ENCOUNTERMONSTER;
		monONE.X = NULL;
		monONE.Y = NULL;
	}
	if (monTWO.X == g_sChar.m_cLocation.X && monTWO.Y == g_sChar.m_cLocation.Y)
	{
		saveData();
		puzzleChange = true;
		g_eGameState = S_ENCOUNTERMONSTER;
		monTWO.X = NULL;
		monTWO.Y = NULL;
	}
	if (monTHREE.X == g_sChar.m_cLocation.X && monTHREE.Y == g_sChar.m_cLocation.Y)
	{
		saveData();
		puzzleChange = true;
		g_eGameState = S_ENCOUNTERMONSTER;
		monTHREE.X = NULL;
		monTHREE.Y = NULL;
	}
	if (BOSS_ENGAGE_RANGE_X && monBOSS.Y == g_sChar.m_cLocation.Y)
	{
		g_eGameState = S_BOSSENCOUNTER;
		PlaySound(TEXT("bossfight.wav"), NULL, SND_ASYNC | SND_LOOP);
		system("0");
	}

	if (bSomethingHappened)
    {
        // set the bounce time to some time in the future to prevent accidental triggers
        g_dBounceTime = g_dElapsedTime + 0.25; 
    }

}

void bossEncounter() {
	bool bSomethingHappened = false;
	if (g_dBounceTime > g_dElapsedTime) {
		return;
	}

	if (g_abKeyPressed[K_ONE]) {
		g_eGameState = S_WRONG;
		bSomethingHappened = true;
	}
	else if (g_abKeyPressed[K_TWO]) {
		monBOSS.X = NULL;
		monBOSS.Y = NULL;
		g_eGameState = S_CORRECT;
		bSomethingHappened = true;
	}
	else if (g_abKeyPressed[K_THREE]) {
		g_eGameState = S_WRONG;
		bSomethingHappened = true;
	}

	if (bSomethingHappened)
	{
		// set the bounce time to some time in the future to prevent accidental triggers
		g_dBounceTime = g_dElapsedTime + 0.25;
	}
}

void correctScreen() {
	if (g_abKeyPressed[K_ENTER]) {
		g_eGameState = S_GAME;
		PlaySound(TEXT("boss.wav"), NULL, SND_ASYNC | SND_LOOP);
	}
}

void wrongScreen() {
	if (g_abKeyPressed[K_ENTER]) {
		g_eGameState = S_BOSSENCOUNTER;
	}
}

void renderBossEncounter() {
	COORD c;
	// sphinx
	{
		c.X = g_Console.getConsoleSize().X / 2 + 3;
		c.Y = 8;
		g_Console.writeToBuffer(c, "               ___", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "             .\"///\".", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "            /|<> <>!\\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "           /-|  ^  !-\\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "          /-- \\_=_/ --\\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "          )---| W |---(", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "         /-\\--| W |--/-\\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "        (_-_--|_-_|--___)", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "       (-___  -_-- _-- -_)", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "       )-_ _--_ _ ___--__|", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "       (___ --__  __ __--(", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "      /-_  / __ -_ -__  \\_\\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "     _>/  -- /|___| _ \\ -_ )", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "    /--  _ - _/ _ \\>\\ -  -- \\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "   ( / / /   > |~l \\   \\ \\ \\_)", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "   | |-' | |/  \"\"\"  \\| |   |_|", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "   L_|_|_|_/         L_L_|_l_)", 0x06);
	c.X -= 6;
	c.Y = 6;
	for (int i = 0; i <= 18; i++) {
		g_Console.writeToBuffer(c, "|", 0x06);
		c.Y += 1;
	}
	c.Y = 6;
	c.X = 37;
	for (int i = 0; i <= 20; i++) {
		g_Console.writeToBuffer(c, " ", 0x06);
		c.X += 1;
		g_Console.writeToBuffer(c, "_", 0x06);
		c.X += 1;
	}
	c.Y = 7;
	c.X = 39;
	g_Console.writeToBuffer(c, "Solve the riddles of the Sphinx to pass!", 0x06);
}
	// chest riddle
	{
		c.X = 1;
		c.Y = 1;
		g_Console.writeToBuffer(c, "In front of you are 3 chests:", 0x08);
		c.Y += 2;
		g_Console.writeToBuffer(c, "Chest 1 is labelled \"100 gold coins\".", 0x08);
		c.Y += 1;
		g_Console.writeToBuffer(c, "Chest 2 is labelled \"50 gold, 50 silver coins\".", 0x08);
		c.Y += 1;
		g_Console.writeToBuffer(c, "Chest 3 is labelled \"100 silver coins\".", 0x08);
		c.Y += 2;
		g_Console.writeToBuffer(c, "You are told that all of the labels", 0x08);
		c.Y += 1;
		g_Console.writeToBuffer(c, "are incorrectly placed, as they", 0x08);
		c.Y += 1;
		g_Console.writeToBuffer(c, "describe the contents of another one", 0x08);
		c.Y += 1;
		g_Console.writeToBuffer(c, "of the chests. To help you determine", 0x08);
		c.Y += 1;
		g_Console.writeToBuffer(c, "which is the chest that contains", 0x08);
		c.Y += 1;
		g_Console.writeToBuffer(c, "100 gold coins, you are allowed to", 0x08);
		c.Y += 1;
		g_Console.writeToBuffer(c, "pick a random coin from a chest of", 0x08);
		c.Y += 1;
		g_Console.writeToBuffer(c, "your choice.", 0x08);
		c.Y += 2;
		g_Console.writeToBuffer(c, "Which chest should you pick?", 0x08);
		c.X += 8;
		c.Y += 4;
		g_Console.writeToBuffer(c, "<1>   <2>   <3>", 0x0F);
	} // chest puzzle
}

void renderCorrect() {
	COORD c;
	// sphinx
	{
		c.X = g_Console.getConsoleSize().X / 2 - 16;
		c.Y = 4;
		g_Console.writeToBuffer(c, "               ___", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "             .\"///\".", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "            /|<> <>!\\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "           /-|  ^  !-\\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "          /-- \\_=_/ --\\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "          )---| W |---(", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "         /-\\--| W |--/-\\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "        (_-_--|_-_|--___)", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "       (-___  -_-- _-- -_)", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "       )-_ _--_ _ ___--__|", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "       (___ --__  __ __--(", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "     _>/  -- /|___| _ \\ -_ )", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "    /--  _ - _/ _ \\>\\ -  -- \\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "   ( / / /   > |~l \\   \\ \\ \\_)", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "   | |-' | |/  \"\"\"  \\| |   |_|", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "   L_|_|_|_/         L_L_|_l_)", 0x06);
		c.Y += 3;
		c.X = g_Console.getConsoleSize().X / 2 - 6;
		g_Console.writeToBuffer(c, "<Press Enter>", 0x02);
		c.Y = g_Console.getConsoleSize().Y / 2 - 2;
		c.X = g_Console.getConsoleSize().X / 2 - 27;
		g_Console.writeToBuffer(c, "  ____    ___    ____    ____    _____    ____   _____ ", 0x0F);
		c.Y += 1;
		g_Console.writeToBuffer(c, " / ___|  / _ \\  |  _ \\  |  _ \\  | ____|  / ___| |_   _|", 0x0F);
		c.Y += 1;
		g_Console.writeToBuffer(c, "| |     | | | | | |_) | | |_) | |  _|   | |       | |  ", 0x0F);
		c.Y += 1;
		g_Console.writeToBuffer(c, "| |___  | |_| | |  _ <  |  _ <  | |___  | |___    | |  ", 0x0F);
		c.Y += 1;
		g_Console.writeToBuffer(c, " \\____|  \\___/  |_| \\_\\ |_| \\_\\ |_____|  \\____|   |_|  ", 0x0F);
	}

}

void renderWrong() {
	COORD c;
	// sphinx
	{
		c.X = g_Console.getConsoleSize().X / 2 - 16;
		c.Y = 4;
		g_Console.writeToBuffer(c, "               ___", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "             .\"///\".", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "            /|<> <>!\\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "           /-|  ^  !-\\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "          /-- \\_=_/ --\\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "          )---| W |---(", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "         /-\\--| W |--/-\\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "        (_-_--|_-_|--___)", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "       (-___  -_-- _-- -_)", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "       )-_ _--_ _ ___--__|", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "       (___ --__  __ __--(", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "     _>/  -- /|___| _ \\ -_ )", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "    /--  _ - _/ _ \\>\\ -  -- \\", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "   ( / / /   > |~l \\   \\ \\ \\_)", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "   | |-' | |/  \"\"\"  \\| |   |_|", 0x06);
		c.Y += 1;
		g_Console.writeToBuffer(c, "   L_|_|_|_/         L_L_|_l_)", 0x06);
		c.Y += 3;
		c.X = g_Console.getConsoleSize().X / 2 - 6;
		g_Console.writeToBuffer(c, "<Press Enter>", 0x02);
		c.Y = g_Console.getConsoleSize().Y / 2 - 2;
		c.X = g_Console.getConsoleSize().X / 2 - 23;
		g_Console.writeToBuffer(c, "__        __  ____     ___    _   _    ____ ", 0x04);
		c.Y += 1;
		g_Console.writeToBuffer(c, "\\ \\      / / |  _ \\   / _ \\  | \\ | |  / ___|", 0x04);
		c.Y += 1;
		g_Console.writeToBuffer(c, " \\ \\ /\\ / /  | |_) | | | | | |  \\| | | |  _ ", 0x04);
		c.Y += 1;
		g_Console.writeToBuffer(c, "  \\ V  V /   |  _ <  | |_| | | |\\  | | |_| |", 0x04);
		c.Y += 1;
		g_Console.writeToBuffer(c, "   \\_/\\_/    |_| \\_\\  \\___/  |_| \\_|  \\____|", 0x04);
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
	c.Y += 1;
	c.X = c.X / 2 - 8;
	g_Console.writeToBuffer(c, "1. Level Select", 0x06);
	c.Y += 2;
	/*c.X = g_Console.getConsoleSize().X / 2 - 9;
	g_Console.writeToBuffer(c, "2. Continue Game", 0x06);
	c.Y += 2;*/
	c.X = g_Console.getConsoleSize().X / 2 - 5;
	g_Console.writeToBuffer(c, "2. Options", 0x06);
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 5;
	g_Console.writeToBuffer(c, "3. Credits", 0x06);
	c.X = g_Console.getConsoleSize().X / 2 - 33;
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

void renderOptions()
{
	COORD c = g_Console.getConsoleSize();
	c.Y /= 2;
	c.Y -= 2;
	c.Y += 1;
	c.X = c.X / 2 - 10;
	g_Console.writeToBuffer(c, "1. Character Color", 0x06);
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 5;
	g_Console.writeToBuffer(c, "2. Volume", 0x06);
	c.X = g_Console.getConsoleSize().X / 2 - 33;
	c.Y += 3;
	c.X = g_Console.getConsoleSize().X / 2 - 11;
	g_Console.writeToBuffer(c, "<Press Esc to go Back>", 0x02);
	c.X = g_Console.getConsoleSize().X / 2 - 33;
}

void renderVolume()
{
	COORD c = g_Console.getConsoleSize();
	c.Y /= 2;
	c.Y += 1;
	c.X = c.X / 2 - 25;
	g_Console.writeToBuffer(c, "1. Low Volume", 0x06);
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	g_Console.writeToBuffer(c, "2. Medium Volume", 0x06);
	c.X = g_Console.getConsoleSize().X / 2 - 33;
	c.X = g_Console.getConsoleSize().X / 2 + 11;
	g_Console.writeToBuffer(c, "3. High Volume", 0x06);
	c.X = g_Console.getConsoleSize().X / 2 - 33;
	c.Y += 3;
	c.X = g_Console.getConsoleSize().X / 2 - 11;
	g_Console.writeToBuffer(c, "<Press Esc to go Back>", 0x02);
	c.X = g_Console.getConsoleSize().X / 2 - 33;
}

void renderCredits()
{
	COORD c = g_Console.getConsoleSize();
	c.Y /= 2;
	c.Y -= 3;
	c.X = c.X / 2 - 10;
	g_Console.writeToBuffer(c, "Team Leader: Rafiq", 0x06);
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 10;
	g_Console.writeToBuffer(c, "Team Member: Lorenzo", 0x06);
	c.X = g_Console.getConsoleSize().X / 2 - 33;
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 10;
	g_Console.writeToBuffer(c, "Team Member: Tian Yuan", 0x06);
	c.X = g_Console.getConsoleSize().X / 2 - 33;
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 10;
	g_Console.writeToBuffer(c, "Team Member: Darius", 0x06);
	c.X = g_Console.getConsoleSize().X / 2 - 33;
	c.Y += 4;
	c.X = g_Console.getConsoleSize().X / 2 - 11;
	g_Console.writeToBuffer(c, "<Press Esc to go Back>", 0x02);
	c.X = g_Console.getConsoleSize().X / 2 - 33;
}

void winscreenRender() {
	COORD c;
	c.X = g_Console.getConsoleSize().X / 2 - 30;
	c.Y = g_Console.getConsoleSize().Y / 2 - 6;
	g_Console.writeToBuffer(c, "          _______                      _______  _        _ ", 0x02);
	c.Y += 1;
	g_Console.writeToBuffer(c, "|\\     /|(  ___  )|\\     /|  |\\     /|(  ___  )( (    /|( )", 0x02);
	c.Y += 1;
	g_Console.writeToBuffer(c, "( \\   / )| (   ) || )   ( |  | )   ( || (   ) ||  \\  ( || |", 0x02);
	c.Y += 1;
	g_Console.writeToBuffer(c, " \\ (_) / | |   | || |   | |  | | _ | || |   | ||   \\ | || |", 0x02);
	c.Y += 1;
	g_Console.writeToBuffer(c, "  \\   /  | |   | || |   | |  | |( )| || |   | || (\\ \\) || |", 0x02);
	c.Y += 1;
	g_Console.writeToBuffer(c, "   ) (   | |   | || |   | |  | || || || |   | || | \\   |(_)", 0x02);
	c.Y += 1;
	g_Console.writeToBuffer(c, "   | |   | (___) || (___) |  | () () || (___) || )  \\  | _ ", 0x02);
	c.Y += 1;
	g_Console.writeToBuffer(c, "   \\_/   (_______)(_______)  (_______)(_______)|/    )_)(_)", 0x02);
	c.Y += 3;
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	g_Console.writeToBuffer(c, "<Enter> To Restart", 0x06);
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 8;
	g_Console.writeToBuffer(c, "<Escape> To Quit", 0x06);
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
	g_Console.writeToBuffer(c, "<Press Esc to go back>", 0x02);
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
	if (levelOneC) {
		g_Console.writeToBuffer(c, "2. Level Two", 0x06);
	}
	else {
		g_Console.writeToBuffer(c, "Level Locked", 0x04);
	}
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	if (levelTwoC) {
		g_Console.writeToBuffer(c, "3. Level Three", 0x06);
	}
	else {
		g_Console.writeToBuffer(c, "Level Locked", 0x04);
	}
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	if (levelThreeC) {
		g_Console.writeToBuffer(c, "4. Level Four", 0x06);
	}
	else {
		g_Console.writeToBuffer(c, "Level Locked", 0x04);
	}
	c.Y += 2;
	c.X = g_Console.getConsoleSize().X / 2 - 9;
	if (levelFourC) {
		g_Console.writeToBuffer(c, "5. Level Five", 0x06);
	}
	else {
		g_Console.writeToBuffer(c, "Level Locked", 0x04);
	}
	c.Y += 3;
	c.X = g_Console.getConsoleSize().X / 2 - 13;
	g_Console.writeToBuffer(c, "<Press Esc to go Back>", 0x02);
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
					if (g_sChar.m_hasKey)
						map[r][c] = BLANK_SPACE;
					break;
				case GATE: g_Console.writeToBuffer(c, r, (char)219, 0x0A);
					break;
				case INVISBLOCKS: g_Console.writeToBuffer(c, r, (char)219, 0x00);
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
    g_Console.writeToBuffer(g_sChar.m_cLocation, (char)158, charColor);

	// draw key at bottom left if player has picked up key
	if (g_sChar.m_hasKey) {
		g_Console.writeToBuffer((g_Console.getConsoleSize().X / 2 - 6), 0, "KEY OBTAINED", 0x0F);
	}
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


void monsterALL()				// how the monster is rendered in game
{
	g_Console.writeToBuffer(monONE, "\u0444", 0x0C);
	g_Console.writeToBuffer(monTWO, "\u0444", 0x0C);
	g_Console.writeToBuffer(monTHREE, "\u0444", 0x0C);
	g_Console.writeToBuffer(monBOSS, "@-@", 0x0C);
}

void Mon_Char_Location()			// logic for the monst location
{
	if (g_currentlevel == L_LEVELONE) {
		monONE.X = (g_Console.getConsoleSize().X / 2 - 16);
		monONE.Y = (g_Console.getConsoleSize().Y / 2 - 10);
		monTWO.X = (g_Console.getConsoleSize().X / 2 + 36);
		monTWO.Y = (g_Console.getConsoleSize().Y / 2 + 10);
		monTHREE.X = NULL;
		monTHREE.Y = NULL;
		monBOSS.X = NULL;
		monBOSS.Y = NULL;
		//player && key
		g_sChar.m_cLocation.X = g_Console.getConsoleSize().X / 2;
		g_sChar.m_cLocation.Y = g_Console.getConsoleSize().Y / 2 + 1;
		g_sChar.m_hasKey = false;
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
		//player && key
		g_sChar.m_cLocation.X = g_Console.getConsoleSize().X / 2 - 1;
		g_sChar.m_cLocation.Y = g_Console.getConsoleSize().Y / 2 - 6;
		g_sChar.m_hasKey = false;
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
		//player && key
		g_sChar.m_cLocation.X = g_Console.getConsoleSize().X / 2 - 33;
		g_sChar.m_cLocation.Y = g_Console.getConsoleSize().Y / 2 - 10;
		g_sChar.m_hasKey = false;
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
		//player && key
		g_sChar.m_cLocation.X = g_Console.getConsoleSize().X / 2 - 38;
		g_sChar.m_cLocation.Y = g_Console.getConsoleSize().Y / 2 + 10;
		g_sChar.m_hasKey = false;
	}
	else if (g_currentlevel == L_LEVELFIVE)
	{
		monONE.X = NULL;
		monONE.Y = NULL;
		monTWO.X = NULL;
		monTWO.Y = NULL;
		monTHREE.X = NULL;
		monTHREE.Y = NULL;
		monBOSS.X = (g_Console.getConsoleSize().X / 2 - 2);
		monBOSS.Y = (g_Console.getConsoleSize().Y / 2 );
		//player && key
		g_sChar.m_cLocation.X = g_Console.getConsoleSize().X / 2 - 3;
		g_sChar.m_cLocation.Y = g_Console.getConsoleSize().Y / 2 + 10;
		g_sChar.m_hasKey = false;
	}
}

void monsterAI()				// base code for how monster  acts
{
	MON_IDLE currentMOV;
	currentMOV = monsterIDLEMOV();
	
	MON_NO MONS_NUM;
	MONS_NUM = monsterTABLE();
	COORD MON_ALL;				// this will be the value which hold a monster's movement
	

switch (MONS_NUM) {				// this takes out the original position of a monster
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
		//Monsters roaming movements (checks if the space is a blank space before being able to move)
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
		
		
	
			if (MONS_NUM == monsONE)		// this puts back the updated positions of the monster
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
	// monster's speed
	monsterSPEED = g_dElapsedTime + 0.06;

	}
}

void monsterCHASE()						// sub-function for monster to chase player (checks for a player being near the monster)
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
		//monster's speed
		monsterSPEED = g_dElapsedTime + 0.03;
	}
}

MON_IDLE monsterIDLEMOV()	// randomiser for monster's movement when monster is not near the player
{
	MON_IDLE MON_MOVEMENT = MON_NOTHING;


	int monz;
	monz = rand() % 10000;	

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

MON_NO monsterTABLE()		//randomiser for which monster gets to move when not chasing player
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

void saveData()
{
	g_save.savedlevel = g_currentlevel;
	g_save.x = g_sChar.m_cLocation.X;
	g_save.y = g_sChar.m_cLocation.Y;
}

void loadData()
{
	g_currentlevel = g_save.savedlevel;
	g_sChar.m_cLocation.X = g_save.x;
	g_sChar.m_cLocation.Y = g_save.y;
}
//------------------------------------------
//			SEQUENCE OF EVENTS 
//------------------------------------------
// > When monster touches the player, switch game state to S_ENCOUNTERMONSTER.
// > Use an enum to determine what control + level set to use. 
// > RENDER IN THE FOLLOWING ORDER: MONSTER, PUZZLE + TEXT.

// monster1Puzzle works like gameplay() in gameplay logic.
// loadPuzzle() works the same way as loadMap()
// puzzleControls() will process all the puzzles.


void monsterPuzzle()
{
	// make a randomiser, determines what level is done here.
	monsterRandomiser();
	loadPuzzle();
	puzzlePosition();
	puzzleControls();


	if (map[g_sChar.m_cLocation.Y][g_sChar.m_cLocation.X] == PORTAL)
	{
		g_eGameState = S_GAME;
		loadData();
		if (g_currentlevel == L_LEVELONE || g_currentlevel == L_LEVELTWO)	//starts the level's music again after monster puzzle is done
		{
			PlaySound(TEXT("levels1-2.wav"), NULL, SND_ASYNC | SND_LOOP);
		}
		else if (g_currentlevel == L_LEVELTHREE || g_currentlevel == L_LEVELFOUR)
		{
			PlaySound(TEXT("levels3-4.wav"), NULL, SND_ASYNC | SND_LOOP);
		}
		else if (g_currentlevel == L_LEVELFIVE)
		{
			PlaySound(TEXT("boss.wav"), NULL, SND_ASYNC | SND_LOOP);
		}
		levelChange = true;
	}
}

void monsterRandomiser()
{
	if (puzzleChange)
	{
		int RNGGOFUCKYOURSELF;
		RNGGOFUCKYOURSELF = rand() % 2 + 1;
		
		switch (RNGGOFUCKYOURSELF) {
		case 1:
			P_TYPE = PUZZLE_SLIME;
			break;
		case 2: P_TYPE = PUZZLE_ICE;
			break;
		default:
			break;
		}
	}

}

void loadPuzzle() {
	ifstream file;
	string row;
	if (puzzleChange) {
		switch (P_TYPE)
		{
		case PUZZLE_SLIME: break;
		default:
		{
			switch (P_TYPE) {
			case PUZZLE_ICE:
				file.open("PuzzleMap/puzzle1.txt");
				PlaySound(TEXT("puzzle.wav"), NULL, SND_ASYNC | SND_LOOP);
				system("0");
				break;
			default: break;
			}

			for (int i = 0; i < 25; i++) {
				getline(file, row);
				for (int j = 0; j < 80; j++) {
					map[i][j] = row[j];
				}
			}
			file.close();


		}
		}

	}
}

void puzzlePosition() {
	if (puzzleChange)
	{
		switch (P_TYPE)
		{
		case PUZZLE_SLIME: 
			g_sChar.m_cLocation.X = 1;
			g_sChar.m_cLocation.Y = 0;
			break;
		case PUZZLE_ICE:
			g_sChar.m_cLocation.X = 50;
			g_sChar.m_cLocation.Y = 6;
			break;
		}
	}
	puzzleChange = false;
}

void puzzleControls()
{

	switch (P_TYPE)
	{
	case PUZZLE_SLIME: puzzleControls1();
		break;
	case PUZZLE_ICE:
		switch (directionICE)
		{
		case D_NONE:
			moveCharacterICE();		// input for ice movement
			moveCharacter(); 	    // moves the character, collision detection, physics, etc

		default: slideICE();		// logic for ice movement
		}
		break;

	}
}

void puzzleControls1()
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

		if ((digit1 == NUM_NIL) && (digit2 == NUM_NIL))
		{
			puzzle1Input = 0;
		}
		else if ((digit1 != NUM_NIL) && (digit2 == NUM_NIL))
		{
			puzzle1Input = d1;
		}
		else if ((digit1 != NUM_NIL) && (digit2 != NUM_NIL))
		{
			puzzle1Input = ((d1 * 10) + d2);
		}

		//now check answer.
		puzzle1Ans = puzzle1Integer1 * puzzle1Integer2;

		//	if (puzzle1Input != puzzle1Ans) //wrong ans
		//		g_eGameState = S_SPLASHSCREEN; //output for now, in future replace with a more suitable sequence.

		if (puzzle1Ans == puzzle1Input) {	// correct answer
			g_eGameState = S_GAME;
			loadData();
			puzzle1Integer1 = rand() % 9 + 1;
			puzzle1Integer2 = rand() % 9 + 1;
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

void renderEncounterMonster()
{
	renderMonster();
	COORD c = g_Console.getConsoleSize();
	c.X = 0;
	c.Y = 15;

	switch (P_TYPE)
	{
	case PUZZLE_SLIME: g_Console.writeToBuffer(c, "--------------------------------------------------------------------------------", 0x02);

		break;
	default: break;
	}

	renderMonsterPuzzle();
	renderPuzzlePosition();
}

void renderMonster()
{

	switch (P_TYPE)
	{
	case PUZZLE_SLIME: renderMonster1();
		break;
	case PUZZLE_ICE: //renderMonster2(); // TO BE ADDED LATER.
		break;
	default: return;
	}
}

void renderMonster1()
{
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

void renderMonsterPuzzle()
{

	switch (P_TYPE)
	{
	case PUZZLE_SLIME: renderMonsterPuzzle1();
		break;
	case PUZZLE_ICE:

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
				case PORTAL: g_Console.writeToBuffer(c, r, char(219), 0x0A);

				}

			}
		}
	default:
		break;
	}

}

void renderMonsterPuzzle1()
{
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

void renderPuzzlePosition() {
	g_Console.writeToBuffer(g_sChar.m_cLocation, (char)158, charColor);
}
	