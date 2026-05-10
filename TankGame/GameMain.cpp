#include"GameMain.h"
#include"Global.h"
#include"Scenes.h"
#include"DebugTools.h"
#include"GUIs.h"
#include"DirectSound.h"
//当前游戏的场景
Scene *scene = NULL;

//当前游戏状态
GAME_STATE Game_State;

//读取游戏设置
bool ReadPlayerSettingInHD();

// Startup and loading code goes here
bool Game_Init(HWND window)
{
    if (!Direct3D_Init(window, Global::Window::ScreenWidth, Global::Window::ScreenHeight, Global::Window::FullScreen))
    {
        ShowMessage("Direct3D初始化失败");
        return false; 
    }
    if (!DirectInput_Init(window))
    {
        ShowMessage("Direct Input 初始化失败");
        return false;
    }
    if (!DirectSound_Init(window)) {
        ShowMessage("Direct Sound 初始化失败");
        return false;
    }
	//初始化声音资源
	Sound::Sound_Init();
	//
    GUI::Cursor::Init();
	//
	Global::Window::Now_GAME_STATE = 0;
	//声音开关
	//Global::Sound::SoundSwicth = true;
	//初始化玩家控制键  //完整版需要读取硬盘中的游戏配置
	{
		//玩家一 (使用DirectInput键码)
		Global::PlayerControl::Player1[0] = 0xC8; // DIK_UP
		Global::PlayerControl::Player1[1] = 0xD0; // DIK_DOWN
		Global::PlayerControl::Player1[2] = 0xCB; // DIK_LEFT
		Global::PlayerControl::Player1[3] = 0xCD; // DIK_RIGHT
		Global::PlayerControl::Player1[4] = 0x2D; // DIK_X
		Global::PlayerControl::Player1[5] = 0x2C; // DIK_Z
		//玩家二
		Global::PlayerControl::Player2[0] = 0x11; // DIK_W
		Global::PlayerControl::Player2[1] = 0x1F; // DIK_S
		Global::PlayerControl::Player2[2] = 0x1E; // DIK_A
		Global::PlayerControl::Player2[3] = 0x20; // DIK_D
		Global::PlayerControl::Player2[4] = 0x24; // DIK_J
		Global::PlayerControl::Player2[5] = 0x25; // DIK_K
	}
	//
	ReadPlayerSettingInHD();
	// 应用从配置文件读取的窗口大小设置
	ApplyWindowSize(Global::Window::WindowSizeLevel);
    //切换到欢迎场景
    Game_ChangeScene(GAME_STATE::Home);

    return true;
}

/*
    逻辑处理函数
*/
void Game_Update(HWND window)
{
    //获取最新的鼠标键盘输入
    DirectInput_Update(window);

    GUI::Cursor::Update();

    //执行当前场景的Update逻辑处理函数
    if (scene != NULL)
        scene->Update();
	//在主界面检测退出键按下后退出游戏
	if (Global::Window::Now_GAME_STATE == 0)
	{
		if (Key_Up(DIK_ESCAPE))
			PostMessage(window, WM_DESTROY, 0, 0);
	}
}

/*
    渲染处理函数
*/
void Game_Render(HWND window, HDC device)
{
    //确认DX已经生效
    if (!d3dDev) return;

    //清屏
    d3dDev->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 65, 105, 225 ), 1.0f, 0);

    //开始渲染
    if (d3dDev->BeginScene())
    {
        spriteObj->Begin(D3DXSPRITE_ALPHABLEND);

        //调用当前场景的渲染函数
        if (scene != NULL)
            scene->Render();

        if (Global::Debug::ShowDebugInfo)
        {
            DebugTools::PrintMouseInfo();
        }


        GUI::Cursor::Render();

        spriteObj->End();
        d3dDev->EndScene();

        // 计算保持宽高比的视口区域，将后缓冲区内容等比缩放到窗口客户区
        RECT clientRect;
        GetClientRect(window, &clientRect);
        int windowWidth = clientRect.right - clientRect.left;
        int windowHeight = clientRect.bottom - clientRect.top;

        if (windowWidth > 0 && windowHeight > 0)
        {
            RECT viewportRect = GetViewportRect(windowWidth, windowHeight);
            d3dDev->Present(NULL, &viewportRect, NULL, NULL);
        }
        else
        {
            d3dDev->Present(NULL, NULL, NULL, NULL);
        }
    }
}
//切换游戏场景
void Game_ChangeScene(GAME_STATE to)
{
    if (to != Game_State)
    {
        if (scene != NULL)
        {
            //释放当前场景的资源
            scene->End();
            delete scene;
        }
        switch (to)
        {
        case GAME_STATE::Home:
			Global::Window::Now_GAME_STATE = 0;
            scene = new HomeScene();
			break;
		case GAME_STATE::DoublePlayer:
        case GAME_STATE::SinglePlayer:
			Global::Window::Now_GAME_STATE = 1;
			scene = new GamingScene();
            break;
		case GAME_STATE::DesignMap:
			Global::Window::Now_GAME_STATE = 1;
			scene = new DesignMapScene();
			break;
        case GAME_STATE::About:
			Global::Window::Now_GAME_STATE = 1;
			scene = new AboutScene();
            break;
		case GAME_STATE::GameSatting:
			Global::Window::Now_GAME_STATE = 1;
			scene = new GameSettingScene();
			break;
		case GAME_STATE::StageSelect:
			Global::Window::Now_GAME_STATE = 1;
			scene = new StageSelectScene();
			break;
        default:
			Global::Window::Now_GAME_STATE = 0;
			scene = NULL;
            break;
        }
        //调用场景的初始化函数
        if (scene == NULL || !scene->Init())
        {
            //如果场景初始化出错则结束游戏
			//这里应弹出窗口说明出错
            EndApplication();
        }
        Game_State = to;
    }
}

// 只允许在消息处理函数WinProc中调用此函数，要想关闭游戏，调用WinMain里的EndApplication
void Game_Free(HWND window, HDC device)
{
    //调用场景的关闭函数并释放场景
    if (scene != NULL)
    {
        scene->End();
        delete scene;
    }
    DirectInput_Shutdown();
    DirectSound_Shutdown();
    Direct3D_Shutdown();
    ReleaseDC(window, device);
}
//读取游戏设置
bool ReadPlayerSettingInHD()
{
	unsigned char buf;
	ifstream in("GameSet.set", ios::in | ios::binary);
	if (!in.is_open())
	{
		// 配置文件不存在时使用默认值，不报错
		return false;
	}
	for (int i = 0; i < 6; i++)
	{
		in.read((char*)&buf, 1);
		Global::PlayerControl::Player1[i]=buf;
	}
	for (int i = 0; i < 6; i++)
	{
		in.read((char*)&buf, 1);
		Global::PlayerControl::Player2[i]=buf;
	}
	in.read((char*)&buf, 1);
	Global::Sound::SoundSwicth=buf;
	// 读取窗口大小档位（兼容旧版11字节配置文件）
	if (in.read((char*)&buf, 1))
	{
		int level = buf;
		if (level >= 0 && level < Global::Window::WindowSizeLevelCount)
			Global::Window::WindowSizeLevel = level;
	}
	// 读取Debug模式（兼容旧版12字节配置文件）
	if (in.read((char*)&buf, 1))
	{
		Global::Debug::ShowDebugInfo = (buf != 0);
	}
	in.close();
	return true;
}
