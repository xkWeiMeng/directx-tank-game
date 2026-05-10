#include "GameSettingScene.h"
#include "KeyNames.h"
#include <cmath>
#include <cstdlib>
#pragma warning(disable:4996)
namespace GSS {
	LPD3DXFONT font = NULL;
	LPD3DXFONT fontShadow = NULL;
	LPD3DXFONT titleFont = NULL;
	LPD3DXFONT titleFontShadow = NULL;
	LPD3DXFONT hintFont = NULL;
	LPDIRECT3DTEXTURE9 OverlayTex = NULL;
	LPDIRECT3DTEXTURE9 TankTex = NULL;
	RECT mouseRect;

	int Choose=0;
	int scrollY = 0;
	const int Ymove = 80;

	bool WritePlayerSettingIbHD();
	int ReadK_B();
	void UsingMouseChoose(RECT & mrect);
	void NowSettingFoucs();
	void FillRect(RECT & rect, long l, long r, long t, long b);

	// 窗口大小档位名称
	const char* WindowSizeLevelNames[] = { "小", "中", "大", "全屏" };

	// ---------- 粒子系统 ----------
	static const int MAX_PARTICLES = 50;
	struct Particle {
		float x, y, speedY, speedX, size;
		int alpha, maxAlpha;
		D3DCOLOR color;
	};
	static Particle particles[MAX_PARTICLES];
	static bool particlesInited = false;

	static void InitParticles()
	{
		for (int i = 0; i < MAX_PARTICLES; i++)
		{
			Particle &p = particles[i];
			p.x = (float)(rand() % 1024);
			p.y = (float)(rand() % 960);
			p.speedY = -(15.0f + (float)(rand() % 25));
			p.speedX = (float)(rand() % 20 - 10) * 0.5f;
			p.size = 1.5f + (float)(rand() % 30) * 0.1f;
			p.maxAlpha = 40 + rand() % 60;
			p.alpha = rand() % p.maxAlpha;
			int colorType = rand() % 3;
			if (colorType == 0)
				p.color = D3DCOLOR_XRGB(255, 180, 60);
			else if (colorType == 1)
				p.color = D3DCOLOR_XRGB(100, 160, 255);
			else
				p.color = D3DCOLOR_XRGB(180, 180, 200);
		}
		particlesInited = true;
	}

	static void UpdateParticles(float dt)
	{
		for (int i = 0; i < MAX_PARTICLES; i++)
		{
			Particle &p = particles[i];
			p.y += p.speedY * dt;
			p.x += p.speedX * dt;
			if (p.y < -10.0f)
			{
				p.y = 970.0f;
				p.x = (float)(rand() % 1024);
				p.speedY = -(15.0f + (float)(rand() % 25));
				p.speedX = (float)(rand() % 20 - 10) * 0.5f;
				p.alpha = 0;
			}
			if (p.x < -10.0f) p.x = 1034.0f;
			if (p.x > 1034.0f) p.x = -10.0f;
		}
	}

	static void RenderParticles()
	{
		float t = (float)GetTickCount() / 1000.0f;
		for (int i = 0; i < MAX_PARTICLES; i++)
		{
			Particle &p = particles[i];
			float flicker = 0.6f + 0.4f * sinf(t * (2.0f + i * 0.1f) + (float)i);
			int a = (int)(p.maxAlpha * flicker);
			if (a < 0) a = 0;
			if (a > 255) a = 255;
			D3DCOLOR c = D3DCOLOR_ARGB(a,
				(p.color >> 16) & 0xFF,
				(p.color >> 8) & 0xFF,
				p.color & 0xFF);
			Sprite_Transform_Draw(OverlayTex, (int)p.x, (int)p.y, 1, 1,
				0, 1, 0, p.size, c);
		}
	}

	// ---------- 渲染辅助 ----------
	// 带阴影的文字渲染
	static void FontPrint3D(LPD3DXFONT mainFont, LPD3DXFONT shadowFont,
		int x, int y, const char* text, D3DCOLOR mainColor,
		D3DCOLOR shadowColor = D3DCOLOR_ARGB(160, 0, 0, 0))
	{
		FontPrint(shadowFont, x + 2, y + 3, text, shadowColor);
		FontPrint(mainFont, x, y, text, mainColor);
	}

	// 绘制选中高亮条
	static void DrawHighlightBar(int x, int y, float barW, float barH, float t)
	{
		float pulse = 0.3f + 0.15f * sinf(t * 3.0f);
		int barAlpha = (int)(pulse * 255);
		if (barAlpha > 255) barAlpha = 255;
		Sprite_Transform_Draw(OverlayTex, x, y, 1, 1, 0, 1, 0,
			barW, barH, D3DCOLOR_ARGB(barAlpha, 255, 180, 30));
	}

	// 绘制分隔线
	static void DrawSeparator(int y, int alpha)
	{
		Sprite_Transform_Draw(OverlayTex, 80, y, 1, 1, 0, 1, 0,
			864.0f, 1.0f, D3DCOLOR_ARGB(alpha, 100, 120, 150));
	}

	static DWORD lastFrameTime = 0;

	// ---------- 渲染设置内容 ----------
	static void RenderSettings(int x, int y)
	{
		float t = (float)GetTickCount() / 1000.0f;
		const int Xmove = 400;

		// ===== 分隔线: 标题下方 =====
		DrawSeparator(y - 10, 60);

		// ===== 玩家一标题 =====
		float p1Pulse = 0.5f + 0.5f * sinf(t * 2.5f);
		int p1G = (int)(180 + p1Pulse * 60);
		FontPrint3D(font, fontShadow, x - 128, y, "玩家一：",
			D3DCOLOR_XRGB(100, p1G, 255));

		// ===== 玩家二标题 =====
		float p2Pulse = 0.5f + 0.5f * sinf(t * 2.5f + 1.0f);
		int p2G = (int)(200 + p2Pulse * 55);
		FontPrint3D(font, fontShadow, x + Xmove - 128, y, "玩家二：",
			D3DCOLOR_XRGB(80, 255, p2G));

		// ===== 按键标签和值 =====
		const char* labels[] = { "上：", "下：", "左：", "右：", "攻击：", "切换：" };
		int labelX_short = x;
		int labelX_long = x;
		int valueX_short = x + 96;
		int valueX_long = x + 160;

		for (int i = 0; i < 6; i++)
		{
			int row = i + 1;
			int lx = (i < 4) ? labelX_short : labelX_long;
			int vx = (i < 4) ? valueX_short : valueX_long;

			// 玩家一标签
			float subtle = 0.5f + 0.5f * sinf(t * 1.5f + i * 0.3f);
			int gray = (int)(160 + subtle * 40);
			FontPrint3D(font, fontShadow, lx, y + row * Ymove, labels[i],
				D3DCOLOR_XRGB(gray, gray, gray + 15));

			// 玩家一值
			bool isSelected = (Choose == i);
			if (isSelected)
			{
				DrawHighlightBar(vx - 8, y + row * Ymove - 4, 140.0f, 72.0f, t);
				// 坦克光标
				int cursorFrame = 1 * 8 + ((GetTickCount() / 150) % 2);
				float bob = sinf(t * 5.0f) * 3.0f;
				Sprite_Transform_Draw(TankTex, vx - 50 + (int)bob, y + row * Ymove + 6, 28, 28,
					cursorFrame, 8, 0, 1.2f, D3DCOLOR_XRGB(255, 255, 100));
				float txtPulse = 0.5f + 0.5f * sinf(t * 4.0f);
				int mainG = (int)(220 + txtPulse * 35);
				int mainB = (int)(60 + txtPulse * 40);
				FontPrint3D(font, fontShadow, vx, y + row * Ymove,
					GetDIKKeyName(Global::PlayerControl::Player1[i]),
					D3DCOLOR_XRGB(255, mainG, mainB));
			}
			else
			{
				FontPrint3D(font, fontShadow, vx, y + row * Ymove,
					GetDIKKeyName(Global::PlayerControl::Player1[i]),
					D3DCOLOR_XRGB(200, 200, 210));
			}

			// 玩家二标签
			FontPrint3D(font, fontShadow, lx + Xmove, y + row * Ymove, labels[i],
				D3DCOLOR_XRGB(gray, gray, gray + 15));

			// 玩家二值
			bool isSelected2 = (Choose == i + 6);
			if (isSelected2)
			{
				DrawHighlightBar(vx + Xmove - 8, y + row * Ymove - 4, 140.0f, 72.0f, t);
				int cursorFrame2 = 1 * 8 + ((GetTickCount() / 150) % 2);
				float bob2 = sinf(t * 5.0f) * 3.0f;
				Sprite_Transform_Draw(TankTex, vx + Xmove - 50 + (int)bob2, y + row * Ymove + 6, 28, 28,
					cursorFrame2, 8, 0, 1.2f, D3DCOLOR_XRGB(100, 255, 150));
				float txtPulse2 = 0.5f + 0.5f * sinf(t * 4.0f);
				int mainG2 = (int)(220 + txtPulse2 * 35);
				int mainB2 = (int)(60 + txtPulse2 * 40);
				FontPrint3D(font, fontShadow, vx + Xmove, y + row * Ymove,
					GetDIKKeyName(Global::PlayerControl::Player2[i]),
					D3DCOLOR_XRGB(255, mainG2, mainB2));
			}
			else
			{
				FontPrint3D(font, fontShadow, vx + Xmove, y + row * Ymove,
					GetDIKKeyName(Global::PlayerControl::Player2[i]),
					D3DCOLOR_XRGB(200, 200, 210));
			}
		}

		// ===== 分隔线: 按键设置下方 =====
		DrawSeparator(y + 7 * Ymove - 15, 40);

		// ===== 窗口大小 =====
		FontPrint3D(font, fontShadow, 320, y + 7 * Ymove, "窗口大小：",
			D3DCOLOR_XRGB(180, 180, 200));
		// < 按钮
		float arrowPulse = 0.5f + 0.5f * sinf(t * 3.0f);
		int arrowBright = (int)(180 + arrowPulse * 75);
		FontPrint3D(font, fontShadow, 608, y + 7 * Ymove, "<",
			D3DCOLOR_XRGB(arrowBright, arrowBright, 255));
		FontPrint3D(font, fontShadow, 660, y + 7 * Ymove,
			WindowSizeLevelNames[Global::Window::WindowSizeLevel],
			D3DCOLOR_XRGB(255, 220, 100));
		FontPrint3D(font, fontShadow, 780, y + 7 * Ymove, ">",
			D3DCOLOR_XRGB(arrowBright, arrowBright, 255));

		// ===== 音乐 =====
		FontPrint3D(font, fontShadow, 384, y + 8 * Ymove, "音乐：",
			D3DCOLOR_XRGB(180, 180, 200));
		if (Global::Sound::SoundSwicth)
			FontPrint3D(font, fontShadow, 544, y + 8 * Ymove, "开",
				D3DCOLOR_XRGB(80, 255, 120));
		else
			FontPrint3D(font, fontShadow, 544, y + 8 * Ymove, "关",
				D3DCOLOR_XRGB(255, 80, 80));

		// ===== Debug模式 =====
		FontPrint3D(font, fontShadow, 320, y + 9 * Ymove, "Debug模式：",
			D3DCOLOR_XRGB(180, 180, 200));
		if (Global::Debug::ShowDebugInfo)
			FontPrint3D(font, fontShadow, 608, y + 9 * Ymove, "开",
				D3DCOLOR_XRGB(80, 255, 120));
		else
			FontPrint3D(font, fontShadow, 608, y + 9 * Ymove, "关",
				D3DCOLOR_XRGB(255, 80, 80));
	}
}
using namespace GSS;
bool GameSettingScene::Init()
{
	scrollY = 0;
	font = MakeFont("微软雅黑", 64);
	fontShadow = MakeFont("微软雅黑", 66);
	titleFont = MakeFont("Impact", 80);
	titleFontShadow = MakeFont("Impact", 82);
	hintFont = MakeFont("微软雅黑", 24);

	TankTex = LoadTexture(Resource::Texture::Player_1, D3DCOLOR_XRGB(0, 0, 0));

	// 创建1x1白色纹理（用于遮罩、粒子、高亮条）
	HRESULT result = D3DXCreateTexture(d3dDev, 1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &OverlayTex);
	if (result == D3D_OK)
	{
		D3DLOCKED_RECT lr;
		if (SUCCEEDED(OverlayTex->LockRect(0, &lr, NULL, 0)))
		{
			*((DWORD*)lr.pBits) = 0xFFFFFFFF;
			OverlayTex->UnlockRect(0);
		}
	}

	lastFrameTime = GetTickCount();
	particlesInited = false;
	InitParticles();

	return true;
}

void GameSettingScene::End()
{
	if (font) { font->Release(); font = NULL; }
	if (fontShadow) { fontShadow->Release(); fontShadow = NULL; }
	if (titleFont) { titleFont->Release(); titleFont = NULL; }
	if (titleFontShadow) { titleFontShadow->Release(); titleFontShadow = NULL; }
	if (hintFont) { hintFont->Release(); hintFont = NULL; }
	SAFE_RELEASE(OverlayTex);
	SAFE_RELEASE(TankTex);
}

void GameSettingScene::Update()
{
	// 更新粒子
	DWORD now = GetTickCount();
	float dt = (float)(now - lastFrameTime) / 1000.0f;
	if (dt > 0.05f) dt = 0.05f;
	lastFrameTime = now;
	UpdateParticles(dt);

	if (Key_Up(DIK_ESCAPE))
	{
		WritePlayerSettingIbHD();
		Game_ChangeScene(GAME_STATE::Home);
		return;
	}

	if (Key_Up(DIK_RETURN))
	{
		WritePlayerSettingIbHD();
		Game_ChangeScene(GAME_STATE::Home);
		return;
	}

	// 鼠标滚轮滚动
	int wheelDelta = Mouse_Z();
	if (wheelDelta != 0)
	{
		scrollY += wheelDelta / 3;
		if (scrollY > 0) scrollY = 0;
		int contentBottom = 40 + 9 * Ymove + 64;
		int minScroll = contentBottom > 960 ? -(contentBottom - 960) : 0;
		if (scrollY < minScroll) scrollY = minScroll;
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
	float t = (float)GetTickCount() / 1000.0f;

	spriteObj->Begin(D3DXSPRITE_ALPHABLEND);

	// ===== 深色背景覆盖 =====
	Sprite_Transform_Draw(OverlayTex, 0, 0, 1, 1, 0, 1, 0,
		1024.0f, 960.0f, D3DCOLOR_XRGB(10, 10, 26));

	// ===== 动态背景粒子 =====
	RenderParticles();

	// ===== 标题 "游戏设定" =====
	float pulse = 0.5f + 0.5f * sinf(t * 2.0f);
	int glowR = 255;
	int glowG = (int)(160 + pulse * 80);
	int glowB = (int)(20 + pulse * 40);

	FontPrint(titleFontShadow, 354, 24, "游 戏 设 定", D3DCOLOR_ARGB(180, 0, 0, 0));
	FontPrint(titleFontShadow, 352, 20, "游 戏 设 定",
		D3DCOLOR_ARGB((int)(50 + pulse * 40), glowR, glowG, 0));
	FontPrint(titleFont, 352, 20, "游 戏 设 定", D3DCOLOR_XRGB(glowR, glowG, glowB));

	// ===== 设置内容 =====
	RenderSettings(200, 130 + scrollY);

	// ===== 底部提示 =====
	if ((GetTickCount() / 600) % 2 == 0)
	{
		FontPrint(hintFont, 370, 920, "ESC / ENTER 返回主菜单",
			D3DCOLOR_XRGB(180, 180, 180));
	}

	spriteObj->End();
}

bool GSS::WritePlayerSettingIbHD()
{
	unsigned char buf;
	ofstream out("GameSet.set", ios::out | ios::binary);
	if (!out.is_open())
	{
		ShowMessage("无法打开游戏的设置文件");
		out.close();
		return false;
	}
	out.seekp(0, ios::beg);
	for (int i = 0; i < 6; i++)
	{
		buf= (unsigned char)Global::PlayerControl::Player1[i];
		out.write((char*)&buf, 1); 
	}
	for (int i = 0; i < 6; i++)
	{
		buf = (unsigned char)Global::PlayerControl::Player2[i];
		out.write((char*)&buf, 1);
	}
	buf = (unsigned char)Global::Sound::SoundSwicth;
	out.write((char*)&buf, 1);
	buf = (unsigned char)Global::Window::WindowSizeLevel;
	out.write((char*)&buf, 1);
	buf = (unsigned char)Global::Debug::ShowDebugInfo;
	out.write((char*)&buf, 1);
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
	int by = 130 + scrollY;
	//设置玩家一
	FillRect(rect, 296, 424, by + Ymove, by + Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 0;
	FillRect(rect, 296, 424, by + 2*Ymove, by + 2*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 1;
	FillRect(rect, 296, 424, by + 3*Ymove, by + 3*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 2;
	FillRect(rect, 296, 424, by + 4*Ymove, by + 4*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 3;
	FillRect(rect, 360, 488, by + 5*Ymove, by + 5*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 4;
	FillRect(rect, 360, 488, by + 6*Ymove, by + 6*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 5;
	//设置玩家二
	FillRect(rect, 696, 824, by + Ymove, by + Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 6;
	FillRect(rect, 696, 824, by + 2*Ymove, by + 2*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 7;
	FillRect(rect, 696, 824, by + 3*Ymove, by + 3*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 8;
	FillRect(rect, 696, 824, by + 4*Ymove, by + 4*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 9;
	FillRect(rect, 760, 888, by + 5*Ymove, by + 5*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 10;
	FillRect(rect, 760, 888, by + 6*Ymove, by + 6*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Choose = 11;
	//设置音乐开关
	FillRect(rect, 544, 608, by + 8*Ymove, by + 8*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Global::Sound::SoundSwicth= !Global::Sound::SoundSwicth;

	// 窗口大小 "<" 按钮
	FillRect(rect, 608, 660, by + 7*Ymove, by + 7*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
	{
		int level = Global::Window::WindowSizeLevel - 1;
		if (level < 0) level = Global::Window::WindowSizeLevelCount - 1;
		ApplyWindowSize(level);
	}
	// 窗口大小 ">" 按钮
	FillRect(rect, 780, 832, by + 7*Ymove, by + 7*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
	{
		int level = (Global::Window::WindowSizeLevel + 1) % Global::Window::WindowSizeLevelCount;
		ApplyWindowSize(level);
	}

	// Debug模式开关
	FillRect(rect, 608, 672, by + 9*Ymove, by + 9*Ymove + 64);
	if (IntersectRect(&nothing, &rect, &mrect))
		Global::Debug::ShowDebugInfo = !Global::Debug::ShowDebugInfo;

}
//现在的设置焦点
void GSS::NowSettingFoucs()
{
	int buf;
	if (Choose / 6 == 0)
	{
		buf= ReadK_B();
		if (buf != -1)
		{
			Global::PlayerControl::Player1[Choose] = buf;
			Choose = Choose == 11 ? 0 : Choose + 1;
		}
	}
	else
	{
		buf = ReadK_B();
		if (buf != -1)
		{
			Global::PlayerControl::Player2[Choose-6] = buf;
			Choose = Choose == 11 ? 0 : Choose + 1;
		}

	}
}
//填充RECT
void GSS::FillRect(RECT&rect, long l, long r, long t, long b)
{
	rect.left = l;
	rect.right = r;
	rect.top = t;
	rect.bottom = b;
}
