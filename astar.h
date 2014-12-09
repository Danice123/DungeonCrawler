#ifndef __astar_
#define __astar_
#include "Floor.h"
#include <queue>
#include <vector>

class Node {
public:
	Node(Node* parent, int x, int y, int cost) : x(x), y(y), heuristic(0), cost(cost), parent(parent) {}
	Node(const Node& n) : x(n.x), y(n.y), heuristic(n.heuristic), cost(n.cost), parent(n.parent) {}
	void generateHeuristic(int ex, int ey) {
		heuristic = sqrt(pow(ex - x, 2) + pow(ey - y, 2));
	}
	friend bool operator==(const Node& lhs, const Node& rhs) {
		if (lhs.x == rhs.x && lhs.y == rhs.y) return true;
		return false;
	}
	friend bool operator< (const Node& lhs, const Node& rhs) {
		if (lhs.cost + lhs.heuristic > rhs.cost + rhs.heuristic) return true;
		return false;
	}
	int x;
	int y;
	double cost;
	double heuristic;
	Node* parent;
};

class AStar {
public:
	AStar(Floor* floor, int startx, int starty, int endx, int endy):
		floor(floor), start(0, startx, starty, 0), ex(endx), ey(endy) {}

	void run() {
		end = 0;
		std::priority_queue<Node> q;
		std::vector<Node> list;
		q.push(start);
		list.push_back(start);
		do {
			Node* parent = new Node(q.top());
			if (parent->x == ex && parent->y == ey) {
				end = parent;
				break;
			}
			q.pop();
			//up
			if (floor->getTile(parent->x, parent->y - 1) != 0) {
				Node n(parent, parent->x, parent->y - 1, parent->cost + parent->heuristic);
				if (!searchList(list, n)) {
					n.generateHeuristic(ex, ey);
					q.push(n);
					list.push_back(n);
				}
			}
			//down
			if (floor->getTile(parent->x, parent->y + 1) != 0) {
				Node n(parent, parent->x, parent->y + 1, parent->cost + parent->heuristic);
				if (!searchList(list, n)) {
					n.generateHeuristic(ex, ey);
					q.push(n);
					list.push_back(n);
				}
			}
			//left
			if (floor->getTile(parent->x - 1, parent->y) != 0) {
				Node n(parent, parent->x - 1, parent->y, parent->cost + parent->heuristic);
				if (!searchList(list, n)) {
					n.generateHeuristic(ex, ey);
					q.push(n);
					list.push_back(n);
				}
			}
			//right
			if (floor->getTile(parent->x + 1, parent->y) != 0) {
				Node n(parent, parent->x + 1, parent->y, parent->cost + parent->heuristic);
				if (!searchList(list, n)) {
					n.generateHeuristic(ex, ey);
					q.push(n);
					list.push_back(n);
				}
			}
		} while(q.size() > 0);
	}

	std::pair<int, int> getNextStep() {
		if (end == 0) throw "BAD";
		Node* search = end;
		while(search->parent->parent != 0) {
			search = search->parent;
		}
		return std::pair<int, int>(search->x, search->y);
	}
private:
	Floor* floor;
	Node start;
	int ex, ey;
	Node* end;

	bool searchList(std::vector<Node>& list, Node& n) {
		for (int i = 0; i < list.size(); i++) if (list[i] == n) return true;
		return false;
	}
};
#endif