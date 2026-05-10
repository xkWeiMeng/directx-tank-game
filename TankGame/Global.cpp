#include"Global.h"
namespace Global {
    namespace Window {
        int x = 0, y = 0;
        bool EnableBackgroundRunning = true;
        bool isActity = true;
		int ScreenWidth;
		int ScreenHeight;
		float ScaleX = 1.0f;
		float ScaleY = 1.0f;
		int Now_GAME_STATE;
		int WindowSizeLevel = 2;
    }
    namespace Home {
        int selectedType;
		int SelectedStage = 0;
    }
    namespace Debug {
        bool ShowDebugInfo = false;
        int currentFPS = 0;
    }
	namespace DesignMap {
		string NewMapName;
	}
	namespace PlayerControl {
		int Player1[6];
		int Player2[6];
	}
	namespace Sound {
		bool SoundSwicth;
	}
	namespace Difficulty {
		int Level = 1; // 默认普通
		//                       简单  普通  困难
		static int enemyTotals[]   = { 20,   30,   40 };
		static int spawnIntervals[]= { 15,   10,    6 };
		static int highGradeChance[]={20,   37,   60 };
		static int playerLives[]   = {  3,    1,    1 };
		static float scoreMult[]   = { 0.8f, 1.0f, 1.5f };

		int GetEnemyTotal()        { return enemyTotals[Level]; }
		int GetSpawnInterval()     { return spawnIntervals[Level]; }
		int GetHighGradeChance()   { return highGradeChance[Level]; }
		int GetPlayerLives()       { return playerLives[Level]; }
		float GetScoreMultiplier() { return scoreMult[Level]; }
	}
}
