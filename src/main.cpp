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

#include "3DEngine.h"
#include "freeLook.h"
#include "freeMove.h"

class TestGame : public Game
{
public:
	TestGame() {}
	
	virtual void Init(const Window& window);
protected:
private:
	TestGame(const TestGame& other) {}
	void operator=(const TestGame& other) {}
};

void TestGame::Init(const Window& window)
{
	Material bricks("bricks", Texture("bricks.jpg"), 0.0f, 0, Texture("bricks_normal.jpg"), Texture("bricks_disp.png"), 0.03f, -0.5f);
	
	LoadMap("test.temap");

	//AddToScene((new Entity(Vector3f(0, -1, 5), Quaternion(), 32.0f))
		//->AddComponent(new MeshRenderer(Mesh("terrain02.obj"), Material("bricks"))));
	
	AddToScene((new Entity(Vector3f(), Quaternion(Vector3f(1,0,0), ToRadians(-45))))
		->AddComponent(new DirectionalLight(Vector3f(1,1,1), 0.4f, 10, 80.0f, 1.0f)));
	
	AddToScene((new Entity(Vector3f(0, 2, 0), Quaternion(Vector3f(0,1,0), 0.4f), 1.0f))
		->AddComponent(new CameraComponent(Matrix4f().InitPerspective(ToRadians(70.0f), window.GetAspect(), 0.1f, 1000.0f)))
		->AddComponent(new FreeLook(window.GetCenter()))
		->AddComponent(new FreeMove()));
}

int main()
{
	TestGame game;
	Window window(800, 600, "3D Game Engine");
	RenderingEngine renderer(window);
	
	//window.SetFullScreen(true);
	
	CoreEngine engine(60, &window, &renderer, &game);
	engine.Start();
	
	return 0;
}
