#include "GameSettingScene.h"
#pragma warning(disable:4996)
namespace GSS {
	LPD3DXFONT font;
	LPD3DXFONT font_s;

	LPDIRECT3DSURFACE9 BlackRect = NULL;
	LPDIRECT3DTEXTURE9 GameSettingPNG=NULL;
	RECT mouseRect;

	int Choose=0;
	void ShowSetting(int x, int y);

	bool WritePlayerSettingIbHD();

	int ReadK_B();

	void UsingMouseChoose(RECT & mrect);

	void NowSettingFoucs();

	void DrawChooseBlackRect();

	void DrawBlackRect(int x, int y);

	void FillRect(RECT & rect, long l, long r, long t, long b);

}
using namespace GSS;
bool GameSettingScene::Init()
{
	font = MakeFont("微软雅黑", 64);
	font_s = MakeFont("微软雅黑", 30);

	GameSettingPNG = LoadTexture(Resource::Texture::GameSetting, D3DCOLOR_XRGB(255, 255, 255));
	if (!GameSettingPNG)
	{
		ShowMessage("装载 设置背景 纹理失败!");
		return false;
	}
	d3dDev->CreateOffscreenPlainSurface(
		100,
		100,
		D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT,
		&BlackRect,
		NULL
	);

	return true;
}

void GameSettingScene::End()
{
	font->Release();
	BlackRect->Release();
}

void GameSettingScene::Update()
{
	if (Key_Up(DIK_ESCAPE))
	{
		WritePlayerSettingIbHD();
		Game_ChangeScene(GAME_STATE::Home);
	}

	if (Key_Up(DIK_RETURN))
	{
		WritePlayerSettingIbHD();
		Game_ChangeScene(GAME_STATE::Home);
	}

	if (Mouse_Button(MLButton))
	{
		mouseRect = { mousePoint.x,mousePoint.y,mousePoint.x + 10,mousePoint.y + 10 };
		UsingMouseChoose(mouseRect);
	}
	NowSettingFoucs();
}

void GameSettingScene::Render()
{
//	Sprite_Transform_Draw(GameSettingPNG, 0, 0,
	//	510, 430, 0, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	ShowSetting(200, 100);
	DrawChooseBlackRect();
}
void GSS::ShowSetting(int x, int y)
{
	char buf[10];
	const int Ymove = 100;
	FontPrint(font_s, 20, y, "可用鼠标选择，选中后按下需要替换的键\n（默认：使用方向键移动，X键发射面包）");
	FontPrint(font, x, y + 1 * Ymove, "上：");
	FontPrint(font, x, y + 2 * Ymove, "下：");
	FontPrint(font, x, y + 3 * Ymove, "左：");
	FontPrint(font, x, y + 4 * Ymove, "右：");
	FontPrint(font, x, y + 5 * Ymove, "攻击：");

	itoa(Global::PlayerControl::Player1[0], buf, 10);
	FontPrint(font, x + 96, y + 1 * Ymove, buf);
	itoa(Global::PlayerControl::Player1[1], buf, 10);
	FontPrint(font, x + 96, y + 2 * Ymove, buf);
	itoa(Global::PlayerControl::Player1[2], buf, 10);
	FontPrint(font, x + 96, y + 3 * Ymove, buf);
	itoa(Global::PlayerControl::Player1[3], buf, 10);
	FontPrint(font, x + 96, y + 4 * Ymove, buf);
	itoa(Global::PlayerControl::Player1[4], buf, 10);
	FontPrint(font, x + 160, y + 5 * Ymove, buf);

	//const int Xmove = 400;
	//FontPrint(font, x + Xmove-128, y, "玩家二：");
	//FontPrint(font, x + Xmove, y + 1 * Ymove, "上：");
	//FontPrint(font, x + Xmove, y + 2 * Ymove, "下：");
	//FontPrint(font, x + Xmove, y + 3 * Ymove, "左：");
	//FontPrint(font, x + Xmove, y + 4 * Ymove, "右：");
	//FontPrint(font, x + Xmove, y + 5 * Ymove, "攻击：");

	//itoa(Global::PlayerControl::Player2[0], buf, 10);
	//FontPrint(font, x + 96 + Xmove, y + 1 * Ymove, buf);
	//itoa(Global::PlayerControl::Player2[1], buf, 10);
	//FontPrint(font, x + 96 + Xmove, y + 2 * Ymove, buf);
	//itoa(Global::PlayerControl::Player2[2], buf, 10);
	//FontPrint(font, x + 96 + Xmove, y + 3 * Ymove, buf);
	//itoa(Global::PlayerControl::Player2[3], buf, 10);
	//FontPrint(font, x + 96 + Xmove, y + 4 * Ymove, buf);
	//itoa(Global::PlayerControl::Player2[4], buf, 10);
	//FontPrint(font, x + 160 + Xmove, y + 5 * Ymove, buf);

	FontPrint(font, 384, 832, "音乐：");
	if (Global::Sound::SoundSwicth)
		FontPrint(font, 544, 832, "开");
	else
		FontPrint(font, 544, 832, "关");

}

bool GSS::WritePlayerSettingIbHD()
{
	char buf;
	ofstream out("GameSet.set", ios::out | ios::binary);
	if (!out.is_open())
	{
		ShowMessage("无法打开游戏的设置文件");
		out.close();
		return false;
	}
	out.seekp(0, ios::beg);
	for (int i = 0; i < 5; i++)
	{
		buf= Global::PlayerControl::Player1[i];
		out.write(&buf, 1); 
	}
	for (int i = 0; i < 5; i++)
	{
		buf = Global::PlayerControl::Player2[i];
		out.write(&buf, 1);
	}
	buf = Global::Sound::SoundSwicth;
	out.write(&buf, 1);
	out.close();
	return true;
}
//扫描键盘按键信息
int GSS::ReadK_B()
{

	if (Key_Up(DIK_A))
		return DIK_A;
	if (Key_Up(DIK_B))
		return DIK_B;
	if (Key_Up(DIK_C))
		return DIK_C;
	if (Key_Up(DIK_D))
		return DIK_D;
	if (Key_Up(DIK_E))
		return DIK_E;
	if (Key_Up(DIK_F))
		return DIK_F;
	if (Key_Up(DIK_G))
		return DIK_G;
	if (Key_Up(DIK_H))
		return DIK_H;
	if (Key_Up(DIK_I))
		return DIK_I;
	if (Key_Up(DIK_J))
		return DIK_J;
	if (Key_Up(DIK_K))
		return DIK_K;
	if (Key_Up(DIK_L))
		return DIK_L;
	if (Key_Up(DIK_M))
		return DIK_M;
	if (Key_Up(DIK_N))
		return DIK_N;
	if (Key_Up(DIK_O))
		return DIK_O;
	if (Key_Up(DIK_P))
		return DIK_P;
	if (Key_Up(DIK_Q))
		return DIK_Q;
	if (Key_Up(DIK_R))
		return DIK_R;
	if (Key_Up(DIK_S))
		return DIK_S;
	if (Key_Up(DIK_T))
		return DIK_T;
	if (Key_Up(DIK_U))
		return DIK_U;
	if (Key_Up(DIK_V))
		return DIK_V;
	if (Key_Up(DIK_W))
		return DIK_W;
	if (Key_Up(DIK_X))
		return DIK_X;
	if (Key_Up(DIK_Y))
		return DIK_Y;
	if (Key_Up(DIK_Z))
		return DIK_Z;

	if (Key_Up(DIK_0))
		return DIK_0;
	if (Key_Up(DIK_1))
		return DIK_1;
	if (Key_Up(DIK_2))
		return DIK_2;
	if (Key_Up(DIK_3))
		return DIK_3;
	if (Key_Up(DIK_4))
		return DIK_4;
	if (Key_Up(DIK_5))
		return DIK_5;
	if (Key_Up(DIK_6))
		return DIK_6;
	if (Key_Up(DIK_7))
		return DIK_7;
	if (Key_Up(DIK_8))
		return DIK_8;
	if (Key_Up(DIK_9))
		return DIK_9;


	if (Key_Up(DIK_BACK))
		return DIK_BACK;
	if (Key_Up(DIK_RETURN))
		return DIK_RETURN;
	if (Key_Up(DIK_MINUS))
		return DIK_MINUS;
	if (Key_Up(DIK_EQUALS))
		return DIK_EQUALS;
	if (Key_Up(DIK_BACK))
		return DIK_BACK;
	if (Key_Up(DIK_TAB))
		return DIK_TAB;
	if (Key_Up(DIK_LBRACKET))
		return DIK_LBRACKET;
	if (Key_Up(DIK_RBRACKET))
		return DIK_RBRACKET;
	if (Key_Up(DIK_RETURN))
		return DIK_RBRACKET;
	if (Key_Up(DIK_LCONTROL))
		return DIK_LCONTROL;
	if (Key_Up(DIK_SEMICOLON))
		return DIK_SEMICOLON;
	if (Key_Up(DIK_APOSTROPHE))
		return DIK_APOSTROPHE;
	if (Key_Up(DIK_GRAVE))
		return DIK_GRAVE;
	if (Key_Up(DIK_LSHIFT))
		return DIK_LSHIFT;
	if (Key_Up(DIK_BACKSLASH))
		return DIK_BACKSLASH;
	if (Key_Up(DIK_COMMA))
		return DIK_COMMA;;
	if (Key_Up(DIK_PERIOD))
		return DIK_PERIOD;
	if (Key_Up(DIK_SLASH))
		return DIK_SLASH;
	if (Key_Up(DIK_RSHIFT))
		return DIK_RSHIFT;
	if (Key_Up(DIK_MULTIPLY))
		return DIK_MULTIPLY;
	if (Key_Up(DIK_LMENU))
		return DIK_LMENU;
	if (Key_Up(DIK_SPACE))
		return DIK_SPACE;
	if (Key_Up(DIK_CAPITAL))
		return DIK_CAPITAL;
	if (Key_Up(DIK_NUMLOCK))
		return DIK_NUMLOCK;
	if (Key_Up(DIK_SCROLL))
		return DIK_SCROLL;
	if (Key_Up(DIK_NUMPAD7))
		return DIK_NUMPAD7;
	if (Key_Up(DIK_NUMPAD8))
		return DIK_NUMPAD8;
	if (Key_Up(DIK_NUMPAD9))
		return DIK_NUMPAD9;
	if (Key_Up(DIK_SUBTRACT))
		return DIK_SUBTRACT;
	if (Key_Up(DIK_NUMPAD4))
		return DIK_NUMPAD4;
	if (Key_Up(DIK_NUMPAD5))
		return DIK_NUMPAD5;
	if (Key_Up(DIK_NUMPAD6))
		return DIK_NUMPAD6;
	if (Key_Up(DIK_ADD))
		return DIK_ADD;
	if (Key_Up(DIK_NUMPAD1))
		return DIK_NUMPAD1;
	if (Key_Up(DIK_NUMPAD2))
		return DIK_NUMPAD2;
	if (Key_Up(DIK_NUMPAD3))
		return DIK_NUMPAD3;
	if (Key_Up(DIK_NUMPAD0))
		return DIK_NUMPAD0;
	if (Key_Up(DIK_DECIMAL))
		return DIK_DECIMAL;
	if (Key_Up(DIK_OEM_102))
		return DIK_OEM_102;
	if (Key_Up(DIK_KANA))
		return DIK_KANA;
	if (Key_Up(DIK_ABNT_C1))
		return DIK_ABNT_C1;
	if (Key_Up(DIK_CONVERT))
		return DIK_CONVERT;
	if (Key_Up(DIK_NOCONVERT))
		return DIK_NOCONVERT;
	if (Key_Up(DIK_YEN))
		return DIK_YEN;
	if (Key_Up(DIK_ABNT_C2))
		return DIK_ABNT_C2;
	if (Key_Up(DIK_NUMPADEQUALS))
		return DIK_NUMPADEQUALS;
	if (Key_Up(DIK_PREVTRACK))
		return DIK_PREVTRACK;
	if (Key_Up(DIK_AT))
		return DIK_AT;
	if (Key_Up(DIK_COLON))
		return DIK_COLON;
	if (Key_Up(DIK_UNDERLINE))
		return DIK_UNDERLINE;
	if (Key_Up(DIK_KANJI))
		return DIK_KANJI;
	if (Key_Up(DIK_STOP))
		return DIK_STOP;
	if (Key_Up(DIK_AX))
		return DIK_AX;
	if (Key_Up(DIK_UNLABELED))
		return DIK_UNLABELED;
	if (Key_Up(DIK_NEXTTRACK))
		return DIK_NEXTTRACK;
	if (Key_Up(DIK_NUMPADENTER))
		return DIK_NUMPADENTER;
	if (Key_Up(DIK_RCONTROL))
		return DIK_RCONTROL;;
	if (Key_Up(DIK_MUTE))
		return DIK_MUTE;
	if (Key_Up(DIK_CALCULATOR))
		return DIK_CALCULATOR;
	if (Key_Up(DIK_PLAYPAUSE))
		return DIK_PLAYPAUSE;
	if (Key_Up(DIK_MEDIASTOP))
		return DIK_MEDIASTOP;
	if (Key_Up(DIK_VOLUMEDOWN))
		return DIK_VOLUMEDOWN;
	if (Key_Up(DIK_VOLUMEUP))
		return DIK_VOLUMEUP;
	if (Key_Up(DIK_WEBHOME))
		return DIK_WEBHOME;
	if (Key_Up(DIK_NUMPADCOMMA))
		return DIK_NUMPADCOMMA;
	if (Key_Up(DIK_DIVIDE))
		return DIK_DIVIDE;;
	if (Key_Up(DIK_SYSRQ))
		return DIK_SYSRQ;
	if (Key_Up(DIK_RMENU))
		return DIK_RMENU;
	if (Key_Up(DIK_PAUSE))
		return DIK_PAUSE;
	if (Key_Up(DIK_HOME))
		return DIK_HOME;
	if (Key_Up(DIK_UP))
		return DIK_UP;
	if (Key_Up(DIK_PRIOR))
		return DIK_PRIOR;
	if (Key_Up(DIK_LEFT))
		return DIK_LEFT;
	if (Key_Up(DIK_RIGHT))
		return DIK_RIGHT;
	if (Key_Up(DIK_END))
		return DIK_END;
	if (Key_Up(DIK_DOWN))
		return DIK_DOWN;
	if (Key_Up(DIK_NEXT))
		return DIK_NEXT;
	if (Key_Up(DIK_INSERT))
		return DIK_INSERT;
	if (Key_Up(DIK_DELETE))
		return DIK_DELETE;
	if (Key_Up(DIK_LWIN))
		return DIK_LWIN;
	if (Key_Up(DIK_RWIN))
		return DIK_RWIN;
	if (Key_Up(DIK_APPS))
		return DIK_APPS;
	if (Key_Up(DIK_POWER))
		return DIK_POWER;
	if (Key_Up(DIK_SLEEP))
		return DIK_SLEEP;
	if (Key_Up(DIK_WAKE))
		return DIK_WAKE;
	if (Key_Up(DIK_WEBSEARCH))
		return DIK_WEBSEARCH;
	if (Key_Up(DIK_WEBFAVORITES))
		return DIK_WEBFAVORITES;
	if (Key_Up(DIK_WEBREFRESH))
		return DIK_WEBREFRESH;
	if (Key_Up(DIK_WEBSTOP))
		return DIK_WEBSTOP;
	if (Key_Up(DIK_WEBFORWARD))
		return DIK_WEBFORWARD;
	if (Key_Up(DIK_WEBBACK))
		return DIK_WEBBACK;
	if (Key_Up(DIK_MYCOMPUTER))
		return DIK_MYCOMPUTER;
	if (Key_Up(DIK_MAIL))
		return DIK_MAIL;
	if (Key_Up(DIK_MEDIASELECT))
		return DIK_MEDIASELECT;

	return -1;
}
//使用鼠标进行选择
void GSS::UsingMouseChoose(RECT&mrect)
{
	RECT rect, nothing;
	rect = { 296,200,424,264 };
	//设置玩家一
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 0;
	rect = { 296,300,424,364 };
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 1;
	rect = { 296,400,424,464 };
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 2;
	rect = { 296,500,424,564 };
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 3;
	rect = { 296,600,424,664 };
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 4;
	//设置玩家二
	rect = { 696,200,824,264 };
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 5;
	rect = { 696,300,824,364 };
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 6;
	rect = { 696,400,824,464 };
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 7;
	rect = { 696,500,824,564 };
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 8;
	rect = { 696,600,824,664 };
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 9;
	//设置音乐开关
	rect = { 544,832,608,896 };
	if (IntersectRect(&nothing, &rect, &mrect))
		Global::Sound::SoundSwicth= !Global::Sound::SoundSwicth;

}
//现在的设置焦点
void GSS::NowSettingFoucs()
{
	int buf;
	if (Choose / 5 == 0)
	{
		buf= ReadK_B();
		if (buf != -1)
		{
			Global::PlayerControl::Player1[Choose] = buf;
			Choose = Choose == 9 ? 0 : Choose + 1;
		}
	}
	else
	{
		buf = ReadK_B();
		if (buf != -1)
		{
			Global::PlayerControl::Player2[Choose-5] = buf;
			Choose = Choose == 9 ? 0 : Choose + 1;
		}

	}
}
//用黑框凸显现在选择的选项
void GSS::DrawChooseBlackRect()
{
	switch (Choose)
	{
	case 0:
		DrawBlackRect(296,200);
		break;
	case 1:
		DrawBlackRect(296,300);
		break;
	case 2:
		DrawBlackRect(293,400);
		break;
	case 3:
		DrawBlackRect(296,500);
		break;
	case 4:
		DrawBlackRect(360,600);
		break;

	case 5:
		DrawBlackRect(696,200);
		break;
	case 6:
		DrawBlackRect(696,300);
		break;
	case 7:
		DrawBlackRect(696,400);
		break;
	case 8:
		DrawBlackRect(696,500);
		break;
	case 9:
		DrawBlackRect(760,600);
		break;
	default:
		break;
	}
}
//画黑色方框
void GSS::DrawBlackRect(int x, int y)
{
	RECT rect;
	FillRect(rect, x - 3, x + 67, y - 3, y);
	d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, x - 3, x + 67, y + 64, y + 67);
	d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, x - 3, x, y, y + 64);
	d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, x + 64, x + 67, y, y + 64);
	d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);

}
//填充RECT
void GSS::FillRect(RECT&rect, long l, long r, long t, long b)
{
	rect.left = l;
	rect.right = r;
	rect.top = t;
	rect.bottom = b;
}
