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
}
