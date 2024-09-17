#pragma once
#include"Scene.h"
//»¶Ó­³¡¾°
class HomeScene : public virtual Scene
{
public:
    bool Init();

    void End();

    void Update();

    void Render();

private:
	LPDIRECT3DTEXTURE9 background = NULL;

    LPD3DXFONT font;
    CSound *bgm;
    int choose;
private:
    void Draw_Background();
    bool Create_Background();
};

