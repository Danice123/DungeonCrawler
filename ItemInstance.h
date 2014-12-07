#ifndef __ITEMINSTANCE_
#define __ITEMINSTANCE_
#include "Items.h"

class ItemInstance {
public:
	ItemInstance(int level, Items i) {
		name = i.getName();
		health = i.getHealth() * i.getScale() * level;
		currentHealth = health;
		attack = i.getAttack() * i.getScale() * level;
		armor = i.getArmor() * i.getScale() * level;
	}

	std::string getName() { return name; }
	int getHealth() { return health; }
	int getCurrentHealth() { return currentHealth; }
	int getAttack() { return attack; }
	int getArmor() { return armor; }

	int getX() { return x; }
	int getY() { return y; }
	void setCoords(int x, int y) { this->x = x; this->y = y; }
private:
	std::string name;
	int health;
	int currentHealth;
	int attack;
	int armor;

	int x, y;
};
#endif