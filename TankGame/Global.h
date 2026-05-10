#pragma once
#include<iostream>
#include<Windows.h>
#include"Resource.h"
#include"WinMain.h"
#include"DirectX.h"

#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }  //释放资源
using namespace std;
enum GAME_STATE
{
    //任何场景的初始状态
    None = 0,
    //欢迎界面
    Home = 1,
    //游戏界面
    SinglePlayer = 2,
	//
	DoublePlayer = 3,
	//设计地图
	DesignMap=4,
    //关于场景
    About = 5,
	//游戏设置界面
	GameSatting=6,
	//选关界面
	StageSelect=7
};
namespace Global {
    namespace Window {
        const string GameTitle = "Our Game";
		extern int ScreenWidth;
		extern int ScreenHeight ;
		// Base design resolution and scale factors
		const int BaseWidth = 1024;
		const int BaseHeight = 960;
		extern float ScaleX;
		extern float ScaleY;
        //窗口的坐标
        extern int x, y;
        extern bool EnableBackgroundRunning;
        extern bool isActity;
        //鼠标灵敏度
        const float CursorSensitivity = 1.0f;
        const bool FullScreen =  false;
		// 窗口大小档位：0=小(768x720), 1=中(1024x960), 2=大(适应屏幕), 3=全屏(无边框)
		extern int WindowSizeLevel;
		const int WindowSizeLevelCount = 4;
        //指定逻辑刷新速度
        const float targetFps = 480.0f;
			//当前的游戏状态
		extern int Now_GAME_STATE;
    }
    namespace Home {
        //最终的选项 0=单人游戏 1=双人游戏 2=设计地图 3=关于作者
        extern int selectedType;
		//选择的关卡编号，0=从第1关开始
		extern int SelectedStage;
    }
    namespace Debug {
        //是否显示调试信息
        extern bool ShowDebugInfo;
        //当前的帧率
        extern int currentFPS;
    }
	namespace DesignMap {
			//设计地图的名称
		extern string NewMapName;
	}
	namespace PlayerControl {
			//玩家一的控制方式
		extern  int Player1[6];
			//玩家二的控制方式
		extern  int Player2[6];
	}
	namespace Sound {
		//声音开关
		extern bool SoundSwicth;
	}
	namespace Difficulty {
		// 0=简单, 1=普通, 2=困难
		extern int Level;
		// 难度参数查表
		int GetEnemyTotal();       //每关敌人总数
		int GetSpawnInterval();    //敌人生成间隔(100ms单位)
		int GetHighGradeChance();  //高级敌人(grade 4+)概率(百分比)
		int GetPlayerLives();      //玩家初始生命
		float GetScoreMultiplier();//分数倍率
	}

}
