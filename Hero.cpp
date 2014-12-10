#include "Hero.h"

Hero::Hero() : Entity() {
	maxHealth = 10;
	health = 10;
	equippedArmor = -1;
	equippedWeapon = -1;
}
