#include "StageSelectScene.h"
#include "Global.h"
#include "DirectX.h"
#include "GameMain.h"
#include <fstream>
#include <vector>
#include <string>
#include <windows.h>

using namespace std;

namespace SSS {
	LPD3DXFONT titleFont;
	LPD3DXFONT listFont;
	LPD3DXFONT hintFont;
	int stageCount = 0;
	vector<string> customMapNames; // 自定义地图文件名（不含路径和扩展名）
	int choose = 0;      // 0=直接开始, 1~N=选择具体关卡/地图
	int scrollTop = 0;   // 列表滚动偏移
	const int maxVisible = 16; // 一屏最多显示的关卡数
	int tab = 0;         // 0=官方关卡, 1=自定义地图

	// 扫描Map目录下的stage文件数量
	int ScanStageCount()
	{
		int count = 0;
		while (true)
		{
			string path = "Map\\stage" + to_string(count + 1) + ".map";
			ifstream f(path, ios::in | ios::binary);
			if (!f.is_open())
				break;
			f.close();
			count++;
		}
		return count;
	}

	// 扫描Map/custom/目录下的自定义地图
	void ScanCustomMaps()
	{
		customMapNames.clear();
		WIN32_FIND_DATAA findData;
		HANDLE hFind = FindFirstFileA("Map\\custom\\*.map", &findData);
		if (hFind == INVALID_HANDLE_VALUE)
			return;
		do
		{
			string name = findData.cFileName;
			// 去掉.map扩展名
			if (name.length() > 4)
				name = name.substr(0, name.length() - 4);
			customMapNames.push_back(name);
		} while (FindNextFileA(hFind, &findData));
		FindClose(hFind);
	}

	int GetTotalItems()
	{
		if (tab == 0)
			return 1 + stageCount; // "直接开始" + 官方关卡
		else
			return (int)customMapNames.size(); // 自定义地图列表
	}
}

using namespace SSS;

bool StageSelectScene::Init()
{
	titleFont = MakeFont("微软雅黑", 40);
	listFont = MakeFont("微软雅黑", 28);
	hintFont = MakeFont("微软雅黑", 18);
	stageCount = ScanStageCount();
	ScanCustomMaps();
	choose = 0;
	scrollTop = 0;
	tab = 0;
	return true;
}

void StageSelectScene::End()
{
	SAFE_RELEASE(titleFont);
	SAFE_RELEASE(listFont);
	SAFE_RELEASE(hintFont);
	customMapNames.clear();
}

void StageSelectScene::Update()
{
	int totalItems = GetTotalItems();

	// 左右键切换标签页
	if (Key_Up(DIK_LEFT) || Key_Up(DIK_RIGHT))
	{
		tab = 1 - tab;
		choose = 0;
		scrollTop = 0;
	}

	if (Key_Up(DIK_DOWN))
	{
		if (totalItems > 0)
		{
			choose++;
			if (choose >= totalItems)
				choose = 0;
		}
	}
	if (Key_Up(DIK_UP))
	{
		if (totalItems > 0)
		{
			choose--;
			if (choose < 0)
				choose = totalItems - 1;
		}
	}

	// 调整滚动
	if (tab == 0)
	{
		// 官方关卡：第0项"直接开始"不参与滚动
		if (choose > 0)
		{
			int listIndex = choose - 1;
			if (listIndex < scrollTop)
				scrollTop = listIndex;
			if (listIndex >= scrollTop + maxVisible)
				scrollTop = listIndex - maxVisible + 1;
		}
		else
		{
			scrollTop = 0;
		}
	}
	else
	{
		// 自定义地图
		if (choose < scrollTop)
			scrollTop = choose;
		if (choose >= scrollTop + maxVisible)
			scrollTop = choose - maxVisible + 1;
	}

	if (Key_Up(DIK_SPACE) || Key_Up(DIK_RETURN))
	{
		if (tab == 0)
		{
			// 官方关卡
			if (choose == 0)
				Global::Home::SelectedStage = 1;
			else
				Global::Home::SelectedStage = choose;

			Global::DesignMap::NewMapName.clear();
			if (Global::Home::selectedType == 0)
				Game_ChangeScene(GAME_STATE::SinglePlayer);
			else
				Game_ChangeScene(GAME_STATE::DoublePlayer);
		}
		else
		{
			// 自定义地图
			if (totalItems > 0 && choose >= 0 && choose < (int)customMapNames.size())
			{
				Global::DesignMap::NewMapName = "custom\\" + customMapNames[choose];
				Global::Home::SelectedStage = 0;
				if (Global::Home::selectedType == 0)
					Game_ChangeScene(GAME_STATE::SinglePlayer);
				else
					Game_ChangeScene(GAME_STATE::DoublePlayer);
			}
		}
	}

	if (Key_Up(DIK_ESCAPE))
	{
		Game_ChangeScene(GAME_STATE::Home);
	}
}

void StageSelectScene::Render()
{
	const D3DCOLOR selectedColor = D3DCOLOR_XRGB(255, 128, 128);
	const D3DCOLOR normalColor = D3DCOLOR_XRGB(200, 200, 200);
	const D3DCOLOR dimColor = D3DCOLOR_XRGB(100, 100, 100);
	const D3DCOLOR titleColor = D3DCOLOR_XRGB(255, 255, 255);

	int centerX = 512;

	// 标题
	FontPrint(titleFont, centerX - 80, 30, "选择关卡", titleColor);

	// 标签页
	int tabY = 80;
	FontPrint(listFont, centerX - 160, tabY, "官方关卡",
		tab == 0 ? selectedColor : dimColor);
	FontPrint(listFont, centerX + 40, tabY, "自定义地图",
		tab == 1 ? selectedColor : dimColor);

	int startY = 130;

	if (tab == 0)
	{
		// 官方关卡
		FontPrint(listFont, centerX - 60, startY, "直接开始",
			choose == 0 ? selectedColor : normalColor);

		int listStartY = startY + 50;
		int visibleCount = min(maxVisible, stageCount - scrollTop);
		for (int i = 0; i < visibleCount; i++)
		{
			int stageNum = scrollTop + i + 1;
			string text = "第 " + to_string(stageNum) + " 关";
			D3DCOLOR color = (choose == stageNum) ? selectedColor : normalColor;
			FontPrint(listFont, centerX - 40, listStartY + i * 40, text, color);
		}

		if (scrollTop > 0)
			FontPrint(listFont, centerX + 120, listStartY - 10, "...", normalColor);
		if (scrollTop + maxVisible < stageCount)
			FontPrint(listFont, centerX + 120, listStartY + visibleCount * 40, "...", normalColor);
	}
	else
	{
		// 自定义地图
		if (customMapNames.empty())
		{
			FontPrint(listFont, centerX - 80, startY + 40, "暂无自定义地图", dimColor);
		}
		else
		{
			int visibleCount = min(maxVisible, (int)customMapNames.size() - scrollTop);
			for (int i = 0; i < visibleCount; i++)
			{
				int idx = scrollTop + i;
				D3DCOLOR color = (choose == idx) ? selectedColor : normalColor;
				FontPrint(listFont, centerX - 60, startY + i * 40, customMapNames[idx], color);
			}

			if (scrollTop > 0)
				FontPrint(listFont, centerX + 140, startY - 10, "...", normalColor);
			if (scrollTop + maxVisible < (int)customMapNames.size())
				FontPrint(listFont, centerX + 140, startY + visibleCount * 40, "...", normalColor);
		}
	}

	// 底部操作提示
	FontPrint(hintFont, centerX - 200, 920, "左右键切换标签  上下键选择  空格/回车确认  ESC返回",
		D3DCOLOR_XRGB(160, 160, 160));
}
