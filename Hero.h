#ifndef __HERO_H_
#define __HERO_H_

#include "Entity.h"
class Hero : public Entity {
public:
	Hero();
	void setFacingRight(bool in){facingRight = in;}
	bool isFacingRight(){return facingRight;}
private:
	int health;
	int attack;
	int armor;
	bool facingRight;
	
};
#endif