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
		ShowMessage("¿ªÊ¼ÉùÒô×°ÔØÊ§°Ü");

	Moving = LoadSound(Resource::Sound_Rescource::Moving);
	if (Moving == NULL)
		ShowMessage("Ì¹¿ËÒÆ¶¯ÉùÒô×°ÔØÊ§°Ü");

	Stop = LoadSound(Resource::Sound_Rescource::Stop);
	if (Stop == NULL)
		ShowMessage("Ì¹¿ËÍ£Ö¹ÒıÇæÉùÒô×°ÔØÊ§°Ü");

	BGM = LoadSound(Resource::Sound_Rescource::BGM);
	if (BGM == NULL)
		ShowMessage("BGM×°ÔØÊ§°Ü");
	PlayerBoom= LoadSound(Resource::Sound_Rescource::PlayerBoom);
	if (PlayerBoom == NULL)
		ShowMessage("Íæ¼Ò±¬Õ¨ÉùÒô×°ÔØÊ§°Ü");
	/*
	GameOver = LoadSound(Resource::Sound_Rescource::GameOver);
	if (GameOver == NULL)
		ShowMessage("ÓÎÏ·Ê§°ÜÉùÒô×°ÔØÊ§°Ü");
	*/
}

 