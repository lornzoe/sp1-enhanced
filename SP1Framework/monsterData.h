#include <iostream>
#include <string>

void renderEncounterMonster();
void lineBreak();

struct monsterData {
	const char *monsterImage1;
	const char *monsterImage2;
	const char *monsterImage3;
	const char *monsterImage4;
	const char *monsterImage5;
	const char *monsterImage6;
	const char *monsterImage7;
	const char *monsterImage8;
	const char *monsterImage9;
	const char *monsterImage10;
	const char *monsterImage11;
	const char *monsterImage12;
	const char *monsterImage13;
	const char *monsterImage14;
	const char *monsterImage15;

};

// List down monsters: void monsterMONSTER() . Follow from Slime.
void monsterSlime();


// type of puzzle that will be used when encountering monster
enum puzzleType {
	PUZZLE_0 //slime puzzle

};

//List down puzzles: void 