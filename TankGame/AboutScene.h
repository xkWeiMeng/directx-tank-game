#pragma once
#include"Scene.h"
#include"DirectX.h"
#include"Global.h"
#include"Sound.h"
class AboutScene : public virtual Scene
{
public:
    bool Init();
    void End();
    void Render();
    void Update();
private:
    LPDIRECT3DTEXTURE9 SkyBG = NULL;
    LPDIRECT3DTEXTURE9 MountainFar = NULL;
    LPDIRECT3DTEXTURE9 ForegroundGrass = NULL;
    LPDIRECT3DTEXTURE9 ForegroundGround = NULL;
    LPDIRECT3DTEXTURE9 CloudSheet = NULL;
    LPDIRECT3DTEXTURE9 AirshipSheet = NULL;
    LPDIRECT3DTEXTURE9 LeafSheet = NULL;
};

