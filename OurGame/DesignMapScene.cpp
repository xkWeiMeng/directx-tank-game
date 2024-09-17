#include "DesignMapScene.h"
using namespace std;
namespace DMS {
	int Map[13][13];
	int MapPieceChoose;
	int NowMyChoose;
	int NowPage;
	bool IsDesigning;
	void DrawNet();
	void DrawMapPieceChoose(int choose);
	void DrawMapPiece(int choose, int x, int y);
	void DrawMap();
	void UsingMouseChoose(RECT & mrect);
	void DrawBlackRect(int x, int y);
	void DesignMapName();
	char ReadK_B();
	bool WriteMapToHD(string filename);
	void FillRect(RECT&rect, long l = -1, long r = -1, long t = -1, long b = -1);
	string FileNameBuf;
	RECT mouseRect;
	LPD3DXFONT font;
	LPD3DXFONT NumFont;
	LPDIRECT3DTEXTURE9 Player_1 = NULL;
	LPDIRECT3DTEXTURE9 Player_2 = NULL;
	LPDIRECT3DTEXTURE9 Enemy_TXTTURE = NULL;
	LPDIRECT3DSURFACE9 BlackRect = NULL;
	LPDIRECT3DTEXTURE9 Tile = NULL;
}
using namespace DMS;
bool DesignMapScene::Init()
{
	NowPage = 0;
	NowMyChoose = 1;
	IsDesigning = false;
	HRESULT result;
	font = MakeFont("微软雅黑", 86);
	NumFont=MakeFont("微软雅黑", 24);
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
	Tile = LoadTexture(Resource::Texture::Tile, D3DCOLOR_XRGB(4, 4, 4));
	if (!Tile)
	{
		ShowMessage("装载 砖 纹理失败!");
		return false;
	}

	Player_1 = LoadTexture(Resource::Texture::Player_1, D3DCOLOR_XRGB(0, 0, 0));
	if (!Player_1)
	{
		ShowMessage("装载 主玩家 纹理失败!");
		return false;
	}
	Player_2 = LoadTexture(Resource::Texture::Player_2, D3DCOLOR_XRGB(0, 0, 0));
	if (!Player_2)
	{
		ShowMessage("装载 玩家二 纹理失败!");
		return false;
	}

	return true;
}

void DesignMapScene::End()
{
	Player_2->Release();
	Player_1->Release();
	Enemy_TXTTURE->Release();
	BlackRect->Release();
	Tile->Release();
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++)
			Map[j][i] = 0;
	FileNameBuf.clear();
}

void DesignMapScene::Update()
{
	int mX, mY;
	//编辑开始
	if(IsDesigning)
	{
		if (Mouse_Button(MLButton))
		{
			mX = mousePoint.x / 64 - 1;
			mY = mousePoint.y / 64 - 1;
			if (mX >= 0 && mY >= 0 && mX <= 12 && mY <= 12)
				Map[mY][mX]=NowMyChoose;
			mouseRect = { mousePoint.x,mousePoint.y,mousePoint.x + 10,mousePoint.y + 10 };
			UsingMouseChoose(mouseRect);
		}
		if (Mouse_Button(MRButton))
		{
			mX = mousePoint.x / 64 - 1;
			mY = mousePoint.y / 64 - 1;
			if (mX >= 0 && mY >= 0 && mX <= 12 && mY <= 12)
				Map[mY][mX] = 0;
		}
		//回车键保存地图并退回主页面
		if (Key_Up(DIK_RETURN))
		{		
			WriteMapToHD(FileNameBuf);
			Global::DesignMap::NewMapName = FileNameBuf;
			Game_ChangeScene(GAME_STATE::Home);
		}

		if (Key_Up(DIK_UP))
		{
			NowMyChoose=NowMyChoose==1?33:NowMyChoose-1;
		}

		if (Key_Up(DIK_DOWN))
		{
			NowMyChoose=NowMyChoose==33?1:NowMyChoose+1;
		}

		if (Key_Up(DIK_LEFT))
		{
			MapPieceChoose=MapPieceChoose==0?4:MapPieceChoose-1;

			switch (MapPieceChoose)
			{
			case 0:
				NowMyChoose = 1;
				break;
			case 1:
				NowMyChoose = 7;
				break;
			case 2:
				NowMyChoose = 14;
				break;
			case 3:
				NowMyChoose = 20;
				break;
			case 4:
				NowMyChoose = 27;
				break;
			default:
				break;
			}
		}

		if (Key_Up(DIK_RIGHT))
		{
			MapPieceChoose=MapPieceChoose==4?0:MapPieceChoose+1;

			switch (MapPieceChoose)
			{
			case 0:
				NowMyChoose = 1;
				break;
			case 1:
				NowMyChoose = 7;
				break;
			case 2:
				NowMyChoose = 14;
				break;
			case 3:
				NowMyChoose = 20;
				break;
			case 4:
				NowMyChoose = 27;
				break;
			default:
				break;
			}

		}
	}
	//确认文件名
	else
	{
		DesignMapName();
	}

	if (Key_Up(DIK_ESCAPE))
	{
		Game_ChangeScene(GAME_STATE::Home);
	}

}

void DesignMapScene::Render()
{
	if (IsDesigning)
	{			
		DrawNet();
		DrawMap();
		//画当前选择的地图块并加黑方框凸显
		DrawMapPiece(NowMyChoose, mousePoint.x+10, mousePoint.y+20);
		DrawBlackRect(mousePoint.x+10, mousePoint.y+20);
		DrawMapPieceChoose(MapPieceChoose);

	}
	else
	{
		d3dDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0f, 0);
		FontPrint(font, 128, 400, "地图名称：");
		if(FileNameBuf.length()!=0)
		FontPrint(font, 458, 400, FileNameBuf);
	}
}

void DMS::DrawBlackRect(int x, int y)
{
	RECT rect;
	FillRect(rect, x-3, x+67, y-3, y);
	d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, x-3, x+67, y+64, y+67);
	d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, x-3, x, y, y+64);
	d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, x+64, x+67, y, y+64);
	d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);

}

void DMS::DesignMapName()
{
	char cbuf;
	cbuf = ReadK_B();
	if (cbuf == -2)
	{
		string buf = "Map\\";
		buf += FileNameBuf;
		buf += ".map";
		ifstream find(buf);
		if (find.is_open()) {
			ShowMessage("已有地图使用该名称。");
			FileNameBuf.clear();
		}
		else {
		//	FileNameBuf = buf;
			IsDesigning = true;
		}
		return;
	}
	else if (cbuf == -1)
	{
		if (FileNameBuf.length() != 0)
			FileNameBuf.pop_back();
	}
	else if (cbuf != 0)
		FileNameBuf.push_back(cbuf);

}
//读取键盘按键信息
char DMS::ReadK_B()
{
	if (Key_Up(DIK_BACK))
		return -1;
	if (Key_Up(DIK_RETURN))
		return -2;
	if (Key_Up(DIK_A))
		return 'a';
	if (Key_Up(DIK_B))
		return 'b';
	if (Key_Up(DIK_C))
		return 'c';
	if (Key_Up(DIK_D))
		return 'd';
	if (Key_Up(DIK_E))
		return 'e';
	if (Key_Up(DIK_F))
		return 'f';
	if (Key_Up(DIK_G))
		return 'g';
	if (Key_Up(DIK_H))
		return 'h';
	if (Key_Up(DIK_I))
		return 'i';
	if (Key_Up(DIK_J))
		return 'j';
	if (Key_Up(DIK_K))
		return 'k';
	if (Key_Up(DIK_L))
		return 'l';
	if (Key_Up(DIK_M))
		return 'm';
	if (Key_Up(DIK_N))
		return 'n';
	if (Key_Up(DIK_O))
		return 'o';
	if (Key_Up(DIK_P))
		return 'p';
	if (Key_Up(DIK_Q))
		return 'q';
	if (Key_Up(DIK_R))
		return 'r';
	if (Key_Up(DIK_S))
		return 's';
	if (Key_Up(DIK_T))
		return 't';
	if (Key_Up(DIK_U))
		return 'u';
	if (Key_Up(DIK_V))
		return 'v';
	if (Key_Up(DIK_W))
		return 'w';
	if (Key_Up(DIK_X))
		return 'x';
	if (Key_Up(DIK_Y))
		return 'y';
	if (Key_Up(DIK_Z))
		return 'z';

	if (Key_Up(DIK_0))
		return '0';
	if (Key_Up(DIK_1))
		return '1';
	if (Key_Up(DIK_2))
		return '2';
	if (Key_Up(DIK_3))
		return '3';
	if (Key_Up(DIK_4))
		return '4';
	if (Key_Up(DIK_5))
		return '5';
	if (Key_Up(DIK_6))
		return '6';
	if (Key_Up(DIK_7))
		return '7';
	if (Key_Up(DIK_8))
		return '8';
	if (Key_Up(DIK_9))
		return '9';

	return 0;
}
//写当前地图信息到硬盘
bool DMS::WriteMapToHD(string filename)
{
	char buf;
	string sbuf = "Map\\";
	sbuf += FileNameBuf;
	sbuf += ".map";

	ofstream out(sbuf, ios::out | ios::binary);
	if (!out.is_open())
	{
		ShowMessage(sbuf);
		out.close();
	}
	for (int i = 0; i < 13; i++)
		for (int j = 0; j < 13; j++)
		{
			buf = Map[i][j];
			out.write(&buf, 1);
		}
	out.close();
	return 0;
}

//填充RECT
void DMS::FillRect(RECT&rect, long l, long r , long t, long b)
{
	rect.left = l;
	rect.right = r;
	rect.top = t;
	rect.bottom = b;
}

//画辅助网格
void DMS::DrawNet()
{
	RECT rect;
	//for (int i = 0; i < 5; i++)
//		FontPrint(font, 558 + i * 86, 400, FileNameBuf);

	for (int i = 0; i < 12; i++)
	{
		FillRect(rect, 128 + i * 64, 131 + i * 64, 64, 896);
		d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	}

	for (int i = 0; i < 12; i++)
	{
		FillRect(rect, 64, 896, 128 + i * 64, 131 + i * 64);
		d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	}
	FillRect(rect, 64, 896, 64, 67);
	d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, 64, 896, 895, 899);
	d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, 64, 67, 64, 896);
	d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);
	FillRect(rect, 895, 899, 64, 896);
	d3dDev->StretchRect(BlackRect, NULL, backBuffer, &rect, D3DTEXF_NONE);

}

//画地图块选项
void DMS::DrawMapPieceChoose(int choose)
{
	switch (choose)
	{
	case 0:
		DrawMapPiece(1, 928, 256);
		DrawBlackRect(928, 256);

		DrawMapPiece(2, 928, 340);
		DrawBlackRect(928, 340);

		DrawMapPiece(3, 928, 424);
		DrawBlackRect(928, 424);

		DrawMapPiece(4, 928, 508);
		DrawBlackRect(928, 508);

		DrawMapPiece(5, 928, 592);
		DrawBlackRect(928, 592);

		DrawMapPiece(6, 928, 676);
		DrawBlackRect(928, 676);
		break;
	case 1:
		DrawMapPiece(7, 928, 256);
		DrawBlackRect(928, 256);

		DrawMapPiece(8, 928, 340);
		DrawBlackRect(928, 340);

		DrawMapPiece(9, 928, 424);
		DrawBlackRect(928, 424);

		DrawMapPiece(10, 928, 508);
		DrawBlackRect(928, 508);

		DrawMapPiece(11, 928, 592);
		DrawBlackRect(928, 592);

		DrawMapPiece(12, 928, 676);
		DrawBlackRect(928, 676);

		DrawMapPiece(13, 928, 760);
		DrawBlackRect(928, 760);
		break;
	case 2:
		DrawMapPiece(14, 928, 256);
		DrawBlackRect(928, 256);

		DrawMapPiece(15, 928, 340);
		DrawBlackRect(928, 340);

		DrawMapPiece(16, 928, 424);
		DrawBlackRect(928, 424);

		DrawMapPiece(17, 928, 508);
		DrawBlackRect(928, 508);

		DrawMapPiece(18, 928, 592);
		DrawBlackRect(928, 592);

		DrawMapPiece(19, 928, 676);
		DrawBlackRect(928, 676);
		break;
	case 3:
		DrawMapPiece(20, 928, 256);
		DrawBlackRect(928, 256);

		DrawMapPiece(21, 928, 340);
		DrawBlackRect(928, 340);

		DrawMapPiece(22, 928, 424);
		DrawBlackRect(928, 424);

		DrawMapPiece(23, 928, 508);
		DrawBlackRect(928, 508);

		DrawMapPiece(24, 928, 592);
		DrawBlackRect(928, 592);

		DrawMapPiece(25, 928, 676);
		DrawBlackRect(928, 676);

		DrawMapPiece(26, 928, 760);
		DrawBlackRect(928, 760);
		break;
	case 4:
		DrawMapPiece(27, 928, 256);
		DrawBlackRect(928, 256);

		DrawMapPiece(28, 928, 340);
		DrawBlackRect(928, 340);

		DrawMapPiece(29, 928, 424);
		DrawBlackRect(928, 424);

		DrawMapPiece(30, 928, 508);
		DrawBlackRect(928, 508);

		DrawMapPiece(31, 928, 592);
		DrawBlackRect(928, 592);

		DrawMapPiece(32, 928, 676);
		DrawBlackRect(928, 676);

		DrawMapPiece(33, 928, 760);
		DrawBlackRect(928, 760);

		break;
	default:
		break;
	}
}
//
void DMS::DrawMapPiece(int choose,int x,int y)
{
	switch (choose)
	{
	case 0:break;
		//
	case 13:Sprite_Transform_Draw(Tile, x, y,
		32, 32, 0, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

	case 26:Sprite_Transform_Draw(Tile, x, y,
		32, 32, 1, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

	case 27:Sprite_Transform_Draw(Tile, x, y,
		32, 32, 2, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

	case 28:Sprite_Transform_Draw(Tile, x, y,
		32, 32, 3, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

	case 29:Sprite_Transform_Draw(Tile, x, y,
		32, 32, 4, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

	case 30:Sprite_Transform_Draw(Tile, x, y,
		32, 32, 5, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

	case 31:Sprite_Transform_Draw(Player_1, x, y, 28, 28,
		0, 1, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

	case 32:Sprite_Transform_Draw(Player_2, x, y, 28, 28,
		0, 1, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

	case 33:Sprite_Transform_Draw(Enemy_TXTTURE, x,y, 28, 28,
		0, 1, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

		//
	case 1:Sprite_Transform_Draw(Tile, x, y,
		16, 16, 0, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 2:Sprite_Transform_Draw(Tile, x+32, y,
		16, 16, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 3:Sprite_Transform_Draw(Tile, x+32, y+32,
		16, 16, 14, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 4:Sprite_Transform_Draw(Tile, x, y+32,
		16, 16, 15, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

	case 5:Sprite_Transform_Draw(Tile, x, y,
		32, 16, 0, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 6:Sprite_Transform_Draw(Tile, x+32, y,
		16, 32, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 7:Sprite_Transform_Draw(Tile, x, y+32,
		32, 16, 0, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 8:Sprite_Transform_Draw(Tile, x, y,
		16, 32, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

	case 9:Sprite_Transform_Draw(Tile, x, y,
		16, 32, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		Sprite_Transform_Draw(Tile, x+32, y,
			16, 16, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 10:Sprite_Transform_Draw(Tile, x, y,
		16, 16, 0, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		Sprite_Transform_Draw(Tile, x+32, y,
			16, 32, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 11:Sprite_Transform_Draw(Tile, x, y+32,
		32, 16, 0, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		Sprite_Transform_Draw(Tile, x+32, y,
			16, 16, 14, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 12:Sprite_Transform_Draw(Tile, x, y,
		16, 32, 1, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		Sprite_Transform_Draw(Tile, x+32, y+32,
			16, 16, 15, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

	case 14:Sprite_Transform_Draw(Tile, x, y,
		16, 16, 2, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 15:Sprite_Transform_Draw(Tile, x+32, y,
		16, 16, 3, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 16:Sprite_Transform_Draw(Tile, x+32, y+32,
		16, 16, 16, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 17:Sprite_Transform_Draw(Tile, x, y+32,
		16, 16, 17, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

	case 18:Sprite_Transform_Draw(Tile, x, y,
		32, 16, 1, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 19:Sprite_Transform_Draw(Tile, x+32, y,
		16, 32, 3, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 20:Sprite_Transform_Draw(Tile, x, y+32,
		32, 16, 1, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 21:Sprite_Transform_Draw(Tile, x, y,
		16, 32, 2, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

	case 22:Sprite_Transform_Draw(Tile, x, y,
		32, 16, 1, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		Sprite_Transform_Draw(Tile, x, y+32,
			16, 16, 17, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 23:Sprite_Transform_Draw(Tile, x, y,
		16, 16, 2, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		Sprite_Transform_Draw(Tile, x+32, y,
			16, 32, 3, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 24:Sprite_Transform_Draw(Tile, x, y+32,
		32, 16, 1, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		Sprite_Transform_Draw(Tile, x+32, y,
			16, 16, 3, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	case 25:Sprite_Transform_Draw(Tile, x, y+32,
		32, 16, 1, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255));
		Sprite_Transform_Draw(Tile, x, y,
			16, 16, 2, 14, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;
	default:
		break;
	}

}
//游戏地图绘画函数       
void DMS::DrawMap()
{
	for (int i = 0; i<13; i++)
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

			case 30:Sprite_Transform_Draw(Tile, (i + 1) * 64, (j + 1) * 64,
				32, 32, 5, 7, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

			case 31:Sprite_Transform_Draw(Player_1, (i + 1) * 64, (j + 1) * 64, 28, 28,
				0,1, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

			case 32:Sprite_Transform_Draw(Player_2, (i + 1) * 64, (j + 1) * 64, 28, 28,
				0, 1, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

			case 33:Sprite_Transform_Draw(Enemy_TXTTURE, (i + 1) * 64, (j + 1) * 64, 28, 28,
				0, 1, 0, 2, D3DCOLOR_XRGB(255, 255, 255)); break;

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
//用鼠标选择地图块
void DMS::UsingMouseChoose(RECT &mrect)
{
	RECT rect,nothing;
	rect = { 928,256,992,256 + 64 };
	if (IntersectRect(&nothing, &rect, &mrect))
	{
		switch (MapPieceChoose)
		{
		case 0:
			NowMyChoose = 1;
			break;
		case 1:
			NowMyChoose = 7;
			break;
		case 2:
			NowMyChoose = 14;
			break;
		case 3:
			NowMyChoose = 20;
			break;
		case 4:
			NowMyChoose = 27;
			break;
		default:
			break;
		}
		return;
	}

	rect = { 928,256,992,340 + 64 };
	if (IntersectRect(&nothing, &rect, &mrect))
	{
		switch (MapPieceChoose)
		{
		case 0:
			NowMyChoose = 2;
			break;
		case 1:
			NowMyChoose = 8;
			break;
		case 2:
			NowMyChoose = 15;
			break;
		case 3:
			NowMyChoose = 21;
			break;
		case 4:
			NowMyChoose = 28;
			break;
		default:
			break;
		}
		return;
	}

	rect = { 928,256,992,424 + 64 };
	if (IntersectRect(&nothing, &rect, &mrect))
	{
		switch (MapPieceChoose)
		{
		case 0:
			NowMyChoose = 3;
			break;
		case 1:
			NowMyChoose = 9;
			break;
		case 2:
			NowMyChoose = 16;
			break;
		case 3:
			NowMyChoose = 22;
			break;
		case 4:
			NowMyChoose = 29;
			break;
		default:
			break;
		}
		return;
	}

	rect = { 928,256,992,508 + 64 };
	if (IntersectRect(&nothing, &rect, &mrect))
	{
		switch (MapPieceChoose)
		{
		case 0:
			NowMyChoose = 4;
			break;
		case 1:
			NowMyChoose = 10;
			break;
		case 2:
			NowMyChoose = 17;
			break;
		case 3:
			NowMyChoose = 23;
			break;
		case 4:
			NowMyChoose = 30;
			break;
		default:
			break;
		}
		return;
	}

	rect = { 928,256,992,592 + 64 };
	if (IntersectRect(&nothing, &rect, &mrect))
	{
		switch (MapPieceChoose)
		{
		case 0:
			NowMyChoose = 5;
			break;
		case 1:
			NowMyChoose = 11;
			break;
		case 2:
			NowMyChoose = 18;
			break;
		case 3:
			NowMyChoose = 24;
			break;
		case 4:
			NowMyChoose = 31;
			break;
		default:
			break;
		}
		return;
	}

	rect = { 928,256,992,676 + 64 };
	if (IntersectRect(&nothing, &rect, &mrect))
	{
		switch (MapPieceChoose)
		{
		case 0:
			NowMyChoose = 6;
			break;
		case 1:
			NowMyChoose = 12;
			break;
		case 2:
			NowMyChoose = 19;
			break;
		case 3:
			NowMyChoose = 25;
			break;
		case 4:
			NowMyChoose = 32;
			break;
		default:
			break;
		}
		return;
	}

	if (!(MapPieceChoose == 0 || MapPieceChoose == 2))
	{
		rect = { 928,256,992,760 + 64 };
		if (IntersectRect(&nothing, &rect, &mrect))
		{
			switch (MapPieceChoose)
			{
			case 0:
				NowMyChoose = 2;
				break;
			case 1:
				NowMyChoose = 13;
				break;
			case 2:
				NowMyChoose = 20;
				break;
			case 3:
				NowMyChoose = 26;
				break;
			case 4:
				NowMyChoose = 33;
				break;
			default:
				break;
			}
			return;
		}
	}
}