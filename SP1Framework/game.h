#ifndef _GAME_H
#define _GAME_H

#define MON_DETECT_RANGE_X 15
#define MON_DETECT_RANGE_Y 5
#define BLANK_SPACE 48

#include "Framework\timer.h"
#include <fstream>

extern CStopWatch g_swTimer;
extern bool g_bQuitGame;

// Enumeration to store the control keys that your game will have
enum EKEYS
{
    K_UP,
    K_DOWN,
    K_LEFT,
    K_RIGHT,
    K_ESCAPE,
    K_SPACE,
 	K_ONE,
	K_TWO,
	K_THREE,
	K_FOUR,
	K_FIVE,
	//adding keys 6-0, enter + backspace for encounterMonster.
	K_SIX,
	K_SEVEN,
	K_EIGHT,
	K_NINE,
	K_ZERO,
	K_ENTER,
	K_BACKSPACE,
	K_COUNT // unsure what this does, leaving it at the end.

};

// Enumeration for the different screen states
enum EGAMESTATES
{
    S_SPLASHSCREEN,
    S_GAME,
	S_LEVELSELECT,
	S_CHARACTERCOLOR,
	S_COUNT
};

enum ELEVELS
{
	L_LEVELONE,
	L_LEVELTWO,
	L_LEVELTHREE,
	L_LEVELFOUR,
	L_LEVELFIVE,
	L_COUNT
};
enum MON_IDLE
{
	MON_NOTHING,
	MON_UP,
	MON_DOWN,
	MON_LEFT,
	MON_RIGHT
};

// struct for the game character
struct SGameChar
{
    COORD m_cLocation;
    bool  m_bActive;
};


void init        ( void );      // initialize your variables, allocate memory, etc
void getInput    ( void );      // get input from player
void update      ( double dt ); // update the game and the state of the game
void render      ( void );      // renders the current state of the game to the console
void shutdown    ( void );      // do clean up, free memory

void splashScreenWait();    // waits for time to pass in splash screen
void splashScreenSelect();	// handles keypresses in main menu
void renderLevelSelectBG(); // handles level select 
void LevelScreenSelect();	// handles the key for level select
void renderCharacterColour();	// renders character colour select screen
void charactercolourselect();	//handles logic for character colour select screen
void renderLevelSelect();	// handles renderlevelselectBG
void gameplay();            // gameplay logic
void moveCharacter();       // moves the character, collision detection, physics, etc
void processUserInput();    // checks if you should change states or do something else with the game, e.g. pause, exit
void clearScreen();         // clears the current screen and draw from scratch 
void renderSplashScreen();  // renders the splash screen
void renderGame();          // renders the game stuff
void renderMap();           // renders the map to the buffer first
void renderCharacter();     // renders the character into the buffer
void renderFramerate();     // renders debug information, frame rate, elapsed time, etc
void renderToScreen();      // dump the contents of the buffer to the screen, one frame worth of game
void monsterAI();			// The monster's actions
void monsterLOC();			// renders all monsters' location
void monsterONE();			// monster number 1 position and looks
void loadMap();				// loads text file into array
#endif // _GAME_H