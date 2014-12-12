#ifndef __DUNGEONFLOOR_
#define __DUNGEONFLOOR_
#include "Room.h"
#include "OvalRoom.h"
#include "TreasureRoom.h"
#include <vector>
#include <fstream>
#include "MonsterInstance.h"
#include "ItemInstance.h"

class Floor {
public:
	Floor() {initialized = false;}
	~Floor() {
		for (int i = 0; i < height; i++) delete [] map[i];
		delete [] map;
	}

	void initializeRandom(int, std::vector<Monster>&, std::vector<Items>&);
	void initializeFromFile(std::ifstream&, std::vector<Monster>&, std::vector<Items>&);

	void genFloorLayout();

	int getTile(int x, int y) {
		return map[y][x];
	}

	int getWidth() { return width; }
	int getHeight() { return height; }
	std::vector<MonsterInstance>& getMonsters() { return monsters; }
	MonsterInstance* getMonster(int x, int y) {
		for (int i = 0; i < monsters.size(); i++)
			if (x == monsters[i].getX() && y == monsters[i].getY() && monsters[i].getCurrentHealth() > 0) return &monsters[i];
		return 0;
	}

	std::vector<ItemInstance>& getItems() { return items; }
	ItemInstance* getItem(int x, int y) {
		for (int i = 0; i < items.size(); i++)
			if (x == items[i].getX() && y == items[i].getY()) return &items[i];
		return 0;
	}

	void setNoStairs() { stairRoom = -1; }

	int sx, sy;
private:
	bool initialized;
	Room* spawn;
	int** map;
	int width, height;
	int stairRoom, stairX, stairY;
	std::vector<MonsterInstance> monsters;
	std::vector<ItemInstance> items;
};
#endif