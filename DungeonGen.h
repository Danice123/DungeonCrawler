#ifndef __DUNGEONGEN_
#define __DUNGEONGEN_
#include "Floor.h"
#include "Monster.h"
#include "Items.h"
#include <vector>

class DungeonGen {
public:
	DungeonGen() {}

	void generateRandom(int floors);
	void loadFromFile(std::string);
	void loadMonsters();
	void loadItems();

	Floor& getFloor(int i) { return floors[i]; }
	int getAmountFloors() { return nFloors; }
private:
	int nFloors;
	Floor* floors;
	std::vector<Monster> monsterList;
	std::vector<Items> itemList;
};
#endif