#include <windows.h>
#include <iostream>
#include <time.h>
#include"Global.h"
#include"GameMain.h"
#include "winres.h"
#include"resource1.h"
using namespace std;

HWND window;
HDC device;
bool Gameover = false;

// Window callback function
LRESULT CALLBACK WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static RECT rect;
    switch (message)
    {
    case WM_DESTROY:
        Gameover = true;
        PostQuitMessage(0);
        break;
    case WM_PAINT:
		//获取窗口在屏幕的坐标
        if (!Global::Window::FullScreen)
        {
            GetClientRect(hWnd, &rect);
            Global::Window::x = rect.left;
            Global::Window::y = rect.top;
            
        }
        if (!Gameover)
            Game_Render(hWnd, device);
        break;
		/*case WM_SIZE://禁止改变窗口大小
            LONG_PTR Style = ::GetWindowLongPtr(hWnd, GWL_STYLE);
            Style = Style &~WS_CAPTION &~WS_SYSMENU &~WS_SIZEBOX;
            ::SetWindowLongPtr(hWnd, GWL_STYLE, Style);
            break;*/
    case WM_ACTIVATE:
        Global::Window::isActity = !(wParam == WA_INACTIVE);
        break;
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

// MyRegiserClass function sets program window properties
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    //create the window class structure
    WNDCLASSEX wc;
    wc.cbSize = sizeof(WNDCLASSEX);

    //fill the struct with info
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = (WNDPROC)WinProc;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = hInstance;
    wc.hIcon = NULL;
    wc.hCursor =NULL;
    wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
    wc.lpszMenuName = NULL;
    wc.lpszClassName = Global::Window::GameTitle.c_str();
    wc.hIconSm = NULL;

    //set up the window with the class info
    return RegisterClassEx(&wc);
}

// Helper function to create the window and refresh it
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    //create a new window
	long MetricsX = ::GetSystemMetrics(SM_CXSCREEN);
	long MetricsY = ::GetSystemMetrics(SM_CYSCREEN);

	//D3D后缓冲区固定为基准分辨率，由D3D自动拉伸到窗口客户区
	Global::Window::ScreenWidth = Global::Window::BaseWidth;
	Global::Window::ScreenHeight = Global::Window::BaseHeight;
	Global::Window::ScaleX = 1.0f;
	Global::Window::ScaleY = 1.0f;

	// 根据窗口大小档位计算窗口尺寸
	int GameScreenWidth, GameScreenHeight;
	DWORD windowStyle;
	int winX, winY, windowWidth, windowHeight;

	switch (Global::Window::WindowSizeLevel)
	{
	case 0: // 小窗口
		GameScreenWidth = 768;
		GameScreenHeight = 720;
		windowStyle = WS_OVERLAPPEDWINDOW;
		break;
	case 1: // 中窗口
		GameScreenWidth = 1024;
		GameScreenHeight = 960;
		windowStyle = WS_OVERLAPPEDWINDOW;
		break;
	case 3: // 全屏(无边框)
		GameScreenWidth = MetricsX;
		GameScreenHeight = MetricsY;
		windowStyle = WS_POPUP;
		break;
	default: // 2 = 大窗口(适应屏幕)
	{
		const float GameY_Screen_rate = 0.8888889f;
		const float GameY_GameX_rate = 0.9375f;
		GameScreenHeight = (int)(MetricsY * GameY_Screen_rate);
		GameScreenWidth = (int)(GameScreenHeight / GameY_GameX_rate);
		windowStyle = WS_OVERLAPPEDWINDOW;
		break;
	}
	}

	if (windowStyle == WS_POPUP)
	{
		// 全屏无边框，窗口直接覆盖整个屏幕
		winX = 0;
		winY = 0;
		windowWidth = MetricsX;
		windowHeight = MetricsY;
	}
	else
	{
		RECT windowRect = { 0, 0, GameScreenWidth, GameScreenHeight };
		AdjustWindowRect(&windowRect, windowStyle, FALSE);
		windowWidth = windowRect.right - windowRect.left;
		windowHeight = windowRect.bottom - windowRect.top;
		winX = MetricsX / 2 - windowWidth / 2;
		winY = MetricsY / 2 - windowHeight / 2;
	}

    window = CreateWindow(
        Global::Window::GameTitle.c_str(),
        Global::Window::GameTitle.c_str(),
        windowStyle,
		winX, winY,
		windowWidth, windowHeight,
        NULL, NULL, hInstance, NULL);

    if (window == 0) return 0;

    //display the window
    ShowWindow(window, nCmdShow);
    UpdateWindow(window);

    //get device context for drawing
    device = GetDC(window);

    return 1;
}
DWORD currentTime = 0;
DWORD lastCurrentTime = 0;
int currentCount = 0;
int refreshTime = 0;
// Entry point function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

    //create window
    MyRegisterClass(hInstance);
    if (!InitInstance(hInstance, nCmdShow)) return -1;

    //initialize the game
    if (!Game_Init(window)) {
		ShowMessage("游戏初始化失败");
        return -1;
    }

    while (!Gameover)
    {
		//处理Windows消息，优先处理
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
			//如果窗口没有激活，也不允许后台运行游戏时，不更新游戏逻辑运行
            if (Global::Window::isActity == false) {
                if (Global::Window::EnableBackgroundRunning == false)
                    continue;
            }

			//获取当前时间，精确毫秒
            currentTime = timeGetTime();

			//-------计算帧率--------
			//每执行一次循环currentCount自加1
            currentCount++;
			//如果上一次循环超过1秒后currentCount即为当前的FPS帧率
            if (currentTime > lastCurrentTime + 1000)
            {
                Global::Debug::currentFPS = currentCount;
                currentCount = 0;
                lastCurrentTime = currentTime;
            }
            //-----------------------

			//设定逻辑刷新速度为指定的帧率，如果距上一次刷新的时间间隔大于帧率的倒数时间执行Update
            if (currentTime > refreshTime + 1000.0f / Global::Window::targetFps)
            {
                refreshTime = currentTime;
				Game_Update(window);//DirectX循环
            }

			//任何时间全部做渲染
			Game_Render(window, device);//DirectX渲染
        }
    }
	//释放资源
    Game_Free(window, device);

    return msg.wParam;
}

// 应用窗口大小设置
void ApplyWindowSize(int level)
{
	long MetricsX = ::GetSystemMetrics(SM_CXSCREEN);
	long MetricsY = ::GetSystemMetrics(SM_CYSCREEN);

	int GameScreenWidth, GameScreenHeight;
	DWORD windowStyle;

	switch (level)
	{
	case 0:
		GameScreenWidth = 768;
		GameScreenHeight = 720;
		windowStyle = WS_OVERLAPPEDWINDOW;
		break;
	case 1:
		GameScreenWidth = 1024;
		GameScreenHeight = 960;
		windowStyle = WS_OVERLAPPEDWINDOW;
		break;
	case 3:
		GameScreenWidth = MetricsX;
		GameScreenHeight = MetricsY;
		windowStyle = WS_POPUP;
		break;
	default:
	{
		const float GameY_Screen_rate = 0.8888889f;
		const float GameY_GameX_rate = 0.9375f;
		GameScreenHeight = (int)(MetricsY * GameY_Screen_rate);
		GameScreenWidth = (int)(GameScreenHeight / GameY_GameX_rate);
		windowStyle = WS_OVERLAPPEDWINDOW;
		break;
	}
	}

	Global::Window::WindowSizeLevel = level;

	::SetWindowLongPtr(window, GWL_STYLE, windowStyle);

	if (windowStyle == WS_POPUP)
	{
		::SetWindowPos(window, HWND_TOP, 0, 0, MetricsX, MetricsY,
			SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	}
	else
	{
		RECT windowRect = { 0, 0, GameScreenWidth, GameScreenHeight };
		AdjustWindowRect(&windowRect, windowStyle, FALSE);
		int windowWidth = windowRect.right - windowRect.left;
		int windowHeight = windowRect.bottom - windowRect.top;
		int winX = MetricsX / 2 - windowWidth / 2;
		int winY = MetricsY / 2 - windowHeight / 2;
		::SetWindowPos(window, HWND_NOTOPMOST, winX, winY, windowWidth, windowHeight,
			SWP_FRAMECHANGED | SWP_SHOWWINDOW);
	}
}

//结束游戏
void EndApplication()
{
    PostMessage(window, WM_DESTROY, 0, 0);
}
//弹出一个以游戏名称为标题，带有一个确定按钮的消息框
void ShowMessage(string text)
{
    MessageBox(window, text.c_str(), Global::Window::GameTitle.c_str(), MB_OK);
}
