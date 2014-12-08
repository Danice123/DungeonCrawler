#include "Floor.h"
#include <queue>
#include <iostream>
using std::cout;
using std::endl;

void Floor::initializeRandom(std::vector<Monster>& tempM, std::vector<Items>& tempI) {
	initialized = true;
	std::queue<Room*> rl;
	int mRooms = rand() % 10 + 10;
	int nRooms = 1;
	stairRoom = rand() % mRooms;
	spawn = new Room();
	spawn->initializeRandom(tempM, tempI, 1);
	rl.push(spawn);
	cout << "Spawn: " << spawn->id << endl;
	do {
		Room* r = rl.front();
		rl.pop();
		cout << "Expanding: " << r->id << endl;

		int adjRooms = rand() % (mRooms - nRooms < 4 - r->getAdjRooms()?mRooms - nRooms:4 - r->getAdjRooms()) + 1;
		cout << "Adding " << adjRooms << " Rooms" << endl;

		int side = rand() % 4;
		cout << "Starting on side " << side << endl;

		for (int i = 0; i < adjRooms; i++) {
			if (r->getRoom((dir) side) != 0) {
				cout << "Side " << side << " is full" << endl;
				side++;
				if (side == 4) side = 0;
				i--;
				continue;
			}
			Room* newRoom;
			switch (rand() % 3) {
			case 1:
				newRoom = new OvalRoom();
				break;
			case 2:
				newRoom = new TreasureRoom();
			default:
				newRoom = new Room();
			}
			newRoom->initializeRandom(tempM, tempI, 1);
			r->setRoom((dir) side, newRoom);
			newRoom->setRoom((dir) (side % 2 == 1?side - 1:side + 1), r);
			rl.push(newRoom);
			cout << "Added id: " << newRoom->id << " to side " << side << endl;
			side++;
			if (side == 4) side = 0;
			nRooms++;
		}
		cout << mRooms - nRooms << " Rooms remaining" << endl;
	} while (nRooms != mRooms);
}

void Floor::initializeFromFile(std::ifstream& fin, std::vector<Monster>& list, std::vector<Items>& list2) {
	initialized = true;
	int nRooms;
	int level;
	fin >> level;
	fin >> nRooms;
	fin >> stairRoom;
	std::vector<Room*> rooms;
	for (int i = 0; i < nRooms; i++) {
		std::string type;
		fin >> type;
		Room* r;
		if (!type.compare("Oval")) r = new OvalRoom();
		else r = new Room();
		r->initializeFromFile(fin, list, list2, level);
		rooms.push_back(r);
	}
	spawn = rooms[0];
	for (int i = 0; i < nRooms; i++) {
		int n,s,e,w;
		fin >> n >> s >> e >> w;
		if (n >= 0) rooms[i]->setRoom(NORTH, rooms[n]);
		if (s >= 0) rooms[i]->setRoom(SOUTH, rooms[s]);
		if (e >= 0) rooms[i]->setRoom(EAST, rooms[e]);
		if (w >= 0) rooms[i]->setRoom(WEST, rooms[w]);
	}
}

bool search(std::vector<Room*> v, Room* object) {
	for (unsigned i = 0; i < v.size(); i++) {
		if (v[i] == object) return true;
	}
	return false;
}

int randDist() {
	return rand() % 4 + 4;
}

#include <vector>
void Floor::genFloorLayout() {
	std::vector<Room*> rl;
	rl.push_back(spawn);
	unsigned index = 0;

	int left = -1;
	int up = -1;
	int right = spawn->getWidth();
	int down = spawn->getHeight();

	do {
		Room* r = rl[index];
		index++;

		if (r->getRoom(NORTH) != 0 && !search(rl, r->getRoom(NORTH))) {
			Room* nr = r->getRoom(NORTH);
 			nr->x = r->x;
			nr->y = r->y - nr->getHeight() - randDist();
			rl.push_back(nr);

			if (up > nr->y) up = nr->y - 1;
			if (down < nr->y + nr->getHeight() + 1) down = nr->y + nr->getHeight() + 1;
			if (left > nr->x) left = nr->x;
			if (right < nr->x + nr->getWidth() + 1) right = nr->x + nr->getWidth() + 1;
		}

		if (r->getRoom(SOUTH) != 0 && !search(rl, r->getRoom(SOUTH))) {
			Room* nr = r->getRoom(SOUTH);
			nr->x = r->x;
			nr->y = r->y + r->getHeight() + randDist();
			rl.push_back(nr);

			if (up > nr->y) up = nr->y;
			if (down < nr->y + nr->getHeight() + 1) down = nr->y + nr->getHeight() + 1;
			if (left > nr->x) left = nr->x;
			if (right < nr->x + nr->getWidth() + 1) right = nr->x + nr->getWidth() + 1;
		}

		if (r->getRoom(EAST) != 0 && !search(rl, r->getRoom(EAST))) {
			Room* nr = r->getRoom(EAST);
			nr->x = r->x + r->getWidth() + randDist();
			nr->y = r->y;
			rl.push_back(nr);

			if (up > nr->y) up = nr->y;
			if (down < nr->y + nr->getHeight() + 1) down = nr->y + nr->getHeight() + 1;
			if (left > nr->x) left = nr->x;
			if (right < nr->x + nr->getWidth() + 1) right = nr->x + nr->getWidth() + 1;
		}

		if (r->getRoom(WEST) != 0 && !search(rl, r->getRoom(WEST))) {
			Room* nr = r->getRoom(WEST);
			nr->x = r->x - nr->getWidth() - randDist();
			nr->y = r->y;
			rl.push_back(nr);

			if (up > nr->y) up = nr->y;
			if (down < nr->y + nr->getHeight() + 1) down = nr->y + nr->getHeight() + 1;
			if (left > nr->x) left = nr->x - 1;
			if (right < nr->x + nr->getWidth() + 1) right = nr->x + nr->getWidth() + 1;
		}
	} while(index < rl.size());

	width = -left + right;
	height = -up + down;

	map = new int*[height];
	for (int i = 0; i < height; i++) {
		map[i] = new int[width];
		for (int j = 0; j < width; j++) map[i][j] = 0;
	}
	std::vector<Room*> check;
	for (unsigned i = 0; i < rl.size(); i++) {
		rl[i]->printRoom(map, rl[i]->x - left, rl[i]->y - up, monsters, items);

		if (rl[i]->getRoom(NORTH) != 0 && !search(check, rl[i]->getRoom(NORTH))) {
			int sbound = (rl[i]->x > rl[i]->getRoom(NORTH)->x?rl[i]->x:rl[i]->getRoom(NORTH)->x) - left;
			int lbound = (rl[i]->x + rl[i]->getWidth() < rl[i]->getRoom(NORTH)->x + rl[i]->getRoom(NORTH)->getWidth()
				?rl[i]->x + rl[i]->getWidth():rl[i]->getRoom(NORTH)->x + rl[i]->getRoom(NORTH)->getWidth()) - left;
			int vary = lbound - sbound;
			int coord;
			if (vary == 0) coord = sbound;
			else coord = rand() % vary + sbound;
			int length = rl[i]->y - rl[i]->getRoom(NORTH)->y - rl[i]->getRoom(NORTH)->getHeight() + 1;
			for(int k = 1; k < length; k++) map[rl[i]->y - k - up][coord] = 1;
		}

		if (rl[i]->getRoom(SOUTH) != 0 && !search(check, rl[i]->getRoom(SOUTH))) {
			int sbound = (rl[i]->x > rl[i]->getRoom(SOUTH)->x?rl[i]->x:rl[i]->getRoom(SOUTH)->x) - left;
			int lbound = (rl[i]->x + rl[i]->getWidth() < rl[i]->getRoom(SOUTH)->x + rl[i]->getRoom(SOUTH)->getWidth()
				?rl[i]->x + rl[i]->getWidth():rl[i]->getRoom(SOUTH)->x + rl[i]->getRoom(SOUTH)->getWidth()) - left;
			int vary = lbound - sbound;
			int coord;
			if (vary == 0) coord = sbound;
			else coord = rand() % vary + sbound;
			int length = rl[i]->getRoom(SOUTH)->y - rl[i]->y - rl[i]->getHeight() + 1;
			for(int k = 1; k < length; k++) map[rl[i]->y + rl[i]->getHeight() - 1 + k - up][coord] = 1;
		}

		if (rl[i]->getRoom(EAST) != 0 && !search(check, rl[i]->getRoom(EAST))) {
			int sbound = (rl[i]->y > rl[i]->getRoom(EAST)->y?rl[i]->y:rl[i]->getRoom(EAST)->y) - up;
			int lbound = (rl[i]->y + rl[i]->getHeight() < rl[i]->getRoom(EAST)->y + rl[i]->getRoom(EAST)->getHeight()
				?rl[i]->y + rl[i]->getHeight():rl[i]->getRoom(EAST)->y + rl[i]->getRoom(EAST)->getHeight()) - up;
			int vary = lbound - sbound;
			int coord;
			if (vary == 0) coord = sbound;
			else coord = rand() % vary + sbound;
			int length = rl[i]->getRoom(EAST)->x - rl[i]->x - rl[i]->getWidth() + 1;
			for(int k = 1; k < length; k++) map[coord][rl[i]->x + rl[i]->getWidth() - 1 + k - left] = 1;
		}

		if (rl[i]->getRoom(WEST) != 0 && !search(check, rl[i]->getRoom(WEST))) {
			int sbound = (rl[i]->y > rl[i]->getRoom(WEST)->y?rl[i]->y:rl[i]->getRoom(WEST)->y) - up;
			int lbound = (rl[i]->y + rl[i]->getHeight() < rl[i]->getRoom(WEST)->y + rl[i]->getRoom(WEST)->getHeight()
				?rl[i]->y + rl[i]->getHeight():rl[i]->getRoom(WEST)->y + rl[i]->getRoom(WEST)->getHeight()) - up;
			int vary = lbound - sbound;
			int coord;
			if (vary == 0) coord = sbound;
			else coord = rand() % vary + sbound;
			int length = rl[i]->x - rl[i]->getRoom(WEST)->x - rl[i]->getRoom(WEST)->getWidth() + 1;
			for(int k = 1; k < length; k++) map[coord][rl[i]->x - k - left] = 1;
		}
		check.push_back(rl[i]);
	}

	stairX = rl[stairRoom]->x + rand() % rl[stairRoom]->getWidth() - left;
	stairY = rl[stairRoom]->y + rand() % rl[stairRoom]->getHeight() - up;

	sx = rand() % spawn->getWidth() - left;
	sy = rand() % spawn->getHeight() - up;
	map[stairY][stairX] = 9;

	/*for (int i = 0; i < monsters.size(); i++) {
		char c = monsters[i].getName()[0];
		map[monsters[i].getY()][monsters[i].getX()] = c;
	}
	for (int i = 0; i < items.size(); i++) {
		char c = items[i].getName()[0];
		map[items[i].getY()][items[i].getX()] = c;
	}*/
}