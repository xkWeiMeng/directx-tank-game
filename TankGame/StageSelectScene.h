#pragma once
#include"Scene.h"

//选关场景
class StageSelectScene : public virtual Scene
{
public:
	bool Init();
	void End();
	void Update();
	void Render();
};
