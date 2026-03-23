#include"Global.h"
namespace Global {
    namespace Window {
        int x = 0, y = 0;
        bool EnableBackgroundRunning = true;
        bool isActity = true;
		int ScreenWidth;
		int ScreenHeight;
		int Now_GAME_STATE;
    }
    namespace Home {
        int selectedType;
    }
    namespace Debug {
        int currentFPS = 0;
    }
	namespace DesignMap {
		string NewMapName;
	}
	namespace PlayerControl {
		int Player1[5];
		int Player2[5];
	}
	namespace Sound {
		bool SoundSwicth;
	}
}
