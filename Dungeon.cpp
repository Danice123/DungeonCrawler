 #include "Dungeon.h"

const std::string images[] = { "img/tiles.png", "img/aliens.png", "img/hero_sprite_sheet.png", "img/chest.png", "img/red.png", "img/green.png", "img/menuBG - Nathan Snyder.png", "img/health_potion.png"};
const int nTextures = 8;



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
	this->floor = floor;
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

	player.x = gen.getFloor(floor).sx;
	player.y = gen.getFloor(floor).sy;
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

	redBar.initialize(graphics, 0, 0, 0, &textures[4]);
	redBar.setX(0);
	redBar.setY(GAME_HEIGHT - 20);
	redBar.setScaleX(200);
	redBar.setScaleY(20);
	greenBar.initialize(graphics, 0, 0, 0, &textures[5]);
	greenBar.setX(0);
	greenBar.setY(GAME_HEIGHT - 20);
	greenBar.setScaleX(100);
	greenBar.setScaleY(20);

	gen.loadMonsters();
	gen.loadItems();

	pm.initialize(graphics);
	pm.setCurrentFrame(0, 0);
	menuBG.initialize(graphics, 1, 1, 10, &textures[6]);


	for (int i = 0; i < 100; i++) {
		monsters[i].initialize(this, 32, 32, 3, &textures[1]);
		if(i%2){
			monsters[i].setFrameDelay(2.0f);
			monsters[i].setFrames(0,2);
		}
		else{
			monsters[i].setFrameDelay(2.0f);
			monsters[i].setFrames(3,5);
		}
	}
	for (int i = 0; i < 100; i++) items[i].initialize(this, 0, 0, 0, &textures[3]);

	player.initialize(this, 50, 50, 11, &textures[2]);
	player.setScale(32.0f/50.0f);
	player.setFrameDelay(0.1f);
	player.setCurrentFrame(3);
	player.setX(GAME_WIDTH / 2);
	player.setY(GAME_HEIGHT / 2 - 16);

	activeMenu = false;
	inventory = new Menu();
	inventory->initialize(graphics, input, NULL, &(player.getInventory()), "Inventory");
	menuBG.setScaleX(200);
	menuBG.setScaleY(GAME_HEIGHT);
	menuBG.setX(inventory->getAnchorX()-20);
	gameStates = SPLASH_SCREEN;

	mainMenu = new Menu();
	mainMenu->initialize(graphics, input, NULL);

	mainMenu->setMenuHeading("Raiders of the Lost Dungeon");

	std::vector<std::string> menuItems;
	menuItems.push_back("New Game");	// Menu 1
	menuItems.push_back("Exit Game");	// Menu 2
	mainMenu->setMenuItems(menuItems);
	timeInState = 0;
}

bool turnTaken = false;
//=============================================================================
// Update all game items
//=============================================================================
void Dungeon::update()
{
	gameStateUpdate();
	switch(gameStates) {
	case LEVEL1:
	case LEVEL2:
	case LEVEL3:
	case LEVEL4:
	case LEVEL5:
		if(activeMenu) {
			inventory->update();
			if(inventory->getMenuState() != -1) {
				switch(player.getInventory()[inventory->getMenuState()].getType()) {
				case 0:	// Weapon
					player.setEquippedWeapon(inventory->getMenuState());
					break;
				case 1:	// Armor
					player.setEquippedArmor(inventory->getMenuState());
					break;
				case 2:	// Health
					player.setHealth(player.getHealth()+player.getInventory()[inventory->getMenuState()].getValue());
					player.getInventory().erase(player.getInventory().begin() + inventory->getMenuState());
					break;
				}
			}
		} else {
			if (player.getHealth() <= 0) { //Death
				gameStates = START_MENU;
				return;
			}
			if (input->wasKeyPressed(VK_UP) && gen.getFloor(floor).getTile(player.x, player.y - 1) != 0) {
				if (gen.getFloor(floor).getMonster(player.x, player.y - 1) != 0) {
					MonsterInstance* m = gen.getFloor(floor).getMonster(player.x, player.y - 1);
					int damage = player.getAttack() - m->getArmor();
					m->setCurrentHealth(m->getCurrentHealth() - damage);
					pm.createParticleEffect(VECTOR2(player.getCenterX(), player.getCenterY()), VECTOR2(0,-100), 1); 
				} else {
					player.y--;
					if (gen.getFloor(floor).getItem(player.x, player.y) != 0) {
						ItemInstance* i = gen.getFloor(floor).getItem(player.x, player.y);
						player.getInventory().push_back(*i);
						i->isOnGround = false;
					}
					if (gen.getFloor(floor).getTile(player.x, player.y) == 9) loadFloor(floor + 1);
				}
				turnTaken = true;
			}
			if (input->wasKeyPressed(VK_DOWN) && gen.getFloor(floor).getTile(player.x, player.y + 1) != 0) {
				if (gen.getFloor(floor).getMonster(player.x, player.y + 1) != 0) {
					MonsterInstance* m = gen.getFloor(floor).getMonster(player.x, player.y + 1);
					int damage = player.getAttack() - m->getArmor();
					m->setCurrentHealth(m->getCurrentHealth() - damage);
					pm.createParticleEffect(VECTOR2(player.getCenterX(), player.getCenterY()), VECTOR2(0,-100), 1);
				} else {
					player.y++;
					if (gen.getFloor(floor).getItem(player.x, player.y) != 0) {
						ItemInstance* i = gen.getFloor(floor).getItem(player.x, player.y);
						player.getInventory().push_back(*i);
						i->isOnGround = false;
					}
					if (gen.getFloor(floor).getTile(player.x, player.y) == 9) loadFloor(floor + 1);
				}
				turnTaken = true;
			}
			if (input->wasKeyPressed(VK_RIGHT) && gen.getFloor(floor).getTile(player.x + 1, player.y) != 0) {
				player.setFrames(0, 10);
				player.setFacingRight(true);
				player.flipHorizontal(!player.isFacingRight());
				if (gen.getFloor(floor).getMonster(player.x + 1, player.y) != 0) {
					MonsterInstance* m = gen.getFloor(floor).getMonster(player.x + 1, player.y);
					int damage = player.getAttack() - m->getArmor();
					m->setCurrentHealth(m->getCurrentHealth() - damage);
					pm.createParticleEffect(VECTOR2(player.getCenterX(), player.getCenterY()), VECTOR2(0,-100), 1);
				} else {
					player.x++;
					if (gen.getFloor(floor).getItem(player.x, player.y) != 0) {
						ItemInstance* i = gen.getFloor(floor).getItem(player.x, player.y);
						player.getInventory().push_back(*i);
						i->isOnGround = false;
					}
					if (gen.getFloor(floor).getTile(player.x, player.y) == 9) loadFloor(floor + 1);
				}
				turnTaken = true;
			}
			if (input->wasKeyPressed(VK_LEFT) && gen.getFloor(floor).getTile(player.x - 1, player.y) != 0) {
				player.setFrames(0,10);	
				player.setFacingRight(false);
				player.flipHorizontal(!player.isFacingRight());

				if (gen.getFloor(floor).getMonster(player.x - 1, player.y) != 0) {
					MonsterInstance* m = gen.getFloor(floor).getMonster(player.x - 1, player.y);
					int damage = player.getAttack() - m->getArmor();
					m->setCurrentHealth(m->getCurrentHealth() - damage);
					pm.createParticleEffect(VECTOR2(player.getCenterX(), player.getCenterY()), VECTOR2(0,-100), 1);
				} else {
					player.x--;
					if (gen.getFloor(floor).getItem(player.x, player.y) != 0) {
						ItemInstance* i = gen.getFloor(floor).getItem(player.x, player.y);
						player.getInventory().push_back(*i);
						i->isOnGround = false;
					}
					if (gen.getFloor(floor).getTile(player.x, player.y) == 9) loadFloor(floor + 1);
				}
				turnTaken = true;
			}
		}
		if (input->wasKeyPressed(VK_ESCAPE)) {
			activeMenu = !activeMenu;
		}
		player.update(frameTime);
		greenBar.setScaleX((player.getHealth() / (float)player.getMaxHealth()) * 200);
		for(int a = 0; a < gen.getFloor(floor).getMonsters().size(); a++){
			monsters[a].update(frameTime);
		}
		pm.update(frameTime);
		break;	// End case

	case SPLASH_SCREEN:

		break;
	case START_MENU:
		mainMenu->update();
		switch(mainMenu->getMenuState()) {
		case NEW_GAME:
			{
			srand(time(0));
			gen.generateRandom(5);
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
			loadFloor(0);
			player.setHealth(player.getMaxHealth());
			player.getInventory().clear();
			player.getInventory().push_back(ItemInstance(1, gen.getItemList()[0]));
			player.setEquippedWeapon(0);
			player.setEquippedArmor(-1);
			gameStates = LEVEL1;
			}
			break;
		case EXIT_GAME:
			exitGame();
			break;
		}
		break;
	}
}

//=============================================================================
// Artificial Intelligence
//=============================================================================
void Dungeon::ai()
{
	if (turnTaken) {
		for (int i = 0; i < gen.getFloor(floor).getMonsters().size(); i++) {
			if (gen.getFloor(floor).getMonsters()[i].getCurrentHealth() <= 0) continue;
			int mx = gen.getFloor(floor).getMonsters()[i].getX();
			int my = gen.getFloor(floor).getMonsters()[i].getY();
			int distance = sqrt(pow(player.x - mx, 2) + pow(player.y - my, 2));
			if (distance < 10) {
				AStar a(&gen.getFloor(floor), mx, my, player.x, player.y);
				a.run();
				std::pair<int, int> c = a.getNextStep();
				if (gen.getFloor(floor).getMonster(c.first, c.second) == 0)
					if (player.x == c.first && player.y == c.second) {
						int damage = gen.getFloor(floor).getMonsters()[i].getAttack() - player.getArmor();
						player.setHealth(player.getHealth() - damage);
					} else {
						gen.getFloor(floor).getMonsters()[i].setCoords(c.first, c.second);
					}
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
	graphics->spriteBegin();
	int xoffset = player.x - GAME_WIDTH / 64;
	int yoffset = player.y - GAME_HEIGHT / 64;

	switch(gameStates) {
	case LEVEL1:
	case LEVEL2:
	case LEVEL3:
	case LEVEL4:
	case LEVEL5:	

		for (int i = 0; i < gen.getFloor(floor).getHeight(); i++)
			for (int j = 0; j < gen.getFloor(floor).getWidth(); j++) {
				mapImg[i][j].setX((j - xoffset) * 32);
				mapImg[i][j].setY((i - yoffset) * 32);
				mapImg[i][j].draw();
			}

		for (int i = 0; i < gen.getFloor(floor).getMonsters().size(); i++) {
			if (gen.getFloor(floor).getMonsters()[i].getCurrentHealth() <= 0) continue;
			monsters[i].setX((gen.getFloor(floor).getMonsters()[i].getX() - xoffset) * 32);
			monsters[i].setY((gen.getFloor(floor).getMonsters()[i].getY() - yoffset) * 32);
			monsters[i].draw();
		}

		for (int i = 0; i < gen.getFloor(floor).getItems().size(); i++) {
			if (!gen.getFloor(floor).getItems()[i].isOnGround) continue;
			items[i].setX((gen.getFloor(floor).getItems()[i].getX() - xoffset) * 32);
			items[i].setY((gen.getFloor(floor).getItems()[i].getY() - yoffset) * 32);
			items[i].draw();
		}

		player.draw();
		redBar.draw();
		greenBar.draw();
		pm.draw();
		if(activeMenu) {
			menuBG.draw();
			inventory->displayMenu(frameTime,player.getEquippedArmor(),player.getEquippedWeapon());
		}
		break;

	case SPLASH_SCREEN:
		break;

	case START_MENU:
		mainMenu->displayMenu(frameTime);
		break;
	}
	graphics->spriteEnd();
}

void Dungeon::gameStateUpdate()
{
	timeInState += frameTime;
	switch(gameStates) {
	if(gameStates != START_MENU)
		
	case SPLASH_SCREEN:
		if(input->wasKeyPressed(VK_SPACE) || timeInState>1.0f) {
			gameStates = START_MENU;
			//timeInState = 0;
		}
		break;
	case START_MENU:

		break;
	case LEVEL1:
		timeInState = 0;
		break;
	}
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

