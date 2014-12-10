// Programming 2D Games
// Copyright (c) 2011,2012 by: 
// Charles Kelly
// StickFight.h v1.0

#ifndef _COLLISION_TYPES_H      // Prevent multiple definitions if this 
#define _COLLISION_TYPES_H      // file is included in more than one place
#define WIN32_LEAN_AND_MEAN

class Dungeon;

#include "game.h"
#include "textureManager.h"
#include "image.h"
#include "textDX.h"
#include "entity.h"
#include "DungeonGen.h"
#include "MonsterEntity.h"
#include "astar.h"
#include <cmath>
#include "Hero.h"

//=============================================================================
// This class is the core of the game
//=============================================================================
class Dungeon : public Game
{
public:
    // Constructor
    Dungeon();

    // Destructor
    virtual ~Dungeon();

    // Initialize the game
    void initialize(HWND hwnd);
    void update();      // must override pure virtual from Game
    void ai();          // "
    void collisions();  // "
    void render();      // "
    void releaseAll();
    void resetAll();
	void gameStateUpdate();
private:
	TextureManager* textures;
	TextDX text;
	bool keyDownLastFrame;
	DungeonGen gen;
	void loadFloor(int);
	Image** mapImg;
	MonsterEntity monsters[100];
	Entity items[100];
	int floor;
	int px, py;


	Hero player;


	GameStates gameStates;
	float timeInState;

};

#endif
