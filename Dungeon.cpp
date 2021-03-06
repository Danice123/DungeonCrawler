 #include "Dungeon.h"

const std::string images[] = { "img/tiles.png", "img/enemy_ss.png", "img/hero_sprite_sheet.png", "img/chest.png", "img/red.png", "img/green.png", "img/menuBG - Nathan Snyder.png", "img/health_potion.png", "img/menu.png", "img/gameOver.png" };
const int nTextures = 10;



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
	if (floor == 5) {
		floor = 0;
		gen.loadFromFile("Boss.txt");
		gen.getFloor(0).setNoStairs();
		for (int i = 0; i < gen.getAmountFloors(); i++) gen.getFloor(i).genFloorLayout();
		gameStates = LEVEL5;
	}
	
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
		std::string name = gen.getFloor(floor).getMonsters()[i].getName();
		if (!name.compare("Bat")) {
			monsters[i].setFrames(16, 19);
			monsters[i].setFrameDelay(0.1f);
		}
		if (!name.compare("Spider")) {
			monsters[i].setFrames(0, 3);
			monsters[i].setFrameDelay(0.1f);
		}
		if (!name.compare("Nazi")) {
			monsters[i].setFrames(20, 20);
			monsters[i].setCurrentFrame(20);
		}
		if (!name.compare("Hitler")) {
			monsters[i].setFrames(24, 24);
			monsters[i].setCurrentFrame(24);
		}
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

	graphics->setBackColor(graphicsNS::BLACK);

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
	ppm.initialize(graphics);
	ppm.setCurrentFrame(0, 0);
	menuBG.initialize(graphics, 1, 1, 1, &textures[6]);
	gameOver.initialize(graphics,640,480,1,&textures[8]);
	menuImage.initialize(graphics,640,480,1,&textures[8]);


	for (int i = 0; i < 100; i++) monsters[i].initialize(this, 32, 32, 4, &textures[1]);
	for (int i = 0; i < 100; i++) items[i].initialize(this, 0, 0, 0, &textures[3]);

	player.initialize(this, 50, 50, 11, &textures[2]);
	player.setScale(32.0f/50.0f);
	player.setFrameDelay(0.05f);
	player.setCurrentFrame(3);
	player.setX(GAME_WIDTH / 2);
	player.setY(GAME_HEIGHT / 2 - 16);
	player.setFacing(EAST);

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
	menuItems.push_back("Toggle Invincibility");
	menuItems.push_back("Exit Game");
	mainMenu->setMenuItems(menuItems);
	timeInState = 0;
	audio->playCue("themeMusic");

	bigText = new TextDX();
	if(bigText->initialize(graphics, 50, true, false, "Calibri") == false)
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menuItem font"));
	bigText->setFontColor(graphicsNS::WHITE);
	text = new TextDX();
	if(text->initialize(graphics, 20, true, false, "Calibri") == false)
        throw(GameError(gameErrorNS::FATAL_ERROR, "Error initializing menuItem font"));
	text->setFontColor(graphicsNS::WHITE);

	bodyCount = 0;
	hiScore = 0;
	birmingham = false;
	
}

bool turnTaken = false;
bool isWalking = false;
//=============================================================================
// Update all game items
//=============================================================================
void Dungeon::update()
{
	if(birmingham) player.setHealth(player.getMaxHealth());
	gameStateUpdate();
	switch(gameStates) {
	case LEVEL5:
	case LEVEL1:
	case LEVEL2:
	case LEVEL3:
	case LEVEL4:
		if(activeMenu) {
			inventory->update();
			if(inventory->getMenuState() != -1) {
				if(player.getEquippedArmor() > inventory->getMenuState()) {
					player.setEquippedArmor(player.getEquippedArmor()-1);
				}
				if(player.getEquippedWeapon() > inventory->getMenuState()) {
					player.setEquippedWeapon(player.getEquippedWeapon()-1);
				}

				switch(player.getInventory()[inventory->getMenuState()].getType()) {
				case 0:	// Weapon
					player.setEquippedWeapon(inventory->getMenuState());
					break;
				case 1:	// Armor
					player.setEquippedArmor(inventory->getMenuState());
					break;
				case 2:	// Health
					int heal = player.getHealth()+player.getInventory()[inventory->getMenuState()].getValue();
					if (heal > player.getMaxHealth()) heal = player.getMaxHealth();
					player.setHealth(heal);
					player.getInventory().erase(player.getInventory().begin() + inventory->getMenuState());
					break;
				}
			}
		} else {
			if (player.getHealth() <= 0) { //Death
				audio->playCue(DEATH);
				gameStates = GAME_OVER;
				won = false;
				return;
			}
			if (!turnTaken && !isWalking && input->wasKeyPressed(VK_SPACE)) {
				turnTaken = true;
			}
			if (!turnTaken &&!isWalking && input->wasKeyPressed(VK_UP) && gen.getFloor(floor).getTile(player.x, player.y - 1) != 0) {
				player.setFacing(NORTH);
				player.setCurrentFrame(14);
				if (gen.getFloor(floor).getMonster(player.x, player.y - 1) != 0) {
					MonsterInstance* m = gen.getFloor(floor).getMonster(player.x, player.y - 1);
					int damage = player.getAttack() - m->getArmor();
					m->setCurrentHealth(m->getCurrentHealth() - damage);
					if(m->getCurrentHealth() == 0) bodyCount++;	// Let the bodies hit the floor
					audio->playCue("hit");
					pm.setCurrentFrame(damage);
					pm.createParticleEffect(VECTOR2(player.getX(), player.getY()-32), VECTOR2(0,-100), 1);
					turnTaken = true;
				} else {
					player.offset = 0;
					isWalking = true;
				}
			}
			if (!turnTaken && !isWalking && input->wasKeyPressed(VK_DOWN) && gen.getFloor(floor).getTile(player.x, player.y + 1) != 0) {
				player.setFacing(SOUTH);
				player.setCurrentFrame(17);
				if (gen.getFloor(floor).getMonster(player.x, player.y + 1) != 0) {
					MonsterInstance* m = gen.getFloor(floor).getMonster(player.x, player.y + 1);
					int damage = player.getAttack() - m->getArmor();
					m->setCurrentHealth(m->getCurrentHealth() - damage);
					if(m->getCurrentHealth() == 0) bodyCount++;
					pm.setCurrentFrame(damage);
					audio->playCue("hit");
					pm.createParticleEffect(VECTOR2(player.getX(), player.getY()+32), VECTOR2(0,-100), 1);
					turnTaken = true;
				} else {
					player.offset = 0;
					isWalking = true;
				}
			}
			if (!turnTaken && !isWalking && input->wasKeyPressed(VK_RIGHT) && gen.getFloor(floor).getTile(player.x + 1, player.y) != 0) {
				player.setFrames(0, 10);
				if(player.getFacing() != EAST){
					player.flipHorizontal(false);
				}
				player.setFacing(EAST);
				if (gen.getFloor(floor).getMonster(player.x + 1, player.y) != 0) {
					MonsterInstance* m = gen.getFloor(floor).getMonster(player.x + 1, player.y);
					int damage = player.getAttack() - m->getArmor();
					m->setCurrentHealth(m->getCurrentHealth() - damage);
					if(m->getCurrentHealth() == 0) bodyCount++;
					pm.setCurrentFrame(damage);
					audio->playCue("hit");
					pm.createParticleEffect(VECTOR2(player.getX()+32, player.getY()), VECTOR2(0,-100), 1);
					turnTaken = true;
				} else {
					player.offset = 0;
					isWalking = true;
				}
			}
			if (!turnTaken && !isWalking && input->wasKeyPressed(VK_LEFT) && gen.getFloor(floor).getTile(player.x - 1, player.y) != 0) {
				player.setFrames(0,10);
				if(player.getFacing()!= WEST){
					player.flipHorizontal(true);
				}
				player.setFacing(WEST);
				if (gen.getFloor(floor).getMonster(player.x - 1, player.y) != 0) {
					MonsterInstance* m = gen.getFloor(floor).getMonster(player.x - 1, player.y);
					int damage = player.getAttack() - m->getArmor();
					m->setCurrentHealth(m->getCurrentHealth() - damage);
					if(m->getCurrentHealth() == 0) bodyCount++;
					pm.setCurrentFrame(damage);
					audio->playCue("hit");
					pm.createParticleEffect(VECTOR2(player.getX()-32, player.getY()), VECTOR2(0,-100), 1);
					turnTaken = true;
				} else {
					player.offset = 0;
					isWalking = true;
				}
			}
		}
		if (input->getCharIn() == 'i') {
			input->clearCharIn();
			activeMenu = !activeMenu;
		}
		greenBar.setScaleX((player.getHealth() / (float)player.getMaxHealth()) * 200);
		//Updates
		player.update(frameTime);
		for(int a = 0; a < gen.getFloor(floor).getMonsters().size(); a++){
			monsters[a].update(frameTime);
		}
		pm.update(frameTime);
		ppm.update(frameTime);
		break;	// End case

	case SPLASH_SCREEN:

		break;
	case START_MENU:
		mainMenu->update();
		switch(mainMenu->getMenuState()) {
		case NEW_GAME:
			{
			pm.setInvisibleAllParticles();
			ppm.setInvisibleAllParticles();
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
			player.getInventory().push_back(ItemInstance(gen.getItemList()[0]));
			player.setEquippedWeapon(0);
			player.setEquippedArmor(-1);
			gameStates = LEVEL1;
			bodyCount = 0;
			}
			break;
		case EXIT_GAME:
			exitGame();
			break;
		case BIRMINGHAM:
			birmingham = !birmingham;
			if(birmingham) {
				std::vector<std::string> items = mainMenu->getMenuItems();
				items[1] = "+ Toggle Invincibility";
				mainMenu->setMenuItems(items);
			}
			else {
				std::vector<std::string> items = mainMenu->getMenuItems();
				items[1] = " Toggle Invincibility";
				mainMenu->setMenuItems(items);
			}
			break;
		}
		break;
	case GAME_OVER:
		if(input->wasKeyPressed(VK_SPACE))
			gameStates = START_MENU;
		break;
	}
}

int moffset = 0;
//=============================================================================
// Artificial Intelligence
//=============================================================================
void Dungeon::ai()
{
	if (turnTaken && !isWalking) {
		for (int i = 0; i < gen.getFloor(floor).getMonsters().size(); i++) {
			if (gen.getFloor(floor).getMonsters()[i].getCurrentHealth() <= 0) {
				if(gameStates == LEVEL5) { gameStates = GAME_OVER; won = true; } else { continue; }
			}
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
						if (damage < 0) damage = 0;
						player.setHealth(player.getHealth() - damage);
						ppm.setCurrentFrame(damage);
						audio->playCue("hit");
						ppm.createParticleEffect(VECTOR2(player.getX(), player.getY()), VECTOR2(0,100), 1);
					} else {
						if (gen.getFloor(floor).getMonsters()[i].getX() < c.first) {
							monsters[i].facing = EAST;
							if (!gen.getFloor(floor).getMonsters()[i].getName().compare("Nazi")) monsters[i].setCurrentFrame(21);
							if (!gen.getFloor(floor).getMonsters()[i].getName().compare("Hitler")) monsters[i].setCurrentFrame(25);
						}
						if (gen.getFloor(floor).getMonsters()[i].getX() > c.first){
							monsters[i].facing = WEST;
							if (!gen.getFloor(floor).getMonsters()[i].getName().compare("Nazi")) monsters[i].setCurrentFrame(22);
							if (!gen.getFloor(floor).getMonsters()[i].getName().compare("Hitler")) monsters[i].setCurrentFrame(26);
						}
						if (gen.getFloor(floor).getMonsters()[i].getY() > c.second){
							monsters[i].facing = NORTH;
							if (!gen.getFloor(floor).getMonsters()[i].getName().compare("Nazi")) monsters[i].setCurrentFrame(23);
							if (!gen.getFloor(floor).getMonsters()[i].getName().compare("Hitler")) monsters[i].setCurrentFrame(27);
						}
						if (gen.getFloor(floor).getMonsters()[i].getY() < c.second){
							monsters[i].facing = SOUTH;
							if (!gen.getFloor(floor).getMonsters()[i].getName().compare("Nazi")) monsters[i].setCurrentFrame(20);
							if (!gen.getFloor(floor).getMonsters()[i].getName().compare("Hitler")) monsters[i].setCurrentFrame(24);
						}
						gen.getFloor(floor).getMonsters()[i].setCoords(c.first, c.second);
						monsters[i].isWalking = true;
					}
			}
		}
		moffset = 0;
		isWalking = true;
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
	int xoffp = 0;
	int yoffp = 0;

	if (isWalking && !turnTaken) {
		player.offset += 2;
		switch (player.getFacing()) {
		case NORTH:
			yoffp += player.offset;
			if (player.offset == 32) {
				player.y--;
				isWalking = false;
				if (gen.getFloor(floor).getItem(player.x, player.y) != 0) {
						ItemInstance* i = gen.getFloor(floor).getItem(player.x, player.y);
						player.getInventory().push_back(*i);
						i->isOnGround = false;
					}
				if (gen.getFloor(floor).getTile(player.x, player.y) == 9) loadFloor(floor + 1);
				player.setCurrentFrame(12);
				player.setFrames(12, 12);
				turnTaken = true;
			}
			break;
		case SOUTH:
			yoffp -= player.offset;
			if (player.offset == 32) {
				player.y++;
				isWalking = false;
				if (gen.getFloor(floor).getItem(player.x, player.y) != 0) {
						ItemInstance* i = gen.getFloor(floor).getItem(player.x, player.y);
						player.getInventory().push_back(*i);
						i->isOnGround = false;
					}
				if (gen.getFloor(floor).getTile(player.x, player.y) == 9) loadFloor(floor + 1);
				player.setCurrentFrame(20);
				player.setFrames(20, 20);
				turnTaken = true;
			}
			break;
		case EAST:
			xoffp -= player.offset;
			if (player.offset == 32) {
				player.x++;
				isWalking = false;
				if (gen.getFloor(floor).getItem(player.x, player.y) != 0) {
						ItemInstance* i = gen.getFloor(floor).getItem(player.x, player.y);
						player.getInventory().push_back(*i);
						i->isOnGround = false;
					}
				if (gen.getFloor(floor).getTile(player.x, player.y) == 9) loadFloor(floor + 1);
				player.setCurrentFrame(3);
				player.setFrames(3, 3);
				turnTaken = true;
			}
			break;
		case WEST:
			xoffp += player.offset;
			if (player.offset == 32) {
				player.x--;
				isWalking = false;
				if (gen.getFloor(floor).getItem(player.x, player.y) != 0) {
						ItemInstance* i = gen.getFloor(floor).getItem(player.x, player.y);
						player.getInventory().push_back(*i);
						i->isOnGround = false;
					}
				if (gen.getFloor(floor).getTile(player.x, player.y) == 9) loadFloor(floor + 1);
				player.setCurrentFrame(3);
				player.setFrames(3, 3);
				turnTaken = true;
			}
			break;
		}
	}

	switch(gameStates) {
	case LEVEL1:
	case LEVEL2:
	case LEVEL3:
	case LEVEL4:
	case LEVEL5:	

		for (int i = 0; i < gen.getFloor(floor).getHeight(); i++)
			for (int j = 0; j < gen.getFloor(floor).getWidth(); j++) {
				mapImg[i][j].setX((j - xoffset) * 32 + xoffp);
				mapImg[i][j].setY((i - yoffset) * 32 + yoffp);
				mapImg[i][j].draw();
			}

		for (int i = 0; i < gen.getFloor(floor).getMonsters().size(); i++) {
			if (gen.getFloor(floor).getMonsters()[i].getCurrentHealth() <= 0) continue;
			if (turnTaken && isWalking && monsters[i].isWalking) {
				switch (monsters[i].facing) {
				case NORTH:
					monsters[i].setX((gen.getFloor(floor).getMonsters()[i].getX() - xoffset) * 32 + xoffp);
					monsters[i].setY((gen.getFloor(floor).getMonsters()[i].getY() - yoffset) * 32 + yoffp + (32 - moffset));
					break;
				case SOUTH:
					monsters[i].setX((gen.getFloor(floor).getMonsters()[i].getX() - xoffset) * 32 + xoffp);
					monsters[i].setY((gen.getFloor(floor).getMonsters()[i].getY() - yoffset) * 32 + yoffp - (32 - moffset));
					break;
				case EAST:
					monsters[i].setX((gen.getFloor(floor).getMonsters()[i].getX() - xoffset) * 32 + xoffp - (32 - moffset));
					monsters[i].setY((gen.getFloor(floor).getMonsters()[i].getY() - yoffset) * 32 + yoffp);
					break;
				case WEST:
					monsters[i].setX((gen.getFloor(floor).getMonsters()[i].getX() - xoffset) * 32 + xoffp + (32 - moffset));
					monsters[i].setY((gen.getFloor(floor).getMonsters()[i].getY() - yoffset) * 32 + yoffp);
					break;
				}
			} else {
				monsters[i].setX((gen.getFloor(floor).getMonsters()[i].getX() - xoffset) * 32 + xoffp);
				monsters[i].setY((gen.getFloor(floor).getMonsters()[i].getY() - yoffset) * 32 + yoffp);
			}
			monsters[i].draw();
		}

		if (turnTaken && isWalking) {
			moffset += 2;
			if (moffset == 32) {
				isWalking = false;
				turnTaken = false;
				for (int i = 0; i < gen.getFloor(floor).getMonsters().size(); i++) {
					monsters[i].isWalking = false;
				}
			}
		}

		for (int i = 0; i < gen.getFloor(floor).getItems().size(); i++) {
			if (!gen.getFloor(floor).getItems()[i].isOnGround) continue;
			items[i].setX((gen.getFloor(floor).getItems()[i].getX() - xoffset) * 32 + xoffp);
			items[i].setY((gen.getFloor(floor).getItems()[i].getY() - yoffset) * 32 + yoffp);
			items[i].draw();
		}

		player.draw();
		redBar.draw();
		greenBar.draw();
		pm.draw();
		ppm.draw();
		if(activeMenu) {
			menuBG.draw();
			inventory->displayMenu(frameTime,player.getEquippedArmor(),player.getEquippedWeapon());
		}
		break;

	case SPLASH_SCREEN:
		break;

	case START_MENU:
		menuImage.draw();
		mainMenu->displayMenu(frameTime);
		text->print("Controls: Arrow Keys to move, 'i' to open inventory, Enter to select item", 20, 450);

		break;
	case GAME_OVER:
		gameOver.draw();
		if(bodyCount>hiScore) {
			hiScore = bodyCount;
		}
		if(won)
			bigText->print("You Won!",400,200);
		else
			bigText->print("You Lost....",400,200);
		std::stringstream s;
		s << bodyCount;
		text->print("Score: "+s.str(), 400, 260);
		s.str("");
		s << hiScore;
		text->print("High Score: "+s.str(), 500,10);
		if(timeInState < 0.75)
			text->print("Press Space to Continue....", 420,460);
		if(timeInState > 1.5)
			timeInState = 0.0;
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
	case LEVEL2:
	case LEVEL3:
	case LEVEL4:
	case LEVEL5:
		timeInState = 0;
		break;
		
	case GAME_OVER:

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

