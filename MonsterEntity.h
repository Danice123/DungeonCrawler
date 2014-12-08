#ifndef __MonsterEntity__
#define __MonsterEntity__
#include "entity.h"
#include "MonsterInstance.h"

class MonsterEntity : public Entity {
public:
	MonsterEntity() : Entity() {}
	void setMonster(MonsterInstance* i) {
		instance = i;
	}

	MonsterInstance* getInfo() { return instance; }
private:
	MonsterInstance* instance;
};
#endif