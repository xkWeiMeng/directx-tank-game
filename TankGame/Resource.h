#pragma once
#include<iostream>
#include"DirectX.h";
using namespace std;
//所有资源的路径
namespace Resource {
    namespace Home {
        char* const Backgroud = "Resources\\Home\\UI.bmp";
        //char* const BGM = "Resources\\Home\\3.wav";
        const D3DCOLOR SelectedColor = D3DCOLOR_XRGB(255, 128, 128);
        const D3DCOLOR UnselectedColor = D3DCOLOR_XRGB(64, 64, 64);

        char* const OptionsStr[] = { "单人游戏","双人游戏","设计地图","游戏设定","关于我们" };
    }
    namespace Cursor {
        char* const Normal = "Resources\\Cursor\\光标.png";
    }
    namespace About {
        char* const Backgroud = "Resources\\About\\Background.jpg";
		char* const BKG = "Resources\\About\\山景.bmp";
		char* const Cloud1 = "Resources\\About\\云1.png";
		char* const Cloud2 = "Resources\\About\\云2.png";
		char* const Cloud3 = "Resources\\About\\云3.png";
		char* const Feiting = "Resources\\About\\飞艇.tga";

    }
	namespace Sound_Rescource {
		char* const Start = "Resources\\Sound\\开始1.wav";
		char*const Moving = "Resources\\Sound\\坦克移动.wav";
		char*const Stop = "Resources\\Sound\\坦克停止移动.wav";
		char*const GameOver = "Resources\\Sound\\游戏结束.aif";
		char*const BGM = "Resources\\Sound\\bgm.wav";
		char*const PlayerBoom = "Resources\\Sound\\爆炸.wav";
	}
	namespace Texture {
		char*const Flag = "Resources\\Texture\\旗子.bmp";
		char*const Something = "Resources\\Texture\\杂项.bmp";
		char*const Tile = "Resources\\Texture\\砖.bmp";
		char*const Player_1 = "Resources\\Texture\\wxz.png";
		char*const Player_2 = "Resources\\Texture\\玩家二.bmp";
		char*const Bullet = "Resources\\Texture\\子弹.bmp";
		char*const Boom1 = "Resources\\Texture\\爆炸一.bmp";
		char*const Boom2 = "Resources\\Texture\\爆炸二.bmp";
		char*const Enemy = "Resources\\Texture\\敌人.bmp";
		char*const Award = "Resources\\Texture\\奖励.bmp";
		char*const GameOver = "Resources\\Texture\\游戏结束.bmp";
		char*const Shield = "Resources\\Texture\\盾牌.bmp";
		char*const Hole = "Resources\\Texture\\孔.bmp";
		char*const Number = "Resources\\Texture\\数字.bmp";
		char*const GameSetting = "Resources\\Texture\\GameSetting.png";
	}
}
