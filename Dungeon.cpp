#include "Dungeon.h"

const std::string images[] = { "img/tiles.png", "img/person.png" };
const int nTextures = 2;

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
	for (int i = 0; i < gen.getFloor(floor).getHeight(); i++)
		for (int j = 0; j < gen.getFloor(floor).getWidth(); j++)
			switch (gen.getFloor(floor).getTile(j, i)) {
			case 0:
				if (i != gen.getFloor(floor).getHeight() - 1 && gen.getFloor(floor).getTile(j, i + 1))
					mapImg[i][j].setCurrentFrame(1);
				else
					mapImg[i][j].setCurrentFrame(0);
				break;
			case 1:
				mapImg[i][j].setCurrentFrame(2);
				break;
			case 9:
				mapImg[i][j].setCurrentFrame(10);
				break;
			}

	for (int i = 0; i < gen.getFloor(floor).getMonsters().size(); i++) {
		//set monster sprite
	}

	px = gen.getFloor(floor).sx;
	py = gen.getFloor(floor).sy;
}

#include <time.h>
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
	srand(time(0));
	gen.generateRandom(1);

	int maxHeight = 0;
	int maxWidth = 0;
	for (int i = 0; i < gen.getAmountFloors(); i++) {
		gen.getFloor(i).genFloorLayout();
		if (maxHeight < gen.getFloor(i).getHeight()) maxHeight = gen.getFloor(i).getHeight();
		if (maxWidth < gen.getFloor(i).getWidth()) maxWidth = gen.getFloor(i).getWidth();
	}

	mapImg = new Image*[maxHeight];
	for (int i = 0; i < maxHeight; i++) {
		mapImg[i] = new Image[maxWidth];
		for (int j = 0; j < maxWidth; j++) {
			mapImg[i][j].initialize(graphics, 32, 32, 10, &textures[0]);
		}
	}

	for (int i = 0; i < 100; i++) monsters[i].initialize(this, 0, 0, 0, &textures[1]);

	floor = 0;
	loadFloor(floor);

	player.initialize(this, 0, 0, 0, &textures[1]);

	player.setX(GAME_WIDTH / 2);
	player.setY(GAME_HEIGHT / 2 - 16);
}

bool turnTaken = false;
//=============================================================================
// Update all game items
//=============================================================================
void Dungeon::update()
{
	if (input->wasKeyPressed(VK_UP) && gen.getFloor(0).getTile(px, py - 1) != 0 
		&& gen.getFloor(floor).getMonster(px, py - 1) == 0) {
		py--;
		turnTaken = true;
	}
	if (input->wasKeyPressed(VK_DOWN) && gen.getFloor(0).getTile(px, py + 1) != 0
		&& gen.getFloor(floor).getMonster(px, py + 1) == 0) {
		py++;
		turnTaken = true;
	}
	if (input->wasKeyPressed(VK_RIGHT) && gen.getFloor(0).getTile(px + 1, py) != 0
		&& gen.getFloor(floor).getMonster(px + 1, py) == 0) {
		px++;
		turnTaken = true;
	}
	if (input->wasKeyPressed(VK_LEFT) && gen.getFloor(0).getTile(px - 1, py) != 0
		&& gen.getFloor(floor).getMonster(px - 1, py) == 0) {
		px--;
		turnTaken = true;
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
	if (turnTaken) {
		for (int i = 0; i < gen.getFloor(floor).getMonsters().size(); i++) {
			int mx = gen.getFloor(floor).getMonsters()[i].getX();
			int my = gen.getFloor(floor).getMonsters()[i].getY();
			int distance = sqrt(pow(px - mx, 2) + pow(py - my, 2));
			if (distance < 10) {
				AStar a(&gen.getFloor(floor), mx, my, px, py);
				a.run();
				std::pair<int, int> c = a.getNextStep();
				if (gen.getFloor(floor).getMonster(c.first, c.second) == 0 && (px != c.first && py != c.second))
					gen.getFloor(floor).getMonsters()[i].setCoords(c.first, c.second);
			}
		}
		turnTaken = false;
	}
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
	int xoffset = px - GAME_WIDTH / 64;
	int yoffset = py - GAME_HEIGHT / 64;
	graphics->spriteBegin();
	for (int i = 0; i < gen.getFloor(floor).getHeight(); i++)
		for (int j = 0; j < gen.getFloor(floor).getWidth(); j++) {
			mapImg[i][j].setX((j - xoffset) * 32);
			mapImg[i][j].setY((i - yoffset) * 32);
			mapImg[i][j].draw();
		}

	for (int i = 0; i < gen.getFloor(floor).getMonsters().size(); i++) {
		monsters[i].setX((gen.getFloor(floor).getMonsters()[i].getX() - xoffset) * 32);
		monsters[i].setY((gen.getFloor(floor).getMonsters()[i].getY() - yoffset) * 32);
		monsters[i].draw();
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