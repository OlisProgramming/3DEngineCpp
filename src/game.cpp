/*
 * Copyright (C) 2014 Benny Bobaganoosh
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "game.h"
#include "renderingEngine.h"
#include "meshRenderer.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <queue>

void Game::ProcessInput(const Input& input, float delta)
{
	m_inputTimer.StartInvocation();
	m_root.ProcessInputAll(input, delta);
	m_inputTimer.StopInvocation();
}

void Game::Update(float delta)
{
	m_updateTimer.StartInvocation();
	m_root.UpdateAll(delta);
	m_updateTimer.StopInvocation();
}

void Game::Render(RenderingEngine* renderingEngine)
{
	renderingEngine->Render(m_root);
}

void Game::LoadMap(const std::string& mapName) {
	
	std::ifstream infile("./res/maps/" + mapName);
	std::string line;
	std::istringstream iss;
	std::queue<Entity*> currentEntities;

	while (std::getline(infile, line))
	{
		Entity* entNewest;
		if (currentEntities.size() == 0)
			entNewest = nullptr;
		else
			entNewest = currentEntities.back();

		iss = std::istringstream(line);
		std::string command;
		iss >> command;

		if (command == "E")	// Declaration of an entity
		{
			float x, y, z, qx, qy, qz, qr, s;
			if (!(iss >> x >> y >> z >> qx >> qy >> qz >> qr >> s))
			{
				std::cout << "Invalid map line: " << line << std::endl;
				return;
			}
			currentEntities.push(new Entity(Vector3f(x, y, z), Quaternion(Vector3f(qx, qy, qz), ToRadians(qr)), s));
		}
		else if (command == "e")	// Finalisation of an entity
		{
			if (currentEntities.size() == 1) {
				AddToScene(entNewest);
				std::cout << entNewest->ToString();
				currentEntities.pop();
			}
		}
		else if (command == "M")	// MeshRenderer component
		{
			std::string meshStr, materialStr;
			if (!(iss >> meshStr >> materialStr))
			{
				std::cout << "Invalid map line: " << line << std::endl;
				return;
			}
			entNewest->AddComponent(new MeshRenderer(Mesh(meshStr), Material(materialStr)));
		}
		else if (command == "LD")	// Light (Directional) component
		{
			float colR, colG, colB, intensity, shadowMapSize, shadowArea, shadowSoft;
			if (!(iss >> colR >> colG >> colB >> intensity >> shadowMapSize >> shadowArea >> shadowSoft))
			{
				std::cout << "Invalid map line: " << line << std::endl;
				return;
			}
			entNewest->AddComponent(new DirectionalLight(Vector3f(colR, colG, colB), intensity, shadowMapSize, shadowArea, shadowSoft));
		}
	}
}