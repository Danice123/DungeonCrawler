#ifndef __MONSTER_
#define __MONSTER_
#include <string>

class Monster {
public:
	Monster(std::string);

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