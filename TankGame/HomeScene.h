#pragma once
#include"Scene.h"
//欢迎场景
class HomeScene : public virtual Scene
{
public:
    bool Init();

    void End();

    void Update();

    void Render();

private:
    LPDIRECT3DSURFACE9 background = NULL;

    LPD3DXFONT font;
    CSound *bgm;
    int choose;
private:
    void Draw_Background();
    bool Create_Background();
};

