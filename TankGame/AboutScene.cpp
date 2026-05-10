#include "AboutScene.h"
#include"DirectX.h"
#include"GameMain.h"
#include<cmath>

// 云动画参数: 1536x640, 4列x2行, 每帧384x320
static const int CLOUD_FRAME_W = 384;
static const int CLOUD_FRAME_H = 320;
static const int CLOUD_COLUMNS = 4;

// 飞艇动画参数: 3584x800, 4列x2行, 每帧896x400
static const int AIRSHIP_FRAME_W = 896;
static const int AIRSHIP_FRAME_H = 400;
static const int AIRSHIP_COLUMNS = 4;

// 落叶动画参数: 512x256, 4列x2行, 每帧128x128
static const int LEAF_FRAME_W = 128;
static const int LEAF_FRAME_H = 128;
static const int LEAF_COLUMNS = 4;
static const int LEAF_TOTAL_FRAMES = 8;

namespace AS {
	static DWORD lastTime = 0;
	static LPD3DXFONT font = NULL;
	static LPD3DXFONT fontSmall = NULL;

	struct Cloud {
		float x, y;
		int frame;
		float speed;
		float scale;
	};

	static const int FAR_CLOUD_COUNT = 5;   // 超远: 山脊处, 被山遮挡
	static const int MID_CLOUD_COUNT = 5;   // 中远: 画面中间偏上
	static const int NEAR_CLOUD_COUNT = 5;  // 近处: 画面顶部
	static Cloud farClouds[FAR_CLOUD_COUNT];
	static Cloud midClouds[MID_CLOUD_COUNT];
	static Cloud nearClouds[NEAR_CLOUD_COUNT];

	struct Airship {
		bool active;
		float x, y;
		float speed;
		int frame;
		float animTimer;
		float cooldown;
		float scale;
	};
	static Airship airship;

	// ====== 落叶系统 ======
	// 运动模式
	enum LeafMotion {
		MOTION_WAVE = 0,    // 波浪飘落
		MOTION_SPIRAL,      // 螺旋下降
		MOTION_ZIGZAG,      // Z字型折返
		MOTION_TUMBLE,      // 翻滚直落
		MOTION_COUNT
	};

	struct LeafType {
		float fallSpeed;
		float swayAmp;
		float swayFreq;
		float rotSpeed;
		float windDrift;
		float baseScale;
	};

	static const LeafType leafTypes[8] = {
		// 帧0: 枫叶
		{ 65.0f, 80.0f, 1.2f, 0.8f, 120.0f, 0.10f },
		// 帧1: 枝叶
		{ 90.0f, 40.0f, 0.8f, 0.5f, 95.0f, 0.09f },
		// 帧2: 圆叶
		{ 55.0f, 90.0f, 1.5f, 1.2f, 135.0f, 0.09f },
		// 帧3: 散叶
		{ 35.0f, 120.0f, 1.8f, 1.5f, 160.0f, 0.08f },
		// 帧4: 残花
		{ 100.0f, 25.0f, 0.6f, 0.3f, 70.0f, 0.08f },
		// 帧5: 宽叶
		{ 110.0f, 35.0f, 0.7f, 0.4f, 80.0f, 0.11f },
		// 帧6: 尖叶
		{ 70.0f, 70.0f, 1.3f, 1.0f, 110.0f, 0.09f },
		// 帧7: 细叶
		{ 28.0f, 140.0f, 2.0f, 2.0f, 190.0f, 0.07f },
	};

	struct Leaf {
		float x, y;
		int frame;
		float rotation;
		float scale;
		float phase;
		float lifetime;
		int alpha;
		int motion;        // LeafMotion
		float spiralRadius; // 螺旋半径
		float spiralSpeed;  // 螺旋角速度
	};

	static const int LEAF_COUNT = 7;
	static Leaf leaves[LEAF_COUNT];

	static void SpawnLeaf(Leaf& lf) {
		int sw = Global::Window::ScreenWidth;
		int sh = Global::Window::ScreenHeight;
		lf.frame = rand() % LEAF_TOTAL_FRAMES;
		// 从右侧生成，向左飘
		lf.x = (float)sw + (float)(rand() % 300);
		lf.y = (float)(sh * 55 / 100) + (float)(rand() % (sh * 20 / 100));
		lf.rotation = (float)(rand() % 628) / 100.0f;
		const LeafType& lt = leafTypes[lf.frame];
		lf.scale = lt.baseScale * (0.8f + (float)(rand() % 40) / 100.0f);
		lf.phase = (float)(rand() % 628) / 100.0f;
		lf.lifetime = 0;
		lf.alpha = 200 + rand() % 56;
		lf.motion = rand() % MOTION_COUNT;
		lf.spiralRadius = 20.0f + (float)(rand() % 40);
		lf.spiralSpeed = 2.0f + (float)(rand() % 200) / 100.0f;
	}

	static void InitLeaves() {
		for (int i = 0; i < LEAF_COUNT; i++) {
			SpawnLeaf(leaves[i]);
			int sw = Global::Window::ScreenWidth;
			int sh = Global::Window::ScreenHeight;
		// 初始分布在屏幕内
			leaves[i].x = (float)(rand() % (sw + 200)) - 100.0f;
			leaves[i].y = (float)(sh * 55 / 100) + (float)(rand() % (sh * 45 / 100));
			leaves[i].lifetime = (float)(rand() % 500) / 100.0f;
		}
	}

	static float c1 = 255, c2 = 255, c3 = 255;

	// ====== 风吹摇曳系统 ======
	static float windTime = 0;

	static void DrawWithShear(LPDIRECT3DTEXTURE9 tex, int w, int h, float shearX) {
		D3DXMATRIX mat;
		D3DXMatrixIdentity(&mat);
		mat._21 = -shearX;
		mat._41 = shearX * (float)h;
		spriteObj->SetTransform(&mat);

		RECT srcRect = { 0, 0, w, h };
		spriteObj->Draw(tex, &srcRect, NULL, NULL, D3DCOLOR_XRGB(255, 255, 255));

		D3DXMatrixIdentity(&mat);
		spriteObj->SetTransform(&mat);
	}

	// Cloud 结构: { x坐标, y坐标, 精灵帧索引, 移动速度(px/s), 缩放比例 }
	// 精灵表布局: 4列x2行, 帧0-3=第1行(扁平小云), 帧4-7=第2行(高大积云)
	// 同一帧可实例化多个云朵对象, 用不同位置/速度/缩放产生变化
	// 所有位置和速度均基于屏幕尺寸比例, 适配不同分辨率

	// 超远云: 山脊附近, 部分被山景遮挡, 最小最慢
	static void SpawnFarCloud(Cloud& c, bool initialSpread) {
		int sw = Global::Window::ScreenWidth;
		int sh = Global::Window::ScreenHeight;
		c.frame = rand() % 4;           // 帧0-3: 扁平小云
		c.scale = 0.4f + (float)(rand() % 12) / 100.0f;  // 0.38-0.50
		c.speed = (float)sw * (0.004f + (float)(rand() % 6) / 1000.0f); // ~0.4%-1.0% sw/s
		c.y = (float)sh * (0.29f + (float)(rand() % 13) / 100.0f);      // y=29%-42% sh
		if (initialSpread)
			c.x = (float)(rand() % (int)(sw * 1.4f)) - sw * 0.2f;
		else
			c.x = (float)sw + sw * 0.05f + (float)(rand() % (int)(sw * 0.5f));
	}

	// 中远云: 画面中间偏上, 山景之前, 中等大小
	static void SpawnMidCloud(Cloud& c, bool initialSpread) {
		int sw = Global::Window::ScreenWidth;
		int sh = Global::Window::ScreenHeight;
		c.frame = rand() % 8;           // 帧0-7: 任意云朵
		c.scale = 0.2f + (float)(rand() % 20) / 100.0f;  // 0.28-0.48
		c.speed = (float)sw * (0.005f + (float)(rand() % 5) / 1000.0f);  // ~0.5%-1.0% sw/s
		c.y = (float)sh * (0.125f + (float)(rand() % 19) / 100.0f);      // y=12.5%-31% sh
		if (initialSpread)
			c.x = (float)(rand() % (int)(sw * 1.5f)) - sw * 0.25f;
		else
			c.x = (float)sw + sw * 0.06f + (float)(rand() % (int)(sw * 0.6f));
	}

	// 近处云: 画面顶部, 最大最快
	static void SpawnNearCloud(Cloud& c, bool initialSpread) {
		int sw = Global::Window::ScreenWidth;
		int sh = Global::Window::ScreenHeight;
		c.frame = 4 + rand() % 4;       // 帧4-7: 高大积云
		c.scale = 0.35f + (float)(rand() % 15) / 100.0f;  // 0.35-0.50
		c.speed = (float)sw * (0.005f + (float)(rand() % 10) / 1000.0f); // ~0.5%-1.5% sw/s
		c.y = (float)sh * (-0.063f + (float)(rand() % 15) / 100.0f);     // y=-6.3%~8.3% sh
		if (initialSpread)
			c.x = (float)(rand() % (int)(sw * 1.6f)) - sw * 0.3f;
		else
			c.x = (float)sw + sw * 0.08f + (float)(rand() % (int)(sw * 0.8f));
	}

	static void InitClouds() {
		for (int i = 0; i < FAR_CLOUD_COUNT; i++)
			SpawnFarCloud(farClouds[i], true);
		for (int i = 0; i < MID_CLOUD_COUNT; i++)
			SpawnMidCloud(midClouds[i], true);
		for (int i = 0; i < NEAR_CLOUD_COUNT; i++)
			SpawnNearCloud(nearClouds[i], true);
	}

	static void InitAirship() {
		airship.active = false;
		airship.cooldown = 5.0f + (float)(rand() % 8);
		airship.frame = 0;
		airship.animTimer = 0;
		airship.scale = 0.30f;
		airship.speed = 90.0f;
		airship.x = 0;
		airship.y = 0;
	}

	static void SpawnAirship() {
		int sw = Global::Window::ScreenWidth;
		airship.active = true;
		airship.x = (float)(sw + 50);
		airship.y = 180.0f + (float)(rand() % 150);
		airship.speed = 70.0f + (float)(rand() % 40);
		airship.frame = 0;
		airship.animTimer = 0;
	}
}

bool AboutScene::Init()
{
	AS::font = MakeFont("黑体", 50);
	AS::fontSmall = MakeFont("黑体", 28);

	SkyBG = LoadTexture(Resource::About::SkyBG);
	MountainFar = LoadTexture(Resource::About::MountainFar, D3DCOLOR_XRGB(255, 255, 255));
	ForegroundGrass = LoadTexture(Resource::About::ForegroundGrass, D3DCOLOR_XRGB(255, 255, 255));
	ForegroundGround = LoadTexture(Resource::About::ForegroundGround, D3DCOLOR_XRGB(255, 255, 255));
	CloudSheet = LoadTexture(Resource::About::CloudSheet, D3DCOLOR_XRGB(255, 255, 255));
	AirshipSheet = LoadTexture(Resource::About::AirshipSheet, D3DCOLOR_XRGB(255, 255, 255));
	LeafSheet = LoadTexture(Resource::About::LeafSheet, D3DCOLOR_XRGB(255, 255, 255));

	AS::InitClouds();
	AS::InitAirship();
	AS::InitLeaves();
	AS::windTime = 0;
	AS::c1 = 255; AS::c2 = 255; AS::c3 = 255;
	AS::lastTime = timeGetTime();

	if (Global::Sound::SoundSwicth)
		LoopSound(Sound::BGM);

	return true;
}

void AboutScene::End()
{
	Sound::BGM->Stop();
	SAFE_RELEASE(SkyBG);
	SAFE_RELEASE(MountainFar);
	SAFE_RELEASE(ForegroundGrass);
	SAFE_RELEASE(ForegroundGround);
	SAFE_RELEASE(CloudSheet);
	SAFE_RELEASE(AirshipSheet);
	SAFE_RELEASE(LeafSheet);
	SAFE_RELEASE(AS::font);
	SAFE_RELEASE(AS::fontSmall);
}

void AboutScene::Render()
{
	int sw = Global::Window::ScreenWidth;
	int sh = Global::Window::ScreenHeight;

	// 1. 天空背景
	Sprite_Transform_Draw(SkyBG, 0, 0, sw, sh, 0, 1, 0, 1.0f, D3DCOLOR_XRGB(255, 255, 255));

	// 2. 超远云 (山景之后, 被山部分遮挡, 最小最淡)
	for (int i = 0; i < AS::FAR_CLOUD_COUNT; i++) {
		AS::Cloud& c = AS::farClouds[i];
		Sprite_Transform_Draw(CloudSheet, c.x, c.y,
			CLOUD_FRAME_W, CLOUD_FRAME_H, c.frame, CLOUD_COLUMNS,
			0, c.scale, c.scale, D3DCOLOR_ARGB(130, 255, 255, 255));
	}

	// 3. 远景山脉
	Sprite_Transform_Draw(MountainFar, 0, 0, sw, sh, 0, 1, 0, 1.0f, D3DCOLOR_XRGB(255, 255, 255));

	// 4. 中远云 (山景之前, 画面中部, 中等大小)
	for (int i = 0; i < AS::MID_CLOUD_COUNT; i++) {
		AS::Cloud& c = AS::midClouds[i];
		Sprite_Transform_Draw(CloudSheet, c.x, c.y,
			CLOUD_FRAME_W, CLOUD_FRAME_H, c.frame, CLOUD_COLUMNS,
			0, c.scale, c.scale, D3DCOLOR_ARGB(190, 255, 255, 255));
	}

	// 4b. 近处云 (画面顶部, 最大最浓)
	for (int i = 0; i < AS::NEAR_CLOUD_COUNT; i++) {
		AS::Cloud& c = AS::nearClouds[i];
		Sprite_Transform_Draw(CloudSheet, c.x, c.y,
			CLOUD_FRAME_W, CLOUD_FRAME_H, c.frame, CLOUD_COLUMNS,
			0, c.scale, c.scale, D3DCOLOR_ARGB(220, 255, 255, 255));
	}

	// 5. 飞艇
	if (AS::airship.active) {
		Sprite_Transform_Draw(AirshipSheet, (int)AS::airship.x, (int)AS::airship.y,
			AIRSHIP_FRAME_W, AIRSHIP_FRAME_H, AS::airship.frame, AIRSHIP_COLUMNS,
			0, AS::airship.scale, D3DCOLOR_XRGB(255, 255, 255));
	}

	// 6. 前景草丛（带摇曳）
	{
		float t = AS::windTime;
		float primarySway = sinf(t * 0.8f) * 0.018f;
		float secondarySway = sinf(t * 2.3f) * 0.006f;
		float gust = sinf(t * 0.3f);
		gust = (gust > 0.5f) ? (gust - 0.5f) * 2.0f * 0.015f : 0.0f;
		float shearX = primarySway + secondarySway + gust;
		AS::DrawWithShear(ForegroundGrass, sw, sh, shearX);
	}

	// 7. 前景地面（固定不动）
	Sprite_Transform_Draw(ForegroundGround, 0, 0, sw, sh, 0, 1, 0, 1.0f, D3DCOLOR_XRGB(255, 255, 255));

	// 8. 落叶（在前景之上，增加飘落感）
	for (int i = 0; i < AS::LEAF_COUNT; i++) {
		AS::Leaf& lf = AS::leaves[i];
		Sprite_Transform_Draw(LeafSheet, (int)lf.x, (int)lf.y,
			LEAF_FRAME_W, LEAF_FRAME_H, lf.frame, LEAF_COLUMNS,
			lf.rotation, lf.scale, D3DCOLOR_ARGB(lf.alpha, 255, 255, 255));
	}

	// 9. 文字
	//string text = "作者：XK|YYQ";
	//FontPrint(AS::font, 30, 60, text, D3DCOLOR_XRGB((int)AS::c1, (int)AS::c2, (int)AS::c3));
	FontPrint(AS::fontSmall, 30, 900, "BGM: Bit Rush: Arcade Ahri", D3DCOLOR_XRGB(220, 220, 220));
}

void AboutScene::Update()
{
	DWORD now = timeGetTime();
	float dt = (now - AS::lastTime) / 1000.0f;
	AS::lastTime = now;
	if (dt > 0.1f) dt = 0.1f;

	int sw = Global::Window::ScreenWidth;
	int sh = Global::Window::ScreenHeight;

	AS::windTime += dt;

	AS::c1 -= 240.0f * dt;
	AS::c2 -= 192.0f * dt;
	AS::c3 -= 144.0f * dt;
	if (AS::c1 < 0) AS::c1 = 255.0f;
	if (AS::c2 < 0) AS::c2 = 255.0f;
	if (AS::c3 < 0) AS::c3 = 255.0f;

	for (int i = 0; i < AS::FAR_CLOUD_COUNT; i++) {
		AS::Cloud& c = AS::farClouds[i];
		c.x -= c.speed * dt;
		float w = CLOUD_FRAME_W * c.scale;
		if (c.x < -w) {
			AS::SpawnFarCloud(c, false);
		}
	}

	for (int i = 0; i < AS::MID_CLOUD_COUNT; i++) {
		AS::Cloud& c = AS::midClouds[i];
		c.x -= c.speed * dt;
		float w = CLOUD_FRAME_W * c.scale;
		if (c.x < -w) {
			AS::SpawnMidCloud(c, false);
		}
	}

	for (int i = 0; i < AS::NEAR_CLOUD_COUNT; i++) {
		AS::Cloud& c = AS::nearClouds[i];
		c.x -= c.speed * dt;
		float w = CLOUD_FRAME_W * c.scale;
		if (c.x < -w) {
			AS::SpawnNearCloud(c, false);
		}
	}

	if (AS::airship.active) {
		AS::airship.x -= AS::airship.speed * dt;
		AS::airship.animTimer += dt;
		if (AS::airship.animTimer >= 0.12f) {
			AS::airship.animTimer = 0;
			AS::airship.frame = (AS::airship.frame + 1) % 8;
		}
		float w = AIRSHIP_FRAME_W * AS::airship.scale;
		if (AS::airship.x < -w) {
			AS::airship.active = false;
			AS::airship.cooldown = 15.0f + (float)(rand() % 20);
		}
	} else {
		AS::airship.cooldown -= dt;
		if (AS::airship.cooldown <= 0) {
			AS::SpawnAirship();
		}
	}

	// 更新落叶
	for (int i = 0; i < AS::LEAF_COUNT; i++) {
		AS::Leaf& lf = AS::leaves[i];
		const AS::LeafType& lt = AS::leafTypes[lf.frame];
		float t = lf.lifetime;

		lf.lifetime += dt;

		switch (lf.motion) {
		case AS::MOTION_WAVE:
		{
			// Z字型折返: 锯齿波 + 下落 + 风吹偏移
			lf.y += lt.fallSpeed * dt;
			float sway = sinf(t * lt.swayFreq + lf.phase) * lt.swayAmp;
			lf.x += sway * dt - lt.windDrift * dt;
			lf.rotation += sinf(t * lt.swayFreq * 0.7f + lf.phase) * lt.rotSpeed * dt;
			break;
		}
		case AS::MOTION_SPIRAL:
		{
			// 螺旋下降: 圆形轨迹 + 缓慢下落 + 风吹偏移
			float angle = t * lf.spiralSpeed;
			float r = lf.spiralRadius;
			lf.x += cosf(angle) * r * dt * lf.spiralSpeed - lt.windDrift * 0.7f * dt;
			lf.y += lt.fallSpeed * 0.6f * dt + sinf(angle) * r * 0.3f * dt * lf.spiralSpeed;
			lf.rotation = angle * 0.5f;
			break;
		}
		case AS::MOTION_ZIGZAG:
		{
			// Z字型折返: 锯齿波 + 下落 + 风吹偏移
			lf.y += lt.fallSpeed * 0.8f * dt;
			float zigPhase = fmodf(t * lt.swayFreq * 0.5f + lf.phase, 6.2832f);
			float zigDir = (zigPhase < 3.1416f) ? 1.0f : -1.0f;
			lf.x += zigDir * lt.swayAmp * 0.8f * dt - lt.windDrift * dt;
			// 每次折返时翻转
			lf.rotation += zigDir * lt.rotSpeed * 2.5f * dt;
			break;
		}
		case AS::MOTION_TUMBLE:
		{
			// 翻滚直落: 快速下落 + 高速旋转 + 微弱水平飘移
			lf.y += lt.fallSpeed * 1.3f * dt;
			lf.x -= lt.windDrift * 0.5f * dt;
			lf.x += sinf(t * 3.0f + lf.phase) * 15.0f * dt;
			lf.rotation += lt.rotSpeed * 4.0f * dt;
			break;
		}
		}

		// 超出屏幕下方或左侧时重新生成
		float leafW = LEAF_FRAME_W * lf.scale;
		float leafH = LEAF_FRAME_H * lf.scale;
		if (lf.y > (float)sh + leafH || lf.x < -leafW - 50.0f) {
			AS::SpawnLeaf(lf);
		}
	}

	if (Key_Up(DIK_ESCAPE))
	{
		Game_ChangeScene(GAME_STATE::Home);
	}
}
