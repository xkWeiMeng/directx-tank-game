#include "HomeScene.h"
#include "Global.h"
#include "DirectX.h"
#include "GameMain.h"
#include <cmath>
#include <cstdlib>
#include <ctime>

using namespace Global;
using namespace Global::Window;

// ============================================================
// Attract Mode — 自包含AI对战演示（增强视觉版）
// ============================================================
namespace HS {
	enum { DIR_UP = 0, DIR_RIGHT = 1, DIR_DOWN = 2, DIR_LEFT = 3 };

	// ---------- 配置 ----------
	static const int MAX_TANKS = 10;
	static const int MAX_BULLETS = 40;
	static const int MAX_BOOMS = 30;
	static const int MAX_SPARKS = 60;  // 火花粒子
	static const int GAME_LEFT = 64;
	static const int GAME_TOP = 64;
	static const int GAME_RIGHT = 896;
	static const int GAME_BOTTOM = 896;
	static const int CELL = 64;
	static const int TANK_SIZE = 56;

	// ---------- 数据结构 ----------
	struct ATank {
		float x, y;
		int dir, grade, team, hp;
		float speed;
		bool alive;
		bool moveFrame;
		int lastShootTime;
		int shootInterval;
		int lastDirChangeTime;
		bool hasShield; // 出生无敌
		int shieldEndTime;
	};

	struct ABullet {
		float x, y;
		float velX, velY;
		int dir, team;
		bool alive;
	};

	struct ABoom {
		float x, y;
		int startTime;
		bool alive;
		int type; // 1=小, 2=大
	};

	// 火花粒子（击中/爆炸时飞溅）
	struct Spark {
		float x, y, vx, vy;
		int life, maxLife;
		bool alive;
		D3DCOLOR color;
	};

	static ATank tanks[MAX_TANKS];
	static ABullet bullets[MAX_BULLETS];
	static ABoom booms[MAX_BOOMS];
	static Spark sparks[MAX_SPARKS];
	static int Map[13][13];
	static DWORD lastFrameTime = 0;
	static DWORD battleStartTime = 0;
	static const DWORD BATTLE_TIMEOUT = 30000;
	static int screenFlash = 0; // 击杀闪屏倒计时（毫秒）
	static int kills0 = 0, kills1 = 0; // 两队击杀数

	// 纹理指针
	static LPDIRECT3DTEXTURE9 pTile = NULL;
	static LPDIRECT3DTEXTURE9 pPlayer1 = NULL;
	static LPDIRECT3DTEXTURE9 pPlayer2 = NULL;
	static LPDIRECT3DTEXTURE9 pEnemy = NULL;
	static LPDIRECT3DTEXTURE9 pBullet = NULL;
	static LPDIRECT3DTEXTURE9 pBoom1 = NULL;
	static LPDIRECT3DTEXTURE9 pBoom2 = NULL;
	static LPDIRECT3DTEXTURE9 pShield = NULL;
	static LPDIRECT3DTEXTURE9 pOverlayTex = NULL;
	static LPDIRECT3DSURFACE9 pGray = NULL;

	// ---------- 火花系统 ----------
	static void SpawnSparks(float sx, float sy, int count, D3DCOLOR baseColor)
	{
		for (int n = 0; n < count; n++)
		{
			for (int i = 0; i < MAX_SPARKS; i++)
			{
				if (!sparks[i].alive)
				{
					sparks[i].alive = true;
					sparks[i].x = sx;
					sparks[i].y = sy;
					float angle = (float)(rand() % 360) * 3.14159f / 180.0f;
					float spd = 100.0f + (float)(rand() % 300);
					sparks[i].vx = cosf(angle) * spd;
					sparks[i].vy = sinf(angle) * spd;
					sparks[i].maxLife = 200 + rand() % 300;
					sparks[i].life = sparks[i].maxLife;
					sparks[i].color = baseColor;
					break;
				}
			}
		}
	}

	// ---------- 地图生成（更密集） ----------
	static void GenerateMap()
	{
		memset(Map, 0, sizeof(Map));
		for (int y = 0; y < 13; y++)
		{
			for (int x = 0; x < 13; x++)
			{
				// 中间留空（但窄通道）
				if (x == 6 && y == 6) continue;

				int r = rand() % 100;
				if (r < 35)
					Map[y][x] = 13;   // 砖块（35%）
				else if (r < 42)
					Map[y][x] = 26;   // 钢块（7%）
				else if (r < 48)
					Map[y][x] = 27;   // 草地（6%）
				else if (r < 52)
					Map[y][x] = 28;   // 水（4%）
			}
		}
		// 确保一些对称结构增加美感
		for (int y = 0; y < 13; y++)
		{
			for (int x = 0; x < 6; x++)
			{
				if (rand() % 3 == 0)
					Map[y][12 - x] = Map[y][x];
			}
		}
		// 清除坦克出生点及相邻格子，防止卡墙
		// 队伍1出生格: (0,0),(3,0),(6,0),(9,0),(12,0)
		// 队伍2出生格: (0,12),(3,12),(6,12),(9,12),(12,12)
		int spawnCells[10][2] = {
			{0,0},{3,0},{6,0},{9,0},{12,0},
			{0,12},{3,12},{6,12},{9,12},{12,12}
		};
		for (int s = 0; s < 10; s++)
		{
			int sx = spawnCells[s][0], sy = spawnCells[s][1];
			// 清除2x2区域（坦克56px接近2格宽）
			for (int dy = 0; dy <= 1; dy++)
				for (int dx = 0; dx <= 1; dx++)
				{
					int cx = sx + dx, cy = sy + dy;
					if (cx >= 0 && cx < 13 && cy >= 0 && cy < 13)
						Map[cy][cx] = 0;
				}
		}
	}

	// ---------- 初始化战斗 ----------
	static void InitBattle()
	{
		srand((unsigned)time(NULL) ^ GetTickCount());
		GenerateMap();
		battleStartTime = GetTickCount();
		screenFlash = 0;
		kills0 = kills1 = 0;

		// 队伍1: 5辆坦克在上方（使用 Player_1/Player_2 纹理）
		int team1Pos[5][2] = { {0,0}, {3,0}, {6,0}, {9,0}, {12,0} };
		// 队伍2: 5辆坦克在下方（使用 Enemy 纹理）
		int team2Pos[5][2] = { {0,12}, {3,12}, {6,12}, {9,12}, {12,12} };

		for (int i = 0; i < 5; i++)
		{
			ATank &t1 = tanks[i];
			t1.x = (float)((team1Pos[i][0] + 1) * CELL);
			t1.y = (float)((team1Pos[i][1] + 1) * CELL);
			t1.dir = DIR_DOWN;
			t1.grade = rand() % 4;
			t1.team = 0;
			t1.hp = 2 + rand() % 2;
			t1.speed = 160.0f + (float)(rand() % 100);
			t1.alive = true;
			t1.moveFrame = false;
			t1.lastShootTime = GetTickCount() + rand() % 1000;
			t1.shootInterval = 800 + rand() % 1500;
			t1.lastDirChangeTime = GetTickCount();
			t1.hasShield = true;
			t1.shieldEndTime = GetTickCount() + 2000;

			ATank &t2 = tanks[i + 5];
			t2.x = (float)((team2Pos[i][0] + 1) * CELL);
			t2.y = (float)((team2Pos[i][1] + 1) * CELL);
			t2.dir = DIR_UP;
			t2.grade = rand() % 4;
			t2.team = 1;
			t2.hp = 2 + rand() % 2;
			t2.speed = 160.0f + (float)(rand() % 100);
			t2.alive = true;
			t2.moveFrame = false;
			t2.lastShootTime = GetTickCount() + rand() % 1000;
			t2.shootInterval = 800 + rand() % 1500;
			t2.lastDirChangeTime = GetTickCount();
			t2.hasShield = true;
			t2.shieldEndTime = GetTickCount() + 2000;
		}

		for (int i = 0; i < MAX_BULLETS; i++) bullets[i].alive = false;
		for (int i = 0; i < MAX_BOOMS; i++) booms[i].alive = false;
		for (int i = 0; i < MAX_SPARKS; i++) sparks[i].alive = false;
	}

	// ---------- 辅助 ----------
	static bool IsBlocking(int gx, int gy)
	{
		if (gx < 0 || gx >= 13 || gy < 0 || gy >= 13) return true;
		int v = Map[gy][gx];
		if (v >= 1 && v <= 26) return true;
		if (v == 28) return true;
		return false;
	}

	static void CreateBoom(float bx, float by, int type)
	{
		for (int i = 0; i < MAX_BOOMS; i++)
		{
			if (!booms[i].alive)
			{
				booms[i].x = bx;
				booms[i].y = by;
				booms[i].type = type;
				booms[i].startTime = GetTickCount();
				booms[i].alive = true;
				break;
			}
		}
	}

	static void CreateBullet(float bx, float by, int dir, int team)
	{
		for (int i = 0; i < MAX_BULLETS; i++)
		{
			if (!bullets[i].alive)
			{
				bullets[i].x = bx;
				bullets[i].y = by;
				bullets[i].dir = dir;
				bullets[i].team = team;
				bullets[i].alive = true;
				float spd = 650.0f;
				switch (dir)
				{
				case DIR_UP:    bullets[i].velX = 0;    bullets[i].velY = -spd; break;
				case DIR_DOWN:  bullets[i].velX = 0;    bullets[i].velY = spd;  break;
				case DIR_LEFT:  bullets[i].velX = -spd;  bullets[i].velY = 0;   break;
				case DIR_RIGHT: bullets[i].velX = spd;   bullets[i].velY = 0;   break;
				}
				break;
			}
		}
	}

	// 简单追踪AI：概率朝向最近的敌方坦克
	static int DirToward(float fromX, float fromY, float toX, float toY)
	{
		float dx = toX - fromX;
		float dy = toY - fromY;
		if (fabs(dx) > fabs(dy))
			return dx > 0 ? DIR_RIGHT : DIR_LEFT;
		else
			return dy > 0 ? DIR_DOWN : DIR_UP;
	}

	static int FindNearestEnemy(int tankIdx)
	{
		ATank &me = tanks[tankIdx];
		float bestDist = 999999;
		int bestIdx = -1;
		for (int j = 0; j < MAX_TANKS; j++)
		{
			if (j == tankIdx || !tanks[j].alive || tanks[j].team == me.team) continue;
			float d = fabs(tanks[j].x - me.x) + fabs(tanks[j].y - me.y);
			if (d < bestDist) { bestDist = d; bestIdx = j; }
		}
		return bestIdx;
	}

	// ---------- 更新逻辑 ----------
	static void UpdateBattle()
	{
		DWORD now = GetTickCount();
		float dt = (float)(now - lastFrameTime) / 1000.0f;
		if (dt > 0.05f) dt = 0.05f;
		lastFrameTime = now;

		// 闪屏衰减
		if (screenFlash > 0)
		{
			screenFlash -= (int)(dt * 1000);
			if (screenFlash < 0) screenFlash = 0;
		}

		// 检查胜负/超时
		int alive0 = 0, alive1 = 0;
		for (int i = 0; i < MAX_TANKS; i++)
		{
			if (tanks[i].alive)
			{
				if (tanks[i].team == 0) alive0++;
				else alive1++;
			}
		}
		if (alive0 == 0 || alive1 == 0 || (now - battleStartTime > BATTLE_TIMEOUT))
		{
			InitBattle();
			lastFrameTime = now;
			return;
		}

		// 更新坦克
		for (int i = 0; i < MAX_TANKS; i++)
		{
			ATank &t = tanks[i];
			if (!t.alive) continue;

			// 无敌检测
			if (t.hasShield && (int)now >= t.shieldEndTime)
				t.hasShield = false;

			// AI：70%概率追踪最近敌人，30%随机
			if (rand() % 60 == 0 || (now - t.lastDirChangeTime > 1500 + (DWORD)(rand() % 1500)))
			{
				int nearIdx = FindNearestEnemy(i);
				if (nearIdx >= 0 && rand() % 10 < 7)
					t.dir = DirToward(t.x, t.y, tanks[nearIdx].x, tanks[nearIdx].y);
				else
					t.dir = rand() % 4;
				t.lastDirChangeTime = now;
			}

			// 移动
			float newX = t.x, newY = t.y;
			switch (t.dir)
			{
			case DIR_UP:    newY -= t.speed * dt; break;
			case DIR_DOWN:  newY += t.speed * dt; break;
			case DIR_LEFT:  newX -= t.speed * dt; break;
			case DIR_RIGHT: newX += t.speed * dt; break;
			}
			if (newX < (float)GAME_LEFT) newX = (float)GAME_LEFT;
			if (newY < (float)GAME_TOP) newY = (float)GAME_TOP;
			if (newX > (float)(GAME_RIGHT - TANK_SIZE)) newX = (float)(GAME_RIGHT - TANK_SIZE);
			if (newY > (float)(GAME_BOTTOM - TANK_SIZE)) newY = (float)(GAME_BOTTOM - TANK_SIZE);

			bool blocked = false;
			{
				int gx1 = ((int)newX - GAME_LEFT) / CELL;
				int gy1 = ((int)newY - GAME_TOP) / CELL;
				int gx2 = ((int)(newX + TANK_SIZE - 1) - GAME_LEFT) / CELL;
				int gy2 = ((int)(newY + TANK_SIZE - 1) - GAME_TOP) / CELL;
				for (int gy = gy1; gy <= gy2 && !blocked; gy++)
					for (int gx = gx1; gx <= gx2 && !blocked; gx++)
						if (IsBlocking(gx, gy)) blocked = true;
			}

			if (!blocked)
			{
				for (int j = 0; j < MAX_TANKS; j++)
				{
					if (j == i || !tanks[j].alive) continue;
					if (newX < tanks[j].x + TANK_SIZE && newX + TANK_SIZE > tanks[j].x &&
						newY < tanks[j].y + TANK_SIZE && newY + TANK_SIZE > tanks[j].y)
					{
						blocked = true;
						t.dir = rand() % 4;
						t.lastDirChangeTime = now;
						break;
					}
				}
			}

			if (!blocked)
			{
				t.x = newX;
				t.y = newY;
				t.moveFrame = !t.moveFrame;
			}
			else if (rand() % 2 == 0)
			{
				t.dir = rand() % 4;
				t.lastDirChangeTime = now;
			}

			// 射击（更频繁）
			if ((int)(now - t.lastShootTime) > t.shootInterval)
			{
				t.lastShootTime = now;
				float bx = t.x, by = t.y;
				switch (t.dir)
				{
				case DIR_UP:    bx += 20; break;
				case DIR_DOWN:  bx += 20; by += 40; break;
				case DIR_LEFT:  by += 20; break;
				case DIR_RIGHT: bx += 40; by += 20; break;
				}
				CreateBullet(bx, by, t.dir, t.team);
			}
		}

		// 更新子弹
		for (int i = 0; i < MAX_BULLETS; i++)
		{
			ABullet &b = bullets[i];
			if (!b.alive) continue;
			b.x += b.velX * dt;
			b.y += b.velY * dt;

			if (b.x < GAME_LEFT || b.x > GAME_RIGHT || b.y < GAME_TOP || b.y > GAME_BOTTOM)
			{
				b.alive = false;
				SpawnSparks(b.x, b.y, 3, D3DCOLOR_XRGB(200, 200, 200));
				continue;
			}

			int gx = ((int)b.x - GAME_LEFT) / CELL;
			int gy = ((int)b.y - GAME_TOP) / CELL;
			if (gx >= 0 && gx < 13 && gy >= 0 && gy < 13)
			{
				int v = Map[gy][gx];
				if (v >= 1 && v <= 13)
				{
					Map[gy][gx] = 0;
					b.alive = false;
					CreateBoom((float)(gx * CELL + GAME_LEFT), (float)(gy * CELL + GAME_TOP), 1);
					SpawnSparks((float)(gx * CELL + GAME_LEFT + 32), (float)(gy * CELL + GAME_TOP + 32), 8,
						D3DCOLOR_XRGB(255, 180, 80));
					continue;
				}
				else if (v >= 14 && v <= 26)
				{
					b.alive = false;
					SpawnSparks(b.x, b.y, 5, D3DCOLOR_XRGB(200, 200, 255));
					continue;
				}
			}

			for (int j = 0; j < MAX_TANKS; j++)
			{
				ATank &t = tanks[j];
				if (!t.alive || t.team == b.team) continue;
				if (t.hasShield) continue; // 无敌状态不受伤
				if (b.x >= t.x && b.x <= t.x + TANK_SIZE &&
					b.y >= t.y && b.y <= t.y + TANK_SIZE)
				{
					b.alive = false;
					t.hp--;
					if (t.hp <= 0)
					{
						t.alive = false;
						CreateBoom(t.x - 4, t.y - 4, 2);
						SpawnSparks(t.x + 28, t.y + 28, 15, D3DCOLOR_XRGB(255, 100, 30));
						screenFlash = 150; // 击杀闪屏
						if (b.team == 0) kills0++;
						else kills1++;
					}
					else
					{
						CreateBoom(b.x - 14, b.y - 14, 1);
						SpawnSparks(b.x, b.y, 5, D3DCOLOR_XRGB(255, 220, 100));
					}
					break;
				}
			}
		}

		// 更新爆炸
		for (int i = 0; i < MAX_BOOMS; i++)
		{
			if (booms[i].alive)
			{
				int elapsed = (int)(now - booms[i].startTime);
				if ((booms[i].type == 1 && elapsed > 150) ||
					(booms[i].type == 2 && elapsed > 350))
					booms[i].alive = false;
			}
		}

		// 更新火花
		for (int i = 0; i < MAX_SPARKS; i++)
		{
			Spark &s = sparks[i];
			if (!s.alive) continue;
			s.x += s.vx * dt;
			s.y += s.vy * dt;
			s.vy += 400.0f * dt; // 重力
			s.life -= (int)(dt * 1000);
			if (s.life <= 0) s.alive = false;
		}
	}

	// ---------- 渲染 ----------
	static void FillRect(RECT &rect, long l, long r, long t, long b)
	{
		rect.left = l; rect.right = r; rect.top = t; rect.bottom = b;
	}

	static void RenderBattle()
	{
		// 灰色边框
		RECT rect;
		FillRect(rect, 0, 1024, 32, 64);
		d3dDev->StretchRect(pGray, NULL, backBuffer, &rect, D3DTEXF_NONE);
		FillRect(rect, 0, 64, 64, 896);
		d3dDev->StretchRect(pGray, NULL, backBuffer, &rect, D3DTEXF_NONE);
		FillRect(rect, 896, 1024, 64, 896);
		d3dDev->StretchRect(pGray, NULL, backBuffer, &rect, D3DTEXF_NONE);
		FillRect(rect, 0, 1024, 896, 928);
		d3dDev->StretchRect(pGray, NULL, backBuffer, &rect, D3DTEXF_NONE);

		spriteObj->Begin(D3DXSPRITE_ALPHABLEND);

		// 地图（草地层先画，因为在游戏中草地是覆盖在坦克上方的）
		for (int y = 0; y < 13; y++)
		{
			for (int x = 0; x < 13; x++)
			{
				int v = Map[y][x];
				if (v == 0) continue;
				int px = (x + 1) * CELL;
				int py = (y + 1) * CELL;
				if (v >= 1 && v <= 13)
					Sprite_Transform_Draw(pTile, px, py, 32, 32, 0, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
				else if (v >= 14 && v <= 26)
					Sprite_Transform_Draw(pTile, px, py, 32, 32, 1, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
				else if (v == 27)
					Sprite_Transform_Draw(pTile, px, py, 32, 32, 2, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
				else if (v == 28)
				{
					int wf = (GetTickCount() / 500) % 2 == 0 ? 3 : 4;
					Sprite_Transform_Draw(pTile, px, py, 32, 32, wf, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
				}
			}
		}

		// 坦克
		for (int i = 0; i < MAX_TANKS; i++)
		{
			ATank &t = tanks[i];
			if (!t.alive) continue;
			LPDIRECT3DTEXTURE9 tex;
			if (t.team == 0)
				tex = (i < 3) ? pPlayer1 : pPlayer2; // 混用两种玩家纹理
			else
				tex = pEnemy;
			int frame = t.dir * 8 + t.grade * 2 + (t.moveFrame ? 1 : 0);
			Sprite_Transform_Draw(tex, (int)t.x, (int)t.y, 28, 28,
				frame, 8, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
			// 出生无敌盾牌
			if (t.hasShield)
			{
				int sf = (GetTickCount() / 100) % 2;
				Sprite_Transform_Draw(pShield, (int)t.x, (int)t.y, 32, 32,
					sf, 1, 0, 2.0f, D3DCOLOR_XRGB(255, 255, 255));
			}
		}

		// 子弹
		for (int i = 0; i < MAX_BULLETS; i++)
		{
			ABullet &b = bullets[i];
			if (!b.alive) continue;
			int frame = 0;
			switch (b.dir)
			{
			case DIR_UP:    frame = 0; break;
			case DIR_RIGHT: frame = 1; break;
			case DIR_DOWN:  frame = 2; break;
			case DIR_LEFT:  frame = 3; break;
			}
			Sprite_Transform_Draw(pBullet, (int)b.x, (int)b.y, 8, 8,
				frame, 4, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		}

		// 爆炸
		for (int i = 0; i < MAX_BOOMS; i++)
		{
			ABoom &bm = booms[i];
			if (!bm.alive) continue;
			if (bm.type == 1)
			{
				Sprite_Transform_Draw(pBoom1, (int)bm.x + (rand() % 3 - 1), (int)bm.y + (rand() % 3 - 1),
					28, 28, 0, 1, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
			}
			else
			{
				Sprite_Transform_Draw(pBoom1, (int)bm.x, (int)bm.y,
					28, 28, 0, 1, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
				int elapsed = (int)(GetTickCount() - bm.startTime);
				if (elapsed > 40)
				{
					Sprite_Transform_Draw(pBoom2, (int)bm.x - 18 + (rand() % 5 - 2),
						(int)bm.y - 18 + (rand() % 5 - 2),
						64, 64, 0, 1, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
				}
			}
		}

		// 火花粒子（使用1x1纹理+alpha）
		DWORD now = GetTickCount();
		for (int i = 0; i < MAX_SPARKS; i++)
		{
			Spark &s = sparks[i];
			if (!s.alive) continue;
			float alpha = (float)s.life / (float)s.maxLife;
			int a = (int)(alpha * 255);
			if (a < 0) a = 0;
			if (a > 255) a = 255;
			// 用小尺寸overlay纹理当粒子点
			D3DCOLOR c = D3DCOLOR_ARGB(a,
				(s.color >> 16) & 0xFF,
				(s.color >> 8) & 0xFF,
				s.color & 0xFF);
			Sprite_Transform_Draw(pOverlayTex, (int)s.x, (int)s.y, 1, 1,
				0, 1, 0, 3.0f, c);
		}

		// 击杀闪屏效果
		if (screenFlash > 0)
		{
			int flashAlpha = screenFlash * 80 / 150;
			if (flashAlpha > 80) flashAlpha = 80;
			Sprite_Transform_Draw(pOverlayTex, 0, 0, 1, 1, 0, 1, 0,
				1024.0f, 960.0f, D3DCOLOR_ARGB(flashAlpha, 255, 200, 100));
		}

		// 半透明暗色遮罩（通过sprite alpha blending，不用StretchRect！）
		Sprite_Transform_Draw(pOverlayTex, 0, 0, 1, 1, 0, 1, 0,
			1024.0f, 960.0f, D3DCOLOR_ARGB(100, 0, 0, 0));

		spriteObj->End();
	}
}

// ============================================================
// HomeScene 方法实现
// ============================================================
bool HomeScene::Init()
{
	OutputDebugString("欢迎场景开始初始化\n");

	Tile = LoadTexture(Resource::Texture::Tile, D3DCOLOR_XRGB(4, 4, 4));
	Player_1 = LoadTexture(Resource::Texture::Player_1, D3DCOLOR_XRGB(0, 0, 0));
	Player_2 = LoadTexture(Resource::Texture::Player_2, D3DCOLOR_XRGB(0, 0, 0));
	Enemy_TXTTURE = LoadTexture(Resource::Texture::Enemy, D3DCOLOR_XRGB(4, 4, 4));
	Bullet_TXTTURE = LoadTexture(Resource::Texture::Bullet, D3DCOLOR_XRGB(4, 4, 4));
	Boom1 = LoadTexture(Resource::Texture::Boom1, D3DCOLOR_XRGB(0, 0, 0));
	Boom2 = LoadTexture(Resource::Texture::Boom2, D3DCOLOR_XRGB(4, 4, 4));
	Shield = LoadTexture(Resource::Texture::Shield, D3DCOLOR_XRGB(4, 4, 4));

	if (!Tile || !Player_1 || !Player_2 || !Enemy_TXTTURE || !Bullet_TXTTURE || !Boom1 || !Boom2 || !Shield)
	{
		ShowMessage("Attract Mode 纹理加载失败");
		return false;
	}

	// 创建1x1白色纹理（用于遮罩、粒子、闪屏）
	HRESULT result = D3DXCreateTexture(d3dDev, 1, 1, 1, 0, D3DFMT_A8R8G8B8, D3DPOOL_MANAGED, &OverlayTex);
	if (result == D3D_OK)
	{
		D3DLOCKED_RECT lr;
		if (SUCCEEDED(OverlayTex->LockRect(0, &lr, NULL, 0)))
		{
			*((DWORD*)lr.pBits) = 0xFFFFFFFF; // 纯白
			OverlayTex->UnlockRect(0);
		}
	}

	// 灰色边框
	result = d3dDev->CreateOffscreenPlainSurface(4, 4, D3DFMT_X8R8G8B8, D3DPOOL_DEFAULT, &GrayRect, NULL);
	if (result != D3D_OK) { ShowMessage("灰色区域初始化失败"); return false; }
	d3dDev->ColorFill(GrayRect, NULL, D3DCOLOR_XRGB(110, 110, 110));

	choose = 0;
	font = MakeFont("微软雅黑", 30);
	menuFontShadow = MakeFont("微软雅黑", 32);
	titleFont = MakeFont("Impact", 100);
	titleFontShadow = MakeFont("Impact", 102);
	hintFont = MakeFont("微软雅黑", 20);

	// 传递指针
	HS::pTile = Tile;
	HS::pPlayer1 = Player_1;
	HS::pPlayer2 = Player_2;
	HS::pEnemy = Enemy_TXTTURE;
	HS::pBullet = Bullet_TXTTURE;
	HS::pBoom1 = Boom1;
	HS::pBoom2 = Boom2;
	HS::pShield = Shield;
	HS::pOverlayTex = OverlayTex;
	HS::pGray = GrayRect;

	HS::lastFrameTime = GetTickCount();
	HS::InitBattle();

	return true;
}

void HomeScene::End()
{
	if (Tile) { Tile->Release(); Tile = NULL; }
	if (Player_1) { Player_1->Release(); Player_1 = NULL; }
	if (Player_2) { Player_2->Release(); Player_2 = NULL; }
	if (Enemy_TXTTURE) { Enemy_TXTTURE->Release(); Enemy_TXTTURE = NULL; }
	if (Bullet_TXTTURE) { Bullet_TXTTURE->Release(); Bullet_TXTTURE = NULL; }
	if (Boom1) { Boom1->Release(); Boom1 = NULL; }
	if (Boom2) { Boom2->Release(); Boom2 = NULL; }
	if (Shield) { Shield->Release(); Shield = NULL; }
	if (OverlayTex) { OverlayTex->Release(); OverlayTex = NULL; }
	if (GrayRect) { GrayRect->Release(); GrayRect = NULL; }
	if (font) { font->Release(); font = NULL; }
	if (menuFontShadow) { menuFontShadow->Release(); menuFontShadow = NULL; }
	if (titleFont) { titleFont->Release(); titleFont = NULL; }
	if (titleFontShadow) { titleFontShadow->Release(); titleFontShadow = NULL; }
	if (hintFont) { hintFont->Release(); hintFont = NULL; }
}

void HomeScene::Update()
{
	HS::UpdateBattle();

	if (Key_Up(DIK_DOWN))
	{
		choose++;
		choose %= 5;
	}
	if (Key_Up(DIK_UP))
	{
		choose = choose > 0 ? choose - 1 : 4;
		choose %= 5;
	}
	if (Key_Up(DIK_SPACE))
	{
		Global::Home::selectedType = choose;
		Global::Window::Now_GAME_STATE = 1;
		switch (choose)
		{
		case 0:
			Game_ChangeScene(GAME_STATE::StageSelect);
			break;
		case 1:
			Game_ChangeScene(GAME_STATE::StageSelect);
			break;
		case 2:
			Game_ChangeScene(GAME_STATE::DesignMap);
			break;
		case 3:
			Game_ChangeScene(GAME_STATE::GameSatting);
			break;
		case 4:
			Game_ChangeScene(GAME_STATE::About);
			break;
		default:
			break;
		}
	}
}

void HomeScene::Render()
{
	// 渲染Attract Mode战斗背景
	HS::RenderBattle();

	spriteObj->Begin(D3DXSPRITE_ALPHABLEND);

	// 时间驱动动画
	float t = (float)GetTickCount() / 1000.0f;

	// ===== "TANK GAME" 标题 =====
	// 脉冲发光效果
	float pulse = 0.5f + 0.5f * sinf(t * 2.0f);
	int glowR = 255;
	int glowG = (int)(160 + pulse * 80);
	int glowB = (int)(20 + pulse * 40);

	// 阴影层（偏移+深色）
	FontPrint(titleFontShadow, 284, 154, "TANK  GAME", D3DCOLOR_ARGB(180, 0, 0, 0));
	// 外发光层（更亮更大）
	FontPrint(titleFontShadow, 282, 150, "TANK  GAME", D3DCOLOR_ARGB((int)(60 + pulse * 40), glowR, glowG, 0));
	// 主标题
	FontPrint(titleFont, 282, 150, "TANK  GAME", D3DCOLOR_XRGB(glowR, glowG, glowB));

	// ===== 战况信息 =====
	int alive0 = 0, alive1 = 0;
	for (int i = 0; i < HS::MAX_TANKS; i++)
	{
		if (HS::tanks[i].alive)
		{
			if (HS::tanks[i].team == 0) alive0++;
			else alive1++;
		}
	}
	// 用坦克图标显示双方存活数
	for (int i = 0; i < alive0; i++)
		Sprite_Transform_Draw(Player_1, 60 + i * 30, 930, 28, 28, 0, 8, 0, 1.0f, D3DCOLOR_XRGB(255, 255, 255));
	FontPrint(hintFont, 60 + alive0 * 30 + 5, 932, "VS", D3DCOLOR_XRGB(255, 100, 100));
	for (int i = 0; i < alive1; i++)
		Sprite_Transform_Draw(Enemy_TXTTURE, 60 + alive0 * 30 + 40 + i * 30, 930, 28, 28, 0, 8, 0, 1.0f, D3DCOLOR_XRGB(255, 255, 255));

	// ===== 菜单选项（伪3D多层渲染） =====
	float cursorBob = sinf(t * 5.0f) * 3.0f;
	int menuX = 420;
	int menuY = 450;
	int menuSpacing = 52;

	for (int i = 0; i < 5; i++)
	{
		int yPos = menuY + i * menuSpacing;
		if (i == choose)
		{
			// ── 选中项高光背景条 ──
			float barPulse = 0.4f + 0.15f * sinf(t * 3.0f);
			int barAlpha = (int)(barPulse * 255);
			if (barAlpha > 255) barAlpha = 255;
			Sprite_Transform_Draw(OverlayTex, menuX - 6, yPos - 2, 1, 1, 0, 1, 0,
				180.0f, 44.0f, D3DCOLOR_ARGB(barAlpha, 255, 180, 30));

			// ── 动画坦克光标 ──
			int cursorFrame = 1 * 8 + ((GetTickCount() / 150) % 2);
			Sprite_Transform_Draw(Player_1, menuX - 50 + (int)cursorBob, yPos + 2, 28, 28,
				cursorFrame, 8, 0, 1.2f, D3DCOLOR_XRGB(255, 255, 100));

			// ── 文字阴影层（偏移+深色，营造浮雕感） ──
			FontPrint(menuFontShadow, menuX + 1, yPos + 1, Resource::Home::OptionsStr[i],
				D3DCOLOR_ARGB(200, 0, 0, 0));

			// ── 文字外发光层（脉冲橙色光晕） ──
			float txtPulse = 0.5f + 0.5f * sinf(t * 4.0f);
			int txtGlowAlpha = (int)(40 + txtPulse * 50);
			FontPrint(menuFontShadow, menuX, yPos, Resource::Home::OptionsStr[i],
				D3DCOLOR_ARGB(txtGlowAlpha, 255, 200, 0));

			// ── 主文字层（明亮金色脉冲） ──
			int mainG = (int)(220 + txtPulse * 35);
			int mainB = (int)(60 + txtPulse * 40);
			FontPrint(font, menuX, yPos, Resource::Home::OptionsStr[i],
				D3DCOLOR_XRGB(255, mainG, mainB));
		}
		else
		{
			// ── 未选中：阴影层 ──
			FontPrint(menuFontShadow, menuX + 1, yPos + 1, Resource::Home::OptionsStr[i],
				D3DCOLOR_ARGB(120, 0, 0, 0));

			// ── 未选中：主文字（带微光泽的银灰） ──
			float subtlePulse = 0.5f + 0.5f * sinf(t * 1.5f + i * 0.4f);
			int gray = (int)(140 + subtlePulse * 25);
			FontPrint(font, menuX, yPos, Resource::Home::OptionsStr[i],
				D3DCOLOR_XRGB(gray, gray, gray + 10));
		}
	}

	// ===== 闪烁提示文字 =====
	if ((GetTickCount() / 600) % 2 == 0)
	{
		FontPrint(hintFont, 380, 740, "PRESS SPACE TO START", D3DCOLOR_XRGB(200, 200, 200));
	}

	spriteObj->End();
}