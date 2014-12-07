#include "Monster.h"
#include <fstream>
#include <iostream>

Monster::Monster(std::string name) {
	std::ifstream fin;
	fin.open("monsters/" + name + ".mon");
	fin >> this->name;
	fin >> health;
	fin >> attack;
	fin >> armor;
	fin >> scale;

	fin.close();

	std::cout << "Loading " << name << std::endl;
}