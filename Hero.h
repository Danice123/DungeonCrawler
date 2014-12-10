#ifndef __HERO_H_
#define __HERO_H_

#include "Entity.h"
class Hero : public Entity {
public:
	Hero();
	void setFacingRight(bool in){facingRight = in;}
	bool isFacingRight(){return facingRight;}

	int getMaxHealth() { return maxHealth; }
	int getHealth() { return health; }
	void setHealth(int h) { health = h; }
	int getAttack() {
		if (equippedWeapon == -1) return 0;
		return inventory[equippedWeapon].getValue();
	}
	int getArmor() {
		if (equippedArmor == -1) return 0;
		return inventory[equippedArmor].getValue();
	}
	std::vector<ItemInstance>& getInventory() { return inventory; }
	void setEquippedArmor(int e) { equippedArmor = e; }
	int  getEquippedArmor() { return equippedArmor; }
	void setEquippedWeapon(int e) { equippedWeapon = e; }
	int  getEquippedWeapon() { return equippedWeapon; }

	int x;
	int y;
private:
	int maxHealth;
	int health;
	bool facingRight;
	
	std::vector<ItemInstance> inventory;
	int equippedArmor;
	int equippedWeapon;
};
#endif