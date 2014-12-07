#ifndef __OVALROOM_
#define __OVALROOM_
#include "Room.h"

class OvalRoom : public Room {
public:
	OvalRoom() : Room() {
		maxItems = 3;
		maxMonsters = 2;
	}

	virtual void printRoom(char** map, int mx, int my, std::vector<MonsterInstance>& list, std::vector<ItemInstance>& list2) {
		for (int y = -1; y < height + 1; y++) {
			int x = 0;
			if (y == -1 || y == height) x = 1;
			int xbound = width;
			if (y == -1 || y == height) xbound = width - 1;

			for (;x < xbound; x++) map[my + y][mx + x] = '1';
		}

		for (int i = 0; i < monsters.size(); i++) {
			monsters[i].setCoords(monsters[i].getX() + mx, monsters[i].getY() + my);
			list.push_back(monsters[i]);
		}

		for (int i = 0; i < items.size(); i++) {
			items[i].setCoords(items[i].getX() + mx, items[i].getY() + my);
			list2.push_back(items[i]);
		}
	}
};
#endif