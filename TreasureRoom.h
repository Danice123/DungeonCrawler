#ifndef __TREASUREROOM_
#define __TREASUREROOM_
#include "Room.h"

class TreasureRoom : public Room {
public:
	TreasureRoom() : Room() {
		maxItems = 4;
		maxMonsters = 0;
	}
};
#endif