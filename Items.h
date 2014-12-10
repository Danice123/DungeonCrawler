#ifndef __ITEMS_
#define __ITEMS_
#include <string>

class Items {
public:
	Items(std::string);

	std::string getName() { return name; }
	int getType() { return type; }
	int getValue() { return value; }
	float getScale() { return scale; }
private:
	std::string name;
	int type;
	int value;
	float scale;
};
#endif