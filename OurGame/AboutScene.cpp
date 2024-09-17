#include "AboutScene.h"
#include"DirectX.h"
#include"GameMain.h"
float cx1 = 150, cx2 = 200, cx3 = 300;
float cx4 = 777, cx5 = 500, cx6 = 50;
float cx7 = 2333, cx8 = 666, cx9 = 555;
float c1=255, c2=255, c3 = 255;
static LPD3DXFONT font;
static LPD3DXFONT font1;
static LPD3DXFONT IG;
bool AboutScene::Init()
{
	 font = MakeFont("Î¢ÈíÑÅºÚ", 20);
	 font1 = MakeFont("Î¢ÈíÑÅºÚ", 30);

	 IG = MakeFont("Î¢ÈíÑÅºÚ", 300);
    background = LoadSurface(Resource::About::Backgroud);
	Could1 = LoadTexture(Resource::About::Cloud1, D3DCOLOR_XRGB(255, 255, 255));
	Could2 = LoadTexture(Resource::About::Cloud2, D3DCOLOR_XRGB(255, 255, 255));
	Could3 = LoadTexture(Resource::About::Cloud3, D3DCOLOR_XRGB(255, 255, 255));
	Mountain = LoadTexture(Resource::About::BKG);
	Feiting =LoadTexture(Resource::About::Feiting);
 //   return background != NULL;
	if (Global::Sound::SoundSwicth)
		LoopSound(Sound::BGM);
	return true;
}

void AboutScene::End()
{
	Sound::BGM->Stop();
    background->Release();
}

/*
SPRITE sprite[5];
sprite[0].height = 80;
sprite[0].width = 200;
sprite[0].x = 124;
sprite[0].y = 440;

sprite[1].height = 144;
sprite[1].width = 354;
sprite[1].x = 600;
sprite[1].y = 300;

sprite[2].height = 62;
sprite[2].width = 210;
sprite[2].x = 700;
sprite[2].y = 700;

sprite[3].height = 135;
sprite[3].width = 314;
sprite[3].x = 211;
sprite[3].y = 0;
*/
void AboutScene::Render()
{
	d3dDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
    RECT source_rect = {
        0,
        0,
        Global::Window::ScreenWidth,
        Global::Window::ScreenHeight
    };

  //  d3dDev->StretchRect(Mountain, NULL, backBuffer, &source_rect, D3DTEXF_NONE);
	//Sprite_Transform_Draw(Mountain, 0, 120, 1000, 625, 0, 1, 0, 1.03, D3DCOLOR_XRGB(255, 255, 255));	
	Sprite_Transform_Draw(Could1, cx1, 270, 200, 80, 0, 1,  0, 1.0, D3DCOLOR_XRGB(255, 255, 255));
	Sprite_Transform_Draw(Could2, cx2, 220, 354, 144, 0, 1, 0, 1.0, D3DCOLOR_XRGB(255, 255, 255));
	Sprite_Transform_Draw(Could3, cx3, 280, 210, 62, 0, 1,  0, 1.0, D3DCOLOR_XRGB(255, 255, 255));
	
	Sprite_Transform_Draw(Could1, cx4, 400, 200, 80, 0, 1, 0, 1.0, D3DCOLOR_XRGB(255, 255, 255));
	Sprite_Transform_Draw(Could2, cx5, 300, 354, 144, 0, 1, 0, 1.0, D3DCOLOR_XRGB(255, 255, 255));
	Sprite_Transform_Draw(Could3, cx6, 350, 210, 62, 0, 1, 0, 1.06, D3DCOLOR_XRGB(255, 255, 255));

	Sprite_Transform_Draw(Could1, cx7, 450, 200, 80, 0, 1, 0, 1.0, D3DCOLOR_XRGB(255, 255, 255));
	Sprite_Transform_Draw(Could2, cx8, 350, 354, 144, 0, 1, 0, 0.5, D3DCOLOR_XRGB(255, 255, 255));
	Sprite_Transform_Draw(Could3, cx9, 480, 210, 62, 0, 1, 0, 1.01, D3DCOLOR_XRGB(255, 255, 255));


	string text;
	text += "Author:XK";
	FontPrint(font, 0, 10 ,text, D3DCOLOR_XRGB((int)c1, (int)c2, (int)c3));

	string ig;
	ig += "I G\nNB";
	FontPrint(IG, 320, 90, ig, D3DCOLOR_XRGB((int)c1, (int)c2, (int)c3));

	text.clear();
	text += "ÇëÎð´«²¥\nÇÖÈ¨É¾";
	FontPrint(font1, 0, 850, text, D3DCOLOR_XRGB(255, 255, 255));

	c1 -= 0.5;
	c2 -= 0.4;
	c3 -= 0.3;
	if (c1 < 0) { c1 = 255.0; }
	if (c2 < 0) { c2 = 255.0; }
	if (c3 < 0) { c3 = 255.0; }
	cx1 -= 0.31;
	cx2 -= 0.30;
	cx3 -= 0.31;
	if (cx1 <= -300)
	{
		cx1 = Global::Window::ScreenWidth + 300;
	}
	if (cx2 <= -300)
	{
		cx2 = Global::Window::ScreenWidth + 300;
	}
	if (cx3 <= -300)
	{
		cx3 = Global::Window::ScreenWidth + 300;
	}
	cx4 -= 0.22;
	cx5 -= 0.2;
	cx6 -= 0.22;
	if (cx4 <= -400)
	{
		cx4 = Global::Window::ScreenWidth + 400;
	}
	if (cx5 <= -400)
	{
		cx5 = Global::Window::ScreenWidth + 400;
	}
	if (cx6 <= -400)
	{
		cx6 = Global::Window::ScreenWidth + 400;
	}
	cx7 -= 0.15;
	cx8 -= 0.1;
	cx9 -= 0.15;
	if (cx7 <= -500)
	{
		cx7 = Global::Window::ScreenWidth + 500;
	}
	if (cx8 <= -500)
	{
		cx8 = Global::Window::ScreenWidth + 500;
	}
	if (cx9 <= -500)
	{
		cx9 = Global::Window::ScreenWidth + 500;
	}
}

void AboutScene::Update()
{
	
	if (Key_Up(DIK_ESCAPE))
	{
		Game_ChangeScene(GAME_STATE::Home);
	}


}
