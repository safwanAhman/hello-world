//#include <iostream>
//#include <cmath>
//#include <list>
//#include <vector>
//#include <algorithm>
//
//class Vector2
//{
//	int x, y;
//public:
//	Vector2(int _x, int _y) : x(_x), y(_y) {}
//	Vector2() = default;
//	Vector2 operator +(const Vector2& other) {
//		Vector2 temp;
//		temp.x = this->x + other.x;
//		temp.y = this->y + other.y;
//		return temp;
//	}
//	int getX() const { return x; }
//	int getY() const { return y; }
//
//	friend class Map;
//};
//
//struct Node
//{
//	Vector2 position;
//	int G, H, F;
//	Node* parent = nullptr;
//
//	Node() = default;
//	Node(const Node& other) = default;
//	Node(Vector2 pos) :position(pos) {};
//
//	void calc(const Vector2& endPos) {
//		H = static_cast<int>((abs(static_cast<double>(position.getX() - endPos.getX())) + abs(static_cast<double>(position.getY() - endPos.getY()))));
//		G = parent ? parent->G + 1 : 1;
//		F = G + H;
//	}
//
//	bool operator==(const Node& other) const {
//		return (position.getX() == other.position.getX() && position.getY() == other.position.getY());
//	}
//	bool operator!=(const Node& other) const {
//		return !(*this == other);
//	}
//	bool operator<(const Node& other)  const {
//		return(F < other.F);
//	}
//};
//
//class Map
//{
//	std::vector<char> data;
//	int size;
//public:
//	Map() = default;
//	Map(int _size) : size(_size) {
//		data.resize(size * size);
//		for (int i = 0; i < size * size; ++i) data[i] = '.';
//	}
//	void display() const {
//		for (int i = 1; i <= size * size; ++i) {
//			std::cout << data[i - 1] << " ";
//			if (!(i % size)) std::cout << "\n";
//		}
//	}
//	bool getIfInDanger(Vector2 position) const {
//		if (position.y < 0) position.y = 0;
//		if (position.x < 0) position.x = 0;
//		if (position.y >= 20) position.y = size - 1;
//		if (position.x >= 20) position.x = size - 1;
//		return(data[position.getX() + (position.getY() * size)] == 'X');
//	}
//	void setElement(char&& asda, Vector2 position) {
//		data[position.getX() + (position.getY() * size)] = asda;
//	}
//};
//
//class Solver
//{
//	Vector2 startPos, endPos;
//	std::vector<Vector2> directions;
//	Map map;
//public:
//	Solver(Vector2 _startPos, Vector2 _endPos, int size) : startPos(_startPos), endPos(_endPos) {
//		Map temp(size);
//		map = temp;
//
//		map.setElement('X', Vector2(14, 15));
//		map.setElement('X', Vector2(15, 15));
//		map.setElement('X', Vector2(16, 15));
//		map.setElement('X', Vector2(16, 14));
//		map.setElement('X', Vector2(16, 13));
//
//		directions.resize(8);
//		directions[0] = Vector2(-1, 1);
//		directions[1] = Vector2(-1, 0);
//		directions[2] = Vector2(-1, -1);
//		directions[3] = Vector2(0, 1);
//		directions[4] = Vector2(0, -1);
//		directions[5] = Vector2(1, 1);
//		directions[6] = Vector2(1, 0);
//		directions[7] = Vector2(1, -1);
//	}
//	bool aStar() {
//		Node startNode(startPos);
//		Node goalNode(Vector2(endPos.getX(), endPos.getY()));
//
//		if (map.getIfInDanger(startNode.position) || map.getIfInDanger(goalNode.position)) {
//			std::cout << "Either the start of this map is obstructed or so is the end.";
//			return false;
//		}
//
//		std::list<Node> openList;
//		std::list<Node> closedList;
//
//		startNode.calc(endPos);
//		openList.push_back(startNode);
//
//		while (!openList.empty()) {
//			auto current = Node(*std::min_element(openList.begin(), openList.end()));
//
//			current.calc(endPos);
//
//			closedList.push_back(current);
//			openList.remove(current);
//			if (current == goalNode) break;
//
//			for (auto& direction : directions) {
//				Node successor(direction + current.position);
//
//				if (map.getIfInDanger(successor.position) || successor.position.getX() > 20 - 1 ||
//					successor.position.getY() > 20 - 1 || successor.position.getX() < 0 ||
//					successor.position.getY() < 0 ||
//					std::find(closedList.begin(), closedList.end(), successor) != closedList.end()) {
//					continue;
//				}
//
//				successor.calc(endPos);
//
//				auto inOpen = std::find(openList.begin(), openList.end(), successor);
//				if (inOpen == openList.end()) {
//					successor.parent = &closedList.back();
//					successor.calc(endPos);
//
//					openList.push_back(successor);
//				}
//				else
//					if (successor.G < inOpen->G) successor.parent = &closedList.back();
//			}
//		}
//
//		if (!openList.size()) {
//			std::cout << "No path has been found\n";
//			return false;
//		}
//
//		auto inClosed = std::find(closedList.begin(), closedList.end(), goalNode);
//		if (inClosed != closedList.end()) {
//			while (*inClosed != startNode) {
//				map.setElement('Y', inClosed->position);
//				*inClosed = *inClosed->parent;
//			}
//		}
//
//		map.display();
//		return true;
//	}
//};
//
//int main()
//{
//	Solver solve(Vector2(10, 10), Vector2(19, 19), 20);
//	solve.aStar();
//
//	int x;
//	std::cin >> x;
//}