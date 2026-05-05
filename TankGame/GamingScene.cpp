#include "GamingScene.h"
#define EnemyNumberMAX 21
namespace GS {
	LPDIRECT3DSURFACE9 GrayRect = NULL;
	LPDIRECT3DSURFACE9 BlackRect = NULL;
	/*����*/
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
	/*����*/
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
	/*����*/
	Player player;
	Player2 player2;
	Player &Player1 = player;
	void ShowGameOver();

	/*���ߺ���*/
	int  Crash(int iswho, int x, int y, int speed, int dir, int shooter, unsigned long id, int, int powerLevel);
	void DrawMap();
	void CreateMapPiece();
	bool ReadMapInHD(string filename);
	bool ReadMapInHD(char * filename);
	bool WriteMapToHD(char * filename);
	void FillRect(RECT & rect, long l, long r, long t, long b);
	void ReadMap(int x, int y, RECT&rect1, RECT&rect2);//��ȡ��ͼ��Ϣ
	void AddUselessObj(unsigned long id);
	bool DelListNode(EnemyList*listhead, unsigned long id);//ɾ���ɹ�����true�����򷵻�false
	bool DelListNode(BulletList*listhead, unsigned long id);//ɾ���ɹ�����true�����򷵻�false
	bool DelListNode(BoomList*listhead, unsigned long id);//ɾ���ɹ�����true�����򷵻�false
	void DelUselessObj();
	void DrawNet();
	void ClearUselessObj();
	void CreateEnemy(int x, int y, int speed, int hp, int as, int grade, int dir);
	void CreateBoom(int x, int y, int whatboom, int Dir);
	void CreateAward(int x, int y, int type);
	bool DelListNode(AwardItemList*listhead, unsigned long id);
	void CheckAwardCollision();
	void DestroyAllEnemies();
//	int  MaxNumber(int m1, int m2, int m3, int m4, bool r1, bool r2, bool r3, bool r4);
//	int  MinNumber(int m1, int m2, int m3, int m4, bool r1, bool r2, bool r3, bool r4);
	void DIDA();
	void NewStage();
	void ReadNextMap();
	void StartNextStage();
	void RestartThisStage();
	/*���ߺ���*/
	BulletListHead bulletlisthead;//�ӵ�����ͷ
	EnemyListHead enemylisthead;//��������ͷ
	UselessObjHead uselessobjhead;//ʧЧ��������ͷ
	BoomListHead boomlisthead;//��ը����ͷ
	MapPieceListHead mappiecelisthead;
	AwardItemListHead awardlisthead;

	static unsigned long IDNumber = 0;
	int EnemyXY[EnemyNumberMAX][2];//����λ�������
	int Map[13][13]; //��һ����y�ᣬ�ڶ�����x��
}
using namespace GS;
/*--------------------------------------------------------------------
GamingScene�ķ���
----------------------------------------------------------------------*/
//������ʼ��
bool GamingScene::Init()
{
	//
	srand((unsigned)time(0));

	for (int i = 0; i < EnemyNumberMAX; i++)//��ʼ�����������
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
		ShowMessage("��ɫ-���� ��ʼ��ʧ�ܣ�");
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
		ShowMessage("��ɫ-���� ��ʼ��ʧ�ܣ�");
		return false;
	}
	d3dDev->ColorFill(GrayRect, NULL, D3DCOLOR_XRGB(110, 110, 110));
	d3dDev->ColorFill(BlackRect, NULL, D3DCOLOR_XRGB(69, 139, 116));

	Flag = LoadTexture(Resource::Texture::Flag, D3DCOLOR_XRGB(255, 255, 255));
	if (!Flag)
	{
		ShowMessage("װ�� ���� ����ʧ�ܣ�");
		return false;
	}
	Something = LoadTexture(Resource::Texture::Something, D3DCOLOR_XRGB(255, 255, 255));
	if (!Something)
	{
		ShowMessage("װ�� ���� ����ʧ��!");
		return false;
	}
	Tile = LoadTexture(Resource::Texture::Tile, D3DCOLOR_XRGB(4, 4, 4));
	if (!Tile)
	{
		ShowMessage("װ�� ש ����ʧ��!");
		return false;
	}
	Player_1 = LoadTexture(Resource::Texture::Player_1, D3DCOLOR_XRGB(0, 0, 0));
	if (!Player_1)
	{
		ShowMessage("װ�� ����� ����ʧ��!");
		return false;
	}
	Bullet_TXTTURE = LoadTexture(Resource::Texture::Bullet, D3DCOLOR_XRGB(4, 4, 4));
	if (!Bullet_TXTTURE)
	{
		ShowMessage("װ�� �ӵ� ����ʧ��!");
		return false;
	}
	Boom1 = LoadTexture(Resource::Texture::Boom1, D3DCOLOR_XRGB(0, 0, 0));
	if (!Boom1)
	{
		ShowMessage("װ�� ��ըһ ����ʧ��!");
		return false;
	}
	Boom2 = LoadTexture(Resource::Texture::Boom2, D3DCOLOR_XRGB(4, 4, 4));
	if (!Boom2)
	{
		ShowMessage("װ�� ��ը�� ����ʧ��!");
		return false;
	}
	Player_2 = LoadTexture(Resource::Texture::Player_2, D3DCOLOR_XRGB(0, 0, 0));
	if (!Player_2)
	{
		ShowMessage("װ�� ��Ҷ� ����ʧ��!");
		return false;
	}
	Award = LoadTexture(Resource::Texture::Award, D3DCOLOR_XRGB(234, 234, 234));
	if (!Award)
	{
		ShowMessage("װ�� ���� ����ʧ��!");
		return false;
	}
	Shield = LoadTexture(Resource::Texture::Shield, D3DCOLOR_XRGB(234, 234, 234));
	if (!Shield)
	{
		ShowMessage("װ�� ���� ����ʧ��!");
		return false;
	}
	GameOver = LoadTexture(Resource::Texture::GameOver, D3DCOLOR_XRGB(0, 0, 0));
	if (!GameOver)
	{
		ShowMessage("װ�� ��Ϸ���� ����ʧ��!");
		return false;
	}
	Enemy_TXTTURE = LoadTexture(Resource::Texture::Enemy, D3DCOLOR_XRGB(4, 4, 4));
	if (!Enemy_TXTTURE)
	{
		ShowMessage("װ�� ���� ����ʧ��!");
		return false;
	}
	Hole = LoadTexture(Resource::Texture::Hole, D3DCOLOR_XRGB(4, 4, 4));
	if (!Hole)
	{
		ShowMessage("װ�� �� ����ʧ��!");
		return false;
	}
	Number = LoadTexture(Resource::Texture::Number, D3DCOLOR_XRGB(255, 255, 255));
	if (!Number)
	{
		ShowMessage("װ�� ���� ����ʧ��!");
		return false;
	}

	//װ������
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
	int n = 0, i = 960;//���۴��ڴ�С����Ϸ�ֱ������ǲ���
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
	//����ѡ��ؿ�
	if (Global::DesignMap::NewMapName.length() != 0)
		ReadMapInHD(Global::DesignMap::NewMapName);
	else
		ReadMapInHD("stage" + std::to_string(NowLevel));
	//��ȡ��ͼ��Ϣ��������ͼ��
	CreateMapPiece();
	//�ж��Ƿ�˫����Ϸ
	IsDoublePlayer = Global::Home::selectedType == 1 ? true : false;
	//���ſ�ʼ����
	if(Global::Sound::SoundSwicth)
	Sound::Start->Play();
	player.Born();
	if (IsDoublePlayer)
		player2.Born();
	return 1;
}

void GamingScene::End()
{
	//�����ͼ��
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
	//�������
	EnemyList* ep = enemylisthead.next;
	while (ep != NULL)
	{
		AddUselessObj(ep->enemy->ID);
		ep = ep->next;
	}
	//����ӵ�
	BulletList*bp = bulletlisthead.next;
	while (bp != NULL)
	{
		AddUselessObj(bp->bullet->ID);
		bp = bp->next;
	}
	//�����ը
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
	//���õ�ͼ����
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
	if (Sound::Moving->IsSoundPlaying()) Sound::Moving->Stop();
	if (Sound::BGM->IsSoundPlaying()) Sound::BGM->Stop();
}
//��Ϸ��Ⱦ
void GamingScene::Render()
{
	d3dDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);

	/*��Ϸ�߿�*/
	RECT rect;
	FillRect(rect, 0, 1024, 32, 64);   //�ֱ��ʲ�Ϊ1024*960ʱ��Ҫ�޸�
	d3dDev->StretchRect(GrayRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, 0, 64, 64, 896);
	d3dDev->StretchRect(GrayRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, 896, 1024, 64, 896);
	d3dDev->StretchRect(GrayRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, 0, 1024, 896, 928);
	d3dDev->StretchRect(GrayRect, NULL, backBuffer, &rect, D3DTEXF_NONE);

	DrawNet();//��������ʽ��ɾ��
			  /*��Ϸ����*/
	spriteObj->Begin(D3DXSPRITE_ALPHABLEND);
	//侧栏旗帜图标（UI装饰）
	Sprite_Transform_Draw(Flag, 926, 704, 32, 32, 0, 1, 0, 2.0, D3DCOLOR_XRGB(255, 255, 255));
	//游戏区域内基地（老鹰）— 使用砖.bmp第6帧(正常)/第7帧(击毁)
	int eagleFrame = BaseDestroyed ? 6 : 5;
	Sprite_Transform_Draw(Tile, FlagGameX, FlagGameY, 32, 32, eagleFrame, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	//基地加固Shield效果
	if (!BaseDestroyed && (int)GetTickCount() < FortifyEndTime)
	{
		int shieldFrame = (GetTickCount() / 100) % 2;
		Sprite_Transform_Draw(Shield, FlagGameX, FlagGameY, 32, 32, shieldFrame, 1, 0, 2.0f, D3DCOLOR_XRGB(255, 255, 255));
	}
	//���һ����Ϣ
	Sprite_Transform_Draw(Something, 928, 512, 14, 14, 2, 6, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	Sprite_Transform_Draw(Something, 960, 512, 14, 14, 3, 6, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	Sprite_Transform_Draw(Something, 928, 544, 14, 14, 1, 6, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	//��Ҷ�����Ϣ
	if (IsDoublePlayer) {
		Sprite_Transform_Draw(Something, 928, 608, 14, 14, 4, 6, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		Sprite_Transform_Draw(Something, 960, 608, 14, 14, 3, 6, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		Sprite_Transform_Draw(Something, 928, 640, 14, 14, 1, 6, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	}
	//�����һ
	if(player.Alive)
	player.Draw();
	//����Ҷ�
	if (player2.Alive)
	{
		if (IsDoublePlayer)
			player2.Draw();
	}
	//����ͼ
	MapPieceList* mp = mappiecelisthead.next;
	while (mp != NULL)
	{
		mp->mappiece->Draw();
		mp = mp->next;
	}
	//	DrawMap();
	//		Sprite_Transform_Draw(Tile, 512, 832, 32, 32, 5, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
	//��Ⱦ�ӵ� �������ʧЧ�ӵ�
	BulletList*bp = bulletlisthead.next;
	while (bp != NULL)
	{
		bp->bullet->Draw();
		bp = bp->next;
	}

	//��Ⱦ����
	EnemyList *ep = enemylisthead.next;
	while (ep != NULL)
	{
		ep->enemy->Draw();
		ep = ep->next;
	}

	//��Ⱦ��ը
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
	//��Ϸ����
	if (GameOverFlag)
	{
		ShowGameOver();
	}
	DIDA();//����ʱ����Ϣ
}
//��Ϸ�߼�����
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
		//���������� �ж��Ƿ�ʤ��
		if (EnemyNumber <= 0)
		{
			StartNextStage();
		}
		//���һ
		if (player.Alive)
		{
			if (KEY_DOWN(Global::PlayerControl::Player1[0]) && !KEY_DOWN(Global::PlayerControl::Player1[3]) && !KEY_DOWN(Global::PlayerControl::Player1[2]))
			{
				player.Logic(Dirction::up);
				//up
			}
			if (KEY_DOWN(Global::PlayerControl::Player1[1]) && !KEY_DOWN(Global::PlayerControl::Player1[3]) && !KEY_DOWN(Global::PlayerControl::Player1[2]))
			{
				player.Logic(Dirction::below);
				//blow
			}
			if (KEY_DOWN(Global::PlayerControl::Player1[2]))
			{
				player.Logic(Dirction::lift);
				//left
			}
			if (KEY_DOWN(Global::PlayerControl::Player1[3]))
			{
				player.Logic(Dirction::right);
				//right
			}
			//������
			static int ShootTime = 10;
			if (ShowTime)
				ShootTime++;
			if (KEY_DOWN(Global::PlayerControl::Player1[4]) || KEY_DOWN(VK_NUMPAD0))
			{

				if (ShootTime > 10 / player.Attack_Speed)
				{
					player.Shoot(0, player.PowerLevel);
					ShootTime = 0;
				}

			}
		}
		//Player1 movement sound
		if (Global::Sound::SoundSwicth && player.Alive)
		{
			bool isMoving = KEY_DOWN(Global::PlayerControl::Player1[0]) || KEY_DOWN(Global::PlayerControl::Player1[1]) || KEY_DOWN(Global::PlayerControl::Player1[2]) || KEY_DOWN(Global::PlayerControl::Player1[3]);
			if (isMoving && !Sound::Moving->IsSoundPlaying())
				Sound::Moving->Play(0, DSBPLAY_LOOPING);
			else if (!isMoving && Sound::Moving->IsSoundPlaying())
			{
				Sound::Moving->Stop();
				Sound::Stop->Play();
			}
		}
		//��Ҷ�
		static int ShootTime2 = 10;
		if (player2.Alive)
		{
			if (IsDoublePlayer)
			{
				if (KEY_DOWN(Global::PlayerControl::Player2[0]) && !KEY_DOWN(Global::PlayerControl::Player2[3]) && !KEY_DOWN(Global::PlayerControl::Player2[2]))
				{
					player2.Logic(Dirction::up);
					//up
				}
				if (KEY_DOWN(Global::PlayerControl::Player2[1]) && !KEY_DOWN(Global::PlayerControl::Player2[3]) && !KEY_DOWN(Global::PlayerControl::Player2[2]))
				{
					player2.Logic(Dirction::below);
					//blow
				}
				if (KEY_DOWN(Global::PlayerControl::Player2[2]))
				{
					player2.Logic(Dirction::lift);
					//left
				}
				if (KEY_DOWN(Global::PlayerControl::Player2[3]))
				{
					player2.Logic(Dirction::right);
					//right
				}
				//������
				if (ShowTime)
					ShootTime2++;
				if (KEY_DOWN(Global::PlayerControl::Player2[4]))
				{
					if (ShootTime2 > 10 / player2.Attack_Speed)
					{
						player2.Shoot(0, player2.PowerLevel);
						ShootTime2 = 0;
					}
				}
			}
		}
		//�����ӵ��߼�
		BulletList*bp = bulletlisthead.next;
		while (bp != NULL)
		{
			bp->bullet->Logic();
			bp = bp->next;
		}
		//���µ����߼�
		EnemyList*ep = enemylisthead.next;
		bool enemiesFrozen = (int)GetTickCount() < FreezeEndTime;
		while (ep != NULL)
		{
			if (!enemiesFrozen)
				ep->enemy->Logic(ShowTime);
			ep = ep->next;
		}
		//�����µ���
		static int BornEnemy = 30;//���ɵ��˼�ʱ��
		static int NeedBornEnemy = 1;
		if (NeedBornEnemy)
			if (ShowTime)//ShowTime 100msһ��
				BornEnemy++;
		if (BornEnemy >= 10)//�����µĵ���
		{
			if (HaveBornEnemyNumber > 30)
				NeedBornEnemy = 0;
			if (BornEnemyMapPiece.size() != 0)
			{
				//���ݵ������ɵ���������ɵ���
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
		//���±�ը�߼�
		BoomList*boomp = boomlisthead.next;
		while (boomp != NULL)
		{
			boomp->boom->Logic();
			boomp = boomp->next;
		}
		//检测奖励碰撞
		CheckAwardCollision();
		//�ж����Ѫ���Ծ�����Ϸ״̬
		if (!IsDoublePlayer)
		{
			if (player.Health_Point <= 0)
			{
				player.Lift--;
				if (player.Lift <= 0)
					player.Alive = false;
				else
					Player1.Born();
				//������ը
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
					//������ը
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
						//������ը
						CreateBoom(player2.player.x, player2.player.y, 2, player2.Dir);
					player2.Born();
				}
			}
			if (!player.Alive && !player2.Alive)
			{
				GameOverFlag = true;
			}

		}
		//���ʧЧ����
		ClearUselessObj();
		//��ȡʱ����� 
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
GamingScene�ķ������˽���
----------------------------------------------------------------------*/


/*--------------------------------------------------------------------
GameScene�ķ���
----------------------------------------------------------------------*/
//��Ϸ��������
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
//ר�ŷ�����bullet::logic����ײ��⺯��
int  GS::Crash(int iswho, int x, int y, int speed, int dir, 
	           int shooter, unsigned long id, int movedmixel, int powerLevel) {
		//��ͼ�߽�
		static  RECT MapEdgeTop = { 0,0,1024,64 },
			MapEdgeBelow = { 0,896,1024,960 },
			MapEdgeLeft = { 0,0,64,960 },
			MapEdgeRight = { 896,0,1024,960 };
		//�ȼ���ӵ��Ƿ���ײ������
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
					//闪烁敌人掉落星星
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
		//�������ӵ��Ƿ��������
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
		//����ӵ���ײ
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
		return 2;//ĿǰΪ����״̬ ��ʽ��ӦΪ��ը2
		}
		}*/
		//����Ƿ���ײ��ש��
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
//��Ϸ��ͼ�滭����       
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
//������ͼ����
void GS::CreateMapPiece()
	{
		for (int x = 0; x < 13; x++)
			for (int y = 0; y < 13; y++) {

				if (Map[y][x] != 0) {
					//��¼���һ�ĳ����ص�
					if (Map[y][x] == 31)
					{
						BornPlayer1MapPiece.push_back(x);
						BornPlayer1MapPiece.push_back(y);
					}
					//��¼��Ҷ��ĳ����ص�
					if (Map[y][x] == 32)
					{
						BornPlayer2MapPiece.push_back(x);
						BornPlayer2MapPiece.push_back(y);
					}
					//��¼���˵ĳ����ص�
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
					//���µ�ͼ�����ӵ�����
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
//��ȡӲ���ϵ�ͼ����Ϣ
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
	//���ļ��ж�ȡ��ͼ��Ϣ
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++)
		{
			in.read(&buf[i][j], 1);
		}
	//ת��Ϊ��ǰ��ͼ
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
		//���ļ��ж�ȡ��ͼ��Ϣ
		for (int i = 0; i < 13; i++)
			for (int j = 0; j < 13; j++)
			{
				in.read(&buf[i][j], 1);
			}
		//ת��Ϊ��ǰ��ͼ
		for (int i = 0; i < 13; i++)
			for (int j = 0; j < 13; j++)
			{
				Map[i][j] = buf[i][j];
			}

		return 0;
	}
//д��ǰ��ͼ��Ϣ��Ӳ��
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

//������ը
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
//��������
void GS::CreateEnemy(int x, int y, int speed, int hp,
	                 int as, int grade, int dir)
	{
		int NewEnemyX = x;
		int NewEnemyY = y;

		//���ɵ��˶���
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
/*���ߺ���*/
//���RECT
void GS::FillRect(RECT&rect, long l = -1, long r = -1, 
	              long t = -1, long b = -1)
	{
		rect.left = l;
		rect.right = r;
		rect.top = t;
		rect.bottom = b;
	}
//��ȡ��ͼ��Ϣ
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
//����������
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
//���ʧЧ����
void GS::ClearUselessObj()
	{
		UselessObj*up = uselessobjhead.next;
		//���ʧЧ�ӵ�
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
	
		//���ʧЧ���˲����챬ը
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

		//���ʧЧ��ը
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
//ɾ������Ԫ��
void GS::DelUselessObj()
	{
		UselessObj*p = uselessobjhead.next;
		if (p != NULL)
			uselessobjhead.next = p->next;
		delete p;
	}

bool GS::DelListNode(EnemyList*listhead, unsigned long id)//ɾ���ɹ�����true�����򷵻�false
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

bool GS::DelListNode(BulletList*listhead, unsigned long id)//ɾ���ɹ�����true�����򷵻�false
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

bool GS::DelListNode(BoomList*listhead, unsigned long id)//ɾ���ɹ�����true�����򷵻�false
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
//����ʱ������
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
	//�����ͼ��
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
	//�������
	EnemyList* ep = enemylisthead.next;
	while (ep != NULL)
	{
		AddUselessObj(ep->enemy->ID);
		ep = ep->next;
	}
	//����ӵ�
	BulletList*bp = bulletlisthead.next;
	while (bp != NULL)
	{
		AddUselessObj(bp->bullet->ID);
		bp = bp->next;
	}
	//�����ը
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
	//���õ�ͼ����
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
GameScene�ķ������˽���
----------------------------------------------------------------------*/

//����AI
int* idiot(int state, bool cflag)
{
	static int a[2];
	if (cflag)
	{
		if ((rand() % 5) == 1)
		{
			a[0] = state;
			a[1] = 1;//����
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
//���˵Ĺ��캯��
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
}
//���˵���Ⱦ����
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
//�����߼�
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
	//�͵�ͼ�����ײ���
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


//��ը��Ⱦ����
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
//��ը�߼�
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
//��ը���캯��
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
��ҵķ���
----------------------------------------------------------------------*/
//��ʼ�������Ϣ
Player::Player()
{
	Health_Point = 1;//血量
	Speed = 5*64;
	Attack_Speed = 3;
	Dir = Dirction::up;
	Grade = 0;
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
//������
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
		bulletlisthead.next = new BulletList;//���ӵ������ӵ�����
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
//�����Ⱦ����
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
		if (KEY_DOWN(Global::PlayerControl::Player1[2]) || KEY_DOWN(Global::PlayerControl::Player1[3]) || KEY_DOWN(Global::PlayerControl::Player1[0]) || KEY_DOWN(Global::PlayerControl::Player1[1]))
			ChangeFrame = !ChangeFrame;
	}
	else {
		Sprite_Transform_Draw(Player_1, player.x, player.y, player.width, player.height,
			Dir * 8 + Grade * 2 + 1, player.columns, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		if (KEY_DOWN(Global::PlayerControl::Player1[2]) || KEY_DOWN(Global::PlayerControl::Player1[3]) || KEY_DOWN(Global::PlayerControl::Player1[0]) || KEY_DOWN(Global::PlayerControl::Player1[1]))
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
//����߼�����
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
	//�͵�ͼ�����ײ���
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
    //�͵��˵���ײ���
	EnemyList* ep = enemylisthead.next;
	while (ep != NULL)
	{
		EnemyRect.left = ep->enemy->player.x;
		EnemyRect.top = ep->enemy->player.y;
		EnemyRect.bottom = ep->enemy->player.y + 56;
		EnemyRect.right = ep->enemy->player.x + 56;
		if (IntersectRect(&Rect, &EnemyRect, &PlayerRect))
		{
			// �������˺�����ƶ�����
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
	//����Ҷ�����ײ���
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
//��ұ����д�������
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
//����������ɷ���
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

//��Ҷ�
//��ʼ����Ҷ���Ϣ
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
		if (KEY_DOWN(Global::PlayerControl::Player2[2]) || KEY_DOWN(Global::PlayerControl::Player2[3]) || KEY_DOWN(Global::PlayerControl::Player2[0]) || KEY_DOWN(Global::PlayerControl::Player2[1]))
			ChangeFrame = !ChangeFrame;
	}
	else {
		Sprite_Transform_Draw(Player_2, player.x, player.y, player.width, player.height,
			Dir * 8 + Grade * 2 + 1, player.columns, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		if (KEY_DOWN(Global::PlayerControl::Player2[2]) || KEY_DOWN(Global::PlayerControl::Player2[3]) || KEY_DOWN(Global::PlayerControl::Player2[0]) || KEY_DOWN(Global::PlayerControl::Player2[1]))
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
//��Ҷ��߼�����
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
	//�͵�ͼ�����ײ���
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
	//�͵��˵���ײ���
	EnemyList* ep = enemylisthead.next;
	while (ep != NULL)
	{
		EnemyRect.left = ep->enemy->player.x;
		EnemyRect.top = ep->enemy->player.y;
		EnemyRect.bottom = ep->enemy->player.y + 56;
		EnemyRect.right = ep->enemy->player.x + 56;
		if (IntersectRect(&Rect, &EnemyRect, &PlayerRect))
		{
			// �������˺�����ƶ�����
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
	//�����һ����ײ���
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
��ҵķ������˽���
----------------------------------------------------------------------*/

//�ӵ������캯��
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
//�ӵ��ƶ�����ײ��ⷽ��
bool Bullet::Logic()
{
	//��ײ���
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

    int result = Crash( 0,bullet.x, bullet.y, Speed, Dir,Shooter,ID, MovedPixel, PowerLevel);
	if (result == 1)
	{
		if(PowerLevel==0)
		AddUselessObj(ID);//��¼����ID��������
		//������ը
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
//�ӵ���Ⱦ����
bool Bullet::Draw()
{
	if (BoomFlag == 0) {
		switch (Dir)
		{
		case Dirction::up:
			Sprite_Transform_Draw(Bullet_TXTTURE, bullet.x, bullet.y,
				8, 8, 0, 4, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
			break;
		case Dirction::below:
			Sprite_Transform_Draw(Bullet_TXTTURE, bullet.x, bullet.y,
				8, 8, 2, 4, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
			break;
		case Dirction::lift:
			Sprite_Transform_Draw(Bullet_TXTTURE, bullet.x, bullet.y,
				8, 8, 3, 4, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
			break;
		case Dirction::right:
			Sprite_Transform_Draw(Bullet_TXTTURE, bullet.x, bullet.y,
				8, 8, 1, 4, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
			break;
		default:
			break;
		}
		if (PowerLevel == 3)
		{
			if (GetTickCount() > LastFrametime + 50)
			{
				FlickerFrame = FlickerFrame < 8 ? FlickerFrame + 1 : 0;
				LastFrametime = GetTickCount();
			}
			Sprite_Draw_Frame(Flicker[FlickerFrame], bullet.x - 392, bullet.y - 292, 0, 800, 600, 1);
		}
		return true;
	}
	else
	{
		return false;
	}

}

/*--------------------------------------------------------------------
Class MapPiece�ķ���
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
			Sprite_Transform_Draw(Tile, (X + 1) * 64 + (rp->rect->left-160)*2, (Y + 1) * 64 + rp->rect->top*2,
				rp->rect, 0, 1, 0, 2, 2, D3DCOLOR_XRGB(255, 255, 255));
		else
			Sprite_Transform_Draw(Tile, (X + 1) * 64 +( rp->rect->left-192)*2, (Y + 1) * 64 + rp->rect->top*2,
				rp->rect, 0, 1, 0, 2, 2, D3DCOLOR_XRGB(255, 255, 255));
		rp = rp->next;
	}
}
//������ͼ����
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
					rp = rectlisthead->next;
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
Class MapPiece�ķ������˽���
----------------------------------------------------------------------*/
