#include "Items.h"
#include <fstream>
#include <iostream>

Items::Items(std::string name) {
	std::ifstream fin;
	fin.open("items/" + name + ".itm");
	fin >> this->name;
	fin >> health;
	fin >> attack;
	fin >> armor;
	fin >> scale;

	fin.close();

	std::cout << "Loading " << name << std::endl;
}