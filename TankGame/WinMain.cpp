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
        //��ȡ��������Ļ������
        if (!Global::Window::FullScreen)
        {
            GetClientRect(hWnd, &rect);
            Global::Window::x = rect.left;
            Global::Window::y = rect.top;
            
        }
        if (!Gameover)
            Game_Render(hWnd, device);
        break;
        /*case WM_SIZE://�����Ʊ�����
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
	long MetricsX = ::GetSystemMetrics(SM_CXSCREEN);//��ȡ��ʾ���ֱ��������ô��ھ���
	long MetricsY = ::GetSystemMetrics(SM_CYSCREEN);
	//create a new window
	//根据屏幕分辨率计算窗口大小，保持游戏宽高比
	const float GameY_Screen_rate = 0.8888889;
	const float GameY_GameX_rate = 0.9375;

	int GameScreenHeight = MetricsY*GameY_Screen_rate;
	int GameScreenWidth  = GameScreenHeight / GameY_GameX_rate;
	//D3D后缓冲区固定为基准分辨率，由D3D自动拉伸到窗口客户区
	Global::Window::ScreenWidth = Global::Window::BaseWidth;
	Global::Window::ScreenHeight = Global::Window::BaseHeight;
	Global::Window::ScaleX = 1.0f;
	Global::Window::ScaleY = 1.0f;

	//计算包含标题栏和边框的窗口尺寸，使客户区等于期望大小
	RECT windowRect = { 0, 0, GameScreenWidth, GameScreenHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

    window = CreateWindow(
        Global::Window::GameTitle.c_str(),              //window class
        Global::Window::GameTitle.c_str(),              //title bar
        WS_OVERLAPPEDWINDOW,   //window style
		MetricsX / 2 - windowWidth / 2,         //x position of window
		MetricsY / 2 - windowHeight / 2,         //y position of window
		windowWidth,                   //width of the window
		windowHeight,                   //height of the window
        NULL,                  //parent window
        NULL,                  //menu
        hInstance,             //application instance
        NULL);                 //window parameters
                               //was there an error creating the window?
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
        ShowMessage("��Ϸ��ʼ��ʧ��");
        return -1;
    }

    while (!Gameover)
    {
        //�����Windows��Ϣ�����ȴ���
        if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else {
            //�������û�м������Ҳ��������̨������Ϸʱ�������������߼�����
            if (Global::Window::isActity == false) {
                if (Global::Window::EnableBackgroundRunning == false)
                    continue;
            }

            //��ȡ��ǰʱ�䣬��ȷ������
            currentTime = timeGetTime();

            //-------����֡��--------
            //ÿִ��һ��ѭ��currentCount�Լ�1
            currentCount++;
            //�����һ��ѭ������1���Ӻ�currentCount��Ϊ��ǰ��FPS֡��
            if (currentTime > lastCurrentTime + 1000)
            {
                Global::Debug::currentFPS = currentCount;
                currentCount = 0;
                lastCurrentTime = currentTime;
            }
            //-----------------------

            //�趨�߼�ˢ���ٶ�Ϊָ����֡�ʣ�������һ��ˢ�µ�ʱ����������֡�ʵĵ���ʱ��ִ��Update
            if (currentTime > refreshTime + 1000.0f / Global::Window::targetFps)
            {
                refreshTime = currentTime;
                Game_Update(window);//DirectXѭ��
            }

            //����ʱ��ȫ������Ⱦ
            Game_Render(window, device);//DirectX��Ⱦ
        }
    }
    //�ͷ���Դ
    Game_Free(window, device);

    return msg.wParam;
}

//������Ϸ
void EndApplication()
{
    PostMessage(window, WM_DESTROY, 0, 0);
}
//����һ������Ϸ����Ϊ���⣬����һ��ȷ����ť����Ϣ��
void ShowMessage(string text)
{
    MessageBox(window, text.c_str(), Global::Window::GameTitle.c_str(), MB_OK);
}
