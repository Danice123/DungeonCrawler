#ifndef __ITEMINSTANCE_
#define __ITEMINSTANCE_
#include "Items.h"

class ItemInstance {
public:
	ItemInstance(Items i) {
		name = i.getName();
		type = i.getType();
		value = i.getValue();
		isOnGround = true;
	}

	std::string getName() { return name; }
	int getType() { return type; }
	int getValue() { return value; }

	int getX() { return x; }
	int getY() { return y; }
	void setCoords(int x, int y) { this->x = x; this->y = y; }

	bool isOnGround;
private:
	std::string name;
	int type;
	int value;
	int x, y;
};
#endif