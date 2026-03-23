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
    LPDIRECT3DSURFACE9 background = NULL;
	LPDIRECT3DTEXTURE9 Mountain = NULL;
	LPDIRECT3DTEXTURE9 Could1 = NULL;
	LPDIRECT3DTEXTURE9 Could2 = NULL;
	LPDIRECT3DTEXTURE9 Could3 = NULL;
	LPDIRECT3DTEXTURE9 Feiting = NULL;
};

