#include "GamingScene.h"
#define EnemyNumberMAX 21
namespace GS {
	LPDIRECT3DSURFACE9 GrayRect = NULL;
	LPDIRECT3DSURFACE9 BlackRect = NULL;
	/*纹理*/
	LPDIRECT3DTEXTURE9 Flag = NULL;
	LPDIRECT3DTEXTURE9 Something = NULL;
	LPDIRECT3DTEXTURE9 Tile = NULL;
	LPDIRECT3DTEXTURE9 Player_1 = NULL;
	LPDIRECT3DTEXTURE9 Player_1_L = NULL;
	LPDIRECT3DTEXTURE9 Player_2 = NULL;
	LPDIRECT3DTEXTURE9 Bullet_TXTTURE = NULL;
	LPDIRECT3DTEXTURE9 Bullet_TXTTURE_mb = NULL;

	LPDIRECT3DTEXTURE9 Enemy_EDG = NULL;
	LPDIRECT3DTEXTURE9 Enemy_RNG = NULL;
	LPDIRECT3DTEXTURE9 Enemy_KT = NULL;
	LPDIRECT3DTEXTURE9 Enemy_FNC = NULL;
	LPDIRECT3DTEXTURE9 Enemy_G2 = NULL;
	LPDIRECT3DTEXTURE9 Enemy_C9 = NULL;

	LPDIRECT3DTEXTURE9 Cup = NULL;
	LPDIRECT3DTEXTURE9 Award = NULL;
	LPDIRECT3DTEXTURE9 Boom1 = NULL;
	LPDIRECT3DTEXTURE9 Boom2 = NULL;
	LPDIRECT3DTEXTURE9 GameOver = NULL;
	LPDIRECT3DTEXTURE9 Shield = NULL;
	LPDIRECT3DTEXTURE9 Hole = NULL;
	LPDIRECT3DTEXTURE9 Number = NULL;
	LPDIRECT3DTEXTURE9 Flicker[9] = { NULL };
	/*变量*/
	vector<int> BornPlayer1MapPiece;
	vector<int> BornPlayer2MapPiece;
	vector<int> BornEnemyMapPiece;
	string NowMapName = "stage1";
	unsigned long lasttime = 0;
	int HaveBornEnemyNumber = 0;
	int StartTime = 0, NowTime, SurplusTime = 0;
	bool ShowTime = false;
	bool GameOverFlag = false;
	bool IsDoublePlayer = false;
	int SGOy = 960;
	int EnemyNumber = 30;
	/*对象*/
	Player player;
	Player2 player2;
	Player &Player1 = player;
	void ShowGameOver();

	/*工具函数*/
	int  Crash(int iswho, int x, int y, int speed, int dir, int shooter, unsigned long id, int);
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
//	int  MaxNumber(int m1, int m2, int m3, int m4, bool r1, bool r2, bool r3, bool r4);
//	int  MinNumber(int m1, int m2, int m3, int m4, bool r1, bool r2, bool r3, bool r4);
	void DIDA();
	void NewStage();
	void ReadNextMap();
	void StartNextStage();
	void RestartThisStage();
	/*工具函数*/
	BulletListHead bulletlisthead;//子弹链表头
	EnemyListHead enemylisthead;//敌人链表头
	UselessObjHead uselessobjhead;//失效对象链表头
	BoomListHead boomlisthead;//爆炸链表头
	MapPieceListHead mappiecelisthead;

	static unsigned long IDNumber = 0;
	int EnemyXY[EnemyNumberMAX][2];//敌人位置坐标表
	int Map[13][13]; //第一个是y轴，第二个是x轴
}
using namespace GS;
/*--------------------------------------------------------------------
GamingScene的方法
----------------------------------------------------------------------*/
//场景初始化
bool GamingScene::Init()
{
	//
	srand((unsigned)time(0));

	for (int i = 0; i < EnemyNumberMAX; i++)//初始化敌人坐标表
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
		ShowMessage("灰色-格子 初始化失败！");
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
		ShowMessage("黑色-格子 初始化失败！");
		return false;
	}
	d3dDev->ColorFill(GrayRect, NULL, D3DCOLOR_XRGB(110, 110, 110));
	d3dDev->ColorFill(BlackRect, NULL, D3DCOLOR_XRGB(69, 139, 116));

	Flag = LoadTexture(Resource::Texture::Flag, D3DCOLOR_XRGB(255, 255, 255));
	if (!Flag)
	{
		ShowMessage("装载 旗子 纹理失败！");
		return false;
	}
	Something = LoadTexture(Resource::Texture::Something, D3DCOLOR_XRGB(255, 255, 255));
	if (!Something)
	{
		ShowMessage("装载 杂项 纹理失败!");
		return false;
	}
	Tile = LoadTexture(Resource::Texture::Tile, D3DCOLOR_XRGB(4, 4, 4));
	if (!Tile)
	{
		ShowMessage("装载 砖 纹理失败!");
		return false;
	}
	Cup = LoadTexture(Resource::Texture::Cup, D3DCOLOR_XRGB(255, 255, 255));
	if (!Cup)
	{
		ShowMessage("装载 Cup 纹理失败!");
		return false;
	}

	Player_1 = LoadTexture(Resource::Texture::Player_1, D3DCOLOR_XRGB(255, 255, 255));
	if (!Player_1)
	{
		ShowMessage("装载 主玩家 纹理失败!");
		return false;
	}
	Player_1_L = LoadTexture(Resource::Texture::Player_1_L, D3DCOLOR_XRGB(255, 255, 255));
	if (!Player_1_L)
	{
		ShowMessage("装载 主玩家_L 纹理失败!");
		return false;
	}
	Bullet_TXTTURE= LoadTexture(Resource::Texture::Bullet, D3DCOLOR_XRGB(0, 0, 0));
	if (!Bullet_TXTTURE)
	{
		ShowMessage("装载 子弹 纹理失败!");
		return false;
	}

	Bullet_TXTTURE_mb = LoadTexture(Resource::Texture::Bullet_mb, D3DCOLOR_XRGB(255, 255, 255));
	if (!Bullet_TXTTURE)
	{
		ShowMessage("装载 面包 纹理失败!");
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
	Shield = LoadTexture(Resource::Texture::Shield, D3DCOLOR_XRGB(234, 234, 234));
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
	Enemy_EDG = LoadTexture(Resource::Texture::Enemy_EDG, D3DCOLOR_XRGB(255, 255, 255));
	if (!Enemy_EDG)
	{
		ShowMessage("装载 EDG 纹理失败!");
		return false;
	}
	Enemy_RNG = LoadTexture(Resource::Texture::Enemy_RNG, D3DCOLOR_XRGB(255, 255, 255));
	if (!Enemy_RNG)
	{
		ShowMessage("装载 RNG 纹理失败!");
		return false;
	}	
	Enemy_KT = LoadTexture(Resource::Texture::Enemy_KT, D3DCOLOR_XRGB(255, 255, 255));
	if (!Enemy_KT)
	{
		ShowMessage("装载 KT 纹理失败!");
		return false;
	}	
	Enemy_FNC = LoadTexture(Resource::Texture::Enemy_FNC, D3DCOLOR_XRGB(255, 255, 255));
	if (!Enemy_FNC)
	{
		ShowMessage("装载 FNC 纹理失败!");
		return false;
	}	
	Enemy_C9 = LoadTexture(Resource::Texture::Enemy_C9, D3DCOLOR_XRGB(255, 255, 255));
	if (!Enemy_C9)
	{
		ShowMessage("装载 C9 纹理失败!");
		return false;
	}	
	Enemy_G2 = LoadTexture(Resource::Texture::Enemy_G2, D3DCOLOR_XRGB(255, 255, 255));
	if (!Enemy_G2)
	{
		ShowMessage("装载 G2 纹理失败!");
		return false;
	}

	Hole = LoadTexture(Resource::Texture::Hole, D3DCOLOR_XRGB(4, 4, 4));
	if (!Hole)
	{
		ShowMessage("装载 孔 纹理失败!");
		return false;
	}
	Number = LoadTexture(Resource::Texture::Number, D3DCOLOR_XRGB(255, 255, 255));
	if (!Number)
	{
		ShowMessage("装载 数字 纹理失败!");
		return false;
	}

	//装载闪光
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
	int n = 0, i = 960;//无论窗口大小，游戏分辨率总是不变
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
	//在这选择关卡
	if (Global::DesignMap::NewMapName.length() != 0)
		ReadMapInHD(Global::DesignMap::NewMapName);
	else
		ReadMapInHD(NowMapName);
	//读取地图信息并创建地图块
	CreateMapPiece();
	//判断是否双人游戏
	IsDoublePlayer = Global::Home::selectedType == 1 ? true : false;
	//播放开始声音
	//if(Global::Sound::SoundSwicth)
	//Sound::BGM->Play();
	player.Born();
	if (IsDoublePlayer)
		player2.Born();
	return 1;
}

void GamingScene::End()
{
	//清除地图块
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
	//清除敌人
	EnemyList* ep = enemylisthead.next;
	while (ep != NULL)
	{
		AddUselessObj(ep->enemy->ID);
		ep = ep->next;
	}
	//清除子弹
	BulletList*bp = bulletlisthead.next;
	while (bp != NULL)
	{
		AddUselessObj(bp->bullet->ID);
		bp = bp->next;
	}
	//清除爆炸
	BoomList*boomp = boomlisthead.next;
	while (boomp != NULL)
	{
		AddUselessObj(boomp->boom->ID);
		boomp = boomp->next;
	}

	ClearUselessObj();
	//重置玩家数据
	player.Speed = 5 * 64;
	player.Attack_Speed = 5;
	player.Dir = Dirction::up;
	player.Grade = 3;
	player.player.x = 64 * 6;
	player.player.y = 64 * 13;
	player.BulletSpeed = 64 * 12;
	player.Lift = 1;
	player.Health_Point = 1;//玩家血量

	if (IsDoublePlayer)
	{
		player2.Speed = 5 * 64;
		player2.Attack_Speed = 9;
		player2.Dir = Dirction::up;
		player2.Grade = 3;
		player2.player.x = 64 * 6;
		player2.player.y = 64 * 13;
		player2.BulletSpeed = 64 * 12;
		player2.Lift = 1;
		player2.Health_Point = 1;//玩家血量

	}
	//重置地图变量
	SGOy = 960;
	GameOverFlag = false;
}
//游戏渲染
void GamingScene::Render()
{
	d3dDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255, 255, 255), 1.0f, 0);

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

//	DrawNet();//画网格，正式版删除
			  /*游戏内容*/
	spriteObj->Begin(D3DXSPRITE_ALPHABLEND);
	//Sprite_Transform_Draw(Flag, 926, 704, 32, 32, 0, 1, 0, 2.0, D3DCOLOR_XRGB(255, 255, 255));
	////玩家一的信息
	//Sprite_Transform_Draw(Something, 928, 512, 14, 14, 2, 6, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	//Sprite_Transform_Draw(Something, 960, 512, 14, 14, 3, 6, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	//Sprite_Transform_Draw(Something, 928, 544, 14, 14, 1, 6, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	//玩家二的信息
	//if (IsDoublePlayer) {
	//	Sprite_Transform_Draw(Something, 928, 608, 14, 14, 4, 6, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	//	Sprite_Transform_Draw(Something, 960, 608, 14, 14, 3, 6, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	//	Sprite_Transform_Draw(Something, 928, 640, 14, 14, 1, 6, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	//}
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
	//渲染子弹 并清除已失效子弹
	BulletList*bp = bulletlisthead.next;
	while (bp != NULL)
	{
		bp->bullet->Draw();
		bp = bp->next;
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
	//游戏结束
	if (GameOverFlag)
	{
		ShowGameOver();
	}
	DIDA();//产生时间信息
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
			if (KEY_DOWN(VK_UP) && !KEY_DOWN(VK_RIGHT) && !KEY_DOWN(VK_LEFT))
			{
				player.Logic(Dirction::up);
				//up
			}
			if (KEY_DOWN(VK_DOWN) && !KEY_DOWN(VK_RIGHT) && !KEY_DOWN(VK_LEFT))
			{
				player.Logic(Dirction::below);
				//blow
			}
			if (KEY_DOWN(VK_LEFT))
			{
				player.Logic(Dirction::lift);
				//left
			}
			if (KEY_DOWN(VK_RIGHT))
			{
				player.Logic(Dirction::right);
				//right
			}
			//玩家射击
			static int ShootTime = 10;
			if (ShowTime)
				ShootTime++;
			if (KEY_DOWN(0x58) || KEY_DOWN(VK_NUMPAD0))
			{

				if (ShootTime > 10 / player.Attack_Speed)
				{
					player.Shoot(0, Player1.PowerLevel);
					ShootTime = 0;
				}

			}
		}
		//玩家二
		static int ShootTime2 = 10;
		if (player2.Alive)
		{
			if (IsDoublePlayer)
			{
				if (KEY_DOWN(0x57) && !KEY_DOWN(0x44) && !KEY_DOWN(0x41))
				{
					player2.Logic(Dirction::up);
					//up
				}
				if (KEY_DOWN(0x53) && !KEY_DOWN(0x44) && !KEY_DOWN(0x41))
				{
					player2.Logic(Dirction::below);
					//blow
				}
				if (KEY_DOWN(0x41))
				{
					player2.Logic(Dirction::lift);
					//left
				}
				if (KEY_DOWN(0x44))
				{
					player2.Logic(Dirction::right);
					//right
				}
				//玩家射击
				if (ShowTime)
					ShootTime2++;
				if (KEY_DOWN(0x4A))
				{
					if (ShootTime2 > 10 / player2.Attack_Speed)
					{
						player2.Shoot(0, player2.PowerLevel);
						ShootTime2 = 0;
					}
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
		while (ep != NULL)
		{
			ep->enemy->Logic(ShowTime);
			ep = ep->next;
		}
		//生成新敌人
		static int BornEnemy = 30;//生成敌人记时器
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
				//根据敌人生成点来随机生成敌人
				int atbuf= rand() % (BornEnemyMapPiece.size() / 2);
				CreateEnemy((BornEnemyMapPiece.at(atbuf*2)+1) * 64,
					(BornEnemyMapPiece.at(atbuf*2+1)+1) * 64,
					5 * 64, 1, 1, rand() % 8, rand() % 4);
		//		CreateEnemy((BornEnemyMapPiece.at(2)+1) * 64, (BornEnemyMapPiece.at(3)+1) * 64, 5 * 64, 1, 1, rand() % 8, rand() % 4);
				HaveBornEnemyNumber++;

			}
			else
			{
				HaveBornEnemyNumber++;
				CreateEnemy(12 * 64, 12 * 64, 5 * 64, 1, 1, rand() % 8, rand() % 4);
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
		//清除失效对象
		ClearUselessObj();
		//读取时间完毕 
	}
	else
	{
		if (KEY_DOWN(VK_RETURN))
		{
			RestartThisStage();
			GameOverFlag = false;
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
	           int shooter, unsigned long id, int movedmixel) {
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
					CreateBoom(ep->enemy->player.x, ep->enemy->player.y, 2, ep->enemy->Dir);
					DelListNode(enemylisthead.next, ep->enemy->ID);
					EnemyNumber--;
					return 2;//销毁自己
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
		//检测子弹碰撞
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
				if (id != bp->bullet->ID)
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
		return 2;//目前为测试状态 正式版应为爆炸2
		}
		}*/
		//检测是否碰撞到砖块
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
						crashflag1 = mp->mappiece->BeingCrash(0, BulletRect, dir, x, y);
				mp = mp->next;
			}
		}
		else
		{
			while (mp != NULL)
			{
				if (X1 - 1 == mp->mappiece->X)
					if (Y1 - 1 == mp->mappiece->Y)
						crashflag1 = mp->mappiece->BeingCrash(0, BulletRect, dir, x, y);
				mp = mp->next;
			}
			mp = mappiecelisthead.next;
			while (mp != NULL)
			{
				if (X2 - 1 == mp->mappiece->X)
					if (Y2 - 1 == mp->mappiece->Y)
						crashflag2 = mp->mappiece->BeingCrash(crashflag1, BulletRect, dir, x, y);
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
						crashflag1 = mp->mappiece->BeingCrash(crashflag2, BulletRect, dir, x, y);
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
						crashflag2 = mp->mappiece->BeingCrash(crashflag1, BulletRect, dir, x, y);
				mp = mp->next;
			}

		}
		if (crashflag1 || crashflag2)
			return 1;
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
	}
//游戏地图绘画函数       
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

				case 27:Sprite_Transform_Draw(Tile, (i + 1) * 64, (j + 1) * 64,
					32, 32, 2, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

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
//创建地图精灵
void GS::CreateMapPiece()
	{
		for (int x = 0; x < 13; x++)
			for (int y = 0; y < 13; y++) {

				if (Map[y][x] != 0) {
					//记录玩家一的出生地点
					if (Map[y][x] == 31)
					{
						BornPlayer1MapPiece.push_back(x);
						BornPlayer1MapPiece.push_back(y);
					}
					//记录玩家二的出生地点
					if (Map[y][x] == 32)
					{
						BornPlayer2MapPiece.push_back(x);
						BornPlayer2MapPiece.push_back(y);
					}
					//记录敌人的出生地点
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
					//把新地图块链接到链表
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
//读取硬盘上地图的信息
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
/*工具函数*/
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
//画辅助网格
void GS::DrawNet()
	{
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
//清除失效对象
void GS::ClearUselessObj()
	{
		UselessObj*up = uselessobjhead.next;
		//清除失效子弹
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
	
		//清除失效敌人并创造爆炸
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

		//清除失效爆炸
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

	}
//
void GS::AddUselessObj(unsigned long id)
	{
		UselessObj*p = new UselessObj;
		p->next = uselessobjhead.next;
		p->ID = id;
		uselessobjhead.next = p;
	}
//删除链表元素
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
//产生时间脉冲
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
	//清除地图块
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
	//清除敌人
	EnemyList* ep = enemylisthead.next;
	while (ep != NULL)
	{
		AddUselessObj(ep->enemy->ID);
		ep = ep->next;
	}
	//清除子弹
	BulletList*bp = bulletlisthead.next;
	while (bp != NULL)
	{
		AddUselessObj(bp->bullet->ID);
		bp = bp->next;
	}
	//清除爆炸
	BoomList*boomp = boomlisthead.next;
	while (boomp != NULL)
	{
		AddUselessObj(boomp->boom->ID);
		boomp = boomp->next;
	}

	ClearUselessObj();
	//重置玩家数据
	player.Speed = 5 * 64;
	player.Attack_Speed = 5;
	player.Dir = Dirction::up;
	player.Grade = 3;
	player.player.x = 64 * 6;
	player.player.y = 64 * 13;
	player.BulletSpeed = 64 * 12;
	player.Lift = 1;
	player.Health_Point = 1;//玩家血量

	if (IsDoublePlayer)
	{
		player2.Speed = 5 * 64;
		player2.Attack_Speed = 5;
		player2.Dir = Dirction::up;
		player2.Grade = 3;
		player2.player.x = 64 * 6;
		player2.player.y = 64 * 13;
		player2.BulletSpeed = 64 * 12;
		player2.Lift = 1;
		player2.Health_Point = 1;//玩家血量

	}
	//重置地图变量
	SGOy = 960;
	GameOverFlag = false;
	EnemyNumber = 30;
	player.Alive = true;
	player.Born();
	player2.Alive = true;
	player2.Born();
	HaveBornEnemyNumber = 0;
}

void GS::ReadNextMap()
{
	//使当前地图文件名变为下一个文件名
	string sbuf=NowMapName.substr(5,NowMapName.size());
	int ibuf = atoi(sbuf.c_str());
	ibuf++;
	char cbuf[20];
	_itoa_s(ibuf, cbuf, 10);
	NowMapName.erase(5, NowMapName.size());
	NowMapName += cbuf;
	//
	if(!ReadMapInHD(NowMapName))
	{
		Game_ChangeScene(GAME_STATE::Home);
	}
	//
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
int* idiot(int state, bool cflag)
{
	int a[2];
	if (cflag)
	{
		if ((rand() % 5) == 1)
		{
			a[0] = state;
			a[1] = 1;//开炮
			return a;
		}
		a[0] = rand() % 4;
		a[1] = 0;
		return a;
	}
	if ((rand() % 100) == 1)
	{
		a[0] = state;
		a[1] = 1;
		return a;
	}
	if ((rand() % 60) == 13)
	{
		a[0] = rand() % 4;
		a[1] = 0;
		return a;
	}

	a[0] = state;
	a[1] = 0;
	return a;

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
}
//敌人的渲染方法
bool Enemy::Draw()
{
	switch (Grade)
	{
	case 0:
		Sprite_Transform_Draw(Enemy_EDG, player.x, player.y, 54, 55,
			0, player.columns, Dir*0.5*3.14159, 1.1636, D3DCOLOR_XRGB(255, 255, 255));
		break;
	case 1:
		Sprite_Transform_Draw(Enemy_RNG, player.x, player.y, 50, 50,
			0, player.columns, Dir*0.5*3.14159, 1.28, D3DCOLOR_XRGB(255, 255, 255));
		break;
	case 2:
		Sprite_Transform_Draw(Enemy_KT, player.x, player.y, 50, 50,
			0, player.columns, Dir*0.5*3.14159, 1.28, D3DCOLOR_XRGB(255, 255, 255));
		break;
	case 3:
		Sprite_Transform_Draw(Enemy_FNC, player.x, player.y, 50, 50,
			0, player.columns, Dir*0.5*3.14159, 1.28, D3DCOLOR_XRGB(255, 255, 255));
		break;
	case 4:
		Sprite_Transform_Draw(Enemy_G2, player.x, player.y, 50, 50,
			0, player.columns, Dir*0.5*3.14159, 1.28, D3DCOLOR_XRGB(255, 255, 255));
		break;
	case 5:
		Sprite_Transform_Draw(Enemy_C9, player.x, player.y, 50, 50,
			0, player.columns, Dir*0.5*3.14159, 1.28, D3DCOLOR_XRGB(255, 255, 255));

		break;
	case 6:
		Sprite_Transform_Draw(Enemy_RNG, player.x, player.y, 50, 50,
			0, player.columns, Dir*0.5*3.14159, 1.28, D3DCOLOR_XRGB(255, 255, 255));
		break;
	case 7:
		Sprite_Transform_Draw(Enemy_KT, player.x, player.y, 50, 50,
			0, player.columns, Dir*0.5*3.14159, 1.28, D3DCOLOR_XRGB(255, 255, 255));
		break;
	default:
		break;
	}
	return true;

}
//敌人逻辑
bool Enemy::Logic(bool st)
{
	int *a=idiot(Dir, CrashingFlag);
	CrashingFlag = false;
	int d = *a;
	if (*(a + 1) == 1)
		Shoot(2,PowerLevel);
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
	//和地图块的碰撞检测
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
玩家的方法
----------------------------------------------------------------------*/
//初始化玩家信息
Player::Player()
{
	Health_Point = 1;//玩家血量
	Speed = 5*64;
	Attack_Speed = 5;
	Dir = Dirction::up;
	Grade = 1;
	player.scaling = 2;
	player.columns = 8; 
	player.frame = 0;
	player.color= D3DCOLOR_XRGB(255, 255, 255);
	player.x = 64*6;
	player.y = 64*13;
	player.width = 28;
	player.height = 28;
	BulletSpeed = 64*12;
	FlickerFrame = 0;
	Lift = 99;
	PowerLevel = 0;
	Alive = true;
	FlashFlag = false;
}
//玩家射击
bool Player::Shoot(int shooter,int powlv) {
	Bullet*b = new Bullet(shooter,Player::player.x,Player::player.y,
		Player::BulletSpeed, Player::Dir,powlv);
	IDNumber++;
	b->ID = IDNumber;
	if (b == NULL)
		return false;
	BulletList*c = bulletlisthead.next;
	if (c == NULL)
	{
		bulletlisthead.next = new BulletList;//将子弹插入子弹链表
		bulletlisthead.next->bullet = b;
		bulletlisthead.next->next = c;
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
		return true;
}
//玩家渲染方法
bool Player::Draw()
{
	static int FlashTimes = 300;
	static int lasttime=GetTickCount();
	static bool ChangeFrame = false;
	Sprite_Transform_Draw(Number, 960, 544, 14, 14, Lift / 10, 10, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	Sprite_Transform_Draw(Number, 993, 544, 14, 14, Lift % 10, 10, 0, 2, D3DCOLOR_XRGB(255, 255, 255));

	if (ChangeFrame) {
		if(Dir==3)
		{ 
			Sprite_Transform_Draw(Player_1_L, player.x, player.y, 256, 256,
				0, player.columns, 0, 0.25, D3DCOLOR_XRGB(255, 255, 255));
		}
		else {
			Sprite_Transform_Draw(Player_1, player.x, player.y, 256, 256,
				0, player.columns, (Dir - 1)*3.14159*0.5, 0.25, D3DCOLOR_XRGB(255, 255, 255));
		}
		if (KEY_DOWN(VK_LEFT) || KEY_DOWN(VK_RIGHT) || KEY_DOWN(VK_UP) || KEY_DOWN(VK_DOWN))
			ChangeFrame = !ChangeFrame;
	}
	else {
		if (Dir == 3)
		{
			Sprite_Transform_Draw(Player_1_L, player.x, player.y, 256, 256,
				0, player.columns, 0, 0.25, D3DCOLOR_XRGB(255, 255, 255));
		}
		else {
			Sprite_Transform_Draw(Player_1, player.x, player.y, 256, 256,
				0, player.columns, (Dir - 1) * 3.14159*0.5, 0.25, D3DCOLOR_XRGB(255, 255, 255));
		}
		if (KEY_DOWN(VK_LEFT) || KEY_DOWN(VK_RIGHT) || KEY_DOWN(VK_UP) || KEY_DOWN(VK_DOWN))
			ChangeFrame = !ChangeFrame;
	}
	if (FlashFlag)
	{
		if (GetTickCount() > lasttime + 50)
		{
			FlickerFrame = FlickerFrame < 8 ? FlickerFrame + 1 : 0;
			lasttime = GetTickCount();
		}
		Sprite_Draw_Frame(Flicker[FlickerFrame], player.x - 372, player.y - 272, 0, 800, 600, 1);
		FlashTimes--;
		if (FlashTimes == 0)
		{
			FlashTimes = 300;
			FlashFlag = false;
		}
	}
	return false;
}
//玩家逻辑方法
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
	//和地图块的碰撞检测
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
			// 碰到敌人后回退移动距离
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
//玩家被击中处理方法
bool Player::GetHurt(int power)
{
	Health_Point--;
	if (Health_Point == 0)
		return false;
	else
		true;
}
//玩家重新生成方法
void Player::Born()
{
	Health_Point = 1;
	FlashFlag = true;
	if (BornPlayer1MapPiece.size() != 0)
	{
		int atbuf = rand() % (BornPlayer1MapPiece.size() / 2);
		player.x = (BornPlayer1MapPiece.at(atbuf*2) + 1) * 64;
		player.y = (BornPlayer1MapPiece.at(atbuf*2 + 1) + 1) * 64;
	}
	else
	{

	}
}

//玩家二
//初始化玩家二信息
Player2::Player2()
{
	Health_Point = 1;//玩家血量
	Speed = 5 * 64;
	Attack_Speed = 5;
	Dir = Dirction::up;
	Grade = 3;
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

}
//玩家二渲染方法
bool Player2::Draw()
{
	static int FlashTimes = 300;
	static int lasttime = GetTickCount();
	static bool ChangeFrame = false;
	Sprite_Transform_Draw(Number, 960, 640, 14, 14, Lift / 10, 10, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	Sprite_Transform_Draw(Number, 992, 640, 14, 14, Lift % 10, 10, 0, 2, D3DCOLOR_XRGB(255, 255, 255));

	if (ChangeFrame) {
		Sprite_Transform_Draw(Player_2, player.x, player.y, player.width, player.height,
			Dir * 8 + Grade * 2, player.columns, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		if (KEY_DOWN(0x41) || KEY_DOWN(0x44) || KEY_DOWN(0x57) || KEY_DOWN(0x53))
			ChangeFrame = !ChangeFrame;
	}
	else {
		Sprite_Transform_Draw(Player_2, player.x, player.y, player.width, player.height,
			Dir * 8 + Grade * 2 + 1, player.columns, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		if (KEY_DOWN(0x41) || KEY_DOWN(0x44) || KEY_DOWN(0x57) || KEY_DOWN(0x53))
			ChangeFrame = !ChangeFrame;
	}
	if (FlashFlag)
	{
		if (GetTickCount() > lasttime + 50)
		{
			FlickerFrame = FlickerFrame < 8 ? FlickerFrame + 1 : 0;
			lasttime = GetTickCount();
		}
		Sprite_Draw_Frame(Flicker[FlickerFrame], player.x - 372, player.y - 272, 0, 800, 600, 1);
		FlashTimes--;
		if (FlashTimes == 0)
		{
			FlashTimes = 300;
			FlashFlag = false;
		}
	}
	return false;

}
//玩家二逻辑方法
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
	//和地图块的碰撞检测
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
			// 碰到敌人后回退移动距离
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

//子弹对象构造函数
Bullet::Bullet(int shooter,int x, int y, int S, int D,int powlv) :Speed(S), Dir(D),Shooter(shooter)
{
	BoomFlag = 0;
	PowerLevel = powlv;
	bullet.width = 16;
	bullet.height = 16;
	FlickerFrame = 0;
	LastFrametime = GetTickCount();
	switch (D)
	{
	case Dirction::up:
		bullet.x = x + 20;
		bullet.y = y;
		break;
	case Dirction::below:
		bullet.x = x+20;
		bullet.y = y+40;
		break;
	case Dirction::lift:
		bullet.x = x;
		bullet.y = y+20;
		break;
	case Dirction::right:
		bullet.x = x + 40;
		bullet.y = y + 20;
		break;
	default:
		break;
	}
}
//子弹移动和碰撞检测方法
bool Bullet::Logic()
{
	//碰撞检测
	 double srtime = GetTickCount() - lasttime;
	MovedPixel = Speed*srtime / 1000;
	switch (Dir)
	{
	case Dirction::up:
		bullet.y = bullet.y- MovedPixel;
		break;
	case Dirction::below:
		bullet.y = bullet.y + MovedPixel;
		break;
	case Dirction::lift:
		bullet.x = bullet.x- MovedPixel;
		break;
	case Dirction::right:
		bullet.x = bullet.x+ MovedPixel;
		break;
	default:
		break;
	}

    int result = Crash( 0,bullet.x, bullet.y, Speed, Dir,Shooter,ID, MovedPixel);
	if (result == 1)
	{
		if(PowerLevel==0)
		AddUselessObj(ID);//记录对象ID用于销毁
		//创建爆炸s
		CreateBoom(bullet.x-20, bullet.y-20, 1, Dir);
	}
	if (result == 2)
	{
		//if (PowerLevel == 0)
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
		if (Shooter == 2) {
			Sprite_Transform_Draw(Bullet_TXTTURE, bullet.x, bullet.y,
				8, 8, Dir, 4, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		}
		else
		{
			Sprite_Transform_Draw(Bullet_TXTTURE_mb, bullet.x, bullet.y,
				170, 100, 0, 1, (Dir + 1)*3.14159*0.5, 0.25, D3DCOLOR_XRGB(255, 255, 255));
		}
		if (PowerLevel != 0)
		{
			if (GetTickCount() > LastFrametime + 50)
			{
				FlickerFrame = FlickerFrame < 8 ? FlickerFrame + 1 : 0;
				LastFrametime = GetTickCount();
			}
	
			Sprite_Transform_Draw(Flicker[FlickerFrame], bullet.x - 98, bullet.y - 73,
				800, 600, 0, 1, 0, 0.25, D3DCOLOR_XRGB(255, 255, 255));
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
			Sprite_Transform_Draw(Tile, (X + 1) * 64 + (rp->rect->left-96)*2, (Y + 1) * 64 + rp->rect->top*2,
				rp->rect, 0, 1, 0, 2, 2, D3DCOLOR_XRGB(255, 255, 255));
		else if (rp->rect->left<160)
			Sprite_Transform_Draw(Tile, (X + 1) * 64 + (rp->rect->left-128)*2, (Y + 1) * 64 + rp->rect->top*2,
				rp->rect, 0, 1, 0, 2, 2, D3DCOLOR_XRGB(255, 255, 255));
		else if (rp->rect->left<192)

		Sprite_Transform_Draw(Cup, (X + 1) * 64 + (rp->rect->left - 160) * 2, (Y + 1) * 64 + rp->rect->top * 2,
			64, 64, 0, 1, 0, 1, D3DCOLOR_XRGB(255, 255, 255));
		else
			Sprite_Transform_Draw(Tile, (X + 1) * 64 +( rp->rect->left-192)*2, (Y + 1) * 64 + rp->rect->top*2,
				rp->rect, 0, 1, 0, 2, 2, D3DCOLOR_XRGB(255, 255, 255));
		rp = rp->next;
	}
}
//创建地图方框
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

bool MapPiece::BeingCrash(bool flag2, RECT & rect, int dir, int x, int y)
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
		while (rp != NULL)
		{
			Rect1.left = (X + 1) * 64 + (rp->rect->left - 32) * 2;
			Rect1.top = (Y + 1) * 64 + rp->rect->top * 2;
			Rect1.bottom = Rect1.top + (rp->rect->bottom - rp->rect->top) * 2;
			Rect1.right = Rect1.left + (rp->rect->right - rp->rect->left) * 2;
			if (IntersectRect(&Rect, &rect, &Rect1))
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
				rp = rectlisthead->next;
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
