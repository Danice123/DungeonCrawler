#ifndef __DUNGEONROOM_
#define __DUNGEONROOM_
#include <cstdlib>
#include <vector>
#include <fstream>
#include "Monster.h"
#include "MonsterInstance.h"
#include "ItemInstance.h"
#include "Items.h"

enum dir { NORTH = 0, SOUTH = 1, EAST = 2, WEST = 3 };

class Room {
public:
	Room();
	~Room() {
		for (int i = 0; i < 4; i++) {
			if (nextRooms[i] != 0) {
				Room* r = nextRooms[i];
				nextRooms[i] = 0;
				delete r;
			}
		}
	}

	virtual void initializeRandom(std::vector<Monster>&, std::vector<Items>&, int);
	virtual void initializeFromFile(std::ifstream&, std::vector<Monster>&, std::vector<Items>&, int);

	int getWidth() { return width; }
	int getHeight() { return height; }
	int getAdjRooms() { return adjRooms; }

	Room* getRoom(dir d) { return nextRooms[d]; }
	void setRoom(dir d, Room* room);

	virtual void printRoom(int** map, int mx, int my, std::vector<MonsterInstance>& list, std::vector<ItemInstance>& list2);

	int x, y;
	int id;
protected:
	bool initialized;
	int width;
	int height;
	std::vector<MonsterInstance> monsters;
	std::vector<ItemInstance> items;
	int maxMonsters;
	int maxItems;
private:
	Room* nextRooms[4];
	int adjRooms;
};
#endif