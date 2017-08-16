// This is the main file for the game logic and function
//
//test
#include "game.h"
#include "Framework\console.h"
#include <iostream>
#include <iomanip>
#include <sstream>

double  g_dElapsedTime;
double  g_dDeltaTime;
bool    g_abKeyPressed[K_COUNT];
WORD charColor = 0x02; // initialise character colour
COORD monONE;



// Game specific variables here
SGameChar   g_sChar;
EGAMESTATES g_eGameState = S_SPLASHSCREEN;
ELEVELS g_currentlevel;
double  g_dBounceTime; // this is to prevent key bouncing, so we won't trigger keypresses more than once

// Console object
Console g_Console(80, 25, "SP1 Framework");


//--------------------------------------------------------------
// Purpose  : Initialisation function
//            Initialize variables, allocate memory, load data from file, etc. 
//            This is called once before entering into your main loop
// Input    : void
// Output   : void
//--------------------------------------------------------------
void init( void )
{
    // Set precision for floating point output
    g_dElapsedTime = 0.0;
    g_dBounceTime = 0.0;

    // sets the initial state for the game
    g_eGameState = S_SPLASHSCREEN;


    g_sChar.m_cLocation.X = g_Console.getConsoleSize().X / 2;
    g_sChar.m_cLocation.Y = g_Console.getConsoleSize().Y / 2;
    g_sChar.m_bActive = true;

	monONE.X = (g_Console.getConsoleSize().X / 2) +25;
	monONE.Y = (g_Console.getConsoleSize().Y / 2);
    // sets the width, height and the font name to use in the console
    g_Console.setConsoleFont(0, 16, L"Consolas");
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
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_TWO])
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELTWO;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_THREE])
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELTHREE;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_FOUR])
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELFOUR;
		bSomethingHappened = true;
	}
	if (g_abKeyPressed[K_FIVE])
	{
		g_eGameState = S_GAME;
		g_currentlevel = L_LEVELFIVE;
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

void gameplay()            // gameplay logic
{
    processUserInput(); // checks if you should change states or do something else with the game, e.g. pause, exit
    moveCharacter();    // moves the character, collision detection, physics, etc
	monsterAI();       // moves the monster to the player's character's location
}

void moveCharacter()
{
    bool bSomethingHappened = false;
    if (g_dBounceTime > g_dElapsedTime)
        return;

    // Updating the location of the character based on the key press
    // providing a beep sound whenver we shift the character
    if (g_abKeyPressed[K_UP] && g_sChar.m_cLocation.Y > 0)
    {
        //Beep(1440, 30);
        g_sChar.m_cLocation.Y--;
        bSomethingHappened = true;
		monsterAI();
    }
    if (g_abKeyPressed[K_LEFT] && g_sChar.m_cLocation.X > 0)
    {
        //Beep(1440, 30);
        g_sChar.m_cLocation.X--;
        bSomethingHappened = true;
		monsterAI();
    }
    if (g_abKeyPressed[K_DOWN] && g_sChar.m_cLocation.Y < g_Console.getConsoleSize().Y - 1)
    {
        //Beep(1440, 30);
        g_sChar.m_cLocation.Y++;
        bSomethingHappened = true;
		monsterAI();
	}
    if (g_abKeyPressed[K_RIGHT] && g_sChar.m_cLocation.X < g_Console.getConsoleSize().X - 1)
    {
        //Beep(1440, 30);
        g_sChar.m_cLocation.X++;
        bSomethingHappened = true;
		monsterAI();
	}
    if (g_abKeyPressed[K_SPACE])
    {
        g_sChar.m_bActive = !g_sChar.m_bActive;
        bSomethingHappened = true;
    }

    if (bSomethingHappened)
    {
        // set the bounce time to some time in the future to prevent accidental triggers
        g_dBounceTime = g_dElapsedTime + 0.08; 
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
		g_sChar.m_cLocation.X = g_Console.getConsoleSize().X / 2;
		g_sChar.m_cLocation.Y = g_Console.getConsoleSize().Y / 2;
		bSomethingHappened = true;
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


void renderGame()
{
    renderMap();        // renders the map to the buffer first
    renderCharacter();  // renders the character into the buffer
	monsterLOC();		// renders the monsters into the buffer
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
	level1();
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



void monsterLOC()
{
	monsterONE();
}

void monsterONE()
{
	g_Console.writeToBuffer(monONE, "M", 0x0C);
}


void monsterAI()
{
		bool bSomethingHappened = false;
		if (g_dBounceTime > g_dElapsedTime)
			return;
		if ((monONE.X >= (g_sChar.m_cLocation.X - 5)) && (monONE.X <= (g_sChar.m_cLocation.X + 5)) && (monONE.Y <= (g_sChar.m_cLocation.Y + 3)) && (monONE.Y >= (g_sChar.m_cLocation.Y - 3)))			//for the LEFT, RIGHT, TOP and BOTTOM side detection of the monster
		{
			
			if (monONE.X <= g_sChar.m_cLocation.X && monONE.X != g_sChar.m_cLocation.X)
			{
				monONE.X++;
				bSomethingHappened = true;
			}
			if (monONE.X >= g_sChar.m_cLocation.X && monONE.X != g_sChar.m_cLocation.X)
			{
				monONE.X--;
				bSomethingHappened = true;
			}
			if (monONE.Y >= g_sChar.m_cLocation.Y && monONE.Y != g_sChar.m_cLocation.Y)
			{
				monONE.Y--;
				bSomethingHappened = true;
			}
			if (monONE.Y <= g_sChar.m_cLocation.Y && monONE.Y != g_sChar.m_cLocation.Y)
			{
				monONE.Y++;
				bSomethingHappened = true;
			}
		}
		
		if (bSomethingHappened)
		{
			// set the bounce time to some time in the future to prevent accidental triggers
			g_dBounceTime = g_dElapsedTime + 0.08;
		}
	
}
