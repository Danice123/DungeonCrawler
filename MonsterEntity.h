#ifndef __MonsterEntity__
#define __MonsterEntity__
#include "entity.h"
#include "MonsterInstance.h"
#include "Room.h"

class MonsterEntity : public Entity {
public:
	MonsterEntity() : Entity() {isWalking = false;}
	dir facing;
	bool isWalking;
};
#endif