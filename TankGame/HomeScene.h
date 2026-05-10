#pragma once
#include"Scene.h"
#include"DirectX.h"
#include"Global.h"
//欢迎场景 — Attract Mode (AI Demo Battle)
class HomeScene : public virtual Scene
{
public:
    bool Init();
    void End();
    void Update();
    void Render();

private:
    // 游戏纹理（Attract Mode用）
    LPDIRECT3DTEXTURE9 Tile = NULL;
    LPDIRECT3DTEXTURE9 Player_1 = NULL;
    LPDIRECT3DTEXTURE9 Player_2 = NULL;
    LPDIRECT3DTEXTURE9 Enemy_TXTTURE = NULL;
    LPDIRECT3DTEXTURE9 Bullet_TXTTURE = NULL;
    LPDIRECT3DTEXTURE9 Boom1 = NULL;
    LPDIRECT3DTEXTURE9 Boom2 = NULL;
    LPDIRECT3DTEXTURE9 Shield = NULL;
    LPDIRECT3DTEXTURE9 OverlayTex = NULL; // 1x1白色纹理用于半透明遮罩
    LPDIRECT3DSURFACE9 GrayRect = NULL;

    LPD3DXFONT font;
    LPD3DXFONT menuFontShadow; // 菜单伪3D阴影层字体（略大）
    LPD3DXFONT titleFont;
    LPD3DXFONT titleFontShadow;
    LPD3DXFONT hintFont;
    int choose;
};

