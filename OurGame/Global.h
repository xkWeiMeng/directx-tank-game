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
    //无任何场景（初始状态）
    None = 0,
    //欢迎场景
    Home = 1,
    //游戏场景
    SinglePlayer = 2,
	//
	DoublePlayer = 3,
	//设计地图
	DesignMap=4,
    //关于场景
    About = 5,
	//游戏设定场景
	GameSatting=6
};
namespace Global {
    namespace Window {
        const string GameTitle = "IG.WXZ";
		extern int ScreenWidth;
		extern int ScreenHeight ;
        //窗口的坐标
        extern int x, y;
        extern bool EnableBackgroundRunning;
        extern bool isActity;
        //鼠标灵敏度
        const float CursorSensitivity = 1.0f;
        const bool FullScreen =  false;
        //指定逻辑刷新速度
        const float targetFps = 480.0f;
		//当前的游戏状态
		extern int Now_GAME_STATE;
    }
    namespace Home {
        //最终的选项，0：单人游戏；1：双人游戏；2：设计地图；3：关于我们
        extern int selectedType;
    }
    namespace Debug {
        //是否显示调试信息
        const bool ShowDebugInfo = false;
        //当前总帧率
        extern int currentFPS;
    }
	namespace DesignMap {
		//新设计地图的名称
		extern string NewMapName;
	}
	namespace PlayerControl {
		//玩家一的控制方式
		extern  int Player1[5];
		//玩家二的控制方式
		extern  int Player2[5];
	}
	namespace Sound {
		//音乐开关
		extern bool SoundSwicth;
	}

}
