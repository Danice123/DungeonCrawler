#include "Dungeon.h"

const std::string images[] = { "img/wall.bmp", "img/floor.bmp", "img/person.png" };
const int nTextures = 3;

//=============================================================================
// Constructor
//=============================================================================
Dungeon::Dungeon() {
	textures = new TextureManager[nTextures];
}

//=============================================================================
// Destructor
//=============================================================================
Dungeon::~Dungeon()
{
    releaseAll();           // call onLostDevice() for every graphics item
}

void Dungeon::loadFloor(int floor) {
	mapImg = new Image*[gen.getFloor(floor).getHeight()];
	for (int i = 0; i < gen.getFloor(floor).getHeight(); i++) {
		mapImg[i] = new Image[gen.getFloor(floor).getWidth()];
		for (int j = 0; j < gen.getFloor(floor).getWidth(); j++) {
			mapImg[i][j].initialize(graphics, 0, 0, 0, &textures[gen.getFloor(floor).getTile(j, i)]);
		}
	}
}

//=============================================================================
// Initializes the game
// Throws GameError on error
//=============================================================================
void Dungeon::initialize(HWND hwnd) {
    Game::initialize(hwnd); // throws GameError
	for (int i = 0; i < nTextures; i++) {
		if (!textures[i].initialize(graphics, images[i].c_str())) 
			throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing texture"));
	}
	gen.loadMonsters();
	gen.loadItems();
	srand(100);
	gen.generateRandom(1);
	for (int i = 0; i < gen.getAmountFloors(); i++) gen.getFloor(i).genFloorLayout();
	floor = 0;
	loadFloor(floor);

	player.initialize(this, 0, 0, 0, &textures[2]);

	player.setX(GAME_WIDTH / 2);
	player.setY(GAME_HEIGHT / 2);
	px = gen.getFloor(floor).sx;
	py = gen.getFloor(floor).sy;
}

bool isMoving;
int newX;
int newY;

//=============================================================================
// Update all game items
//=============================================================================
void Dungeon::update()
{
	if (input->wasKeyPressed(VK_UP)) {
		//if (gen.getFloor(0).getTile(px, py - 1) == 0) return;
		py--;
	}
	if (input->wasKeyPressed(VK_DOWN)) {
		//if (gen.getFloor(0).getTile(px, py + 1) == 0) return;
		py++;
	}
	if (input->wasKeyPressed(VK_RIGHT)) {
		//if (gen.getFloor(0).getTile(px + 1, py) == 0) return;
		px++;
	}
	if (input->wasKeyPressed(VK_LEFT)) {
		//if (gen.getFloor(0).getTile(px - 1, py) == 0) return;
		px--;
	}
	/*if (!isMoving) {
		if (input->wasKeyPressed(VK_UP)) {
			
			isMoving = true;
			newX = player.getX();
			newY = player.getY() - 32;
		} else if (input->wasKeyPressed(VK_DOWN)) {
			if (gen.getFloor(0).getTile((int) (player.getY() + 32) / 32, (int) player.getX() / 32) == 0) return;
			isMoving = true;
			newX = player.getX();
			newY = player.getY() + 32;
		} else if (input->wasKeyPressed(VK_LEFT)) {
			if (gen.getFloor(0).getTile((int) player.getY() / 32, (int) (player.getX() - 32) / 32) == 0) return;
			isMoving = true;
			newX = player.getX() - 32;
			newY = player.getY();
		} else if (input->wasKeyPressed(VK_RIGHT)) {
			if (gen.getFloor(0).getTile((int) player.getY() / 32, (int) (player.getX() + 32) / 32) == 0) return;
			isMoving = true;
			newX = player.getX() + 32;
			newY = player.getY();
		}
	} else {
		player.setVelocity(VECTOR2(0, 0));
		if (newY < player.getY()) player.setVelocity(VECTOR2(0, -2));
		if (newY > player.getY()) player.setVelocity(VECTOR2(0, 2));
		if (newX < player.getX()) player.setVelocity(VECTOR2(-2, 0));
		if (newX > player.getX()) player.setVelocity(VECTOR2(2, 0));

		player.setX(player.getX() + player.getVelocity().x);
		player.setY(player.getY() + player.getVelocity().y);
		if (newY == player.getY() && newX == player.getX()) isMoving = false;
	}

	player.update(frameTime);*/
}

//=============================================================================
// Artificial Intelligence
//=============================================================================
void Dungeon::ai()
{

}

//=============================================================================
// Handle collisions
//=============================================================================
void Dungeon::collisions()
{

}

//=============================================================================
// Render game items
//=============================================================================
void Dungeon::render()
{
	graphics->spriteBegin();
	for (int i = 0; i < gen.getFloor(floor).getHeight(); i++)
		for (int j = 0; j < gen.getFloor(floor).getWidth(); j++) {
			int xoffset = px * 32;
			int yoffset = py * 32;
			mapImg[i][j].setX(j * 32 - xoffset - GAME_WIDTH / 2);
			mapImg[i][j].setY(i * 32 - yoffset - GAME_HEIGHT / 2);
			mapImg[i][j].draw();
		}
			

	player.draw();
	graphics->spriteEnd();
}

//=============================================================================
// The graphics device was lost.
// Release all reserved video memory so graphics device may be reset.
//=============================================================================
void Dungeon::releaseAll()
{
    Game::releaseAll();
}

//=============================================================================
// The grahics device has been reset.
// Recreate all surfaces.
//=============================================================================
void Dungeon::resetAll()
{
    Game::resetAll();
}