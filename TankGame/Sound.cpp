#include "Sound.h"

namespace Sound {
	 CSound*Start;
	 CSound*GameOver;
	 CSound*Moving;
	 CSound*Stop;
	 CSound*BGM;
	 CSound*PlayerBoom;
}
using namespace Sound;

void Sound::Sound_Init()
{
	Start=LoadSound(Resource::Sound_Rescource::Start);
	if (Start == NULL)
		ShowMessage("开始声音装载失败");

	Moving = LoadSound(Resource::Sound_Rescource::Moving);
	if (Moving == NULL)
		ShowMessage("坦克移动声音装载失败");

	Stop = LoadSound(Resource::Sound_Rescource::Stop);
	if (Stop == NULL)
		ShowMessage("坦克停止引擎声音装载失败");

	BGM = LoadSound(Resource::Sound_Rescource::BGM);
	if (BGM == NULL)
		ShowMessage("BGM装载失败");
	PlayerBoom= LoadSound(Resource::Sound_Rescource::PlayerBoom);
	if (PlayerBoom == NULL)
		ShowMessage("玩家爆炸声音装载失败");
	/*
	GameOver = LoadSound(Resource::Sound_Rescource::GameOver);
	if (GameOver == NULL)
		ShowMessage("游戏失败声音装载失败");
	*/
}

 