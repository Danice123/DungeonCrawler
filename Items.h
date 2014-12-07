#ifndef __ITEMS_
#define __ITEMS_
#include <string>

class Items {
public:
	Items(std::string);

	std::string getName() { return name; }
	int getHealth() { return health; }
	int getAttack() { return attack; }
	int getArmor() { return armor; }
	int getScale() { return scale; }
private:
	std::string name;
	int health;
	int attack;
	int armor;

	int scale;
};
#endif