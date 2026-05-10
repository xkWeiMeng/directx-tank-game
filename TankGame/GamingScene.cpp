#include "GamingScene.h"
#define EnemyNumberMAX 21
namespace GS {
	LPDIRECT3DSURFACE9 GrayRect = NULL;
	LPDIRECT3DSURFACE9 BlackRect = NULL;
	/*贴图*/
	LPDIRECT3DTEXTURE9 Flag = NULL;
	LPDIRECT3DTEXTURE9 Something = NULL;
	LPDIRECT3DTEXTURE9 Tile = NULL;
	LPDIRECT3DTEXTURE9 Player_1 = NULL;
	LPDIRECT3DTEXTURE9 Player_2 = NULL;
	LPDIRECT3DTEXTURE9 Bullet_TXTTURE = NULL;
	LPDIRECT3DTEXTURE9 Enemy_TXTTURE = NULL;
	LPDIRECT3DTEXTURE9 Award = NULL;
	LPDIRECT3DTEXTURE9 Boom1 = NULL;
	LPDIRECT3DTEXTURE9 Boom2 = NULL;
	LPDIRECT3DTEXTURE9 GameOver = NULL;
	LPDIRECT3DTEXTURE9 Shield = NULL;
	LPDIRECT3DTEXTURE9 Hole = NULL;
	LPDIRECT3DTEXTURE9 Number = NULL;
	LPDIRECT3DTEXTURE9 Flicker[9] = { NULL };
	LPD3DXFONT AmmoFont = NULL; //弹药显示字体
	LPD3DXFONT SidebarTitleFont = NULL;
	LPD3DXFONT SidebarValueFont = NULL;
	LPD3DXFONT SidebarSmallFont = NULL;
	LPD3DXLINE SidebarLine = NULL;
	/*变量*/
	vector<int> BornPlayer1MapPiece;
	vector<int> BornPlayer2MapPiece;
	vector<int> BornEnemyMapPiece;
	int NowLevel = 1;
	unsigned long lasttime = 0;
	int HaveBornEnemyNumber = 0;
	int StartTime = 0, NowTime, SurplusTime = 0;
	bool ShowTime = false;
	bool GameOverFlag = false;
	bool IsDoublePlayer = false;
	int SGOy = 960;
	int EnemyNumber = 30;
	int FreezeEndTime = 0;   //定时器：敌人冻结结束时间
	int FortifyEndTime = 0;  //铲子：基地加固结束时间
	//基地（老鹰）在游戏区域内的位置，网格(6,12)
	int FlagGameX = (6 + 1) * 64; // 448
	int FlagGameY = (12 + 1) * 64; // 832
	bool BaseDestroyed = false; //基地是否被击毁
	/*对象*/
	Player player;
	Player2 player2;
	Player &Player1 = player;
	void ShowGameOver();

	/*辅助函数*/
	int  Crash(int iswho, int x, int y, int speed, int dir, int shooter, unsigned long id, int, int powerLevel, int bulletType=0);
	void DrawMap();
	void CreateMapPiece();
	bool ReadMapInHD(string filename);
	bool ReadMapInHD(char * filename);
	bool WriteMapToHD(char * filename);
	void FillRect(RECT & rect, long l, long r, long t, long b);
	void ReadMap(int x, int y, RECT&rect1, RECT&rect2);//读取地图信息
	void AddUselessObj(unsigned long id);
	bool DelListNode(EnemyList*listhead, unsigned long id);//删除成功返回true，否则返回false
	bool DelListNode(BulletList*listhead, unsigned long id);//删除成功返回true，否则返回false
	bool DelListNode(BoomList*listhead, unsigned long id);//删除成功返回true，否则返回false
	void DelUselessObj();
	void DrawNet();
	void ClearUselessObj();
	void CreateEnemy(int x, int y, int speed, int hp, int as, int grade, int dir);
	void CreateBoom(int x, int y, int whatboom, int Dir);
	void CreateAward(int x, int y, int type);
	bool DelListNode(AwardItemList*listhead, unsigned long id);
	void CheckAwardCollision();
	void DestroyAllEnemies();
	void AddBulletToList(Bullet* b); //添加子弹到链表
	void UpdateLaser(Player& p); //激光更新：射线检测+伤害
	void DrawLaserBeam(Player& p); //激光渲染
//	int  MaxNumber(int m1, int m2, int m3, int m4, bool r1, bool r2, bool r3, bool r4);
//	int  MinNumber(int m1, int m2, int m3, int m4, bool r1, bool r2, bool r3, bool r4);
	void DIDA();
	void NewStage();
	void ReadNextMap();
	void StartNextStage();
	void RestartThisStage();
	/*辅助函数*/
	BulletListHead bulletlisthead;//子弹链表头
	EnemyListHead enemylisthead;//敌人链表头
	UselessObjHead uselessobjhead;//失效对象链表头
	BoomListHead boomlisthead;//爆炸链表头
	MapPieceListHead mappiecelisthead;
	AwardItemListHead awardlisthead;

	static unsigned long IDNumber = 0;
	int EnemyXY[EnemyNumberMAX][2];//敌人位置坐标数组
	int Map[13][13]; //第一维是y轴，第二维是x轴
}
using namespace GS;
/*--------------------------------------------------------------------
GamingScene的方法
----------------------------------------------------------------------*/
//变量初始化
bool GamingScene::Init()
{
	//
	srand((unsigned)time(0));

	for (int i = 0; i < EnemyNumberMAX; i++)//初始化敌人坐标数组
	{
		EnemyXY[i][0] = -1;
		EnemyXY[i][1] = -1;
	}
	//
	HRESULT result = d3dDev->CreateOffscreenPlainSurface(
		100,
		100,
		D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT,
		&GrayRect,
		NULL
	);
	
	if (result != D3D_OK)
	{
		ShowMessage("灰色-区域 初始化失败！");
		return false;
	}

	result = d3dDev->CreateOffscreenPlainSurface(
		100,
		100,
		D3DFMT_X8R8G8B8,
		D3DPOOL_DEFAULT,
		&BlackRect,
		NULL
	);
	if (result != D3D_OK)
	{
		ShowMessage("黑色-区域 初始化失败！");
		return false;
	}
	d3dDev->ColorFill(GrayRect, NULL, D3DCOLOR_XRGB(110, 110, 110));
	d3dDev->ColorFill(BlackRect, NULL, D3DCOLOR_XRGB(69, 139, 116));

	Flag = LoadTexture(Resource::Texture::Flag, D3DCOLOR_XRGB(255, 255, 255));
	if (!Flag)
	{
		ShowMessage("装载 旗帜 纹理失败！");
		return false;
	}
	Something = LoadTexture(Resource::Texture::Something, D3DCOLOR_XRGB(255, 255, 255));
	if (!Something)
	{
		ShowMessage("装载 其他 纹理失败!");
		return false;
	}
	Tile = LoadTexture(Resource::Texture::Tile, D3DCOLOR_XRGB(4, 4, 4));
	if (!Tile)
	{
		ShowMessage("装载 砖 纹理失败!");
		return false;
	}
	Player_1 = LoadTexture(Resource::Texture::Player_1, D3DCOLOR_XRGB(0, 0, 0));
	if (!Player_1)
	{
		ShowMessage("装载 玩家一 纹理失败!");
		return false;
	}
	Bullet_TXTTURE = LoadTexture(Resource::Texture::Bullet, D3DCOLOR_XRGB(4, 4, 4));
	if (!Bullet_TXTTURE)
	{
		ShowMessage("装载 子弹 纹理失败!");
		return false;
	}
	Boom1 = LoadTexture(Resource::Texture::Boom1, D3DCOLOR_XRGB(0, 0, 0));
	if (!Boom1)
	{
		ShowMessage("装载 爆炸一 纹理失败!");
		return false;
	}
	Boom2 = LoadTexture(Resource::Texture::Boom2, D3DCOLOR_XRGB(4, 4, 4));
	if (!Boom2)
	{
		ShowMessage("装载 爆炸二 纹理失败!");
		return false;
	}
	Player_2 = LoadTexture(Resource::Texture::Player_2, D3DCOLOR_XRGB(0, 0, 0));
	if (!Player_2)
	{
		ShowMessage("装载 玩家二 纹理失败!");
		return false;
	}
	Award = LoadTexture(Resource::Texture::Award, D3DCOLOR_XRGB(234, 234, 234));
	if (!Award)
	{
		ShowMessage("装载 奖励 纹理失败!");
		return false;
	}
	Shield = LoadTexture(Resource::Texture::Shield, D3DCOLOR_XRGB(4, 4, 4));
	if (!Shield)
	{
		ShowMessage("装载 盾牌 纹理失败!");
		return false;
	}
	GameOver = LoadTexture(Resource::Texture::GameOver, D3DCOLOR_XRGB(0, 0, 0));
	if (!GameOver)
	{
		ShowMessage("装载 游戏结束 纹理失败!");
		return false;
	}
	Enemy_TXTTURE = LoadTexture(Resource::Texture::Enemy, D3DCOLOR_XRGB(4, 4, 4));
	if (!Enemy_TXTTURE)
	{
		ShowMessage("装载 敌人 纹理失败!");
		return false;
	}
	Hole = LoadTexture(Resource::Texture::Hole, D3DCOLOR_XRGB(4, 4, 4));
	if (!Hole)
	{
		ShowMessage("装载 洞 纹理失败!");
		return false;
	}
	Number = LoadTexture(Resource::Texture::Number, D3DCOLOR_XRGB(255, 255, 255));
	if (!Number)
	{
		ShowMessage("装载 数字 纹理失败!");
		return false;
	}

	//装载动画
	string png = ".png";
	string path = "Resources\\Texture\\";
	string buf;
	char buf1;
	for (int i = 0; i < 9; i++)
	{
		buf1 = i + 48;
		buf = buf1 + png;
		Flicker[i] = LoadTexture(path + buf);
		if (!Flicker[i])
			ShowMessage(buf);
	}
	RECT rect;
	int n = 0, i = 960;//不管窗口大小，游戏分辨率都是不变的
	int delayOld = GetTickCount();
	d3dDev->BeginScene();
	for (; n < Global::Window::ScreenHeight / 2; n += 8, i -= 8)
	{
		FillRect(rect, 0, 1024, n, n + 8);
		d3dDev->StretchRect(GrayRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
		FillRect(rect, 0, 1024, i - 8, i);
		d3dDev->StretchRect(GrayRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
		d3dDev->EndScene();
		d3dDev->Present(NULL, NULL, NULL, NULL);
		Sleep(5);
		/**
		while (1)
		{
		if (GetTickCount() > delayOld + 1)
		{
		delayOld = GetTickCount();
		break;
		}
		}
		*/
	}
	//加载选择关卡
	if (Global::DesignMap::NewMapName.length() != 0)
		ReadMapInHD(Global::DesignMap::NewMapName);
	else
	{
		NowLevel = Global::Home::SelectedStage;
		ReadMapInHD("stage" + std::to_string(NowLevel));
	}
	//读取地图信息，创建地图块
	CreateMapPiece();
	//判断是否双人游戏
	IsDoublePlayer = Global::Home::selectedType == 1 ? true : false;
	//播放开始音效
	if(Global::Sound::SoundSwicth)
	Sound::Start->Play();
	player.Born();
	if (IsDoublePlayer)
		player2.Born();
	//创建弹药显示字体
	AmmoFont = MakeFont("SimHei", 16);
	//创建侧栏字体
	SidebarTitleFont = MakeFont("Impact", 22);
	SidebarValueFont = MakeFont("Consolas", 32);
	SidebarSmallFont = MakeFont("Consolas", 18);
	D3DXCreateLine(d3dDev, &SidebarLine);
	return 1;
}

void GamingScene::End()
{
	//清理地图块
	MapPieceList*buf;
	MapPieceList*mp = mappiecelisthead.next;
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++)
		{
			while (mp != NULL)
			{
				if (i == mp->mappiece->X&&j == mp->mappiece->Y)
				{
					buf = mp;
					mp = mp->next;
					delete buf;
				}
				else
					mp = mp->next;
			}
		}
	mappiecelisthead.next = NULL;
	//清理敌人
	EnemyList* ep = enemylisthead.next;
	while (ep != NULL)
	{
		AddUselessObj(ep->enemy->ID);
		ep = ep->next;
	}
	//清理子弹
	BulletList*bp = bulletlisthead.next;
	while (bp != NULL)
	{
		AddUselessObj(bp->bullet->ID);
		bp = bp->next;
	}
	//清理爆炸
	BoomList*boomp = boomlisthead.next;
	while (boomp != NULL)
	{
		AddUselessObj(boomp->boom->ID);
		boomp = boomp->next;
	}
	//清理奖励
	AwardItemList*awpEnd = awardlisthead.next;
	while (awpEnd != NULL)
	{
		AddUselessObj(awpEnd->award->ID);
		awpEnd = awpEnd->next;
	}

	ClearUselessObj();
	//重置玩家属性
	player.Speed = 5 * 64;
	player.Dir = Dirction::up;
	player.player.x = 64 * 6;
	player.player.y = 64 * 13;
	player.Lift = 1;
	player.Health_Point = 1;
	player.Grade = 0;
	player.ApplyGradeStats();

	if (IsDoublePlayer)
	{
		player2.Speed = 5 * 64;
		player2.Dir = Dirction::up;
		player2.player.x = 64 * 6;
		player2.player.y = 64 * 13;
		player2.Lift = 1;
		player2.Health_Point = 1;
		player2.Grade = 0;
		player2.ApplyGradeStats();
	}
	//重置地图数据
	SGOy = 960;
	GameOverFlag = false;
	BaseDestroyed = false;
	EnemyNumber = 30;
	HaveBornEnemyNumber = 0;
	BornPlayer1MapPiece.clear();
	BornPlayer2MapPiece.clear();
	BornEnemyMapPiece.clear();
	SAFE_RELEASE(GrayRect);
	SAFE_RELEASE(BlackRect);
	SAFE_RELEASE(Flag);
	SAFE_RELEASE(Something);
	SAFE_RELEASE(Tile);
	SAFE_RELEASE(Player_1);
	SAFE_RELEASE(Player_2);
	SAFE_RELEASE(Bullet_TXTTURE);
	SAFE_RELEASE(Enemy_TXTTURE);
	SAFE_RELEASE(Award);
	SAFE_RELEASE(Boom1);
	SAFE_RELEASE(Boom2);
	SAFE_RELEASE(GameOver);
	SAFE_RELEASE(Shield);
	SAFE_RELEASE(Hole);
	SAFE_RELEASE(Number);
	for (int i = 0; i < 9; i++) SAFE_RELEASE(Flicker[i]);
	SAFE_RELEASE(AmmoFont);
	SAFE_RELEASE(SidebarTitleFont);
	SAFE_RELEASE(SidebarValueFont);
	SAFE_RELEASE(SidebarSmallFont);
	SAFE_RELEASE(SidebarLine);
	if (Sound::Moving->IsSoundPlaying()) Sound::Moving->Stop();
	if (Sound::BGM->IsSoundPlaying()) Sound::BGM->Stop();
}
//游戏渲染
void GamingScene::Render()
{
	d3dDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	/*游戏边框*/
	RECT rect;
	FillRect(rect, 0, 1024, 32, 64);   //分辨率不为1024*960时需要修改
	d3dDev->StretchRect(GrayRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, 0, 64, 64, 896);
	d3dDev->StretchRect(GrayRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, 896, 1024, 64, 896);
	d3dDev->StretchRect(GrayRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, 0, 1024, 896, 928);
	d3dDev->StretchRect(GrayRect, NULL, backBuffer, &rect, D3DTEXF_NONE);

	DrawNet();//Debug模式下显示网格线
			  /*游戏内容*/
	spriteObj->Begin(D3DXSPRITE_ALPHABLEND);

	//==================== 现代侧栏面板 ====================
	{
		int panelX = 904; // 面板左边缘
		int panelW = 116; // 面板宽度
		int cx = panelX + panelW / 2; // 面板水平中心 = 962

		// --- 剩余敌人区域 ---
		if (SidebarTitleFont)
			FontPrint(SidebarTitleFont, panelX + 8, 70, "ENEMY", D3DCOLOR_XRGB(220, 60, 60));
		// 敌人数量大字
		if (SidebarValueFont)
		{
			char enemyBuf[8];
			sprintf_s(enemyBuf, "%d", EnemyNumber);
			FontPrint(SidebarValueFont, cx - 10, 92, enemyBuf, D3DCOLOR_XRGB(255, 80, 80));
		}
		// 敌人进度条（剩余/总数30）
		spriteObj->End();
		if (SidebarLine)
		{
			SidebarLine->SetWidth(6.0f);
			SidebarLine->SetAntialias(TRUE);
			// 底条（暗色）
			D3DXVECTOR2 barBg[2] = { D3DXVECTOR2((float)(panelX + 10), 132.0f), D3DXVECTOR2((float)(panelX + panelW - 10), 132.0f) };
			SidebarLine->Begin();
			SidebarLine->Draw(barBg, 2, D3DCOLOR_XRGB(60, 20, 20));
			SidebarLine->End();
			// 前景条（红色渐变）
			float barMax = (float)(panelW - 20);
			float barLen = barMax * min(EnemyNumber, 30) / 30.0f;
			if (barLen > 0)
			{
				D3DXVECTOR2 barFg[2] = { D3DXVECTOR2((float)(panelX + 10), 132.0f), D3DXVECTOR2((float)(panelX + 10) + barLen, 132.0f) };
				SidebarLine->Begin();
				SidebarLine->Draw(barFg, 2, D3DCOLOR_XRGB(255, 60, 60));
				SidebarLine->End();
			}
		}
		spriteObj->Begin(D3DXSPRITE_ALPHABLEND);

		// --- 分隔线（用小字符） ---
		if (SidebarSmallFont)
			FontPrint(SidebarSmallFont, panelX + 4, 145, "----------", D3DCOLOR_XRGB(80, 80, 80));

		// --- 关卡信息 ---
		if (SidebarTitleFont)
			FontPrint(SidebarTitleFont, panelX + 8, 165, "STAGE", D3DCOLOR_XRGB(255, 180, 50));
		if (SidebarValueFont)
		{
			char stageBuf[8];
			sprintf_s(stageBuf, "%02d", NowLevel);
			FontPrint(SidebarValueFont, cx - 14, 187, stageBuf, D3DCOLOR_XRGB(255, 220, 100));
		}

		// --- 分隔线 ---
		if (SidebarSmallFont)
			FontPrint(SidebarSmallFont, panelX + 4, 225, "----------", D3DCOLOR_XRGB(80, 80, 80));

		// --- 玩家一信息 ---
		if (SidebarTitleFont)
			FontPrint(SidebarTitleFont, panelX + 8, 245, "1P", D3DCOLOR_XRGB(100, 200, 255));
		// 生命
		if (SidebarSmallFont)
		{
			char lifeBuf[16];
			sprintf_s(lifeBuf, "HP  x%d", player.Lift);
			FontPrint(SidebarSmallFont, panelX + 8, 270, lifeBuf, D3DCOLOR_XRGB(180, 255, 180));
		}
		// 弹药类型与数量
		{
			const char* btNames[] = { "NOR", "SHT", "LAS" };
			D3DCOLOR btColors[] = {
				D3DCOLOR_XRGB(200, 200, 200),
				D3DCOLOR_XRGB(255, 180, 80),
				D3DCOLOR_XRGB(80, 255, 80)
			};
			int bt = player.CurrentBulletType;
			int ammo = (bt == 0) ? 99 : (bt == 1 ? player.ShotgunAmmo : player.LaserAmmo);
			if (ammo < 0) ammo = 0;
			if (SidebarSmallFont)
			{
				char ammoBuf[16];
				sprintf_s(ammoBuf, "%s %02d", btNames[bt], ammo);
				FontPrint(SidebarSmallFont, panelX + 8, 292, ammoBuf, btColors[bt]);
			}
		}
		// 等级（星星用字符表示）
		if (SidebarSmallFont)
		{
			char gradeBuf[16];
			const char* stars[] = { "    ", "*   ", "**  ", "*** " };
			int g = player.Grade; if (g > 3) g = 3;
			sprintf_s(gradeBuf, "LV  %s", stars[g]);
			FontPrint(SidebarSmallFont, panelX + 8, 314, gradeBuf, D3DCOLOR_XRGB(255, 255, 100));
		}

		// --- 玩家二信息 ---
		if (IsDoublePlayer)
		{
			if (SidebarSmallFont)
				FontPrint(SidebarSmallFont, panelX + 4, 340, "----------", D3DCOLOR_XRGB(80, 80, 80));
			if (SidebarTitleFont)
				FontPrint(SidebarTitleFont, panelX + 8, 360, "2P", D3DCOLOR_XRGB(255, 150, 100));
			// 生命
			if (SidebarSmallFont)
			{
				char lifeBuf2[16];
				sprintf_s(lifeBuf2, "HP  x%d", player2.Lift);
				FontPrint(SidebarSmallFont, panelX + 8, 385, lifeBuf2, D3DCOLOR_XRGB(180, 255, 180));
			}
			// 弹药
			{
				const char* btNames[] = { "NOR", "SHT", "LAS" };
				D3DCOLOR btColors[] = {
					D3DCOLOR_XRGB(200, 200, 200),
					D3DCOLOR_XRGB(255, 180, 80),
					D3DCOLOR_XRGB(80, 255, 80)
				};
				int bt = player2.CurrentBulletType;
				int ammo = (bt == 0) ? 99 : (bt == 1 ? player2.ShotgunAmmo : player2.LaserAmmo);
				if (ammo < 0) ammo = 0;
				if (SidebarSmallFont)
				{
					char ammoBuf2[16];
					sprintf_s(ammoBuf2, "%s %02d", btNames[bt], ammo);
					FontPrint(SidebarSmallFont, panelX + 8, 407, ammoBuf2, btColors[bt]);
				}
			}
			// 等级
			if (SidebarSmallFont)
			{
				char gradeBuf2[16];
				const char* stars2[] = { "    ", "*   ", "**  ", "*** " };
				int g2 = player2.Grade; if (g2 > 3) g2 = 3;
				sprintf_s(gradeBuf2, "LV  %s", stars2[g2]);
				FontPrint(SidebarSmallFont, panelX + 8, 429, gradeBuf2, D3DCOLOR_XRGB(255, 255, 100));
			}
		}

		// --- 旗帜图标保留在底部 ---
		Sprite_Transform_Draw(Flag, 926, 704, 32, 32, 0, 1, 0, 2.0, D3DCOLOR_XRGB(255, 255, 255));
	}
	//==================== 侧栏结束 ====================

	//游戏区域内基地（老鹰）— 使用砖.bmp第6帧(正常)/第7帧(击毁)
	int eagleFrame = BaseDestroyed ? 6 : 5;
	Sprite_Transform_Draw(Tile, FlagGameX, FlagGameY, 32, 32, eagleFrame, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	//基地加固Shield效果
	if (!BaseDestroyed && (int)GetTickCount() < FortifyEndTime)
	{
		int shieldFrame = (GetTickCount() / 100) % 2;
		Sprite_Transform_Draw(Shield, FlagGameX, FlagGameY, 32, 32, shieldFrame, 1, 0, 2.0f, D3DCOLOR_XRGB(255, 255, 255));
	}

	//画玩家一
	if(player.Alive)
	player.Draw();
	//画玩家二
	if (player2.Alive)
	{
		if (IsDoublePlayer)
			player2.Draw();
	}
	//画地图
	MapPieceList* mp = mappiecelisthead.next;
	while (mp != NULL)
	{
		mp->mappiece->Draw();
		mp = mp->next;
	}
	//	DrawMap();
	//		Sprite_Transform_Draw(Tile, 512, 832, 32, 32, 5, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	//渲染子弹 并清理失效子弹
	BulletList*bp = bulletlisthead.next;
	while (bp != NULL)
	{
		bp->bullet->Draw();
		bp = bp->next;
	}
	//渲染激光光线
	if (player.LaserActive && player.Alive)
	{
		spriteObj->End();
		DrawLaserBeam(player);
		spriteObj->Begin(D3DXSPRITE_ALPHABLEND);
	}
	if (IsDoublePlayer && player2.LaserActive && player2.Alive)
	{
		spriteObj->End();
		DrawLaserBeam(player2);
		spriteObj->Begin(D3DXSPRITE_ALPHABLEND);
	}

	//渲染敌人
	EnemyList *ep = enemylisthead.next;
	while (ep != NULL)
	{
		ep->enemy->Draw();
		ep = ep->next;
	}

	//渲染爆炸
	BoomList *pboom = boomlisthead.next, *bbuf;
	while (pboom != NULL)
	{
		pboom->boom->Draw();
		pboom = pboom->next;
	}
	//渲染奖励
	AwardItemList *paw = awardlisthead.next;
	while (paw != NULL)
	{
		paw->award->Draw();
		paw = paw->next;
	}
	//游戏结束
	if (GameOverFlag)
	{
		ShowGameOver();
	}
	DIDA();//处理时间相关信息
}
//游戏逻辑更新
void GamingScene::Update()
{
	unsigned long newtime;
	//
	if (Key_Up(DIK_ESCAPE))
	{
		Game_ChangeScene(GAME_STATE::Home);
	}

	static bool StarSoundPlaying = true;
	if (Global::Sound::SoundSwicth)
	{
		if (StarSoundPlaying)
			if (!Sound::Start->IsSoundPlaying())
			{
				Sound::BGM->Play(0, DSBPLAY_LOOPING);
				StarSoundPlaying = false;
			}
	}
	if (!GameOverFlag) {
		//检查敌人数量 判断是否胜利
		if (EnemyNumber <= 0)
		{
			StartNextStage();
		}
		//玩家一
		if (player.Alive)
		{
			if (Key_Down(Global::PlayerControl::Player1[0]) && !Key_Down(Global::PlayerControl::Player1[3]) && !Key_Down(Global::PlayerControl::Player1[2]))
			{
				player.Logic(Dirction::up);
				//up
			}
			if (Key_Down(Global::PlayerControl::Player1[1]) && !Key_Down(Global::PlayerControl::Player1[3]) && !Key_Down(Global::PlayerControl::Player1[2]))
			{
				player.Logic(Dirction::below);
				//blow
			}
			if (Key_Down(Global::PlayerControl::Player1[2]))
			{
				player.Logic(Dirction::lift);
				//left
			}
			if (Key_Down(Global::PlayerControl::Player1[3]))
			{
				player.Logic(Dirction::right);
				//right
			}
			//射击键
			static int ShootTime = 10;
			if (ShowTime)
				ShootTime++;
			if (Key_Down(Global::PlayerControl::Player1[4]) || Key_Down(0x52))
			{
				if (player.CurrentBulletType == 2 && player.LaserAmmo > 0) //激光模式
				{
					player.LaserActive = true;
					UpdateLaser(player);
				}
				else if (player.CurrentBulletType == 2)
				{
					player.LaserActive = false; //弹药耗尽
				}
				else if (ShootTime > 10 / player.Attack_Speed)
				{
					if (player.CurrentBulletType == 1 && player.ShotgunAmmo <= 0)
					{} //散弹弹药耗尽，不发射
					else
					{
						player.Shoot(0, player.PowerLevel);
						ShootTime = 0;
					}
				}
			}
			else
			{
				player.LaserActive = false;
			}
			//切换子弹类型：按切换键循环 普通→散弹→激光→普通
			if (Key_Up(Global::PlayerControl::Player1[5]))
			{
				player.CurrentBulletType = (player.CurrentBulletType + 1) % 3;
			}
		}
		//Player1 movement sound
		if (Global::Sound::SoundSwicth && player.Alive)
		{
			bool isMoving = Key_Down(Global::PlayerControl::Player1[0]) || Key_Down(Global::PlayerControl::Player1[1]) || Key_Down(Global::PlayerControl::Player1[2]) || Key_Down(Global::PlayerControl::Player1[3]);
			if (isMoving && !Sound::Moving->IsSoundPlaying())
				Sound::Moving->Play(0, DSBPLAY_LOOPING);
			else if (!isMoving && Sound::Moving->IsSoundPlaying())
			{
				Sound::Moving->Stop();
				Sound::Stop->Play();
			}
		}
		//玩家二
		static int ShootTime2 = 10;
		if (player2.Alive)
		{
			if (IsDoublePlayer)
			{
				if (Key_Down(Global::PlayerControl::Player2[0]) && !Key_Down(Global::PlayerControl::Player2[3]) && !Key_Down(Global::PlayerControl::Player2[2]))
				{
					player2.Logic(Dirction::up);
					//up
				}
				if (Key_Down(Global::PlayerControl::Player2[1]) && !Key_Down(Global::PlayerControl::Player2[3]) && !Key_Down(Global::PlayerControl::Player2[2]))
				{
					player2.Logic(Dirction::below);
					//blow
				}
				if (Key_Down(Global::PlayerControl::Player2[2]))
				{
					player2.Logic(Dirction::lift);
					//left
				}
				if (Key_Down(Global::PlayerControl::Player2[3]))
				{
					player2.Logic(Dirction::right);
					//right
				}
				//射击键
				if (ShowTime)
					ShootTime2++;
				if (Key_Down(Global::PlayerControl::Player2[4]))
				{
					if (player2.CurrentBulletType == 2 && player2.LaserAmmo > 0) //激光模式
					{
						player2.LaserActive = true;
						UpdateLaser(player2);
					}
					else if (player2.CurrentBulletType == 2)
					{
						player2.LaserActive = false;
					}
					else if (ShootTime2 > 10 / player2.Attack_Speed)
					{
						if (player2.CurrentBulletType == 1 && player2.ShotgunAmmo <= 0)
						{}
						else
						{
							player2.Shoot(0, player2.PowerLevel);
							ShootTime2 = 0;
						}
					}
				}
				else
				{
					player2.LaserActive = false;
				}
			}
		}
		//更新子弹逻辑
		BulletList*bp = bulletlisthead.next;
		while (bp != NULL)
		{
			bp->bullet->Logic();
			bp = bp->next;
		}
		//更新敌人逻辑
		EnemyList*ep = enemylisthead.next;
		bool enemiesFrozen = (int)GetTickCount() < FreezeEndTime;
		while (ep != NULL)
		{
			if (!enemiesFrozen)
				ep->enemy->Logic(ShowTime);
			ep = ep->next;
		}
		//生成新敌人
		static int BornEnemy = 30;//生成敌人计时器
		static int NeedBornEnemy = 1;
		if (NeedBornEnemy)
		if (ShowTime)//ShowTime 100ms一次
				BornEnemy++;
		if (BornEnemy >= 10)//生成新的敌人
		{
			if (HaveBornEnemyNumber > 30)
				NeedBornEnemy = 0;
			if (BornEnemyMapPiece.size() != 0)
			{
				//根据地图生成地点随机生成敌人
				int atbuf= rand() % (BornEnemyMapPiece.size() / 2);
				int grade = rand() % 8;
				int speed, hp, as;
				//根据等级分配不同属性
				if (grade <= 1) {
					//基础型：慢速、低血量
					speed = 4 * 64; hp = 1; as = 1;
				} else if (grade <= 3) {
					//追击型：中速、中血量、较强攻击
					speed = 5 * 64; hp = 1; as = 2;
				} else if (grade <= 5) {
					//快速型：高速、低血量
					speed = 8 * 64; hp = 1; as = 1;
				} else {
					//重型：慢速、高血量、可破钢墙
					speed = 3 * 64; hp = 3; as = 2;
				}
				CreateEnemy((BornEnemyMapPiece.at(atbuf*2)+1) * 64,
					(BornEnemyMapPiece.at(atbuf*2+1)+1) * 64,
					speed, hp, as, grade, rand() % 4);
		//		CreateEnemy((BornEnemyMapPiece.at(2)+1) * 64, (BornEnemyMapPiece.at(3)+1) * 64, 5 * 64, 1, 1, rand() % 8, rand() % 4);
				HaveBornEnemyNumber++;

			}
			else
			{
				HaveBornEnemyNumber++;
				int grade2 = rand() % 8;
				int speed2, hp2, as2;
				if (grade2 <= 1) { speed2 = 4 * 64; hp2 = 1; as2 = 1; }
				else if (grade2 <= 3) { speed2 = 5 * 64; hp2 = 1; as2 = 2; }
				else if (grade2 <= 5) { speed2 = 8 * 64; hp2 = 1; as2 = 1; }
				else { speed2 = 3 * 64; hp2 = 3; as2 = 2; }
				CreateEnemy(12 * 64, 12 * 64, speed2, hp2, as2, grade2, rand() % 4);
			}
			//	CreateEnemy(12 * 64, 3 * 64, 5, 1, 1, rand() % 7, rand() % 4);
			//	CreateEnemy(4 * 64, 3 * 64, 10, 1, 1, rand() % 7, rand() % 4);
			BornEnemy = 0;
		}
		//更新爆炸逻辑
		BoomList*boomp = boomlisthead.next;
		while (boomp != NULL)
		{
			boomp->boom->Logic();
			boomp = boomp->next;
		}
		//检测奖励碰撞
		CheckAwardCollision();
		//判断玩家血量以决定游戏状态
		if (!IsDoublePlayer)
		{
			if (player.Health_Point <= 0)
			{
				player.Lift--;
				if (player.Lift <= 0)
					player.Alive = false;
				else
					Player1.Born();
				//创建爆炸
				CreateBoom(player.player.x,player.player.y, 2, player.Dir);
			}
			if (player.Lift <= 0)
			{
				GameOverFlag = true;
			}
		}
		else
		{
			if (player.Alive)
			{
				if (player.Health_Point <= 0)
				{
					player.Lift--;
					if (player.Lift <= 0)
						player.Alive = false;
					else
						Player1.Born();
					//创建爆炸
					CreateBoom(player.player.x, player.player.y, 2, player.Dir);
				}
			}
			if (player2.Alive)
			{
				if (player2.Health_Point <= 0)
				{
					player2.Lift--;
					if (player2.Lift <= 0)
						player2.Alive = false;
					else
						//创建爆炸
						CreateBoom(player2.player.x, player2.player.y, 2, player2.Dir);
					player2.Born();
				}
			}
			if (!player.Alive && !player2.Alive)
			{
				GameOverFlag = true;
			}

		}
		//清理失效对象
		ClearUselessObj();
		//获取时间相关
	}
	else
	{
		if (KEY_DOWN(VK_RETURN))
		{
			RestartThisStage();
			GameOverFlag = false;
			BaseDestroyed = false;
		}
	}
	ShowTime = false;
	lasttime = GetTickCount();
}
/*--------------------------------------------------------------------
GamingScene的方法到此结束
----------------------------------------------------------------------*/


/*--------------------------------------------------------------------
GameScene的方法
----------------------------------------------------------------------*/
//游戏结束画面
void GS::ShowGameOver()
	{
		static int oldtime = GetTickCount();

		if (GetTickCount() > oldtime + 17)
		{
			SGOy -= 8;
			oldtime = GetTickCount();
		}
		if (SGOy < 320)
			Sprite_Transform_Draw(GameOver, 232, 320, 248, 160,
				0, 1, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		else
			Sprite_Transform_Draw(GameOver, 232, SGOy, 248, 160,
				0, 1, 0, 2, D3DCOLOR_XRGB(255, 255, 255));

	}
//专门服务于bullet::logic的碰撞检测函数
int  GS::Crash(int iswho, int x, int y, int speed, int dir, 
	           int shooter, unsigned long id, int movedmixel, int powerLevel, int bulletType) {
		//地图边界
		static  RECT MapEdgeTop = { 0,0,1024,64 },
			MapEdgeBelow = { 0,896,1024,960 },
			MapEdgeLeft = { 0,0,64,960 },
			MapEdgeRight = { 896,0,1024,960 };
		//先检测子弹是否碰撞到敌人
		RECT BulletRect;
		switch (dir)
		{
		case Dirction::up:
			BulletRect.bottom = y + 16 + movedmixel;
			BulletRect.left = x;
			BulletRect.right = x + 16;
			BulletRect.top = y;
			break;
		case Dirction::right:
			BulletRect.bottom = y + 16;
			BulletRect.left = x - movedmixel;
			BulletRect.right = x + 16;
			BulletRect.top = y;
			break;
		case Dirction::below:
			BulletRect.bottom = y + 16;
			BulletRect.left = x;
			BulletRect.right = x + 16;
			BulletRect.top = y - movedmixel;
			break;
		case Dirction::lift:
			BulletRect.bottom = y + 16;
			BulletRect.left = x;
			BulletRect.right = x + 16 + movedmixel;
			BulletRect.top = y;
			break;
		default:
			break;
		}

		RECT EnemyRect, Rect;
		EnemyList* ep = enemylisthead.next;
		if (shooter == 0)
		{
			while (ep != NULL)
			{
				EnemyRect.left = ep->enemy->player.x;
				EnemyRect.top = ep->enemy->player.y;
				EnemyRect.bottom = ep->enemy->player.y + 56;
				EnemyRect.right = ep->enemy->player.x + 56;
				if (IntersectRect(&Rect, &EnemyRect, &BulletRect))
				{
					if (bulletType == 2) //激光：造成1点伤害，穿透继续
					{
						ep->enemy->Health_Point -= 1;
						if (ep->enemy->Health_Point <= 0)
						{
							if (ep->enemy->IsFlashEnemy)
								CreateAward(ep->enemy->player.x, ep->enemy->player.y, rand() % 6);
							CreateBoom(ep->enemy->player.x, ep->enemy->player.y, 2, ep->enemy->Dir);
							EnemyList* nextEp = ep->next;
							DelListNode(enemylisthead.next, ep->enemy->ID);
							EnemyNumber--;
							ep = nextEp;
							continue; //激光穿透，继续检测下一个敌人
						}
						ep = ep->next;
						continue;
					}
					//普通/散弹：直接击杀
					if (ep->enemy->IsFlashEnemy)
						CreateAward(ep->enemy->player.x, ep->enemy->player.y, rand() % 6);
					CreateBoom(ep->enemy->player.x, ep->enemy->player.y, 2, ep->enemy->Dir);
					DelListNode(enemylisthead.next, ep->enemy->ID);
					EnemyNumber--;
					return 2;
				}
				ep = ep->next;
			}
		}
		//检测敌人子弹是否命中玩家
		RECT PlayerRect;
		if (shooter == 2)
		{
			if (Player1.Alive)
			{
				PlayerRect.bottom = Player1.player.y + 56;
				PlayerRect.right = Player1.player.x + 56;
				PlayerRect.left = Player1.player.x;
				PlayerRect.top = Player1.player.y;
				if (IntersectRect(&Rect, &PlayerRect, &BulletRect))
				{
					Player1.GetHurt(0);
					return 1;
				}
			}

			if (IsDoublePlayer)
			{
				if (player2.Alive)
				{
					PlayerRect.bottom = player2.player.y + 56;
					PlayerRect.right = player2.player.x + 56;
					PlayerRect.left = player2.player.x;
					PlayerRect.top = player2.player.y;
					if (IntersectRect(&Rect, &PlayerRect, &BulletRect))
					{
						player2.GetHurt(0);
						return 1;
					}
				}
			}
		}
		//子弹子弹碰撞
		BulletList*bp = bulletlisthead.next;
		RECT BulletRectTest;
		while (bp != NULL)
		{
			BulletRectTest.bottom = bp->bullet->bullet.y + 16;
			BulletRectTest.right = bp->bullet->bullet.x + 16;
			BulletRectTest.top = bp->bullet->bullet.y;
			BulletRectTest.left = bp->bullet->bullet.x;
			if (IntersectRect(&Rect, &BulletRectTest, &BulletRect))
			{
				if (id != bp->bullet->ID && shooter != bp->bullet->Shooter)
				{
					AddUselessObj(bp->bullet->ID);
					return 1;
				}
			}
			bp = bp->next;
		}
		/*
		for (int i = 0; i < EnemyNumberMAX; i++)
		{
		if (EnemyXY[i][0] == -1)
		continue;
		EnemyRect.left=EnemyXY[i][0];
		EnemyRect.top=EnemyXY[i][1];
		EnemyRect.bottom = EnemyXY[i][1] + 56;
		EnemyRect.right = EnemyXY[i][0] + 56;
		if (IntersectRect(&Rect, &EnemyRect, &BulletRect))
		{
		while (ep != NULL)
		{
		if (ep->enemy->player.x == EnemyRect.left)
		if (ep->enemy->player.y == EnemyRect.top)
		{
		AddUselessObj(ep->enemy->ID);
		EnemyXY[i][0] = -1;
		}
		ep = ep->next;
		}
		return 2;//目前为调试状态 正式版应为爆炸2
		}
		}*/
		//检测是否碰撞到砖墙
		//	int x1 = x - 20, y1 = y - 20;
		int X1, Y1, X2, Y2;
		switch (dir)
		{
		case Dirction::up:
			X1 = (x - 20) / 64;
			Y1 = y / 64;
			X2 = (x + 36) / 64;
			Y2 = y / 64;
			break;
		case Dirction::right:
			X1 = (x + 16) / 64;
			Y1 = (y - 20) / 64;
			X2 = (x + 16) / 64;
			Y2 = (y + 36) / 64;
			break;
		case Dirction::below:
			X1 = (x - 20) / 64;
			Y1 = (y + 16) / 64;
			X2 = (x + 36) / 64;
			Y2 = (y + 16) / 64;
			break;
		case Dirction::lift:
			X1 = x / 64;
			Y1 = (y - 20) / 64;
			X2 = x / 64;
			Y2 = (y + 36) / 64;
			break;
		default:
			break;
		}
		//RECT BoomRect = { x - 20,y - 20,x + 36,y + 36 };
		MapPieceList*mp = mappiecelisthead.next;
		bool crashflag1 = false, crashflag2 = false;
		if (X1 == X2&&Y1 == Y2)
		{
			while (mp != NULL)
			{
				if (X1 - 1 == mp->mappiece->X)
					if (Y1 - 1 == mp->mappiece->Y)
						crashflag1 = mp->mappiece->BeingCrash(0, BulletRect, dir, x, y, powerLevel);
				mp = mp->next;
			}
		}
		else
		{
			while (mp != NULL)
			{
				if (X1 - 1 == mp->mappiece->X)
					if (Y1 - 1 == mp->mappiece->Y)
						crashflag1 = mp->mappiece->BeingCrash(0, BulletRect, dir, x, y, powerLevel);
				mp = mp->next;
			}
			mp = mappiecelisthead.next;
			while (mp != NULL)
			{
				if (X2 - 1 == mp->mappiece->X)
					if (Y2 - 1 == mp->mappiece->Y)
						crashflag2 = mp->mappiece->BeingCrash(crashflag1, BulletRect, dir, x, y, powerLevel);
				mp = mp->next;
			}
		}
		if (!crashflag1)
		{
			mp = mappiecelisthead.next;
			while (mp != NULL)
			{
				if (X1 - 1 == mp->mappiece->X)
					if (Y1 - 1 == mp->mappiece->Y)
						crashflag1 = mp->mappiece->BeingCrash(crashflag2, BulletRect, dir, x, y, powerLevel);
				mp = mp->next;
			}

		}
		else if (!crashflag2)
		{
			mp = mappiecelisthead.next;
			while (mp != NULL)
			{
				if (X2 - 1 == mp->mappiece->X)
					if (Y2 - 1 == mp->mappiece->Y)
						crashflag2 = mp->mappiece->BeingCrash(crashflag1, BulletRect, dir, x, y, powerLevel);
				mp = mp->next;
			}

		}
		if (crashflag1 || crashflag2)
			return 1;
		//检测子弹是否命中基地（老鹰）
		{
			RECT FlagRect = { FlagGameX, FlagGameY, FlagGameX + 64, FlagGameY + 64 };
			if (IntersectRect(&Rect, &FlagRect, &BulletRect))
			{
				if ((int)GetTickCount() >= FortifyEndTime)
				{
					BaseDestroyed = true;
					GameOverFlag = true;
				}
				return 1;
			}
		}
		for (int i = 0; i < 4; i++)
		{
			switch (i)
			{
			case 0:
				if (IntersectRect(&Rect, &MapEdgeBelow, &BulletRect))
					return 3;
				break;
			case 1:
				if (IntersectRect(&Rect, &MapEdgeLeft, &BulletRect))
					return 3;
				break;
			case 2:
				if (IntersectRect(&Rect, &MapEdgeRight, &BulletRect))
					return 3;
				break;
			case 3:
				if (IntersectRect(&Rect, &MapEdgeTop, &BulletRect))
					return 3;
				break;
			default:
				break;
			}
		}
		return 0;
	}
//游戏地图绘画方法
void GS::DrawMap()
	{
		for (int i = 0; i < 13; i++)
			for (int j = 0; j < 13; j++) {
				switch (Map[j][i])
				{
				case 0:break;
					//
				case 13:Sprite_Transform_Draw(Tile, (i + 1) * 64, (j + 1) * 64,
					32, 32, 0, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

				case 26:Sprite_Transform_Draw(Tile, (i + 1) * 64, (j + 1) * 64,
					32, 32, 1, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

				case 27: {
					int waterFrame = (GetTickCount() / 500) % 2 == 0 ? 3 : 4;
					Sprite_Transform_Draw(Tile, (i + 1) * 64, (j + 1) * 64,
					32, 32, waterFrame, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				}

				case 28:Sprite_Transform_Draw(Tile, (i + 1) * 64, (j + 1) * 64,
					32, 32, 3, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

				case 29:Sprite_Transform_Draw(Tile, (i + 1) * 64, (j + 1) * 64,
					32, 32, 4, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
					//
				case 1:Sprite_Transform_Draw(Tile, (i + 1) * 64, (j + 1) * 64,
					16, 16, 0, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 2:Sprite_Transform_Draw(Tile, 64 * i + 96, (j + 1) * 64,
					16, 16, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 3:Sprite_Transform_Draw(Tile, 64 * i + 96, 64 * j + 96,
					16, 16, 14, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 4:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 96,
					16, 16, 15, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

				case 5:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 64,
					32, 16, 0, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 6:Sprite_Transform_Draw(Tile, 64 * i + 96, 64 * j + 64,
					16, 32, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 7:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 96,
					32, 16, 0, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 8:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 64,
					16, 32, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

				case 9:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 64,
					16, 32, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
					Sprite_Transform_Draw(Tile, 64 * i + 96, (j + 1) * 64,
						16, 16, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 10:Sprite_Transform_Draw(Tile, (i + 1) * 64, (j + 1) * 64,
					16, 16, 0, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
					Sprite_Transform_Draw(Tile, 64 * i + 96, 64 * j + 64,
						16, 32, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 11:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 96,
					32, 16, 0, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
					Sprite_Transform_Draw(Tile, 64 * i + 96, (j + 1) * 64,
						16, 16, 14, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 12:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 64,
					16, 32, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
					Sprite_Transform_Draw(Tile, 64 * i + 96, (j + 1) * 96,
						16, 16, 15, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

				case 14:Sprite_Transform_Draw(Tile, (i + 1) * 64, (j + 1) * 64,
					16, 16, 2, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 15:Sprite_Transform_Draw(Tile, 64 * i + 96, (j + 1) * 64,
					16, 16, 3, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 16:Sprite_Transform_Draw(Tile, 64 * i + 96, 64 * j + 96,
					16, 16, 16, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 17:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 96,
					16, 16, 17, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

				case 18:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 64,
					32, 16, 1, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 19:Sprite_Transform_Draw(Tile, 64 * i + 96, 64 * j + 64,
					16, 32, 3, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 20:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 96,
					32, 16, 1, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 21:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 64,
					16, 32, 2, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

				case 22:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 64,
					32, 16, 1, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
					Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 96,
						16, 16, 17, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 23:Sprite_Transform_Draw(Tile, (i + 1) * 64, (j + 1) * 64,
					16, 16, 2, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
					Sprite_Transform_Draw(Tile, 64 * i + 96, 64 * j + 64,
						16, 32, 3, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 24:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 96,
					32, 16, 1, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
					Sprite_Transform_Draw(Tile, 64 * i + 96, (j + 1) * 64,
						16, 16, 3, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				case 25:Sprite_Transform_Draw(Tile, 64 * i + 64, 64 * j + 96,
					32, 16, 1, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
					Sprite_Transform_Draw(Tile, (i + 1) * 64, (j + 1) * 64,
						16, 16, 2, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
				default:
					break;
				}
			}
	}
//创建地图方块
void GS::CreateMapPiece()
	{
		for (int x = 0; x < 13; x++)
			for (int y = 0; y < 13; y++) {

				if (Map[y][x] != 0) {
					//记录玩家一的出生坐标点
					if (Map[y][x] == 31)
					{
						BornPlayer1MapPiece.push_back(x);
						BornPlayer1MapPiece.push_back(y);
					}
					//记录玩家二的出生坐标点
					if (Map[y][x] == 32)
					{
						BornPlayer2MapPiece.push_back(x);
						BornPlayer2MapPiece.push_back(y);
					}
					//记录敌人的出生坐标点
					if (Map[y][x] == 33)
					{
						BornEnemyMapPiece.push_back(x);
						BornEnemyMapPiece.push_back(y);
					}
					MapPiece*b = new MapPiece;
					MapPieceList*h = mappiecelisthead.next;
					MapPieceList*New = new MapPieceList;
					New->mappiece = b;
					b->X = x;
					b->Y = y;
					b->Create(Map[y][x]);
					//更新地图，将新方块加入链表
					if (h == NULL)
					{
						mappiecelisthead.next = New;
						New->last = NULL;
						New->next = NULL;
					}
					else
					{
						if (h->next != NULL)
						{
							New->next = h->next;
							h->next = New;
							New->next->last = New;
							New->last = h;
						}
						else
						{
							h->next = New;
							New->last = h;
							New->next = NULL;
						}
					}
				}
			}
	}
//读取硬盘上地图文件信息
bool GS::ReadMapInHD(string filename)
{
	char buf[13][13];
	string sbuf = "Map\\";
	sbuf += filename;
	sbuf += ".map";

	ifstream in(sbuf, ios::in | ios::binary);
	if (!in.is_open())
	{
		//ShowMessage("sd");
		return false;
	}
	//从文件中读取地图信息
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++)
		{
			in.read(&buf[i][j], 1);
		}
	//转换为当前地图
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++)
		{
			Map[i][j] = buf[i][j];
		}

	return true;
}
bool GS::ReadMapInHD(char*filename)
	{
		char buf[13][13];
		string sbuf = "Map\\";
		sbuf += filename;
		sbuf += ".map";

		ifstream in(sbuf, ios::in | ios::binary);
		if (!in.is_open())
		{
			ShowMessage(sbuf);
		}
		//从文件中读取地图信息
		for (int i = 0; i < 13; i++)
			for (int j = 0; j < 13; j++)
			{
				in.read(&buf[i][j], 1);
			}
		//转换为当前地图
		for (int i = 0; i < 13; i++)
			for (int j = 0; j < 13; j++)
			{
				Map[i][j] = buf[i][j];
			}

		return 0;
	}
//写当前地图信息到硬盘
bool GS::WriteMapToHD(char*filename)
	{
		char buf;
		ofstream out(filename, ios::out | ios::binary);
		if (!out.is_open())
		{
			ShowMessage(filename);
		}
		for (int i = 0; i < 13; i++)
			for (int j = 0; j < 13; j++)
			{
				buf = Map[i][j];
				out.write(&buf, 1);
			}
		return 0;
	}

//创建爆炸
void GS::CreateBoom(int x, int y, int whatboom, int Dir)
	{
		BoomFire*b = new BoomFire(x, y, whatboom, Dir);
		IDNumber++;
		b->ID = IDNumber;
		BoomList*h = boomlisthead.next;
		BoomList*New = new BoomList;
		New->boom = b;
		if (h == NULL)
		{
			boomlisthead.next = New;
			New->last = NULL;
			New->next = NULL;
		}
		else
		{
			if (h->next != NULL)
			{
				New->next = h->next;
				h->next = New;
				New->next->last = New;
				New->last = h;
			}
			else
			{
				h->next = New;
				New->last = h;
				New->next = NULL;
			}
		}

	}
//创建敌人
void GS::CreateEnemy(int x, int y, int speed, int hp,
	                 int as, int grade, int dir)
	{
		int NewEnemyX = x;
		int NewEnemyY = y;

		//生成敌人对象
		Enemy*e = new Enemy(NewEnemyX, NewEnemyY, speed, hp, as, grade, dir);
		IDNumber++;
		e->ID = IDNumber;
		//约20%概率标记为闪烁敌人
		if (rand() % 5 == 0)
			e->IsFlashEnemy = true;
		EnemyList*h = enemylisthead.next;
		EnemyList*newE = new EnemyList;
		newE->enemy = e;
		if (h == NULL)
		{
			enemylisthead.next = newE;
			newE->last = NULL;
			newE->next = NULL;
		}
		else
		{
			if (h->next != NULL)
			{
				newE->next = h->next;
				h->next = newE;
				newE->next->last = newE;
				newE->last = h;
			}
			else
			{
				h->next = newE;
				newE->last = h;
				newE->next = NULL;
			}
		}

	}
/*辅助函数*/
//填充RECT
void GS::FillRect(RECT&rect, long l = -1, long r = -1, 
	              long t = -1, long b = -1)
	{
		rect.left = l;
		rect.right = r;
		rect.top = t;
		rect.bottom = b;
	}
//读取地图信息
void GS::ReadMap(int x, int y, RECT&rect1, RECT&rect2)
	{
		switch (Map[y][x])
		{
		case 0:
		case 27:
		case 28:
		case 29:
			FillRect(rect1);
			FillRect(rect2);
			break;
		case 1:
		case 14:
			FillRect(rect1, 64 * x + 64, 64 * x + 96, 64 * y + 64, 64 * y + 96);
			FillRect(rect2);
			break;
		case 2:
		case 15:
			FillRect(rect1, 64 * x + 96, 64 * x + 128, 64 * y + 64, 64 * y + 96);
			FillRect(rect2);
			break;
		case 3:
		case 16:
			FillRect(rect1, 64 * x + 96, 64 * x + 128, 64 * y + 96, 64 * y + 128);
			FillRect(rect2);
			break;
		case 4:
		case 17:
			FillRect(rect1, 64 * x + 64, 64 * x + 96, 64 * y + 96, 64 * y + 128);
			FillRect(rect2);
			break;

		case 5:
		case 18:
			FillRect(rect1, 64 * x + 64, 64 * x + 128, 64 * y + 64, 64 * y + 96);
			FillRect(rect2);
			break;
		case 6:
		case 19:
			FillRect(rect1, 64 * x + 96, 64 * x + 128, 64 * y + 64, 64 * y + 128);
			FillRect(rect2);
			break;
		case 7:
		case 20:
			FillRect(rect1, 64 * x + 64, 64 * x + 128, 64 * y + 96, 64 * y + 128);
			FillRect(rect2);
			break;
		case 8:
		case 21:
			FillRect(rect1, 64 * x + 64, 64 * x + 96, 64 * y + 64, 64 * y + 128);
			FillRect(rect2);
			break;
			//
		case 9:
		case 22:
			FillRect(rect1, 64 * x + 64, 64 * x + 128, 64 * y + 64, 64 * y + 96);
			FillRect(rect2, 64 * x + 64, 64 * x + 96, 64 * y + 96, 64 * y + 128);
			break;
		case 10:
		case 23:
			FillRect(rect1, 64 * x + 64, 64 * x + 128, 64 * y + 64, 64 * y + 96);
			FillRect(rect2, 64 * x + 96, 64 * x + 128, 64 * y + 96, 64 * y + 128);
			break;
		case 11:
		case 24:
			FillRect(rect1, 64 * x + 96, 64 * x + 128, 64 * y + 64, 64 * y + 128);
			FillRect(rect2, 64 * x + 64, 64 * x + 96, 64 * y + 96, 64 * y + 128);
			break;
		case 12:
		case 25:
			FillRect(rect1, 64 * x + 64, 64 * x + 128, 64 * y + 96, 64 * y + 128);
			FillRect(rect2, 64 * x + 64, 64 * x + 96, 64 * y + 64, 64 * y + 96);
			break;

		case 13:
		case 26:
			FillRect(rect1, 64 * x + 64, 64 * x + 128, 64 * y + 64, 64 * y + 128);
			FillRect(rect2);
			break;

		default:
			break;
		}
	}
//绘制网格线
void GS::DrawNet()
	{
		if (!Global::Debug::ShowDebugInfo) return;
		RECT rect;
		for (int i = 0; i < 12; i++)
		{
			FillRect(rect, 128 + i * 64, 129 + i * 64, 64, 896);
			d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
		}

		for (int i = 0; i < 12; i++)
		{
			FillRect(rect, 64, 896, 128 + i * 64, 129 + i * 64);
			d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
		}
	}
//清理失效对象
void GS::ClearUselessObj()
	{
		UselessObj*up = uselessobjhead.next;
		//清理失效子弹
		BulletList*b = bulletlisthead.next;
		while (up != NULL)
		{
			while (b != NULL&&up != NULL)
			{
				if (b->bullet->ID != up->ID)
					b = b->next;
				else
				{
					DelListNode(bulletlisthead.next, b->bullet->ID);
					DelUselessObj();
					up = uselessobjhead.next;
					break;
				}
			}
			if (bulletlisthead.next == NULL)
				break;
			else
				b = bulletlisthead.next;
			if (up != NULL)
				up = up->next;
		}
	
		//清理失效敌人并创建爆炸
		up = uselessobjhead.next;
		EnemyList*ep = enemylisthead.next;
		while (up != NULL)
		{
		while (ep!=NULL&&up != NULL)
		{
		if (ep->enemy->ID != up->ID)
		ep = ep->next;
		else
		{
		//CreateBoom(ep->enemy->player.x, ep->enemy->player.y, 2, ep->enemy->Dir);
		DelListNode(enemylisthead.next, ep->enemy->ID);
		DelUselessObj();
		up = uselessobjhead.next;
		break;
		}
		}
		if (enemylisthead.next == NULL)
		break;
		else
		ep = enemylisthead.next;
		if (up != NULL)
		up = up->next;
		}

		//清理失效爆炸
		up = uselessobjhead.next;
		BoomList*bp = boomlisthead.next;
		while (up != NULL)
		{
			while (bp != NULL&&up != NULL)
			{
				if (bp->boom->ID != up->ID)
					bp = bp->next;
				else
				{
					DelListNode(boomlisthead.next, bp->boom->ID);
					DelUselessObj();
					up = uselessobjhead.next;
					break;
				}
			}
			if (boomlisthead.next == NULL)
				break;
			else
				bp = boomlisthead.next;
			if (up != NULL)
				up = up->next;
		}

		//清理失效奖励
		up = uselessobjhead.next;
		AwardItemList*awp = awardlisthead.next;
		while (up != NULL)
		{
			while (awp != NULL&&up != NULL)
			{
				if (awp->award->ID != up->ID)
					awp = awp->next;
				else
				{
					DelListNode(awardlisthead.next, awp->award->ID);
					DelUselessObj();
					up = uselessobjhead.next;
					break;
				}
			}
			if (awardlisthead.next == NULL)
				break;
			else
				awp = awardlisthead.next;
			if (up != NULL)
				up = up->next;
		}

	}
//
void GS::AddUselessObj(unsigned long id)
	{
		UselessObj*p = new UselessObj;
		p->next = uselessobjhead.next;
		p->ID = id;
		uselessobjhead.next = p;
	}
//删除单个元素
void GS::DelUselessObj()
	{
		UselessObj*p = uselessobjhead.next;
		if (p != NULL)
			uselessobjhead.next = p->next;
		delete p;
	}

bool GS::DelListNode(EnemyList*listhead, unsigned long id)//删除成功返回true，否则返回false
	{
		EnemyList*p = listhead;
		while (p != NULL)
		{
			if (p->enemy->ID != id)
				p = p->next;
			else
			{
				if (p->last != NULL)
				{
					if (p->next != NULL) {
						p->last->next = p->next;
						p->next->last = p->last;
					}
					else
						p->last->next = NULL;
				}
				else if (p->next != NULL)
				{
					p->next->last = NULL;
					enemylisthead.next = p->next;
				}
				else
				{
					enemylisthead.next = NULL;
				}
				delete p;
				return true;
			}
		}
		return false;
	}

bool GS::DelListNode(BulletList*listhead, unsigned long id)//删除成功返回true，否则返回false
	{
		BulletList*p = listhead;
		while (p != NULL)
		{
			if (p->bullet->ID != id)
				p = p->next;
			else
			{
				if (p->last != NULL)
				{
					if (p->next != NULL) {
						p->last->next = p->next;
						p->next->last = p->last;
					}
					else
						p->last->next = NULL;
				}
				else if (p->next != NULL)
				{
					p->next->last = NULL;
					bulletlisthead.next = p->next;
				}
				else
				{
					bulletlisthead.next = NULL;
				}
				delete p;
				return true;
			}
		}
		return false;
	}

bool GS::DelListNode(BoomList*listhead, unsigned long id)//删除成功返回true，否则返回false
	{
		BoomList*p = listhead;
		while (p != NULL)
		{
			if (p->boom->ID != id)
				p = p->next;
			else
			{
				if (p->last != NULL)
				{
					if (p->next != NULL) {
						p->last->next = p->next;
						p->next->last = p->last;
					}
					else
						p->last->next = NULL;
				}
				else if (p->next != NULL)
				{
					p->next->last = NULL;
					boomlisthead.next = p->next;
				}
				else
				{
					boomlisthead.next = NULL;
				}
				delete p;
				return true;
			}
		}
		return false;
	}
//处理时间相关
void GS::DIDA() {
		NowTime = (int)GetTickCount();
		if (NowTime > StartTime + 100)
		{
			if (StartTime != 0)
				SurplusTime = NowTime - StartTime + 100;
			StartTime = NowTime;
			ShowTime = true;
		}
	}

void GS::NewStage()
{
	//清理地图块
	MapPieceList*buf;
	MapPieceList*mp = mappiecelisthead.next;
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++)
		{
			while (mp != NULL)
			{
				if (i == mp->mappiece->X&&j == mp->mappiece->Y)
				{
					buf = mp;
					mp = mp->next;
					delete buf;
				}
				else
					mp = mp->next;
			}
		}
	mappiecelisthead.next = NULL;
	//清理敌人
	EnemyList* ep = enemylisthead.next;
	while (ep != NULL)
	{
		AddUselessObj(ep->enemy->ID);
		ep = ep->next;
	}
	//清理子弹
	BulletList*bp = bulletlisthead.next;
	while (bp != NULL)
	{
		AddUselessObj(bp->bullet->ID);
		bp = bp->next;
	}
	//清理爆炸
	BoomList*boomp = boomlisthead.next;
	while (boomp != NULL)
	{
		AddUselessObj(boomp->boom->ID);
		boomp = boomp->next;
	}

	ClearUselessObj();
	//清理奖励
	AwardItemList*ap = awardlisthead.next;
	while (ap != NULL)
	{
		AddUselessObj(ap->award->ID);
		ap = ap->next;
	}
	ClearUselessObj();
	//重置奖励效果计时器
	FreezeEndTime = 0;
	FortifyEndTime = 0;
	//保留等级，重置其他属性
	player.Speed = 5 * 64;
	player.Dir = Dirction::up;
	player.player.x = 64 * 6;
	player.player.y = 64 * 13;
	player.Lift = 1;
	player.Health_Point = 1;
	player.ApplyGradeStats();

	if (IsDoublePlayer)
	{
		player2.Speed = 5 * 64;
		player2.Dir = Dirction::up;
		player2.player.x = 64 * 6;
		player2.player.y = 64 * 13;
		player2.Lift = 1;
		player2.Health_Point = 1;
		player2.ApplyGradeStats();
	}
	//重置地图数据
	SGOy = 960;
	GameOverFlag = false;
	BaseDestroyed = false;
	EnemyNumber = 30;
	player.Alive = true;
	player.Health_Point = 1;
	player.FlashFlag = true;
	player.InvincibleEndTime = GetTickCount() + 3000;
	if (BornPlayer1MapPiece.size() != 0)
	{
		int atbuf = rand() % (BornPlayer1MapPiece.size() / 2);
		player.player.x = (BornPlayer1MapPiece.at(atbuf * 2) + 1) * 64;
		player.player.y = (BornPlayer1MapPiece.at(atbuf * 2 + 1) + 1) * 64;
	}
	player2.Alive = true;
	player2.Health_Point = 1;
	player2.FlashFlag = true;
	player2.InvincibleEndTime = GetTickCount() + 3000;
	if (BornPlayer2MapPiece.size() != 0)
	{
		int atbuf = rand() % (BornPlayer2MapPiece.size() / 2);
		player2.player.x = (BornPlayer2MapPiece.at(atbuf * 2) + 1) * 64;
		player2.player.y = (BornPlayer2MapPiece.at(atbuf * 2 + 1) + 1) * 64;
	}
	HaveBornEnemyNumber = 0;
}

void GS::ReadNextMap()
{
	NowLevel++;
	string mapName = "stage" + std::to_string(NowLevel);
	if(!ReadMapInHD(mapName))
	{
		Game_ChangeScene(GAME_STATE::Home);
	}
	CreateMapPiece();
}

void GS::StartNextStage()
{
	NewStage();
	ReadNextMap();
}

void GS::RestartThisStage()
{
	NewStage();
	CreateMapPiece();
}
/*--------------------------------------------------------------------
GameScene的方法到此结束
----------------------------------------------------------------------*/

//敌人AI
//计算朝向目标的方向
int DirToward(float fromX, float fromY, float toX, float toY)
{
	float dx = toX - fromX;
	float dy = toY - fromY;
	if (abs(dx) > abs(dy))
		return dx > 0 ? Dirction::right : Dirction::lift;
	else
		return dy > 0 ? Dirction::below : Dirction::up;
}

// Grade 0-1: 基础AI（笨拙型）
// 碰墙后随机转向，很少射击，随机游走
int* ai_basic(int state, bool cflag)
{
	static int a[2];
	if (cflag)
	{
		if (rand() % 8 == 0)
		{
			a[0] = state;
			a[1] = 1;
			return a;
		}
		a[0] = rand() % 4;
		a[1] = 0;
		return a;
	}
	if (rand() % 150 == 0)
	{
		a[0] = state;
		a[1] = 1;
		return a;
	}
	if (rand() % 80 == 0)
	{
		a[0] = rand() % 4;
		a[1] = 0;
		return a;
	}
	a[0] = state;
	a[1] = 0;
	return a;
}

// Grade 2-3: 追击型AI
// 概率性追踪玩家，频繁射击
int* ai_aggressive(int state, bool cflag, float ex, float ey)
{
	static int a[2];
	float targetX = Player1.player.x;
	float targetY = Player1.player.y;
	//双人模式选择更近的玩家
	if (IsDoublePlayer && player2.Alive)
	{
		float d1 = abs(ex - Player1.player.x) + abs(ey - Player1.player.y);
		float d2 = abs(ex - player2.player.x) + abs(ey - player2.player.y);
		if (d2 < d1)
		{
			targetX = player2.player.x;
			targetY = player2.player.y;
		}
	}

	if (cflag)
	{
		//碰墙时高概率射击
		if (rand() % 3 == 0)
		{
			a[0] = state;
			a[1] = 1;
			return a;
		}
		//碰墙时70%朝向玩家，30%随机
		if (rand() % 10 < 7)
			a[0] = DirToward(ex, ey, targetX, targetY);
		else
			a[0] = rand() % 4;
		a[1] = 0;
		return a;
	}
	//频繁射击（约每50帧1次）
	if (rand() % 50 == 0)
	{
		a[0] = state;
		a[1] = 1;
		return a;
	}
	//较高概率追踪玩家方向
	if (rand() % 40 == 0)
	{
		a[0] = DirToward(ex, ey, targetX, targetY);
		a[1] = 0;
		return a;
	}
	//偶尔随机变向增加不可预测性
	if (rand() % 100 == 0)
	{
		a[0] = rand() % 4;
		a[1] = 0;
		return a;
	}
	a[0] = state;
	a[1] = 0;
	return a;
}

// Grade 4-5: 快速型AI
// 频繁变向，到处乱窜，射击中等频率
int* ai_fast(int state, bool cflag)
{
	static int a[2];
	if (cflag)
	{
		//碰墙时立即转向，偶尔射击
		if (rand() % 5 == 0)
		{
			a[0] = state;
			a[1] = 1;
			return a;
		}
		a[0] = rand() % 4;
		a[1] = 0;
		return a;
	}
	//中等频率射击
	if (rand() % 70 == 0)
	{
		a[0] = state;
		a[1] = 1;
		return a;
	}
	//非常频繁地随机变向（闪避型移动）
	if (rand() % 25 == 0)
	{
		a[0] = rand() % 4;
		a[1] = 0;
		return a;
	}
	a[0] = state;
	a[1] = 0;
	return a;
}

// Grade 6-7: 重型/智能AI
// 以基地为目标，高HP，破坏力强，会主动摧毁挡路的墙
int* ai_heavy(int state, bool cflag, float ex, float ey)
{
	static int a[2];
	//主要目标是基地
	float targetX = (float)FlagGameX;
	float targetY = (float)FlagGameY;

	if (cflag)
	{
		//碰墙时必定射击（清除障碍）
		a[0] = state;
		a[1] = 1;
		return a;
	}
	//较高射击频率（清除前方障碍）
	if (rand() % 35 == 0)
	{
		a[0] = state;
		a[1] = 1;
		return a;
	}
	//80%概率朝基地方向移动
	if (rand() % 30 == 0)
	{
		if (rand() % 10 < 8)
			a[0] = DirToward(ex, ey, targetX, targetY);
		else
			a[0] = rand() % 4;
		a[1] = 0;
		return a;
	}
	a[0] = state;
	a[1] = 0;
	return a;
}

//敌人AI分发函数（根据Grade选择不同AI）
int* enemyAI(int grade, int state, bool cflag, float ex, float ey)
{
	if (grade <= 1)
		return ai_basic(state, cflag);
	else if (grade <= 3)
		return ai_aggressive(state, cflag, ex, ey);
	else if (grade <= 5)
		return ai_fast(state, cflag);
	else
		return ai_heavy(state, cflag, ex, ey);
}
//敌人的构造函数
Enemy::Enemy(int x, int y, int speed, int hp,
	         int as,int grade,int dir)
{
	player.x = x;
	player.y = y;
	Speed = speed;
	Health_Point = hp;
	Attack_Speed = as;
	Grade = grade;
	Dir = dir;
	Time = 0;
	IsFlashEnemy = false;
	LastShootTime = 0;
	//根据等级设置子弹威力
	if (grade >= 6)
		PowerLevel = 3; //重型可破钢墙
	else
		PowerLevel = 0;
	//根据等级设置子弹速度
	if (grade >= 4 && grade <= 5)
		BulletSpeed = 12 * 64; //快速型子弹也快
	else if (grade >= 6)
		BulletSpeed = 8 * 64;
	else
		BulletSpeed = 6 * 64;
}
//敌人的渲染方法
bool Enemy::Draw()
{
	//闪烁敌人交替使用红色调渲染
	D3DCOLOR drawColor = D3DCOLOR_XRGB(255, 255, 255);
	if (IsFlashEnemy && (GetTickCount() / 200) % 2 == 0)
		drawColor = D3DCOLOR_XRGB(255, 100, 100);

	if (Grade <= 3) {
		if (MoveStage) {
			Sprite_Transform_Draw(Enemy_TXTTURE, player.x, player.y, player.width, player.height,
				Dir * 8 + Grade * 2, player.columns, 0, 2, drawColor);
			MoveStage = !MoveStage;
		}
		else
		{
			Sprite_Transform_Draw(Enemy_TXTTURE, player.x, player.y, player.width, player.height,
				Dir * 8 + Grade * 2 + 1, player.columns, 0, 2, drawColor);
			MoveStage = !MoveStage;
		}
	}
	else
	{
		if (MoveStage) {
			Sprite_Transform_Draw(Enemy_TXTTURE, player.x, player.y, player.width, player.height,
				Dir * 8 + Grade * 2 + 24, player.columns, 0, 2, drawColor);
			MoveStage = !MoveStage;
		}
		else
		{
			Sprite_Transform_Draw(Enemy_TXTTURE, player.x, player.y, player.width, player.height,
				Dir * 8 + Grade * 2 + 25, player.columns, 0, 2, drawColor);
			MoveStage = !MoveStage;
		}
	}
		return true;

}
//敌人逻辑
bool Enemy::Logic(bool st)
{
	int *a=enemyAI(Grade, Dir, CrashingFlag, player.x, player.y);
	CrashingFlag = false;
	int d = *a;
	if (*(a + 1) == 1)
	{
		//射击冷却：根据Attack_Speed决定间隔（as=1→3秒，as=2→2秒）
		int cooldown = 3000 / (Attack_Speed > 0 ? Attack_Speed : 1);
		int now = GetTickCount();
		if (now - LastShootTime >= cooldown)
		{
			Shoot(2, PowerLevel);
			LastShootTime = now;
		}
	}
	/**
	if (st)
		Time++;
	if (Time == 6)
	{
		d = rand() % 4;
		Time = 0;
		Shoot(2);
	}
	*/
	double  srtime = GetTickCount() - lasttime;
	switch (d)
	{
	case Dirction::up:
		Dir = Dirction::up;
		player.y -= Speed*srtime / 1000;
		if (player.y < 64)
			player.y = 64;
		break;
	case Dirction::right:
		Dir = Dirction::right;
		player.x += Speed*srtime / 1000;
		if (player.x > 840)
			player.x = 840;
		break;
	case Dirction::below:
		Dir = Dirction::below;
		player.y += Speed*srtime / 1000;
		if (player.y > 840)
			player.y = 840;
		break;
	case Dirction::lift:
		Dir = Dirction::lift;
		player.x -= Speed*srtime / 1000;
		if (player.x < 64)
			player.x = 64;
		break;
	default:
		break;
	}
	RECT PlayerRect = { player.x,player.y,player.x + 56,player.y + 56 };
	RECT Rect;
	//和地图方块碰撞检测
	{
		int X1, Y1, X2, Y2;
		switch (d)
		{
		case Dirction::up:
			X1 = player.x / 64;
			Y1 = player.y / 64;
			X2 = (player.x + 56) / 64;
			Y2 = Y1;
			break;
		case Dirction::right:
			X1 = (player.x + 56) / 64;
			Y1 = player.y / 64;
			X2 = (player.x + 56) / 64;
			Y2 = (player.y + 56) / 64;
			break;
		case Dirction::below:
			X1 = (player.x + 56) / 64;
			Y1 = (player.y + 56) / 64;
			X2 = player.x / 64;
			Y2 = Y1;
			break;
		case Dirction::lift:
			X1 = player.x / 64;
			Y1 = (player.y + 56) / 64;
			X2 = player.x / 64;
			Y2 = player.y / 64;
			break;
		default:
			break;
		}
		MapPieceList*mp = mappiecelisthead.next;
		int result1 = 0, result2 = 0;
		if (X1 == X2&&Y1 == Y2)
		{
			while (mp != NULL)
			{
				if (X1 - 1 == mp->mappiece->X)
					if (Y1 - 1 == mp->mappiece->Y)
						result1 = mp->mappiece->PECrach(d, PlayerRect);
				mp = mp->next;
			}
		}
		else
		{
			while (mp != NULL)
			{
				if (X1 - 1 == mp->mappiece->X)
					if (Y1 - 1 == mp->mappiece->Y)
						result1 = mp->mappiece->PECrach(d, PlayerRect);
				mp = mp->next;
			}
			mp = mappiecelisthead.next;
			while (mp != NULL)
			{
				if (X2 - 1 == mp->mappiece->X)
					if (Y2 - 1 == mp->mappiece->Y)
						result2 = mp->mappiece->PECrach(d, PlayerRect);
				mp = mp->next;
			}

		}
		if (result1 != 0 || result2 != 0)
		{
			CrashingFlag = true;
			switch (d)
			{
			case Dirction::up:
				if (result1 > result2)
					player.y = result1;
				else
					player.y = result2;
				break;
			case Dirction::right:
				if (result2 == 0)
					player.x = result1 - 56;
				else if (result1 == 0)
					player.x = result2 - 56;
				else if (result1<result2)
					player.x = result1 - 56;
				else
					player.x = result2 - 56;
				break;
			case Dirction::below:
				if (result2 == 0)
					player.y = result1 - 56;
				else if (result1 == 0)
					player.y = result2 - 56;
				else if (result1<result2)
					player.y = result1 - 56;
				else
					player.y = result2 - 56;
				break;
			case Dirction::lift:
				if (result1 > result2)
					player.x = result1;
				else
					player.x = result2;
				break;
			default:
				break;
			}
		}
	}
	RECT EnemyRect;
	if (player.alive)
	{
		EnemyRect.bottom = Player1.player.y + 56;
		EnemyRect.right = Player1.player.x + 56;
		EnemyRect.left = Player1.player.x;
		EnemyRect.top = Player1.player.y;
		if (IntersectRect(&Rect, &EnemyRect, &PlayerRect))
		{
			CrashingFlag = true;
			switch (d)
			{
			case Dirction::up:
				player.y = Player1.player.y + 56;
				break;
			case Dirction::right:
				player.x = Player1.player.x - 56;
				break;
			case Dirction::below:
				player.y = Player1.player.y - 56;
				break;
			case Dirction::lift:
				player.x = Player1.player.x + 56;
				break;
			default:
				break;
			}
		}
	}
	if (IsDoublePlayer)
	{
		if (player2.Alive)
		{
			EnemyRect.bottom = player2.player.y + 56;
			EnemyRect.right = player2.player.x + 56;
			EnemyRect.left = player2.player.x;
			EnemyRect.top = player2.player.y;
			if (IntersectRect(&Rect, &EnemyRect, &PlayerRect))
			{
				CrashingFlag = true;
				switch (d)
				{
				case Dirction::up:
					player.y = player2.player.y + 56;
					break;
				case Dirction::right:
					player.x = player2.player.x - 56;
					break;
				case Dirction::below:
					player.y = player2.player.y - 56;
					break;
				case Dirction::lift:
					player.x = player2.player.x + 56;
					break;
				default:
					break;
				}
			}
		}
	}
		// Enemy-enemy collision
	{
		EnemyList* other = enemylisthead.next;
		while (other != NULL)
		{
			if (other->enemy != this && other->enemy->player.alive)
			{
				RECT otherRect = { (long)other->enemy->player.x, (long)other->enemy->player.y,
					(long)other->enemy->player.x + 56, (long)other->enemy->player.y + 56 };
				RECT tmpRect;
				if (IntersectRect(&tmpRect, &otherRect, &PlayerRect))
				{
					CrashingFlag = true;
					switch (d)
					{
					case Dirction::up:
						player.y = other->enemy->player.y + 56;
						break;
					case Dirction::right:
						player.x = other->enemy->player.x - 56;
						break;
					case Dirction::below:
						player.y = other->enemy->player.y - 56;
						break;
					case Dirction::lift:
						player.x = other->enemy->player.x + 56;
						break;
					default:
						break;
					}
				}
			}
			other = other->next;
		}
	}
return false;
}


//爆炸渲染方法
bool BoomFire::Draw()
{

	if (WhatBoom == 1)
	{
		if(rand()%2==1)
		Sprite_Transform_Draw(Boom1, x+rand()%5, y-rand()%5,
			28, 28, 0, 1, 0, 2, D3DCOLOR_XRGB(255, 255, 255));

	}
	else
	{

		Sprite_Transform_Draw(Boom1, x, y,
			28, 28, 0, 1, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		if (GetTickCount() >= Time + 25)
			Sprite_Transform_Draw(Boom2, x - 36 - rand() % 5, y - 36 + rand() % 5,
				64, 64, 0, 1, 0, 2, D3DCOLOR_XRGB(255, 255, 255));

	}
	return true;
}
//爆炸逻辑
void BoomFire::Logic()
{
	if (WhatBoom == 1)
	{
		if (GetTickCount() >= Time+100)
			AddUselessObj(ID);
	}
	else {
		if (GetTickCount() >= Time + 100)
			AddUselessObj(ID);
	}
	return;
}
//爆炸构造函数
BoomFire::BoomFire(int x, int y, int wb, int d) :
	x(x), y(y), WhatBoom(wb), Dir(d)
{
	Time = GetTickCount();
}

/*--------------------------------------------------------------------
奖励物品
----------------------------------------------------------------------*/
AwardItem::AwardItem(int x, int y, int type) : x(x), y(y), Type(type)
{
	CreateTime = GetTickCount();
}

bool AwardItem::Draw()
{
	//闪烁显示
	if ((GetTickCount() / 300) % 2 == 0)
		Sprite_Transform_Draw(Award, x, y, 30, 28, Type, 6, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	return true;
}

void GS::CreateAward(int x, int y, int type)
{
	AwardItem*a = new AwardItem(x, y, type);
	IDNumber++;
	a->ID = IDNumber;
	AwardItemList*h = awardlisthead.next;
	AwardItemList*New = new AwardItemList;
	New->award = a;
	if (h == NULL)
	{
		awardlisthead.next = New;
		New->last = NULL;
		New->next = NULL;
	}
	else
	{
		if (h->next != NULL)
		{
			New->next = h->next;
			h->next = New;
			New->next->last = New;
			New->last = h;
		}
		else
		{
			h->next = New;
			New->last = h;
			New->next = NULL;
		}
	}
}

bool GS::DelListNode(AwardItemList*listhead, unsigned long id)
{
	AwardItemList*p = listhead;
	while (p != NULL)
	{
		if (p->award->ID != id)
			p = p->next;
		else
		{
			if (p->last != NULL)
			{
				if (p->next != NULL) {
					p->last->next = p->next;
					p->next->last = p->last;
				}
				else
					p->last->next = NULL;
			}
			else if (p->next != NULL)
			{
				p->next->last = NULL;
				awardlisthead.next = p->next;
			}
			else
			{
				awardlisthead.next = NULL;
			}
			delete p;
			return true;
		}
	}
	return false;
}

void GS::CheckAwardCollision()
{
	AwardItemList*ap = awardlisthead.next;
	while (ap != NULL)
	{
		AwardItemList*nextAp = ap->next;
		//超时消失（15秒）
		if (GetTickCount() - ap->award->CreateTime > 15000)
		{
			AddUselessObj(ap->award->ID);
			ap = nextAp;
			continue;
		}
		RECT AwardRect = {
			(long)ap->award->x, (long)ap->award->y,
			(long)ap->award->x + 56, (long)ap->award->y + 56
		};
		RECT Rect;
		int awardType = ap->award->Type;
		//检测玩家一碰撞
		if (Player1.Alive)
		{
			RECT P1Rect = {
				(long)Player1.player.x, (long)Player1.player.y,
				(long)Player1.player.x + 56, (long)Player1.player.y + 56
			};
			if (IntersectRect(&Rect, &AwardRect, &P1Rect))
			{
				switch (awardType)
				{
				case 0: //头盔：无敌10秒
					Player1.FlashFlag = true;
					Player1.InvincibleEndTime = GetTickCount() + 10000;
					break;
				case 1: //定时器：冻结敌人10秒
					FreezeEndTime = GetTickCount() + 10000;
					break;
				case 2: //铲子：基地加固20秒
					FortifyEndTime = GetTickCount() + 20000;
					break;
				case 3: //炸弹：消灭所有敌人
					DestroyAllEnemies();
					break;
				case 4: //星星：升级
					Player1.LevelUp();
					break;
				case 5: //坦克：额外生命
					Player1.Lift++;
					break;
				}
				AddUselessObj(ap->award->ID);
				ap = nextAp;
				continue;
			}
		}
		//检测玩家二碰撞
		if (IsDoublePlayer && player2.Alive)
		{
			RECT P2Rect = {
				(long)player2.player.x, (long)player2.player.y,
				(long)player2.player.x + 56, (long)player2.player.y + 56
			};
			if (IntersectRect(&Rect, &AwardRect, &P2Rect))
			{
				switch (awardType)
				{
				case 0:
					player2.FlashFlag = true;
					player2.InvincibleEndTime = GetTickCount() + 10000;
					break;
				case 1:
					FreezeEndTime = GetTickCount() + 10000;
					break;
				case 2:
					FortifyEndTime = GetTickCount() + 20000;
					break;
				case 3:
					DestroyAllEnemies();
					break;
				case 4:
					player2.LevelUp();
					break;
				case 5:
					player2.Lift++;
					break;
				}
				AddUselessObj(ap->award->ID);
				ap = nextAp;
				continue;
			}
		}
		ap = nextAp;
	}
}

//炸弹效果：消灭所有屏幕上的敌人
void GS::DestroyAllEnemies()
{
	EnemyList*ep = enemylisthead.next;
	while (ep != NULL)
	{
		CreateBoom(ep->enemy->player.x, ep->enemy->player.y, 2, ep->enemy->Dir);
		AddUselessObj(ep->enemy->ID);
		EnemyNumber--;
		ep = ep->next;
	}
}

/*--------------------------------------------------------------------
玩家的方法
----------------------------------------------------------------------*/
//初始化玩家信息
Player::Player()
{
	Health_Point = 1;//血量
	Speed = 5*64;
	Attack_Speed = 3;
	Dir = Dirction::up;
	Grade = 0;
	CurrentBulletType = 0; //默认普通子弹
	LaserActive = false;
	LaserLastDamageTime = 0;
	LaserEndX = 0;
	LaserEndY = 0;
	ShotgunAmmo = 99;
	LaserAmmo = 99;
	player.scaling = 2;
	player.columns = 8; 
	player.frame = 0;
	player.color= D3DCOLOR_XRGB(255, 255, 255);
	player.x = 64*6;
	player.y = 64*13;
	player.width = 28;
	player.height = 28;
	BulletSpeed = 8*64;
	FlickerFrame = 0;
	Lift = 99;
	PowerLevel = 0;
	Alive = true;
	FlashFlag = false;
	InvincibleEndTime = 0;
}
void Player::ApplyGradeStats()
{
	switch (Grade)
	{
	case 0:
		BulletSpeed = 8 * 64;
		PowerLevel = 0;
		Attack_Speed = 3;
		break;
	case 1:
		BulletSpeed = 12 * 64;
		PowerLevel = 0;
		Attack_Speed = 5;
		break;
	case 2:
		BulletSpeed = 12 * 64;
		PowerLevel = 0;
		Attack_Speed = 7;
		break;
	case 3:
		BulletSpeed = 12 * 64;
		PowerLevel = 3;
		Attack_Speed = 9;
		break;
	default:
		break;
	}
}
//升级
void Player::LevelUp()
{
	if (Grade < 3)
	{
		Grade++;
		ApplyGradeStats();
	}
}
//添加子弹到链表
void GS::AddBulletToList(Bullet* b)
{
	IDNumber++;
	b->ID = IDNumber;
	BulletList*c = bulletlisthead.next;
	if (c == NULL)
	{
		bulletlisthead.next = new BulletList;
		bulletlisthead.next->bullet = b;
		bulletlisthead.next->next = NULL;
		bulletlisthead.next->last = NULL;
	}
	else
	{
		BulletList*d = new BulletList;
		d->bullet = b;
		if (c->next != NULL)
		{
			d->next = c->next;
			c->next = d;
			d->next->last = d;
			d->last = c;
		}
		else
		{
			c->next = d;
			d->last = c;
			d->next = NULL;
		}
	}
}
//激光更新：从玩家位置沿方向射线检测，找到终点，并对路径上敌人造成伤害
void GS::UpdateLaser(Player& p)
{
	//起点：玩家中心
	float startX = p.player.x + 28;
	float startY = p.player.y + 28;
	float endX = startX, endY = startY;
	//沿方向延伸到边界
	switch (p.Dir)
	{
	case Dirction::up:    endX = startX; endY = 64; break;
	case Dirction::below: endX = startX; endY = 896; break;
	case Dirction::lift:  endX = 64; endY = startY; break;
	case Dirction::right: endX = 896; endY = startY; break;
	}
	//射线检测墙壁：沿方向步进检查MapPiece碰撞
	float stepX = 0, stepY = 0;
	switch (p.Dir)
	{
	case Dirction::up:    stepX = 0; stepY = -4; break;
	case Dirction::below: stepX = 0; stepY = 4; break;
	case Dirction::lift:  stepX = -4; stepY = 0; break;
	case Dirction::right: stepX = 4; stepY = 0; break;
	}
	float testX = startX, testY = startY;
	bool hitWall = false;
	while (!hitWall)
	{
		testX += stepX;
		testY += stepY;
		//超出边界
		if (testX < 64 || testX > 896 || testY < 64 || testY > 896)
		{
			testX -= stepX;
			testY -= stepY;
			break;
		}
		//检测是否碰到不可穿越墙壁
		int gridX = (int)testX / 64 - 1;
		int gridY = (int)testY / 64 - 1;
		if (gridX >= 0 && gridX < 13 && gridY >= 0 && gridY < 13)
		{
			MapPieceList* mp = mappiecelisthead.next;
			while (mp != NULL)
			{
				if (mp->mappiece->X == gridX && mp->mappiece->Y == gridY)
				{
					RectList* rp = mp->mappiece->rectlisthead->next;
					if (rp != NULL)
					{
						//砖墙、钢墙、水面等都阻挡激光（草地除外）
						if (rp->rect->left < 64 || (rp->rect->left >= 96 && rp->rect->left < 128))
						{
							hitWall = true;
						}
					}
				}
				mp = mp->next;
			}
		}
	}
	endX = testX;
	endY = testY;
	p.LaserEndX = endX;
	p.LaserEndY = endY;
	//对路径上的敌人每秒造成1点伤害
	int now = GetTickCount();
	if (now - p.LaserLastDamageTime >= 1000)
	{
		p.LaserLastDamageTime = now;
		p.LaserAmmo--; //每秒消耗1弹药
		RECT laserRect;
		switch (p.Dir)
		{
		case Dirction::up:
			laserRect.left = (LONG)startX - 4;
			laserRect.right = (LONG)startX + 4;
			laserRect.top = (LONG)endY;
			laserRect.bottom = (LONG)startY;
			break;
		case Dirction::below:
			laserRect.left = (LONG)startX - 4;
			laserRect.right = (LONG)startX + 4;
			laserRect.top = (LONG)startY;
			laserRect.bottom = (LONG)endY;
			break;
		case Dirction::lift:
			laserRect.left = (LONG)endX;
			laserRect.right = (LONG)startX;
			laserRect.top = (LONG)startY - 4;
			laserRect.bottom = (LONG)startY + 4;
			break;
		case Dirction::right:
			laserRect.left = (LONG)startX;
			laserRect.right = (LONG)endX;
			laserRect.top = (LONG)startY - 4;
			laserRect.bottom = (LONG)startY + 4;
			break;
		}
		RECT Rect;
		RECT EnemyRect;
		EnemyList* ep = enemylisthead.next;
		while (ep != NULL)
		{
			EnemyRect.left = (LONG)ep->enemy->player.x;
			EnemyRect.top = (LONG)ep->enemy->player.y;
			EnemyRect.right = (LONG)ep->enemy->player.x + 56;
			EnemyRect.bottom = (LONG)ep->enemy->player.y + 56;
			if (IntersectRect(&Rect, &EnemyRect, &laserRect))
			{
				ep->enemy->Health_Point -= 1;
				if (ep->enemy->Health_Point <= 0)
				{
					if (ep->enemy->IsFlashEnemy)
						CreateAward(ep->enemy->player.x, ep->enemy->player.y, rand() % 6);
					CreateBoom(ep->enemy->player.x, ep->enemy->player.y, 2, ep->enemy->Dir);
					EnemyList* nextEp = ep->next;
					DelListNode(enemylisthead.next, ep->enemy->ID);
					EnemyNumber--;
					ep = nextEp;
					continue;
				}
			}
			ep = ep->next;
		}
	}
}
//激光渲染：画一条从玩家到终点的光线（多层叠加，带脉动效果）
void GS::DrawLaserBeam(Player& p)
{
	if (!p.LaserActive || !p.Alive) return;
	float startX = p.player.x + 28;
	float startY = p.player.y + 28;
	//使用D3DXLine绘制多层光线
	ID3DXLine* pLine = NULL;
	if (SUCCEEDED(D3DXCreateLine(d3dDev, &pLine)))
	{
		D3DXVECTOR2 linePoints[2];
		linePoints[0] = D3DXVECTOR2(startX, startY);
		linePoints[1] = D3DXVECTOR2(p.LaserEndX, p.LaserEndY);
		//脉动效果：宽度和亮度随时间变化
		float pulse = 0.8f + 0.4f * sin((float)GetTickCount() * 0.01f);
		float pulse2 = 0.7f + 0.6f * sin((float)GetTickCount() * 0.025f);
		int glowAlpha = (int)(40 * pulse2);
		pLine->SetAntialias(TRUE);
		//第1层：最外层光晕（宽，半透明红色）
		pLine->SetWidth(28.0f * pulse);
		pLine->Begin();
		pLine->Draw(linePoints, 2, D3DCOLOR_ARGB(glowAlpha, 255, 50, 50));
		pLine->End();
		//第2层：外层光晕（橙红色）
		pLine->SetWidth(18.0f * pulse);
		pLine->Begin();
		pLine->Draw(linePoints, 2, D3DCOLOR_ARGB((int)(60 * pulse), 255, 100, 30));
		pLine->End();
		//第3层：中层光芒（亮黄色）
		pLine->SetWidth(10.0f * pulse);
		pLine->Begin();
		pLine->Draw(linePoints, 2, D3DCOLOR_ARGB(160, 255, 220, 80));
		pLine->End();
		//第4层：核心光线（白色高亮）
		pLine->SetWidth(4.0f);
		pLine->Begin();
		pLine->Draw(linePoints, 2, D3DCOLOR_ARGB(255, 255, 255, 230));
		pLine->End();
		//命中点闪光效果
		float flashSize = 12.0f * pulse2;
		D3DXVECTOR2 impactH[2], impactV[2];
		impactH[0] = D3DXVECTOR2(p.LaserEndX - flashSize, p.LaserEndY);
		impactH[1] = D3DXVECTOR2(p.LaserEndX + flashSize, p.LaserEndY);
		impactV[0] = D3DXVECTOR2(p.LaserEndX, p.LaserEndY - flashSize);
		impactV[1] = D3DXVECTOR2(p.LaserEndX, p.LaserEndY + flashSize);
		pLine->SetWidth(6.0f * pulse);
		pLine->Begin();
		pLine->Draw(impactH, 2, D3DCOLOR_ARGB(200, 255, 200, 50));
		pLine->End();
		pLine->Begin();
		pLine->Draw(impactV, 2, D3DCOLOR_ARGB(200, 255, 200, 50));
		pLine->End();
		pLine->Release();
	}
}
//射击方法
bool Player::Shoot(int shooter,int powlv) {
	if (CurrentBulletType == 1) //散弹：发射3颗子弹
	{
		//计算基础角度（弧度）
		float baseAngle;
		switch (Dir)
		{
		case Dirction::up:    baseAngle = 3.14159f / 2.0f; break;
		case Dirction::right: baseAngle = 0.0f; break;
		case Dirction::below: baseAngle = -3.14159f / 2.0f; break;
		case Dirction::lift:  baseAngle = 3.14159f; break;
		default: baseAngle = 0.0f; break;
		}
		float offsetAngle = 3.14159f / 12.0f; // 15度
		//计算子弹起始位置
		int bx = (int)player.x, by = (int)player.y;
		int spreadDist = 12; //散开间距
		switch (Dir)
		{
		case Dirction::up:
			bx = (int)player.x + 20; by = (int)player.y;
			//中间子弹
			{ Bullet*b0 = new Bullet(shooter, bx, by, BulletSpeed, baseAngle, powlv, 1); AddBulletToList(b0); }
			//左侧子弹（向左偏移）
			{ Bullet*b1 = new Bullet(shooter, bx - spreadDist, by, BulletSpeed, baseAngle + offsetAngle, powlv, 1); AddBulletToList(b1); }
			//右侧子弹（向右偏移）
			{ Bullet*b2 = new Bullet(shooter, bx + spreadDist, by, BulletSpeed, baseAngle - offsetAngle, powlv, 1); AddBulletToList(b2); }
			break;
		case Dirction::below:
			bx = (int)player.x + 20; by = (int)player.y + 40;
			{ Bullet*b0 = new Bullet(shooter, bx, by, BulletSpeed, baseAngle, powlv, 1); AddBulletToList(b0); }
			{ Bullet*b1 = new Bullet(shooter, bx - spreadDist, by, BulletSpeed, baseAngle - offsetAngle, powlv, 1); AddBulletToList(b1); }
			{ Bullet*b2 = new Bullet(shooter, bx + spreadDist, by, BulletSpeed, baseAngle + offsetAngle, powlv, 1); AddBulletToList(b2); }
			break;
		case Dirction::lift:
			bx = (int)player.x; by = (int)player.y + 20;
			{ Bullet*b0 = new Bullet(shooter, bx, by, BulletSpeed, baseAngle, powlv, 1); AddBulletToList(b0); }
			{ Bullet*b1 = new Bullet(shooter, bx, by - spreadDist, BulletSpeed, baseAngle - offsetAngle, powlv, 1); AddBulletToList(b1); }
			{ Bullet*b2 = new Bullet(shooter, bx, by + spreadDist, BulletSpeed, baseAngle + offsetAngle, powlv, 1); AddBulletToList(b2); }
			break;
		case Dirction::right:
			bx = (int)player.x + 40; by = (int)player.y + 20;
			{ Bullet*b0 = new Bullet(shooter, bx, by, BulletSpeed, baseAngle, powlv, 1); AddBulletToList(b0); }
			{ Bullet*b1 = new Bullet(shooter, bx, by - spreadDist, BulletSpeed, baseAngle + offsetAngle, powlv, 1); AddBulletToList(b1); }
			{ Bullet*b2 = new Bullet(shooter, bx, by + spreadDist, BulletSpeed, baseAngle - offsetAngle, powlv, 1); AddBulletToList(b2); }
			break;
		}
		ShotgunAmmo--; //消耗散弹弹药
		return true;
	}
	else //普通子弹
	{
		Bullet*b = new Bullet(shooter,Player::player.x,Player::player.y,
			Player::BulletSpeed, Player::Dir,powlv, 0);
		AddBulletToList(b);
		return true;
	}
}
//玩家渲染方法
bool Player::Draw()
{
	static int lasttime=GetTickCount();
	static bool ChangeFrame = false;
	Sprite_Transform_Draw(Number, 960, 544, 14, 14, Lift / 10, 10, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	Sprite_Transform_Draw(Number, 993, 544, 14, 14, Lift % 10, 10, 0, 2, D3DCOLOR_XRGB(255, 255, 255));

	//2222222222222
	if (ChangeFrame) {
		Sprite_Transform_Draw(Player_1, player.x, player.y, player.width, player.height,
			Dir * 8 + Grade * 2, player.columns, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		if (Key_Down(Global::PlayerControl::Player1[2]) || Key_Down(Global::PlayerControl::Player1[3]) || Key_Down(Global::PlayerControl::Player1[0]) || Key_Down(Global::PlayerControl::Player1[1]))
			ChangeFrame = !ChangeFrame;
	}
	else {
		Sprite_Transform_Draw(Player_1, player.x, player.y, player.width, player.height,
			Dir * 8 + Grade * 2 + 1, player.columns, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		if (Key_Down(Global::PlayerControl::Player1[2]) || Key_Down(Global::PlayerControl::Player1[3]) || Key_Down(Global::PlayerControl::Player1[0]) || Key_Down(Global::PlayerControl::Player1[1]))
			ChangeFrame = !ChangeFrame;
	}
	if (FlashFlag)
	{
		//时间基准无敌检测
		if ((int)GetTickCount() >= InvincibleEndTime)
		{
			FlashFlag = false;
		}
		else
		{
			//渲染Shield贴图（32×32，2帧动画）
			int shieldFrame = (GetTickCount() / 100) % 2;
			Sprite_Transform_Draw(Shield, player.x, player.y, 32, 32, shieldFrame, 1, 0, 2.0f, D3DCOLOR_XRGB(255, 255, 255));
		}
	}
	return false;
}
//玩家逻辑更新
bool Player::Logic(int d)
{
	double srtime = GetTickCount() - lasttime;
	switch (d)
	{
	case Dirction::up:
		Dir = Dirction::up;
		player.y -= Speed*srtime / 1000;
		if (player.y < 64)
			player.y = 64;
		break;
	case Dirction::right:
		Dir = Dirction::right;
		player.x += Speed*srtime / 1000;
		if (player.x > 840)
			player.x = 840;
		break;
	case Dirction::below:
		Dir = Dirction::below;
		player.y += Speed*srtime / 1000;
		if (player.y > 840)
			player.y = 840;
		break;
	case Dirction::lift:
		Dir = Dirction::lift;
		player.x -= Speed*srtime / 1000;
		if (player.x < 64)
			player.x = 64;
		break;
	default:
		break;
	}
	RECT PlayerRect = { player.x,player.y,player.x + 56,player.y + 56 };
	RECT EnemyRect, Rect;
	//和地图方块碰撞检测
	{
		int X1, Y1, X2, Y2;
		switch (d)
		{
		case Dirction::up:
			X1 = player.x / 64;
			Y1 = player.y / 64;
			X2 = (player.x + 56) / 64;
			Y2 = Y1;
			break;
		case Dirction::right:
			X1 = (player.x + 56) / 64;
			Y1 = player.y / 64;
			X2 = (player.x + 56) / 64;
			Y2 = (player.y + 56) / 64;
			break;
		case Dirction::below:
			X1 = (player.x + 56) / 64;
			Y1 = (player.y + 56) / 64;
			X2 = player.x / 64;
			Y2 = Y1;
			break;
		case Dirction::lift:
			X1 = player.x / 64;
			Y1 = (player.y + 56) / 64;
			X2 = player.x / 64;
			Y2 = player.y / 64;
			break;
		default:
			break;
		}
		MapPieceList*mp = mappiecelisthead.next;
		int result1 = 0, result2 = 0;
		if (X1 == X2&&Y1 == Y2)
		{
			while (mp != NULL)
			{
				if (X1 - 1 == mp->mappiece->X)
					if (Y1 - 1 == mp->mappiece->Y)
						result1 = mp->mappiece->PECrach(d, PlayerRect);
				mp = mp->next;
			}
		}
		else
		{
			while (mp != NULL)
			{
				if (X1 - 1 == mp->mappiece->X)
					if (Y1 - 1 == mp->mappiece->Y)
						result1 = mp->mappiece->PECrach(d, PlayerRect);
				mp = mp->next;
			}
			mp = mappiecelisthead.next;
			while (mp != NULL)
			{
				if (X2 - 1 == mp->mappiece->X)
					if (Y2 - 1 == mp->mappiece->Y)
						result2 = mp->mappiece->PECrach(d, PlayerRect);
				mp = mp->next;
			}

		}
		if (result1 != 0 || result2 != 0)
		{
			switch (d)
			{
			case Dirction::up:
				if (result1 > result2)
					player.y = result1;
				else
					player.y = result2;
				break;
			case Dirction::right:
				if (result2 == 0)
					player.x = result1 - 56;
				else if (result1 == 0)
					player.x = result2 - 56;
				else if(result1<result2)
					player.x = result1 - 56;
				else
					player.x = result2 - 56;
				break;
			case Dirction::below:
				if (result2 == 0)
					player.y = result1 - 56;
				else if (result1 == 0)
					player.y = result2 - 56;
				else if (result1<result2)
					player.y = result1 - 56;
				else
					player.y = result2 - 56;
				break;
			case Dirction::lift:
				if (result1 > result2)
					player.x = result1;
				else
					player.x = result2;
				break;
			default:
				break;
			}
		}
	}
    //和敌人的碰撞检测
	EnemyList* ep = enemylisthead.next;
	while (ep != NULL)
	{
		EnemyRect.left = ep->enemy->player.x;
		EnemyRect.top = ep->enemy->player.y;
		EnemyRect.bottom = ep->enemy->player.y + 56;
		EnemyRect.right = ep->enemy->player.x + 56;
		if (IntersectRect(&Rect, &EnemyRect, &PlayerRect))
		{
			// 不造成伤害，只阻碍移动方向
			switch (d)
			{
			case Dirction::up:
				player.y = ep->enemy->player.y + 56;
				break;
			case Dirction::right:
				player.x = ep->enemy->player.x - 56;
				break;
			case Dirction::below:
				player.y = ep->enemy->player.y - 56;
				break;
			case Dirction::lift:
				player.x = ep->enemy->player.x + 56;
				break;
			default:
				break;
			}
		}
		ep = ep->next;
	}
	//和玩家二的碰撞检测
	if (IsDoublePlayer)
	{
		if (player2.Alive)
		{
			EnemyRect.bottom = player2.player.y + 56;
			EnemyRect.right = player2.player.x + 56;
			EnemyRect.left = player2.player.x;
			EnemyRect.top = player2.player.y;
			if (IntersectRect(&Rect, &EnemyRect, &PlayerRect))
			{
				switch (d)
				{
				case Dirction::up:
					player.y = player2.player.y + 56;
					break;
				case Dirction::right:
					player.x = player2.player.x - 56;
					break;
				case Dirction::below:
					player.y = player2.player.y - 56;
					break;
				case Dirction::lift:
					player.x = player2.player.x + 56;
					break;
				default:
					break;
				}
			}
		}
	}
	return false;
}
//玩家被命中后的处理方法
bool Player::GetHurt(int power)
{
	//无敌状态免疫伤害
	if (FlashFlag)
		return true;
	Health_Point--;
	if (Health_Point == 0)
	{
		if (Global::Sound::SoundSwicth)
			Sound::PlayerBoom->Play();
		return false;
	}
	else
		return true;
}
//玩家出生重置方法
void Player::Born()
{
	Health_Point = 1;
	FlashFlag = true;
	InvincibleEndTime = GetTickCount() + 3000;
	Grade = 0;
	ApplyGradeStats();
	if (BornPlayer1MapPiece.size() != 0)
	{
		int atbuf = rand() % (BornPlayer1MapPiece.size() / 2);
		player.x = (BornPlayer1MapPiece.at(atbuf*2) + 1) * 64;
		player.y = (BornPlayer1MapPiece.at(atbuf*2 + 1) + 1) * 64;
	}
	else
	{
		player.x = 64 * 6;
		player.y = 64 * 13;
	}
}

//玩家二
//初始化玩家二信息
Player2::Player2()
{
	Health_Point = 1;//血量
	Speed = 5 * 64;
	Attack_Speed = 3;
	Dir = Dirction::up;
	Grade = 0;
	player.scaling = 2;
	player.columns = 8;
	player.frame = 0;
	player.color = D3DCOLOR_XRGB(255, 255, 255);
	player.x = 64 * 9;
	player.y = 64 * 13;
	player.width = 28;
	player.height = 28;
	BulletSpeed = 64 * 12;
	FlickerFrame = 0;
	Lift = 99;
	InvincibleEndTime = 0;
}
bool Player2::Draw()
{
	static int lasttime = GetTickCount();
	static bool ChangeFrame = false;
	Sprite_Transform_Draw(Number, 960, 640, 14, 14, Lift / 10, 10, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	Sprite_Transform_Draw(Number, 992, 640, 14, 14, Lift % 10, 10, 0, 2, D3DCOLOR_XRGB(255, 255, 255));

	if (ChangeFrame) {
		Sprite_Transform_Draw(Player_2, player.x, player.y, player.width, player.height,
			Dir * 8 + Grade * 2, player.columns, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		if (Key_Down(Global::PlayerControl::Player2[2]) || Key_Down(Global::PlayerControl::Player2[3]) || Key_Down(Global::PlayerControl::Player2[0]) || Key_Down(Global::PlayerControl::Player2[1]))
			ChangeFrame = !ChangeFrame;
	}
	else {
		Sprite_Transform_Draw(Player_2, player.x, player.y, player.width, player.height,
			Dir * 8 + Grade * 2 + 1, player.columns, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		if (Key_Down(Global::PlayerControl::Player2[2]) || Key_Down(Global::PlayerControl::Player2[3]) || Key_Down(Global::PlayerControl::Player2[0]) || Key_Down(Global::PlayerControl::Player2[1]))
			ChangeFrame = !ChangeFrame;
	}
	if (FlashFlag)
	{
		if ((int)GetTickCount() >= InvincibleEndTime)
		{
			FlashFlag = false;
		}
		else
		{
			int shieldFrame = (GetTickCount() / 100) % 2;
			Sprite_Transform_Draw(Shield, player.x, player.y, 32, 32, shieldFrame, 1, 0, 2.0f, D3DCOLOR_XRGB(255, 255, 255));
		}
	}
	return false;

}
//玩家二逻辑更新
bool Player2::Logic(int d)
{
	double srtime = GetTickCount() - lasttime;
	switch (d)
	{
	case Dirction::up:
		Dir = Dirction::up;
		player.y -= Speed*srtime / 1000;
		if (player.y < 64)
			player.y = 64;
		break;
	case Dirction::right:
		Dir = Dirction::right;
		player.x += Speed*srtime / 1000;
		if (player.x > 840)
			player.x = 840;
		break;
	case Dirction::below:
		Dir = Dirction::below;
		player.y += Speed*srtime / 1000;
		if (player.y > 840)
			player.y = 840;
		break;
	case Dirction::lift:
		Dir = Dirction::lift;
		player.x -= Speed*srtime / 1000;
		if (player.x < 64)
			player.x = 64;
		break;
	default:
		break;
	}
	RECT PlayerRect = { player.x,player.y,player.x + 56,player.y + 56 };
	RECT EnemyRect, Rect;
	//和地图方块碰撞检测
	{
		int X1, Y1, X2, Y2;
		switch (d)
		{
		case Dirction::up:
			X1 = player.x / 64;
			Y1 = player.y / 64;
			X2 = (player.x + 56) / 64;
			Y2 = Y1;
			break;
		case Dirction::right:
			X1 = (player.x + 56) / 64;
			Y1 = player.y / 64;
			X2 = (player.x + 56) / 64;
			Y2 = (player.y + 56) / 64;
			break;
		case Dirction::below:
			X1 = (player.x + 56) / 64;
			Y1 = (player.y + 56) / 64;
			X2 = player.x / 64;
			Y2 = Y1;
			break;
		case Dirction::lift:
			X1 = player.x / 64;
			Y1 = (player.y + 56) / 64;
			X2 = player.x / 64;
			Y2 = player.y / 64;
			break;
		default:
			break;
		}
		MapPieceList*mp = mappiecelisthead.next;
		int result1 = 0, result2 = 0;
		if (X1 == X2&&Y1 == Y2)
		{
			while (mp != NULL)
			{
				if (X1 - 1 == mp->mappiece->X)
					if (Y1 - 1 == mp->mappiece->Y)
						result1 = mp->mappiece->PECrach(d, PlayerRect);
				mp = mp->next;
			}
		}
		else
		{
			while (mp != NULL)
			{
				if (X1 - 1 == mp->mappiece->X)
					if (Y1 - 1 == mp->mappiece->Y)
						result1 = mp->mappiece->PECrach(d, PlayerRect);
				mp = mp->next;
			}
			mp = mappiecelisthead.next;
			while (mp != NULL)
			{
				if (X2 - 1 == mp->mappiece->X)
					if (Y2 - 1 == mp->mappiece->Y)
						result2 = mp->mappiece->PECrach(d, PlayerRect);
				mp = mp->next;
			}

		}
		if (result1 != 0 || result2 != 0)
		{
			switch (d)
			{
			case Dirction::up:
				if (result1 > result2)
					player.y = result1;
				else
					player.y = result2;
				break;
			case Dirction::right:
				if (result2 == 0)
					player.x = result1 - 56;
				else if (result1 == 0)
					player.x = result2 - 56;
				else if (result1 < result2)
					player.x = result1 - 56;
				else
					player.x = result2 - 56;
				break;
			case Dirction::below:
				if (result2 == 0)
					player.y = result1 - 56;
				else if (result1 == 0)
					player.y = result2 - 56;
				else if (result1 < result2)
					player.y = result1 - 56;
				else
					player.y = result2 - 56;
				break;
			case Dirction::lift:
				if (result1 > result2)
					player.x = result1;
				else
					player.x = result2;
				break;
			default:
				break;
			}
		}
	}
	//和敌人的碰撞检测
	EnemyList* ep = enemylisthead.next;
	while (ep != NULL)
	{
		EnemyRect.left = ep->enemy->player.x;
		EnemyRect.top = ep->enemy->player.y;
		EnemyRect.bottom = ep->enemy->player.y + 56;
		EnemyRect.right = ep->enemy->player.x + 56;
		if (IntersectRect(&Rect, &EnemyRect, &PlayerRect))
		{
			// 不造成伤害，只阻碍移动方向
			switch (d)
			{
			case Dirction::up:
				player.y = ep->enemy->player.y + 56;
				break;
			case Dirction::right:
				player.x = ep->enemy->player.x - 56;
				break;
			case Dirction::below:
				player.y = ep->enemy->player.y - 56;
				break;
			case Dirction::lift:
				player.x = ep->enemy->player.x + 56;
				break;
			default:
				break;
			}
		}
		ep = ep->next;
	}
	//和玩家一的碰撞检测
	if (Player1.Alive)
	{
		EnemyRect.bottom = Player1.player.y + 56;
		EnemyRect.right = Player1.player.x + 56;
		EnemyRect.left = Player1.player.x;
		EnemyRect.top = Player1.player.y;
		if (IntersectRect(&Rect, &EnemyRect, &PlayerRect))
		{
			switch (d)
			{
			case Dirction::up:
				player.y = Player1.player.y + 56;
				break;
			case Dirction::right:
				player.x = Player1.player.x - 56;
				break;
			case Dirction::below:
				player.y = Player1.player.y - 56;
				break;
			case Dirction::lift:
				player.x = Player1.player.x + 56;
				break;
			default:
				break;
			}
		}
	}
	return false;
}
//
void Player2::Born()
{
	Health_Point = 1;
	FlashFlag = true;
	InvincibleEndTime = GetTickCount() + 3000;
	Grade = 0;
	ApplyGradeStats();
	if (BornPlayer2MapPiece.size() != 0)
	{
		int atbuf = rand() % (BornPlayer2MapPiece.size() / 2);
		player.x = (BornPlayer2MapPiece.at(atbuf*2) + 1) * 64;
		player.y = (BornPlayer2MapPiece.at(atbuf*2 + 1) + 1) * 64;
	}
	else
	{

	}

}
/*--------------------------------------------------------------------
玩家的方法到此结束
----------------------------------------------------------------------*/

//子弹的构造函数
Bullet::Bullet(int shooter,int x, int y, int S, int D,int powlv, int btype) :Speed(S), Dir(D),Shooter(shooter)
{
	BoomFlag = 0;
	PowerLevel = powlv;
	BulletType = btype;
	VelX = 0;
	VelY = 0;
	bullet.width = 16;
	bullet.height = 16;
	FlickerFrame = 0;
	LastFrametime = GetTickCount();
	switch (D)
	{
	case Dirction::up:
		bullet.x = x + 20;
		bullet.y = y;
		VelX = 0; VelY = -(float)S;
		break;
	case Dirction::below:
		bullet.x = x+20;
		bullet.y = y+40;
		VelX = 0; VelY = (float)S;
		break;
	case Dirction::lift:
		bullet.x = x;
		bullet.y = y+20;
		VelX = -(float)S; VelY = 0;
		break;
	case Dirction::right:
		bullet.x = x + 40;
		bullet.y = y + 20;
		VelX = (float)S; VelY = 0;
		break;
	default:
		break;
	}
}
//斜向子弹构造（散弹用）
Bullet::Bullet(int shooter, int x, int y, int speed, float angle, int powlv, int btype)
	:Speed(speed), Dir(Dirction::up), Shooter(shooter)
{
	BoomFlag = 0;
	PowerLevel = powlv;
	BulletType = btype;
	bullet.width = 16;
	bullet.height = 16;
	FlickerFrame = 0;
	LastFrametime = GetTickCount();
	VelX = speed * cos(angle);
	VelY = -speed * sin(angle); //屏幕Y轴向下
	bullet.x = (float)x;
	bullet.y = (float)y;
}
//子弹移动和碰撞检测方法
bool Bullet::Logic()
{
	//碰撞检测
	 double srtime = GetTickCount() - lasttime;
	MovedPixel = Speed*srtime / 1000;
	//使用速度分量移动（支持斜向）
	bullet.x = bullet.x + VelX * (float)srtime / 1000.0f;
	bullet.y = bullet.y + VelY * (float)srtime / 1000.0f;

    int result = Crash( 0,bullet.x, bullet.y, Speed, Dir,Shooter,ID, MovedPixel, PowerLevel, BulletType);
	if (result == 1)
	{
		if(PowerLevel==0)
		AddUselessObj(ID);//记录无效ID到回收链表
		//创建爆炸
		CreateBoom(bullet.x-20, bullet.y-20, 1, Dir);
	}
	if (result == 2)
	{
		if (PowerLevel == 0)
			AddUselessObj(ID);
	}
	if (result == 3)
	{
		AddUselessObj(ID);
		CreateBoom(bullet.x - 20, bullet.y - 20, 1, Dir);
	}
	if (result == 1 || result == 2||result==3)
		return true;

	return false;
}
//子弹渲染方法
bool Bullet::Draw()
{
	if (BoomFlag == 0) {
		//根据子弹类型设置颜色
		D3DCOLOR bulletColor = D3DCOLOR_XRGB(255, 255, 255); //普通：白色
		if (BulletType == 1) bulletColor = D3DCOLOR_XRGB(255, 200, 100); //散弹：橙色
		if (BulletType == 2) bulletColor = D3DCOLOR_XRGB(100, 255, 100); //激光：绿色

		//散弹使用旋转角度渲染
		if (BulletType == 1 && (VelX != 0 || VelY != 0))
		{
			float angle = atan2(-VelY, VelX); //屏幕Y轴反向
			float rotation = -angle + 3.14159f / 2.0f; //转换到D3D旋转（0=朝上）
			Sprite_Transform_Draw(Bullet_TXTTURE, bullet.x, bullet.y,
				8, 8, 0, 4, rotation, 2, bulletColor);
		}
		else
		{
			switch (Dir)
			{
			case Dirction::up:
				Sprite_Transform_Draw(Bullet_TXTTURE, bullet.x, bullet.y,
					8, 8, 0, 4, 0, 2, bulletColor);
				break;
			case Dirction::below:
				Sprite_Transform_Draw(Bullet_TXTTURE, bullet.x, bullet.y,
					8, 8, 2, 4, 0, 2, bulletColor);
				break;
			case Dirction::lift:
				Sprite_Transform_Draw(Bullet_TXTTURE, bullet.x, bullet.y,
					8, 8, 3, 4, 0, 2, bulletColor);
				break;
			case Dirction::right:
				Sprite_Transform_Draw(Bullet_TXTTURE, bullet.x, bullet.y,
					8, 8, 1, 4, 0, 2, bulletColor);
				break;
			default:
				break;
			}
		}
		if (PowerLevel == 3)
		{
			if (GetTickCount() > LastFrametime + 50)
			{
				FlickerFrame = FlickerFrame < 8 ? FlickerFrame + 1 : 0;
				LastFrametime = GetTickCount();
			}
			Sprite_Transform_Draw(Flicker[FlickerFrame], bullet.x - 125, bullet.y - 92, 800, 600, 0, 1, 0, 1.0f/3.0f, 1.0f/3.0f, D3DCOLOR_XRGB(255, 255, 255));
		}
		return true;
	}
	else
	{
		return false;
	}

}

/*--------------------------------------------------------------------
Class MapPiece的方法
----------------------------------------------------------------------*/
     MapPiece::MapPiece()
{
	rectlisthead = new RectListHead;
	rectlisthead->next = NULL;
}

void MapPiece::Draw()
{
	RectList*rp = rectlisthead->next;
	while (rp != NULL)
	{
		if(rp->rect->left<32)
		Sprite_Transform_Draw(Tile, (X+1)*64+rp->rect->left*2, (Y + 1) * 64+rp->rect->top*2,
			rp->rect, 0, 1, 0, 2, 2,D3DCOLOR_XRGB(255, 255, 255));
		else if(rp->rect->left<64)
		Sprite_Transform_Draw(Tile, (X+1)*64+(rp->rect->left-32)*2, (Y + 1) * 64+rp->rect->top*2,
			rp->rect, 0, 1, 0, 2, 2,D3DCOLOR_XRGB(255, 255, 255));
		else if(rp->rect->left<96)
			Sprite_Transform_Draw(Tile, X * 64 + rp->rect->left*2-64, (Y + 1) * 64 + rp->rect->top,
				rp->rect, 0, 1, 0, 2, 2, D3DCOLOR_XRGB(255, 255, 255));
		else if (rp->rect->left<128)
		{
			// 水面动画：在第4帧(x=96)和第5帧(x=128)之间交替
			RECT waterRect;
			int waterOffset = ((GetTickCount() / 500) % 2 == 0) ? 96 : 128;
			waterRect.left = waterOffset;
			waterRect.top = 0;
			waterRect.right = waterOffset + 32;
			waterRect.bottom = 32;
			Sprite_Transform_Draw(Tile, (X + 1) * 64, (Y + 1) * 64,
				&waterRect, 0, 1, 0, 2, 2, D3DCOLOR_XRGB(255, 255, 255));
		}
		else if (rp->rect->left<160)
			Sprite_Transform_Draw(Tile, (X + 1) * 64 + (rp->rect->left-128)*2, (Y + 1) * 64 + rp->rect->top*2,
				rp->rect, 0, 1, 0, 2, 2, D3DCOLOR_XRGB(255, 255, 255));
		else if (rp->rect->left<192)
			Sprite_Transform_Draw(Tile, (X + 1) * 64 + (rp->rect->left-160)*2, (Y + 1) * 64 + rp->rect->top*2,
				rp->rect, 0, 1, 0, 2, 2, D3DCOLOR_XRGB(255, 255, 255));
		else
			Sprite_Transform_Draw(Tile, (X + 1) * 64 +( rp->rect->left-192)*2, (Y + 1) * 64 + rp->rect->top*2,
				rp->rect, 0, 1, 0, 2, 2, D3DCOLOR_XRGB(255, 255, 255));
		rp = rp->next;
	}
}
//创建地图方块
void MapPiece::CreateMapRect(int x, int y, int wight, int hight)
{
	RECT *b = new RECT;
	b->left = x;
	b->top = y;
	b->right = x + wight;
	b->bottom = y + hight;
	RectList*New = new RectList;
	New->rect = b;
	if (rectlisthead->next == NULL)
	{
		rectlisthead->next = New;
		New->last = NULL;
		New->next = NULL;
	}
	else
	{
		if (rectlisthead->next->next != NULL)
		{
			New->next = rectlisthead->next->next;
			rectlisthead->next->next = New;
			New->next->last = New;
			New->last = rectlisthead->next;
		}
		else
		{
			rectlisthead->next->next = New;
			New->last = rectlisthead->next;
			New->next = NULL;
		}
	}

}

bool MapPiece::Create(int mapid)
{
	switch (mapid)
	{
	case 0:
		break;
	case 1:
		CreateMapRect(0, 0, 8, 8);
		CreateMapRect(8, 0, 8, 8);
		CreateMapRect(0, 8, 8, 8);
		CreateMapRect(8, 8, 8, 8);
		break;
	case 2:
		CreateMapRect(16, 0, 8, 8);
		CreateMapRect(24, 0, 8, 8);
		CreateMapRect(16, 8, 8, 8);
		CreateMapRect(24, 8, 8, 8);
		break;
	case 3:
		CreateMapRect(16, 16, 8, 8);
		CreateMapRect(24, 16, 8, 8);
		CreateMapRect(16, 24, 8, 8);
		CreateMapRect(24, 24, 8, 8);
		break;
	case 4:
		CreateMapRect(0, 16, 8, 8);
		CreateMapRect(8, 16, 8, 8);
		CreateMapRect(0, 24, 8, 8);
		CreateMapRect(8, 24, 8, 8);
		break;


	case 5:
		CreateMapRect(0, 0, 8, 8);
		CreateMapRect(8, 0, 8, 8);
		CreateMapRect(0, 8, 8, 8);
		CreateMapRect(8, 8, 8, 8);
		CreateMapRect(16, 0, 8, 8);
		CreateMapRect(24, 0, 8, 8);
		CreateMapRect(16, 8, 8, 8);
		CreateMapRect(24, 8, 8, 8);
		break;
	case 6:
		CreateMapRect(16, 0, 8, 8);
		CreateMapRect(24, 0, 8, 8);
		CreateMapRect(16, 8, 8, 8);
		CreateMapRect(24, 8, 8, 8);
		CreateMapRect(16, 16, 8, 8);
		CreateMapRect(24, 16, 8, 8);
		CreateMapRect(16, 24, 8, 8);
		CreateMapRect(24, 24, 8, 8);
		break;
	case 7:
		CreateMapRect(16, 16, 8, 8);
		CreateMapRect(24, 16, 8, 8);
		CreateMapRect(16, 24, 8, 8);
		CreateMapRect(24, 24, 8, 8);
		CreateMapRect(0, 16, 8, 8);
		CreateMapRect(8, 16, 8, 8);
		CreateMapRect(0, 24, 8, 8);
		CreateMapRect(8, 24, 8, 8);
		break;
	case 8:
		CreateMapRect(0, 16, 8, 8);
		CreateMapRect(8, 16, 8, 8);
		CreateMapRect(0, 24, 8, 8);
		CreateMapRect(8, 24, 8, 8);
		CreateMapRect(0, 0, 8, 8);
		CreateMapRect(8, 0, 8, 8);
		CreateMapRect(0, 8, 8, 8);
		CreateMapRect(8, 8, 8, 8);
		break;


	case 9:
		CreateMapRect(0, 0, 8, 8);
		CreateMapRect(8, 0, 8, 8);
		CreateMapRect(0, 8, 8, 8);
		CreateMapRect(8, 8, 8, 8);
		CreateMapRect(16, 0, 8, 8);
		CreateMapRect(24, 0, 8, 8);
		CreateMapRect(16, 8, 8, 8);
		CreateMapRect(24, 8, 8, 8);
		CreateMapRect(0, 16, 8, 8);
		CreateMapRect(8, 16, 8, 8);
		CreateMapRect(0, 24, 8, 8);
		CreateMapRect(8, 24, 8, 8);
		break;
	case 10:
		CreateMapRect(0, 0, 8, 8);
		CreateMapRect(8, 0, 8, 8);
		CreateMapRect(0, 8, 8, 8);
		CreateMapRect(8, 8, 8, 8);
		CreateMapRect(16, 0, 8, 8);
		CreateMapRect(24, 0, 8, 8);
		CreateMapRect(16, 8, 8, 8);
		CreateMapRect(24, 8, 8, 8);
		CreateMapRect(16, 16, 8, 8);
		CreateMapRect(24, 16, 8, 8);
		CreateMapRect(16, 24, 8, 8);
		CreateMapRect(24, 24, 8, 8);
		break;
	case 11:
		CreateMapRect(16, 16, 8, 8);
		CreateMapRect(24, 16, 8, 8);
		CreateMapRect(16, 24, 8, 8);
		CreateMapRect(24, 24, 8, 8);
		CreateMapRect(0, 16, 8, 8);
		CreateMapRect(8, 16, 8, 8);
		CreateMapRect(0, 24, 8, 8);
		CreateMapRect(8, 24, 8, 8);
		CreateMapRect(16, 0, 8, 8);
		CreateMapRect(24, 0, 8, 8);
		CreateMapRect(16, 8, 8, 8);
		CreateMapRect(24, 8, 8, 8);
		break;
	case 12:
		CreateMapRect(16, 16, 8, 8);
		CreateMapRect(24, 16, 8, 8);
		CreateMapRect(16, 24, 8, 8);
		CreateMapRect(24, 24, 8, 8);
		CreateMapRect(0, 16, 8, 8);
		CreateMapRect(8, 16, 8, 8);
		CreateMapRect(0, 24, 8, 8);
		CreateMapRect(8, 24, 8, 8);
		CreateMapRect(0, 0, 8, 8);
		CreateMapRect(8, 0, 8, 8);
		CreateMapRect(0, 8, 8, 8);
		CreateMapRect(8, 8, 8, 8);
		break;

	case 13:
		CreateMapRect(16, 16, 8, 8);
		CreateMapRect(24, 16, 8, 8);
		CreateMapRect(16, 24, 8, 8);
		CreateMapRect(24, 24, 8, 8);
		CreateMapRect(0, 16, 8, 8);
		CreateMapRect(8, 16, 8, 8);
		CreateMapRect(0, 24, 8, 8);
		CreateMapRect(8, 24, 8, 8);
		CreateMapRect(0, 0, 8, 8);
		CreateMapRect(8, 0, 8, 8);
		CreateMapRect(0, 8, 8, 8);
		CreateMapRect(8, 8, 8, 8);
		CreateMapRect(16, 0, 8, 8);
		CreateMapRect(24, 0, 8, 8);
		CreateMapRect(16, 8, 8, 8);
		CreateMapRect(24, 8, 8, 8);
		break;


	case 14:
		CreateMapRect(32, 0, 16, 16);
		break;
	case 15:
		CreateMapRect(48, 0, 16, 16);
		break;
	case 16:
		CreateMapRect(48, 16, 16, 16);
		break;
	case 17:
		CreateMapRect(32, 16, 16, 16);
		break;


	case 18:
		CreateMapRect(32, 0, 16, 16);
		CreateMapRect(48, 0, 16, 16);
		break;
	case 19:
		CreateMapRect(48, 0, 16, 16);
		CreateMapRect(48, 16, 16, 16);
		break;
	case 20:
		CreateMapRect(48, 16, 16, 16);
		CreateMapRect(32, 16, 16, 16);
		break;
	case 21:
		CreateMapRect(32, 0, 16, 16);
		CreateMapRect(32, 16, 16, 16);
		break;


	case 22:
		CreateMapRect(32, 0, 16, 16);
		CreateMapRect(48, 0, 16, 16);
		CreateMapRect(32, 16, 16, 16);
		break;
	case 23:
		CreateMapRect(32, 0, 16, 16);
		CreateMapRect(48, 0, 16, 16);
		CreateMapRect(48, 16, 16, 16);
		break;
	case 24:
		CreateMapRect(48, 16, 16, 16);
		CreateMapRect(32, 16, 16, 16);
		CreateMapRect(48, 0, 16, 16);
		break;
	case 25:
		CreateMapRect(48, 16, 16, 16);
		CreateMapRect(32, 16, 16, 16);
		CreateMapRect(32, 0, 16, 16);
		break;
	case 26:
		CreateMapRect(48, 16, 16, 16);
		CreateMapRect(32, 16, 16, 16);
		CreateMapRect(32, 0, 16, 16);
		CreateMapRect(48, 0, 16, 16);
		break;


	case 27:
		CreateMapRect(64, 0, 32, 32);
		break;
	case 28:
		CreateMapRect(96, 0, 32, 32);
		break;
	case 29:
		CreateMapRect(128, 0, 32, 32);
		break;
	case 30:
		CreateMapRect(160, 0, 32, 32);
		break;
	case 38:
		CreateMapRect(192, 0, 32, 32);
		break;
	default:
		break;
	}
	return false;
}

bool MapPiece::BeingCrash(bool flag2, RECT & rect, int dir, int x, int y, int powerLevel)
{
	bool flag = false, flag1 = true;
	RECT Rect, Rect1, BoomRect = { 0 };
	RectList*rp = rectlisthead->next;
	if (rp == NULL)
		return flag;
	if (flag2)
	{
		switch (dir)
		{
		case Dirction::up:
			BoomRect.left = x - 20;
			BoomRect.top = y;
			BoomRect.right = BoomRect.left + 56;
			BoomRect.bottom = BoomRect.top + 56;
			break;
		case Dirction::right:
			BoomRect.left = x - 40;
			BoomRect.top = y - 20;
			BoomRect.right = BoomRect.left + 56;
			BoomRect.bottom = BoomRect.top + 56;
			break;
		case Dirction::below:
			BoomRect.left = x - 20;
			BoomRect.top = y - 40;
			BoomRect.right = BoomRect.left + 56;
			BoomRect.bottom = BoomRect.top + 56;
			break;
		case Dirction::lift:
			BoomRect.left = x;
			BoomRect.top = y - 20;
			BoomRect.right = BoomRect.left + 56;
			BoomRect.bottom = BoomRect.top + 56;
			break;
		default:
			break;
		}
	}
	if (rp->rect->left < 32)
	{
		while (rp != NULL)
		{
			Rect1.left = (X + 1) * 64 + rp->rect->left * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &rect, &Rect1) || IntersectRect(&Rect, &BoomRect, &Rect1))
			{
				if (rp->last != NULL)
				{
					if (rp->next != NULL) {
						rp->last->next = rp->next;
						rp->next->last = rp->last;
					}
					else
						rp->last->next = NULL;
				}
				else if (rp->next != NULL)
				{
					rp->next->last = NULL;
					rectlisthead->next = rp->next;
				}
				else
				{
					rectlisthead->next = NULL;
				}
				delete rp;
				flag = true;
				if (flag1)
				{
					switch (dir)
					{
					case Dirction::up:
						BoomRect.left = x - 20;
						BoomRect.top = y;
						BoomRect.right = BoomRect.left + 56;
						BoomRect.bottom = BoomRect.top + 56;
						break;
					case Dirction::right:
						BoomRect.left = x - 40;
						BoomRect.top = y - 20;
						BoomRect.right = BoomRect.left + 56;
						BoomRect.bottom = BoomRect.top + 56;
						break;
					case Dirction::below:
						BoomRect.left = x - 20;
						BoomRect.top = y - 40;
						BoomRect.right = BoomRect.left + 56;
						BoomRect.bottom = BoomRect.top + 56;
						break;
					case Dirction::lift:
						BoomRect.left = x;
						BoomRect.top = y - 20;
						BoomRect.right = BoomRect.left + 56;
						BoomRect.bottom = BoomRect.top + 56;
						break;
					default:
						break;
					}
					flag1 = false;
				}
				rp = rectlisthead->next;
			}
			else
				rp = rp->next;
			if (rp == NULL)
				break;
		}
	}
	else if (rp->rect->left < 64)
	{
		//钢墙：普通子弹无法击穿，仅最高等级(PowerLevel>=3)可破坏
		while (rp != NULL)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 32) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &rect, &Rect1))
			{
				flag = true;
				if (powerLevel >= 3)
				{
					if (rp->last != NULL)
					{
						if (rp->next != NULL) {
							rp->last->next = rp->next;
							rp->next->last = rp->last;
						}
						else
							rp->last->next = NULL;
					}
					else if (rp->next != NULL)
					{
						rp->next->last = NULL;
						rectlisthead->next = rp->next;
					}
					else
					{
						rectlisthead->next = NULL;
					}
					delete rp;
					break; //顶级子弹只打掉一层钢墙
				}
				else
				{
					break; //普通子弹碰到钢墙直接停止
				}
			}
			else
				rp = rp->next;
			if (rp == NULL)
				break;
		}

	}
	else
	{
		return flag;
	}
	return flag;
}

int  MapPiece::PECrach(int dir, RECT&playerrect)
{
	RectList*rp = rectlisthead->next;
	RECT Rect, Rect1;
	int result = 0;
	if (rp == NULL)
		return result;
	switch (dir)
	{
	case Dirction::up: {
		if (rp->rect->left < 32)
		{
			while (rp != NULL)
			{
				Rect1.left = (X + 1) * 64 + rp->rect->left * 2;
				Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
				Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
				Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
				if (IntersectRect(&Rect, &playerrect, &Rect1))
				{
					if (Rect1.bottom > result)
						result = Rect1.bottom;
				}
				rp = rp->next;
				if (rp == NULL)
					break;
			}
		}
		else if (rp->rect->left < 64)
		{
			while (rp != NULL)
			{
				Rect1.left = (X + 1) * 64 + (rp->rect->left - 32) * 2;
				Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
				Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
				Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
				if (IntersectRect(&Rect, &playerrect, &Rect1))
				{
					if (Rect1.bottom > result)
						result = Rect1.bottom;
				}
				rp = rp->next;
				if (rp == NULL)
					break;
			}

		}
		else if (rp->rect->left >= 96 && rp->rect->left < 128)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 96) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &playerrect, &Rect1))
			{
				result = Rect1.bottom;

			}
		}
		else if (rp->rect->left >= 128 && rp->rect->left < 160)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 128) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &playerrect, &Rect1))
			{
				result = Rect1.bottom;

			}

		}
		else if (rp->rect->left >= 160 && rp->rect->left < 192)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 160) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &playerrect, &Rect1))
			{
				result = Rect1.bottom;

			}
		}
		else
		{
			return 0;
		}
		break;
	}
	case Dirction::right: {
		if (rp->rect->left < 32)
		{
			while (rp != NULL)
			{
				Rect1.left = (X + 1) * 64 + rp->rect->left * 2;
				Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
				Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
				Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
				if (IntersectRect(&Rect, &playerrect, &Rect1))
				{
					if (result>Rect1.left || result == 0)
						result = Rect1.left;
				}
				rp = rp->next;
				if (rp == NULL)
					break;
			}
		}
		else if (rp->rect->left < 64)
		{
			while (rp != NULL)
			{
				Rect1.left = (X + 1) * 64 + (rp->rect->left - 32) * 2;
				Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
				Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
				Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
				if (IntersectRect(&Rect, &playerrect, &Rect1))
				{
					if (result>Rect1.left || result == 0)
						result = Rect1.left;
				}
				rp = rp->next;
				if (rp == NULL)
					break;
			}

		}
		else if (rp->rect->left >= 96 && rp->rect->left < 128)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 96) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &playerrect, &Rect1))
			{
				result = Rect1.left;
			}
		}
		else if (rp->rect->left >= 128 && rp->rect->left < 160)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 128) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &playerrect, &Rect1))
			{
				result = Rect1.left;
			}

		}
		else if (rp->rect->left >= 160 && rp->rect->left < 192)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 160) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &playerrect, &Rect1))
			{
				result = Rect1.left;
			}
		}
		else
		{
			return 0;
		}
		break;
	}
	case Dirction::below: {
		if (rp->rect->left < 32)
		{
			while (rp != NULL)
			{
				Rect1.left = (X + 1) * 64 + rp->rect->left * 2;
				Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
				Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
				Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
				if (IntersectRect(&Rect, &playerrect, &Rect1))
				{
					if (result>Rect1.top || result == 0)
						result = Rect1.top;
				}
				rp = rp->next;
				if (rp == NULL)
					break;
			}
		}
		else if (rp->rect->left < 64)
		{
			while (rp != NULL)
			{
				Rect1.left = (X + 1) * 64 + (rp->rect->left - 32) * 2;
				Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
				Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
				Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
				if (IntersectRect(&Rect, &playerrect, &Rect1))
				{
					if (result>Rect1.top || result == 0)
						result = Rect1.top;
				}
				rp = rp->next;
				if (rp == NULL)
					break;
			}

		}
		else if (rp->rect->left >= 96 && rp->rect->left < 128)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 96) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &playerrect, &Rect1))
			{
				result = Rect1.top;
			}
		}
		else if (rp->rect->left >= 128 && rp->rect->left < 160)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 128) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &playerrect, &Rect1))
			{
				result = Rect1.top;
			}

		}
		else if (rp->rect->left >= 160 && rp->rect->left < 192)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 160) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &playerrect, &Rect1))
			{
				result = Rect1.top;
			}

		}
		else
		{
			return 0;
		}
		break;
	}
	case Dirction::lift: {
		if (rp->rect->left < 32)
		{
			while (rp != NULL)
			{
				Rect1.left = (X + 1) * 64 + rp->rect->left * 2;
				Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
				Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
				Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
				if (IntersectRect(&Rect, &playerrect, &Rect1))
				{
					if (Rect1.right > result)
						result = Rect1.right;
				}
				rp = rp->next;
				if (rp == NULL)
					break;
			}
		}
		else if (rp->rect->left < 64)
		{
			while (rp != NULL)
			{
				Rect1.left = (X + 1) * 64 + (rp->rect->left - 32) * 2;
				Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
				Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
				Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
				if (IntersectRect(&Rect, &playerrect, &Rect1))
				{
					if (Rect1.right > result)
						result = Rect1.right;
				}
				rp = rp->next;
				if (rp == NULL)
					break;
			}

		}
		else if (rp->rect->left >= 96 && rp->rect->left < 128)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 96) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &playerrect, &Rect1))
			{
				result = Rect1.right;
			}
		}
		else if (rp->rect->left >= 128 && rp->rect->left < 160)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 128) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &playerrect, &Rect1))
			{
				result = Rect1.right;
			}

		}
		else if (rp->rect->left >= 160 && rp->rect->left < 192)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 160) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &playerrect, &Rect1))
			{
				result = Rect1.right;
			}
		}
		else
		{
			return 0;
		}
		break;
	}
	default:
		break;
	}
	return result;
}
/*--------------------------------------------------------------------
Class MapPiece的方法到此结束
----------------------------------------------------------------------*/
