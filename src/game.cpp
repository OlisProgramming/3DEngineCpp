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

	float x, y, z, qx, qy, qz, qr, s;
	std::string meshStr, materialStr;

	while (std::getline(infile, line))
	{
		Entity* entNewest;
		if (currentEntities.size() == 0)
			entNewest = nullptr;
		else
			entNewest = currentEntities.back();

		std::cout << line << std::endl;

		unsigned int len = line.length();
		if (len > 2) iss = std::istringstream(line.substr(2, line.length() - 2));
		switch (line[0])
		{
		case 'E':	// Declaration of an entity
			if (!(iss >> x >> y >> z >> qx >> qy >> qz >> qr >> s))
			{
				std::cout << "Invalid line: " << line << std::endl;
				return;
			}
			currentEntities.push(new Entity(Vector3f(x, y, z), Quaternion(Vector3f(qx, qy, qz), ToRadians(qr)), s));
			break;
		case 'e':	// Finalisation of an entity
			if (currentEntities.size() == 1) {
				AddToScene(entNewest);
				std::cout << entNewest->ToString();
				currentEntities.pop();
			}
			break;
		case 'M':	// MeshRenderer component
			if (!(iss >> meshStr >> materialStr))
			{
				std::cout << "Invalid line: " << line << std::endl;
				return;
			}
			entNewest->AddComponent(new MeshRenderer(Mesh(meshStr), Material(materialStr)));
			break;
		default:
			break;
		}
	}
}