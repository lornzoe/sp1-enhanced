#include "monsterData.h"
#include "Framework\console.h"

// this is more like a database to store monster's data.
// What's stored: 
// - Monster appearance (function: monsterMONSTER)
// - Puzzle type.		(function: puzzleMONSTER)

//Puzzle type will work so that it can be pulled out from an enum. After that the 

Console g_Monster(80, 25, "SP1 Framework");

void monsterSlime()
{
	monsterData Slime;

	Slime.monsterImage1 = "           `......`           ";
	Slime.monsterImage2 = "        -ossoo+++///:-`       ";
	Slime.monsterImage3 = "      `shhyyss++o+/////.      ";
	Slime.monsterImage4 = "      ohyyso(----)////:/`     ";
	Slime.monsterImage5 = "     `yyyyssssso+++/////-     ";
	Slime.monsterImage6 = "     `yyysosyyso+////:-/-     ";
	Slime.monsterImage7 = "     `yysoossoo+/:://:-/-     ";
	Slime.monsterImage8 = "     :syssooo//+/://:-:::     ";
	Slime.monsterImage9 = "   `oo+oyyso+/+////:-.:+/:`   ";
	Slime.monsterImage10 = "   :yys++os+:/++////::/s+:-   ";
	Slime.monsterImage11 = "    :syyssso++////-..:sy//-   ";
	Slime.monsterImage12 = "    `syyyso//::///-..-oy+:`   ";
	Slime.monsterImage13 = "     osssss+///++/:://///`    ";
	Slime.monsterImage14 = "     .+osssso+++/::--//+-     ";
	Slime.monsterImage15 = "   ``.---:///:::---..--.      ";

	COORD c = g_Monster.getConsoleSize();
	c.X = g_Monster.getConsoleSize().X / 2 - 15;
	c.Y = 0;
	g_Monster.writeToBuffer(c, Slime.monsterImage1, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage2, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage3, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage4, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage5, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage6, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage7, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage8, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage9, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage10, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage11, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage12, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage13, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage14, 0x02);
	c.Y += 1;
	g_Monster.writeToBuffer(c, Slime.monsterImage15, 0x02);
}

void lineBreak() {
	COORD c = g_Monster.getConsoleSize();
	c.X = 0;
	c.Y = 15;
	for (int x = 0; x = 80; x++) {
		g_Monster.writeToBuffer(c, "-", 0x02);
	}
}

void renderEncounterMonster(){
	//function encounterMonster
	/*
	TO DO:
	- At start up, use RNG to decide what monster is being sent out

	- Print out (Monster).monsterImage1-15 on the screen
	- Print out line break

	(tbc: use rng to decide what puzzle is used; use enum + switch to decide what game will be used when encountering the monster)
	- Print out puzzle

	- return boolean; if false, player dies ( and consequentally restarts). if true, player exits the screen and the monster that was encountered is now deleted from the map
	*/
}