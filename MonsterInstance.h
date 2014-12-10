#ifndef __MONSTERINSTANCE_
#define __MONSTERINSTANCE_
#include "Monster.h"

class MonsterInstance {
public:
	MonsterInstance(int level, Monster m) {
		name = m.getName();
		health = m.getHealth() * m.getScale() * level;
		currentHealth = health;
		attack = m.getAttack() * m.getScale() * level;
		armor = m.getArmor() * m.getScale() * level;
	}

	std::string getName() { return name; }
	int getHealth() { return health; }
	int getCurrentHealth() { return currentHealth; }
	void setCurrentHealth(int h) { currentHealth = h; }
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