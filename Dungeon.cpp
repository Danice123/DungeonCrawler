#include "Dungeon.h"

const std::string images[] = { "img/wall.bmp", "img/floor.bmp", "img/person.png" };
const int nTextures = 3;

const int map[10][10] ={{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
						{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
						{0, 0, 0, 0, 1, 1, 0, 0, 0, 0},
						{0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
						{0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
						{0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
						{0, 0, 0, 1, 1, 1, 1, 0, 0, 0},
						{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
						{0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
						{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}};

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

	mapImg = new Image*[10];
	for (int i = 0; i < 10; i++) {
		mapImg[i] = new Image[10];
		for (int j = 0; j < 10; j++) {
			mapImg[i][j].initialize(graphics, 0, 0, 0, &textures[map[i][j]]);
			mapImg[i][j].setX(j * 32);
			mapImg[i][j].setY(i * 32);
		}
	}

	player.initialize(this, 0, 0, 0, &textures[2]);

	player.setX(5 * 32);
	player.setY(5 * 32);
}

bool isMoving;
int newX;
int newY;

//=============================================================================
// Update all game items
//=============================================================================
void Dungeon::update()
{
	if (!isMoving) {
		if (input->wasKeyPressed(VK_UP)) {
			if (map[(int) (player.getY() - 32) / 32][(int) player.getX() / 32] == 0) return;
			isMoving = true;
			newX = player.getX();
			newY = player.getY() - 32;
		} else if (input->wasKeyPressed(VK_DOWN)) {
			if (map[(int) (player.getY() + 32) / 32][(int) player.getX() / 32] == 0) return;
			isMoving = true;
			newX = player.getX();
			newY = player.getY() + 32;
		} else if (input->wasKeyPressed(VK_LEFT)) {
			if (map[(int) player.getY() / 32][(int) (player.getX() - 32) / 32] == 0) return;
			isMoving = true;
			newX = player.getX() - 32;
			newY = player.getY();
		} else if (input->wasKeyPressed(VK_RIGHT)) {
			if (map[(int) player.getY() / 32][(int) (player.getX() + 32) / 32] == 0) return;
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

	player.update(frameTime);
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
	for (int i = 0; i < 10; i++)
		for (int j = 0; j < 10; j++)
			mapImg[i][j].draw();

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