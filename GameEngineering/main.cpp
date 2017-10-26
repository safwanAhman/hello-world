#pragma comment(lib,"nclgl.lib")
#pragma comment(lib, "Physics.lib")
#pragma comment(lib, "HUI.lib")
#pragma comment(lib, "FileIO.lib")
#pragma comment(lib, "ResourceManagement.lib")
#pragma comment(lib, "Graphics.lib")

#include "../nclgl/Window.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/GameObject.h"
#include "../HUI/KeyboardGame.h"
#include "../FileIO/LSystem.h"
#include "../Physics/ObjectCollision.h"
#include "../Physics/SUVAT.h"
#include "../ResourceManagement/MeshManagement.h"
#include "../Graphics/GameSimWall.h"
#include "../Graphics/GameSimSoldier.h"
#include "../Graphics/GameSimSoldierSlave.h"
#include "../Graphics/GameNode.h"

#include <vector>
#include <stdlib.h>
#include <list>
#include <cstdlib>
#include <algorithm>

#include "Play.h"

using namespace std;

vector<Vector3> directions;


list<GameNode> AStar(GameSimWall* wall , Vector3 startPos = Vector3(0,0,0), Vector3 endPos = Vector3(0,0,0), bool safe = true) {

	directions.resize(8);
	directions[0] = Vector3(-5, 5,0);
	directions[1] = Vector3(-5, 0,0);
	directions[2] = Vector3(-5, -5,0);
	directions[3] = Vector3(0, 5,0);
	directions[4] = Vector3(0, -5,0);
	directions[5] = Vector3(5, 5,0);
	directions[6] = Vector3(5, 0,0);
	directions[7] = Vector3(5, -5,0);

	GameNode startingPoint =  GameNode();
	GameNode endPoint =  GameNode();


	if (safe) {
		startingPoint.SetSafe();
		endPoint.SetSafe();
	
	}

	if (endPos == Vector3(0, 0, 0) && startPos == Vector3(0, 0, 0)) {
		startingPoint.SetPosition(wall->StartingPosition());
		endPoint.SetPosition(wall->GetEndPosition());
	}
	else {
		startingPoint.SetPosition(startPos);
		endPoint.SetPosition(endPos);
	}

	startingPoint.calc(endPoint.GetPosition());

	list<GameNode> openList;
	list<GameNode> closeList;

	openList.push_back(startingPoint);


	while (!openList.empty()) {

		GameNode lowest = GameNode(*std::min_element(openList.begin(), openList.end()));
		
		if (safe)
			lowest.SetSafe();

		lowest.SetCost(wall->GetSquare(lowest.GetPosition())->getChar());

		lowest.calc(endPoint.GetPosition());

		closeList.push_back(lowest);
		openList.remove(lowest);

		if (lowest == endPoint) {
			break;
		}

		for (auto& direction : directions) {
			GameNode continued(direction + lowest.GetPosition());
			continued.SetSafe();

			if ( wall->GetSquare(continued.GetPosition())->getChar() == 'F' || wall->GetSquare(continued.GetPosition())->getChar() == 'R' || 
				continued.GetPosition().x > wall->GetBox().MAX_X || continued.GetPosition().x < wall->GetBox().MIN_X || continued.GetPosition().y > wall->GetBox().MAX_Y  || continued.GetPosition().y < wall->GetBox().MIN_Y ||
				std::find(closeList.begin(), closeList.end(), continued) != closeList.end()) {
				continue;
			}
			
			continued.SetCost(wall->GetSquare(continued.GetPosition())->getChar());
			
			continued.calc(endPoint.GetPosition());

			auto inOpen = std::find(openList.begin(), openList.end(), continued);
			if (inOpen == openList.end()) {
				continued.SetParent(&closeList.back());
				continued.SetCost(wall->GetSquare(continued.GetPosition())->getChar());

				continued.calc(endPoint.GetPosition());

				openList.push_back(continued);
			}else if (continued.F < inOpen->F) {
				continued.SetParent(&closeList.back());
			}
		}
	}

	if (openList.empty()) {
		cout << "Path cannot be found? " << endl;
	}

	auto inclosed = std::find(closeList.begin(), closeList.end(), endPoint);
	list<GameNode> path;
	int count = 0;
	if (inclosed != closeList.end()) {
		while (*inclosed != startingPoint) {
			*inclosed = *inclosed->GetParent();
			path.push_front(*inclosed);
			count++;
			cout << inclosed->GetPosition();
		}
	}

	for (auto p : path) {
		cout << path.front().GetPosition();
	}

	cout << "size " << path.size() << endl;
	cout << "starting point " << startingPoint.GetPosition();
	cout << "end point " << endPoint.GetPosition();

	return path;

}

int main() {
	Window w("Gaming Simulation", 900, 900, false);
	if (!w.HasInitialised()) {
		return -1;
	}

	GameTimer* ptimer;
	list<GameNode> path;
	//renderer first then Mesh Management (for some reason...)
	//inilization of objectas
	Renderer renderer(w);
	MeshManagement *m = new MeshManagement();

	GameSimWall* map = new GameSimWall();
	map->UseMesh(m->getMesh(2));
	map->SetGrid();

	GameSimSoldier* s = new GameSimSoldier();
	s->UseMesh(m->getMesh(1));
	s->SetTransform(Matrix4::Translation(map->StartingPosition()));
	s->SetColour(Vector4(1, 0, 0, 1));
	s->SetMass(150.0f);
	s->OutOfBound(map->GetBox());

	vector<GameSimSoldierSlave*> arraySlave;      
	arraySlave.resize(9);
	
	for (int i = 0; i < arraySlave.size(); ++i) {

		if (i == 0) {
			arraySlave[i] = new GameSimSoldierSlave(s, (i+1) * 9.7);
			arraySlave[i]->SetTransform(Matrix4::Translation(s->GetTransform().GetPositionVector()));

		}
		else{
			arraySlave[i] = new GameSimSoldierSlave(arraySlave[i - 1], (i + 1) * 9.5); //SHOULD change it tbh
			arraySlave[i]->SetTransform(Matrix4::Translation(arraySlave[i - 1]->GetTransform().GetPositionVector()));

		}
		arraySlave[i]->UseMesh(m->getMesh(1));
		arraySlave[i]->SetMass(150.0f);
		arraySlave[i]->OutOfBound(map->GetBox());
	}

	renderer.AddRenderObjects(map);

	if (!renderer.HasInitialised()) {
		return -1;
	}

	ObjectCollision col =  ObjectCollision();

	col.AddObject(map);

	bool deleted = false;
	bool added = false;

	while (w.UpdateWindow() && ( !w.GetKeyboard()->KeyDown(KEYBOARD_X)) ) {

		ptimer = new GameTimer();
			
		//if k is pressed then add soldiers here, should have a bool statement that does it once
		//add soldiers here so when k is pressed it should add obects
		if (Window::GetKeyboard()->KeyDown(KEYBOARD_K)) {
			if (!added) {
				//soliders appear b
				s->SetKeyboard(true);
				s->setAI(false);
				renderer.AddRenderObjects(s);
				col.AddObject(s);
			

				for (int i = 0; i < arraySlave.size(); ++i) {
					renderer.AddRenderObjects(arraySlave[i]);
					col.AddObject(arraySlave[i]);
					arraySlave[i]->SetKeyboard(false);
					arraySlave[i]->setAI(false);
				}
				added = true;
			}
		}

		renderer.RenderScene();
		col.Collided();

		//should put soldiers update in the if statement as well tbh
		if (added && s != NULL) {
			s->Update(ptimer);
			s->UpdatePhysics(ptimer);

			for (int i = 0; i < arraySlave.size(); ++i) {
				arraySlave[i]->Update(ptimer);
			}
		}

		map->Update();
		
		//if soldiers are gone then delete and turn bool statemen 
		if (!deleted) {
			if (map->EndPosition(s->GetWorldTransform().GetPositionVector()) && !s->GetRemoved()) {
				cout << "Deleted! " << endl;
		
				renderer.RemoveGameSoldier(s);
				col.DeleteColObj(s);
				s->Deleted(true);

				cout << "woo " << endl;

			}

			for (int i = 0; i < arraySlave.size(); ++i) {
				if (map->EndPosition(arraySlave[i]->GetWorldTransform().GetPositionVector()) && !arraySlave[i]->GetRemoved()) {
					cout << "Deleted! " << endl;

					renderer.RemoveGameSoldier(arraySlave[i]);
					col.DeleteColObj(arraySlave[i]);
					arraySlave[i]->Deleted(true);

					cout << "woo " << endl;

				}
			}
		}

		//SQUAD GANG if J IS PRESSED. Go to Castle, fastest point
		if (w.GetKeyboard()->KeyDown(KEYBOARD_J) || added) {
			if (!added) {
				path = AStar(map, s->GetTransform().GetPositionVector(), map->GetEndPosition());

				//soliders appear b
				renderer.AddRenderObjects(s);
				col.AddObject(s);
				s->setAI(true);

				for (int i = 0; i < arraySlave.size(); ++i) {
					renderer.AddRenderObjects(arraySlave[i]);
					col.AddObject(arraySlave[i]);
					arraySlave[i]->setAI(true);
				}

				added = true;

			}

			if (path.size() > 1 && s->GetAISet()) {

				if (s->GetWorldTransform().GetPositionVector() >= path.front().GetPosition() && added) {
					if (!path.empty())
						path.pop_front();
					s->SetPath(path.front().GetPosition());
					cout << "Path size is: " << path.size() << endl;
					cout << "Current path position is: " << path.front().GetPosition();
					cout << "soldier position" << s->GetWorldTransform().GetPositionVector();
				}
			}
		}

		//SQUAD GANG if K IS PRESSED. Go to Castle, safest point
		if (w.GetKeyboard()->KeyDown(KEYBOARD_T) || added) {
			if (!added) {
				path = AStar(map, s->GetTransform().GetPositionVector(), map->GetTacticalSquadEndPos(), true);

				//soliders appear b
				renderer.AddRenderObjects(s);
				col.AddObject(s);
				s->setAI(true);

				for (int i = 0; i < arraySlave.size(); ++i) {
					renderer.AddRenderObjects(arraySlave[i]);
					col.AddObject(arraySlave[i]);
					arraySlave[i]->setAI(true);
				}

				added = true;

			}

			if (path.size() > 1 && s->GetAISet()) {

				if (s->GetWorldTransform().GetPositionVector() >= path.front().GetPosition() && added) {
					if (!path.empty())
						path.pop_front();
					s->SetPath(path.front().GetPosition());
					cout << "Path size is: " << path.size() << endl;
					cout << "Current path position is: " << path.front().GetPosition();
					cout << "soldier position" << s->GetWorldTransform().GetPositionVector();
				}
			}
		}

		delete ptimer;
	}

	delete s;
	for (int i = 0; i < arraySlave.size(); ++i) {
		delete arraySlave[i];
	}


	delete map;
	return 0;
}
